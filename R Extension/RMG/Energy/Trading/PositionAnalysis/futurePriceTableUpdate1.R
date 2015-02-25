futurePriceTableUpdate1 <- function(futurePriceTable,data) {
  browser()
  CCB <- array(NA, length(unique(futurePriceTable$COMMOD_CURVE)))
  for (i in 1:length(unique(futurePriceTable$COMMOD_CURVE))) {
    ind_x <- which(unique(futurePriceTable$COMMOD_CURVE)[i] == futurePriceTable$COMMOD_CURVE)
    ind_y <- grep(as.character(unique(futurePriceTable$COMMOD_CURVE)[i]),
                  as.character(data$COMMOD_CURVE), ignore.case = TRUE)
    CCB[ind_x] <- as.character(data$COMMOD_CURVE_BUCKET[ind_y[1]])
  }
  futurePriceTable <- data.frame(futurePriceTable, COMMOD_CURVE_BUCKET = CCB)

  forwardDate <- sort(unique(futurePriceTable$START_DATE))
  for(i in 1:length(forwardDate)) {
    daysM <- seq(forwardDate[i], (as.Date(paste(format((as.Date(forwardDate[i]) + 31), "%Y-%m"), "-01", sep = ""))-1), 
                 by = "day" )
    daysOfMonth <- weekdays(daysM)
    nDays <- length(daysOfMonth)
    nWeekends <- length(which(daysOfMonth == "Saturday" | daysOfMonth == "Sunday"))
    indFD <- which(futurePriceTable$START_DATE == forwardDate[i])
    futurePriceTableX <- futurePriceTable[which(futurePriceTable$START_DATE == forwardDate[i]),]    
    ind7x8  <- grep("7x8", as.character(futurePriceTableX$COMMOD_CURVE_BUCKET))
    ind2x16 <- grep("2x16H", as.character(futurePriceTableX$COMMOD_CURVE_BUCKET))
    futurePriceTableXX <- rbind(futurePriceTableX[ind7x8,], futurePriceTableX[ind2x16,])
    offPeak <- array("2x16H", length(futurePriceTableXX$PRICE))
    offPeak[1:length(ind7x8)]  <- "7x8"
    futurePriceTableXX <- cbind(futurePriceTableXX, offPeak)
    futurePriceTableXX$COMMOD_CURVE_BUCKET[ind7x8] <- subs("7x8", "offPeak", futurePriceTableXX$COMMOD_CURVE_BUCKET[ind7x8])
    levels(futurePriceTableX$offPeak) <- c(levels(futurePriceTableX$offPeak), "2x16H")
    futurePriceTableX$offPeak[ind2x16] <- "2x16H"
    
    for( j in 1:length(indFD7x8)) {
      coreSTR <- strsplit(as.character(futurePriceTable$COMMOD_CURVE_BUCKET[indFD[indFD7x8[j]]]), split = "7x8")
      indFD2x16H<- which(as.character(futurePriceTable$COMMOD_CURVE_BUCKET[indFD]) == paste(coreSTR, "2x16H", sep = "")
                      & futurePriceTable$PRICING_DATE[indFD] == futurePriceTable$PRICING_DATE[indFD[indFD7x8[j]]]) 
      if(length(indFD2x16H) == 1 ) {
        offPeakRow <- futurePriceTable[indFD[indFD7x8[j]],]
        offPeakRow$COMMOD_CURVE <- sub("7x8", "OFFPEAK", as.character(offPeakRow$COMMOD_CURVE), ignore.case = TRUE)
        offPeakRow$COMMOD_CURVE_BUCKET <- sub("7x8", "offpeak", as.character(offPeakRow$COMMOD_CURVE_BUCKET))
        offPeakRow$PRICE <- ( 8*nDays*offPeakRow$PRICE + 16*nWeekends*futurePriceTable$PRICE[indFD[indFD2x16H]])/
                            (8*nDays+16*nWeekends)
#        browser()                    
        futurePriceTable <- rbind(futurePriceTable,offPeakRow)
      }
    }
  }
  return(futurePriceTable)
}