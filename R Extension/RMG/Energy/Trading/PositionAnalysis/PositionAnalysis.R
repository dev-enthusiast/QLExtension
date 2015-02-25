PositionAnalysis <- function(options) {

  load(options$AirportFile)
  data <- read.deltas.from.CPSPROD(options)
#  browser()   
  con    <- odbcConnectExcel(options$dirLimSymTable)
  sqlTables(con) 
  LimCommodCurveTable <- sqlFetch(con, "Sheet1")
  odbcCloseAll()
  data <- merge(data, LimCommodCurveTable) # add LIM Commod Curve
#  browser()
  dataDOI  <- data[which(data$CONTRACT_DATE == options$DOI), ]
  dataDOI <- subset(dataDOI, SERVICE_TYPE == "ENERGY")
  ind_x <- which(dataDOI$AUX_PEAK == ".OFFPEAK")
  if(length(ind_x) >0) {
    dataDOI$LIM_COMMOD_CURVE <- as.character(dataDOI$LIM_COMMOD_CURVE)
    for (i in 1:length(ind_x)) {
      if( dataDOI$QUANTITY_BUCKET_TYPE[ind_x[i]] == "Offpeak" )
        dataDOI$LIM_COMMOD_CURVE[ind_x[i]] <- paste(dataDOI$LIM_COMMOD_CURVE[ind_x[i]], dataDOI$AUX_PEAK[ind_x[i]], sep = "")
      if(dataDOI$TIME_INTERVAL[ind_x[i]] == "Daily")
        dataDOI$QUANTITY_BUCKET_TYPE[ind_x[i]] <- "7x24"
    }
  }           
  COMMOD_CURVE_BUCKET <- paste(dataDOI$LIM_COMMOD_CURVE, dataDOI$QUANTITY_BUCKET_TYPE, sep="_")
  dataDOI <- cbind(dataDOI,COMMOD_CURVE_BUCKET)
# options$trader <- unique(dataDOI$BOOK_OWNER)
  options$dir <- paste(options$dir, "/", options$trader, "/", format(options$DOI, "%b_%Y"), sep = "")
  dir.create(options$dir,  recursive = TRUE, showWarnings = FALSE)

  for(i in 1:length(unique(dataDOI$COMMOD_CURVE_BUCKET))) {
    ind_x <- which(dataDOI$COMMOD_CURVE_BUCKET == unique(dataDOI$COMMOD_CURVE_BUCKET)[i])
    ind_y <- unique(dataDOI$COMMOD_CURVE[ind_x])
    if(length(ind_y) > 1){
      ind_z <- which(LimCommodCurveTable$LIM_COMMOD_CURVE == dataDOI$LIM_COMMOD_CURVE[ind_x][1])
      ind_1 <- which(as.character(dataDOI$MAPPED_REGION_NAME[ind_x]) == 
                     as.character(LimCommodCurveTable$MAPPED_REGION_NAME[ind_z[1]]))  
      ind_2 <- which(as.character(dataDOI$MARKET_SYMBOL[ind_x]) == 
                     as.character(LimCommodCurveTable$MARKET_SYMBOL[ind_z[1]]))  
      ind_3 <- which(as.character(dataDOI$MAPPED_DELIVERY_POINT[ind_x]) == 
                     as.character(LimCommodCurveTable$MAPPED_DELIVERY_POINT[ind_z[1]]))  
      ind_123 <- intersect(intersect(ind_1,ind_2),ind_3) 
      dataDOI$COMMOD_CURVE[ind_x] <- dataDOI$COMMOD_CURVE[ind_x[ind_123[1]]]                
    }   
  }
#  browser()
  xxx <- ReadDataCurveTable(dataDOI,options)
  dataCurveTable <- xxx[[1]]
  deltaValue     <- xxx[[2]]

  indStartDate <- which(as.Date(hdata$Date) == options$startDate)
  indEndDate   <- which(as.Date(hdata$Date) == options$endDate)
  dataCurveTable <- data.frame(dataCurveTable, Tavg = hdata$Tavg[indStartDate:indEndDate])
  options$curve.names <- as.character(unique(dataDOI$COMMOD_CURVE))
  
  futurePriceTable <- read.prices.from.CPSPROD(options)
  CCB <- array(NA, length(unique(futurePriceTable$COMMOD_CURVE)))
  for (i in 1:length(unique(futurePriceTable$COMMOD_CURVE))) {
    ind_x <- which(unique(futurePriceTable$COMMOD_CURVE)[i] == futurePriceTable$COMMOD_CURVE)
    ind_y <- grep(as.character(unique(futurePriceTable$COMMOD_CURVE)[i]), 
                  as.character(dataDOI$COMMOD_CURVE), ignore.case = TRUE)
    CCB[ind_x] <- as.character(dataDOI$COMMOD_CURVE_BUCKET[ind_y[1]])
  }
  futurePriceTable <- data.frame(futurePriceTable, COMMOD_CURVE_BUCKET = CCB)

  plotPositions(dataCurveTable, dataDOI, deltaValue, futurePriceTable, options)
}




