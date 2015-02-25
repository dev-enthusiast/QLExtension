# Value demand charges
#
#
#

value.DEMCHG <- function(vrp, dd, deals, options){

  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp <- subset(vrp, DEAL.ID %in% deal.ids)
  vrp <- merge(dd[,c("DEAL.ID", "LEG.ID")], vrp)
  deals <- subset(deals, DEAL.ID %in% deal.ids)
  
  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "PAYMENT.DATE")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]

  vrp$Tex <- as.numeric(vrp$PAYMENT.DATE-vrp$COB)/365
  vrp$DF  <- exp(-vrp$IR*vrp$Tex)
  vrp <- subset(vrp, Tex>=0)          # get only deals in the future

  vrp$RMG.unit <- vrp$VAL.PRICE*vrp$DF
  vrp$RMG.MTM  <- vrp$NOTIONAL*vrp$RMG.unit
##   ind <- which(vrp$SIDE == 2)
##   if (length(ind)>0){vrp$RMG.MTM[ind] <- -vrp$RMG.MTM[ind]}
  
  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], vrp, all.y=T)
  head(vrp)
  cat("   Done.\n"); flush.console()
  return(vrp)
}
  
