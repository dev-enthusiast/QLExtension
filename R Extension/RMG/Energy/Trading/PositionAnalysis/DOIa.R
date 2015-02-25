DOIa <-function(data, dataCurveTable, futurePriceTable, options) {
#  browser()
  indDOI <- which(data$CONTRACT_DATE == options$DOI)
  dataDOI <- data[indDOI,]
  UCCBDOI <- unique(dataDOI$COMMOD_CURVE_BUCKET)
  indX <- array(0, length(UCCBDOI))
  for(i in 1:length(indX)) {
    indX[i] <- grep(UCCBDOI[i], colnames(dataCurveTable))    
  }
  dataCurveTableDOI <- dataCurveTable[, c(1,indX)]

  indY <- which(futurePriceTable$COMMOD_CURVE_BUCKET %in% UCCBDOI)
  futurePriceTableDOI <- futurePriceTable[indY,]

  deltaValueDOI     <- array(0, dim = length(UCCBDOI))
  for (i in 1:length(UCCBDOI))
    deltaValueDOI[i] <- sum(dataDOI$DELTA_VALUE[which(dataDOI$COMMOD_CURVE_BUCKET == UCCBDOI[i])])

  return(list(dataCurveTableDOI, dataDOI, deltaValueDOI, futurePriceTableDOI))
}