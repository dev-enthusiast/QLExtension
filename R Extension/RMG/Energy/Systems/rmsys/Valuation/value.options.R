#
#
#
#

value.options <- function(vrp, dd, deals){

  dd  <- subset(dd, INSTRUMENT.TYPE.ID==4)
  deal.ids <- unique(dd$DEAL.ID)
  vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
  vrp   <- subset(vrp, LEG.ID==1)  # the second leg is the payment
#  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE",
    "VOL", "STRIKE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]

  vrp$Tex <- as.numeric(vrp$EXP.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*vrp$Tex)
  # NOTE: the NOTIONAL is negative when you sold the option!
  
  #-----------------------------------Get the CALL.PUT flag-----
  aux <- as.data.frame(table(dd$CALL.PUT.FLAG, dd$DEAL.ID))
  aux <- aux[-which(aux$Freq==0),]  # the deal is either a call or a put
  colnames(aux)[1:2] <- c("CALL.PUT.FLAG","DEAL.ID")
  aux <- merge(vrp[,c("DEAL.ID","SIDE")], aux)
  aux$cp.flag <- ifelse(aux$CALL.PUT.FLAG=="C","CALL","PUT")
  vrp$CALL.PUT.FLAG <- aux$cp.flag
  
  base.deals <- data.frame(S=vrp$PRICE, sigma=vrp$VOL, Tex=vrp$Tex, r=0,
    K=vrp$STRIKE, type=vrp$CALL.PUT.FLAG)
  value <- blackscholes(base.deals)$value
  vrp$RMG.MTM <- value*vrp$NOTIONAL*vrp$DF

  base.deals <- cbind(base.deals, NOTIONAL=vrp$NOTIONAL, DF=vrp$DF,
                      CONTRACT.DATE=vrp$CONTRACT.DATE)
  colnames(base.deals)[which(colnames(base.deals)=="S")] <- "PRICE"
  sdeals <- make.shifted.deals(base.deals, scenario)
  colnames(sdeals)[which(colnames(sdeals)=="PRICE")] <- "S"  
  value  <- blackscholes(sdeals)$value 
  sdeals$MTM <- value*sdeals$NOTIONAL*sdeals$DF
  
  res <- aggregate(sdeals$MTM, by=list(
              CONTRACT.DATE = as.character(sdeals$CONTRACT.DATE),
              SHIFT         = sdeals$SHIFT), sum)
  colnames(res)[3] <- "MTM"
  res <- cbind(INSTRUMENT.TYPE="EXCHOPT", res)
  return(list(res, vrp))



}
