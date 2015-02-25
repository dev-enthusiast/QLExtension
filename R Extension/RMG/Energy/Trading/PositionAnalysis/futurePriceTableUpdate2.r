futurePriceTableUpdate2 <- function(futurePriceTable,data) {
#  browser()
  CCB <- array(NA, length(unique(futurePriceTable$COMMOD_CURVE)))
  for (i in 1:length(unique(futurePriceTable$COMMOD_CURVE))) {
    ind_x <- which(unique(futurePriceTable$COMMOD_CURVE)[i] == futurePriceTable$COMMOD_CURVE)
    ind_y <- grep(as.character(unique(futurePriceTable$COMMOD_CURVE)[i]),
                  as.character(data$COMMOD_CURVE), ignore.case = TRUE)
    CCB[ind_x] <- as.character(data$COMMOD_CURVE_BUCKET[ind_y[1]])
  }
  futurePriceTable <- data.frame(futurePriceTable, COMMOD_CURVE_BUCKET = CCB)

  return(futurePriceTable)
}