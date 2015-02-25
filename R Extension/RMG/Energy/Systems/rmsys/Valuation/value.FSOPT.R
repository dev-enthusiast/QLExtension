# Forward start options, using black-scholes model.
# Small differences, should be able to improve.  Use this for now.
#
#

value.FSOPT <- function(vrp, dd, deals, options){

  source("H:/user/R/RMG/Finance/BlackScholes/blackscholes.R")
  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE",
    "PAYMENT.DATE", "VOL", "STRIKE", "DELTA", "GAMMA", "VEGA")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]
  # get the deals that have not expired yet
  vrp <- subset(vrp, vrp$EXP.DATE >= vrp$COB)
          
  contract.dt <- as.Date(format(vrp$CONTRACT.DATE, "%Y-%m-01"))
  vrp$Tex <- as.numeric(vrp$EXP.DATE-contract.dt)/365   # <-- !!!
  vrp$DF  <- exp(-vrp$IR*as.numeric(vrp$PAYMENT.DATE-vrp$COB)/365)
  # NOTE: the NOTIONAL is negative when you sold the option!
  
  #-----------------------------------Get the CALL.PUT flag-----
  aux <- as.data.frame(table(dd$CALL.PUT.FLAG, dd$DEAL.ID))
  aux <- aux[-which(aux$Freq==0),]  # the deal is either a call or a put
  aux$Freq <- NULL                                               
  colnames(aux) <- c("CALL.PUT.FLAG","DEAL.ID")
  vrp <- merge(vrp, aux)
  cp.flag <- ifelse(vrp$CALL.PUT.FLAG=="C","CALL","PUT")
 
  base.deals <- data.frame(S=vrp$PRICE, sigma=vrp$VOL, Tex=vrp$Tex,
    r=0, K=vrp$STRIKE, type=cp.flag)
  vrp$RMG.unit <- vrp$DF*blackscholes(base.deals)$value
  vrp$RMG.MTM  <- vrp$NOTIONAL * vrp$RMG.unit
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp)
  head(vrp)
  source("H:/user/R/RMG/Finance/BlackScholes/greeksEU.R")
   greeks <- greeksEU(base.deals$S, base.deals$sigma, base.deals$Tex, base.deals$K, 
                  base.deals$r, base.deals$type)
                  
  vrp$RMG.DELTA <- vrp$NOTIONAL*vrp$DF*greeks$DELTA
  vrp$RMG.GAMMA <-  vrp$NOTIONAL*vrp$DF*greeks$GAMMA/100
  vrp$RMG.VEGA  <-  vrp$NOTIONAL*vrp$DF*greeks$VEGA /100   
  
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
  
##   res <- aggregate(sdeals$MTM, by=list(CONTRACT.DATE=sdeals$CONTRACT.DATE,
##                                         SHIFT=sdeals$SHIFT), sum)
##   colnames(res)[3] <- "MTM"
##   res <- cbind(INSTRUMENT.TYPE="FINOPT", res)
