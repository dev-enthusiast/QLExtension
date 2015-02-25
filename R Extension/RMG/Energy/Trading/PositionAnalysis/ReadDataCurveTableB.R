ReadDataCurveTableB <- function(data, options) {
#  browser()
  days <- seq(options$startDate, options$endDate, by = 1)
  uniqueCCB <- unique(data$COMMOD_CURVE_BUCKET)
#  deltaValue <- array(0, dim = length(uniqueCCB))
  dataCurveTable <- NULL
  dataCurveTable <- data.frame(Date = days, XXX = array(0, dim = length(days)))
  for (i in 1:length(uniqueCCB)) {
    idx <- grep(as.character(uniqueCCB[i]),as.character(data[,"COMMOD_CURVE_BUCKET"]))
#    deltaValue[i] <- sum(dataDOI[idx,"DELTA_VALUE"])
    query <- paste("SHOW 1:", as.character(data[idx[1],"TYPE"] ), "of front",
                               as.character(data[idx[1],"LIM_COMMOD_CURVE"]),
                               "WHEN Date is from", format(options$startDate, "%m/%d/%Y"), 
                               "to", format(options$endDate, "%m/%d/%Y"))

    if( as.character(data[idx[1],"TIME_INTERVAL"]) == "Hourly"){
#      options <- NULL
#      options$units <- "hour"
      curve <- read.LIM(query, options)
      curve <- data.frame( year  = as.numeric(format(curve[,"Date"],"%Y")),
                          month  = as.numeric(format(curve[,"Date"],"%m")),
                          day    = as.numeric(format(curve[,"Date"],"%d")),
                          hour   = as.numeric(format(curve[,"Date"],"%H")),
                          symbol = curve[,2] )
      curveBucket  <- bucket.ts(curve, as.character(data[idx[1],"QUANTITY_BUCKET_TYPE"]))
      CurveBucket <-data.frame(as.Date(paste(curveBucket$year,curveBucket$month,curveBucket$day, sep="-")),
                             curveBucket$symbol)
      colnames(CurveBucket) <- c("Date", as.character(uniqueCCB[i]))
      dataCurveTable <- merge(dataCurveTable,CurveBucket, all = TRUE)
    } else {
      curve <- read.LIM(query)
      curve[,1] <- as.Date(curve[,1])
      colnames(curve) <- c("Date", as.character(uniqueCCB[i]))
      dataCurveTable <- merge(dataCurveTable,curve, all = TRUE)
    }
  }
 
  dataCurveTable <- dataCurveTable[, - which(colnames(dataCurveTable) == "XXX")]
  for (i in 1:length(dataCurveTable[1,])) {
    ind <- which(dataCurveTable[,i] < 0) 
    if(length(ind) >0)
      dataCurveTable[ind,i] <- NA
  }    
  
  ind7x8  <- grep("7x8",   colnames(dataCurveTable))     
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
  
  if(options$plot){ 
    for (i in 1:length(uniqueCCB)) {
      windows()
      plot( dataCurveTable$Date, dataCurveTable[,i+1], type = "l", main = as.character(colnames(dataCurveTable)[i+1]), 
                 xlab = "Date", ylab = "Price ($)" )
      grid()
    }                   
  }
  
  return (dataCurveTable)
}