MonthlyAverage <- function(dataCurveTable,futurePriceTable) {

  dateYYYY_mm <- as.Date(paste(format(dataCurveTable$Date, "%Y"), format(dataCurveTable$Date, "%m"), "01", sep ="-"))
  aux <- data.frame(dateYYYYmm = dateYYYY_mm, dataCurveTable)
  dataCurveTableMonthlyAverage <- data.frame(Date = unique(dateYYYY_mm))
  for (i in 3:length(aux))
    dataCurveTableMonthlyAverage <- cbind(dataCurveTableMonthlyAverage,
                                          tapply(aux[,i], aux$dateYYYYmm, mean, na.rm = TRUE))
  colnames(dataCurveTableMonthlyAverage) <- colnames(dataCurveTable)
  forwardDate <- sort(unique(futurePriceTable$START_DATE))
  dataCurveTableMonthlyAverageX <- data.frame(forwardDate, array(NA,
       dim = c(length(forwardDate), length(colnames(dataCurveTableMonthlyAverage))-1)))
  for (j in 2:(length(colnames(dataCurveTableMonthlyAverage))-1)) {
    ind_x <- which(futurePriceTable$COMMOD_CURVE_BUCKET == colnames(dataCurveTableMonthlyAverage)[j])
    dataCurveTableMonthlyAverageX[1:length(ind_x),j] <- futurePriceTable$PRICE[ind_x]
  }
  colnames(dataCurveTableMonthlyAverageX) <- colnames(dataCurveTableMonthlyAverage)
  dataCurveTableMonthlyAverage <- rbind(dataCurveTableMonthlyAverage,dataCurveTableMonthlyAverageX)
  
  return(dataCurveTableMonthlyAverage)
}