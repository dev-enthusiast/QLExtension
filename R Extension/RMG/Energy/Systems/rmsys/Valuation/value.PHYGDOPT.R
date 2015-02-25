# Valued like linears. 
#
#
#

value.PHYGDOPT <- function(vrp, dd, deals, options){

  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp   <- subset(vrp, DEAL.ID %in% deal.ids)
  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "EXP.DATE", "PAYMENT.DATE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]
  
  # get the deals that have not expired yet
  vrp <- subset(vrp, vrp$EXP.DATE >= vrp$COB)
  
  vrp$Tex <- as.numeric(vrp$EXP.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*as.numeric(vrp$PAYMENT.DATE-vrp$COB)/365)
  vrp$RMG.MTM <- vrp$DF * vrp$NOTIONAL * vrp$VAL.PRICE
  
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp)
  head(vrp)

  cat("   Done.\n"); flush.console()
  return(vrp)
}

