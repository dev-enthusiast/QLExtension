DOI <-function(data, dataCurveTable, futurePriceTable, options) {
#  browser()
  indDOI <- which(data$CONTRACT_DATE == options$DOI)
  dataDOI <- data[indDOI,]
  UCCBDOI <- unique(dataDOI$COMMOD_CURVE_BUCKET)
  indDiff <- which(colnames(dataCurveTable) %in% setdiff(colnames(dataCurveTable), UCCBDOI))
  if( length(indDiff) > 2 ) {
    dataCurveTableDOI <- dataCurveTable[,-indDiff[2:(length(indDiff)-1)]]
  } else {
    dataCurveTableDOI <- dataCurveTable
  }

  indDiff <- which(futurePriceTable$COMMOD_CURVE_BUCKET %in% setdiff(futurePriceTable$COMMOD_CURVE_BUCKET, UCCBDOI))
  if( length(indDiff) > 0 ) {
    futurePriceTableDOI <- futurePriceTable[-indDiff,]
  } else {
    futurePriceTableDOI <- futurePriceTable
  }

  deltaValueDOI     <- array(0, dim = length(UCCBDOI))
  for (i in 1:length(UCCBDOI))
    deltaValueDOI[i] <- sum(dataDOI$DELTA_VALUE[which(dataDOI$COMMOD_CURVE_BUCKET == UCCBDOI[i])])

  return(list(dataCurveTableDOI, dataDOI, deltaValueDOI, futurePriceTableDOI, c("Date",as.character(UCCBDOI))))
}