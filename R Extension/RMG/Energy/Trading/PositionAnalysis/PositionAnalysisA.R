PositionAnalysisA <- function(options) {

  data <- read.deltas.from.CPSPROD(options) # Delta for all future months
  con    <- odbcConnectExcel(options$dirLimSymTable)    # read xls file with Lim symbols
  sqlTables(con) 
  LimCommodCurveTable <- sqlFetch(con, "Sheet1")
  odbcCloseAll()  
  data <- dataAUX(data, LimCommodCurveTable) # merge Lim symbols with data structutre
  dataCurveTable <- ReadDataCurveTableB(data,options) # read prices for Lim symbols
  
  # add temperature data
  load(options$AirportFile)  
  indStartDate <- which(as.Date(hdata$Date) == options$startDate)
  indEndDate   <- which(as.Date(hdata$Date) == options$endDate)
  dataCurveTable <- data.frame(dataCurveTable, Tavg = hdata$Tavg[indStartDate:indEndDate]) # add Temperature
    
  options$curve.names <- as.character(unique(data$COMMOD_CURVE))
  if(options$ratioAnalysis) {
    futurePriceTable <- read.prices.from.CPSPROD(options)
    futurePriceTable <- futurePriceTableUpdate(futurePriceTable,data)
    RatioAnalysis(dataCurveTable, futurePriceTable, options)     
  }
  if(options$plotPositions) {
#    futurePriceTable <- read.prices.from.CPSPROD(options)
#    futurePriceTable <- futurePriceTableUpdate(futurePriceTable,data)   
#    DOIX <- DOI(data, dataCurveTable, futurePriceTable, options)
#    plotPositionsA(DOIX[[1]], DOIX[[2]], DOIX[[3]], DOIX[[4]], options)  
    options$pricing.dt.start  <- as.Date("2006-11-01") # forward prices at the initial day in the past
    options$pricing.dt.end    <- options$endDate       # forward prices at the final day in the past
    options$contract.dt.start <- options$DOIi  # forward prices future initial date range
    options$contract.dt.end   <- options$DOIf # forward prices future final date range   
    futurePriceTable <- read.prices.from.CPSPROD(options)
    futurePriceTable <- futurePriceTableUpdate(futurePriceTable,data)   
    MOI <- seq(options$contract.dt.start, options$contract.dt.end, by = "months")
    for(i in 1:length(MOI)) {
      options$DOI <- MOI[i]
      DOIX <- DOI(data, dataCurveTable, futurePriceTable, options)
      plotPositionsB(DOIX[[1]], DOIX[[2]], DOIX[[3]], DOIX[[4]], options)
    }  
  }  
}





