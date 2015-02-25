# American style options.
#
#
#

value.EXCHOPT <- function(vrp, dd, deals, options){

  source("H:/user/R/RMG/Finance/BlackScholes/price.american.R")
  source("H:/user/R/RMG/Finance/BlackScholes/greeks.american.R")
  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
  vrp   <- subset(vrp, LEG.ID==1)  # the second leg is the payment
  deals <- subset(deals, DEAL.ID %in% deal.ids)
                            
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE",
    "VOL", "STRIKE", "DELTA", "GAMMA", "VEGA")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]

  vrp$Tex <- as.numeric(vrp$EXP.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*vrp$Tex)
  # NOTE: the NOTIONAL is negative when you sold the option!
  
  #-----------------------------------Get the CALL.PUT flag-----
  aux <- as.data.frame(table(dd$CALL.PUT.FLAG, dd$DEAL.ID))
  aux <- aux[-which(aux$Freq==0),]  # the deal is either a call or a put
  aux$Freq <- NULL
  colnames(aux) <- c("CALL.PUT.FLAG","DEAL.ID")
  vrp <- merge(vrp, aux)
  cp.flag <- ifelse(vrp$CALL.PUT.FLAG=="C","CALL","PUT")
  
  for (r in 1:nrow(vrp)){  
    vrp$RMG.unit[r] <- price.american(vrp$PRICE[r], vrp$VOL[r], vrp$Tex[r],
         vrp$STRIKE[r], vrp$IR[r], cp.flag[r])
    vrp$RMG.MTM[r]  <- vrp$NOTIONAL[r]*vrp$RMG.unit[r]     
    greeks <- greeks.american(vrp$PRICE[r], vrp$VOL[r], vrp$Tex[r],
         vrp$STRIKE[r], vrp$IR[r], cp.flag[r])
    vrp$RMG.DELTA[r] <-  vrp$NOTIONAL[r]*greeks$delta
    vrp$RMG.GAMMA[r] <-  vrp$NOTIONAL[r]*greeks$gamma/100
    vrp$RMG.VEGA[r]  <-  vrp$NOTIONAL[r]*greeks$vega/100 
  }
  
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp)
  head(vrp)
  
  cat("   Done.\n"); flush.console()
  return(vrp)
}

##   base.deals <- cbind(base.deals, NOTIONAL=vrp$NOTIONAL, DF=vrp$DF,
##                       CONTRACT.DATE=vrp$CONTRACT.DATE)
##   colnames(base.deals)[which(colnames(base.deals)=="S")] <- "PRICE"
##   sdeals <- make.shifted.deals(base.deals, scenario)
##   colnames(sdeals)[which(colnames(sdeals)=="PRICE")] <- "S"  
##   value  <- blackscholes(sdeals)$value 
##   sdeals$MTM <- value*sdeals$NOTIONAL*sdeals$DF
  
##   res <- aggregate(sdeals$MTM, by=list(
##               CONTRACT.DATE = as.character(sdeals$CONTRACT.DATE),
##               SHIFT         = sdeals$SHIFT), sum)
##   colnames(res)[3] <- "MTM"
##   res <- cbind(INSTRUMENT.TYPE="EXCHOPT", res)
