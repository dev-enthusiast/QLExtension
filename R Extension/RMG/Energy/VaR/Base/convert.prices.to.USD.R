# Convert prices to USD. 
# Where den - a data.frame with [curve.name denominated]
#       fx  - a data.frame same format as all.mkt.Prices.RData files. 
#
# Last modified by Adrian Dragulescu on 30-Mar-2007


convert.prices.to.USD <- function(IVaR, den, hFX){

  indDates.IVaR  <- grep("-",names(IVaR))
  aux <- cbind(IVaR[, c("curve.name","contract.dt","vol.type")], ind=1:nrow(IVaR))
  aux <- merge(aux, den, all.x=T)
  aux <- aux[order(aux$ind),]
  aux$denominated[which(is.na(aux$denominated))] <- "USD"
  IVaR$denominated <- paste("USD/", aux$denominated, sep="")

  FX  <- matrix(NA, nrow=nrow(IVaR), ncol=length(indDates.IVaR))
  ind <- which(names(IVaR)[indDates.IVaR] %in% names(hFX))
  FX[, ind] <- as.matrix(hFX[IVaR$denominated,])

  # fill historical FX exchange rates if necessary. 
  aux <- apply(is.na(FX), 2, all)  
  if (any(aux)){
    rLog("Missing some days in the historical FX data frame.  Filling.")
    FX[, which(aux)] <- FX[, min(which(!aux))+1]  # use the last one 
  }
    
  IVaR[, indDates.IVaR] <- as.matrix(IVaR[, indDates.IVaR])*FX

  return(IVaR)
}
