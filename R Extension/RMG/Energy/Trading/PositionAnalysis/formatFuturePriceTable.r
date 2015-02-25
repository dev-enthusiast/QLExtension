formatFuturePriceTable <- function(futurePriceTable,dataCurveTable) {
  nCol <- length(colnames(dataCurveTable))-1
  pricingDate <- sort(unique(futurePriceTable$PRICING_DATE), index = TRUE)$x
  futurePriceTableF <- data.frame(Date = pricingDate, array(NA, dim = c(length(pricingDate), nCol)))
  colnames(futurePriceTableF) <- colnames(dataCurveTable)[1:length(colnames(futurePriceTableF))]
  for (i in 1:length(pricingDate)) {
    indDate <- which(futurePriceTable$PRICING_DATE == pricingDate[i])
    for(j in 1:length(indDate)) {
      indCol <- which(colnames(futurePriceTableF) == futurePriceTable$COMMOD_CURVE_BUCKET[indDate[j]])
      if(length(indCol) == 1)
        futurePriceTableF[i,indCol] <- futurePriceTable$PRICE[indDate[j]]
    }
  }

  ind7x8  <- grep("7x8",   colnames(futurePriceTableF))
  if(length(ind7x8) > 0) {
    for (i in 1:length(ind7x8)) {
      coreSTR <- strsplit(colnames(dataCurveTable)[ind7x8[i]], split = "7x8")
      ind2x16H <- which(colnames(dataCurveTable) == paste(coreSTR, "2x16H", sep = ""))
      if(length(ind2x16H) == 1) {
        offPeakCol <- dataCurveTable[,ind7x8[i]]
        indValid  <- which(is.na(dataCurveTable[,ind2x16H]) == FALSE)
        offPeakCol[indValid] <- (8*offPeakCol[indValid] + 16*dataCurveTable[indValid,ind2x16H])/24
        dataCurveTable <- cbind(dataCurveTable, offPeakCol)
        colnames(dataCurveTable)[length(colnames(dataCurveTable))] <- paste(coreSTR,"offpeak", sep = "")
      }
    }
  }
  return(futurePriceTableF)
}