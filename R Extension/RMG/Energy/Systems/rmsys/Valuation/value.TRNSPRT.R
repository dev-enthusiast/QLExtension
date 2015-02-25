# Value transport spread options 
# _lib elec gas transport fns
#
#

value.TRNSPRT <- function(vrp, dd, deals, options){

  source("H:/user/R/RMG/Finance/BlackScholes/spread.option.R")
  cat(paste("Working on inst.type=", options$instrument.type.id, sep=""))
  flush.console()
  dd  <- subset(dd, INSTRUMENT.TYPE.ID==options$instrument.type.id)
  deal.ids <- unique(dd$DEAL.ID)
  vrp <- subset(vrp, DEAL.ID %in% deal.ids)
  vrp <- merge(dd[,c("DEAL.ID", "LEG.ID", "FUEL2")], vrp)
  deals <- subset(deals, DEAL.ID %in% deal.ids)

  cols <- c("COB","VAL.ID","DEAL.ID","LEG.ID", "SIDE", "CONTRACT.DATE", "VOL",
    "PRICE", "VAL.PRICE", "FXRATE", "IR", "NOTIONAL", "MTM", "PAYMENT.DATE",
    "CORR", "STRIKE", "EXP.DATE", "DELTA", "GAMMA", "VEGA", "FUEL2")
  vrp <- vrp[,colnames(vrp)%in%cols]
  vrp[1:3,]
  vrp <- subset(vrp, CONTRACT.DATE > vrp$COB[1])  # ?not really necessary.
  vrp <- vrp[order(vrp$DEAL.ID, vrp$CONTRACT.DATE, vrp$SIDE), ]
  vrp <- subset(vrp, PRICE!=0)   # there are some zero prices...

  MM.1 <- subset(vrp, SIDE==1)
  MM.2 <- subset(vrp[, c("DEAL.ID","CONTRACT.DATE","PRICE","SIDE", "DELTA", "GAMMA", "VEGA")],SIDE==2)
  MM <- merge(MM.1, MM.2, by=c("DEAL.ID","CONTRACT.DATE"))

  data=NULL
  data$DEAL.ID <- MM$DEAL.ID
  data$CONTRACT.DATE <- MM$CONTRACT.DATE
  data$COB <- MM$COB
  data$VAL.ID <- MM$VAL.ID
  data$NOTIONAL <- MM$NOTIONAL
  #data$EXP.DATE <- MM$EXP.DATE
  data$PAYMENT.DATE <- MM$PAYMENT.DATE
  data$F1  <- MM$PRICE.x
  data$F2  <- MM$PRICE.y * (1- MM$FUEL2)
  data$V1  <- MM$VOL
  data$V2  <- MM$VOL
  data$IR  <- MM$IR
  data$Rho <- MM$CORR
  data$K   <- MM$STRIKE 
  data$Tx  <- as.numeric(MM$EXP.DATE-MM$COB)/365
  data$DF  <- 1
  data$HR  <- 1
  data$delta1 <- MM$DELTA.x
  data$delta2 <- MM$DELTA.y
  data$gamma1 <- MM$GAMMA.x
  data$gamma2 <- MM$GAMMA.y
  data$vega1 <- MM$VEGA.x
  data$vega2 <- MM$VEGA.y
  options$method <- "kirk"
  options$what <- c(value=1, deltas=1, vegas=1, gammas=1, delta.rho=0)
  data <- as.data.frame(data)
  data$MTM <- 2*MM$MTM # rmsys value is spread on both legs
  DF <- exp(-MM$IR*as.numeric(MM$PAYMENT.DATE-MM$COB)/365)
  res <- spread.option(data, options)
  data$unit.RMG <- DF * res$value 
  data$RMG.MTM  <- data$unit.RMG * MM$NOTIONAL
  data$RMG.delta1 <- DF * res$delta1 * MM$NOTIONAL 
  data$RMG.delta2 <- DF * res$delta2 * MM$NOTIONAL  
  data$RMG.gamma1 <- DF * res$gamma1 * MM$NOTIONAL / 100 
  data$RMG.gamma2 <- DF * res$gamma2 * MM$NOTIONAL / 100  
  data$RMG.vega1 <- DF * res$vega1 * MM$NOTIONAL / 100  
  data$RMG.vega2 <- DF * res$vega2 * MM$NOTIONAL / 100 

  #---- add the ETI
  vrp <- merge(deals[,c("DEAL.ID", "DEAL.REFERENCE.CD")], data, all.y=T)
  head(vrp)  
  cat("   Done.\n"); flush.console()
  return(vrp)
}
  
