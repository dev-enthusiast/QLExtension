PositionAnalysisCash <- function(options) {

  load(options$AirportFile)
  data <- read.deltas.from.CPSPROD(options) # Delta for all future months
  con    <- odbcConnectExcel(options$dirLimSymTable)    # read xls file with Lim symbols
  sqlTables(con) 
  LimCommodCurveTable <- sqlFetch(con, "Sheet1")
  odbcCloseAll()  
  data <- dataAUX(data, LimCommodCurveTable) # merge Lim symbols with data structutre
  dataCurveTable <- ReadDataCurveTableB(data,options) # read prices for Lim symbols
  
  # get Historical view of the future prices 
  options$pricing.dt.start  <- as.Date("2006-07-01")    # forward prices at the initial day in the past
  options$pricing.dt.end    <- options$asOfDate         # forward prices at the final day in the past
  options$contract.dt.start <- options$MOI          # forward prices future initial date range
  options$contract.dt.end   <- options$MOI    # forward prices future final date range  
  options$DOI <- options$MOI  
  options$curve.names <- as.character(unique(data$COMMOD_CURVE))     
  futurePriceTable <- read.prices.from.CPSPROD(options)
  futurePriceTable <- futurePriceTableUpdate(futurePriceTable,data)   
  DOIX <- DOIa(data, dataCurveTable, futurePriceTable, options)
  dataCurveTableDOI <- DOIX[[1]] 
  deltaValueDOI <- DOIX[[3]]
#  plotPositionsB(DOIX[[1]], DOIX[[2]], deltaValueDOI, DOIX[[4]], options)
  
# get Historical view going back in years on the basis of spot prices

  options$nYearBack <- 3
  options$nDays <-  as.numeric(seq(options$MOI, options$MOI+100, by = "1 month")[2] - options$MOI) - 1 
  DiffLong <- NULL
  DiffShort <- NULL
  DiffTotal <- NULL  
  for (i in 1:options$nYearBack) {     
    dateBack <- as.Date(paste(as.character(as.numeric(format(options$MOI, "%Y"))-i), "-", 
        format(options$MOI, "%m"), "-", format(options$MOI, "%d"), sep = "")) 
    dateStart <- seq(dateBack, dateBack-100, by = "-1 month")[2]
    dateEnd  <-  seq(dateBack, dateBack+100, by = "1 month")[2] 
    daySeq <- seq(dateStart, dateEnd, by = 1)
    avSpot <- NULL
    for (j in 1:length(daySeq)) {
      days <- seq(daySeq[j], daySeq[j] + options$nDays, by  =1 ) 
      aux <- subset(dataCurveTableDOI, dataCurveTableDOI$Date %in% days)
      avSpot <- rbind(avSpot, data.frame(Date = as.character(as.Date(daySeq[j])), lapply(aux[,-1], mean, na.rm = TRUE)))
    }
    
    NetAverage <- data.frame(Date =  avSpot[,1], 
            t(array(deltaValueDOI, dim = c(length(deltaValueDOI), length(avSpot[,1])))) * avSpot[,2:length(avSpot[1,])])

    indLong  <- which(deltaValueDOI > 0) + 1
    indShort <- which(deltaValueDOI < 0) + 1
    NetShort <- array(NA, dim = length(avSpot[,1]))
    NetLong  <- array(NA, dim = length(avSpot[,1]))
    NetTotal <- array(NA, dim = length(avSpot[,1]))
  
    for (i in 1:length(NetAverage[,1])) {
      NetShort[i] <- 1e-6*sum(NetAverage[i,indShort], na.rm = TRUE)
      NetLong[i]  <- 1e-6*sum(NetAverage[i,indLong], na.rm = TRUE)
      NetTotal[i] <- 1e-6*sum(NetAverage[i, c(indLong,indShort)], na.rm = TRUE)
    }
  
    DiffLong  <- c(DiffLong,  diff(log(abs(NetLong)), lag = 1))
    DiffShort <- c(DiffShort, diff(log(abs(NetShort)), lag = 1))
    DiffTotal <- c(DiffTotal, diff(log(abs(NetTotal)), lag = 1))
  
    Net <- NULL
    for (i in 1:length(NetAverage[,1])) {
      xxx1 <- data.frame(Value = NetTotal[i], Date = NetAverage[i,1], Type = "Total", Year = "2006")
      xxx2 <- data.frame(Value =  NetLong[i], Date = NetAverage[i,1], Type = "Long", Year = "2006")
      xxx3 <- data.frame(Value = NetShort[i], Date = NetAverage[i,1], Type = "Short", Year = "2006")
      Net <- rbind(Net, xxx1, xxx2, xxx3)
    }
    
    Net$Date <- as.Date(Net$Date)
    save.image()
    fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b%Y"), "_AsOf_",
                 format(options$asOfDate,"%d%b%Y"),"_to_",format(dateBack,"%b%Y"), ".pdf", sep = "")
    pdf(fName, width=8.0, heigh=11.0)
    print(xyplot(Value ~ Date| Type, data = Net, groups = Year, lwd = 3, type = "b", layout = c(1,3), 
      scales = list( x = list(tick.number = length(unique(format(Net$Date,"%b")))), y = "free"), ylab = "Value ($M)", 
      main = paste(as.character(options$trader), format(options$DOI,"%b,%Y"), "as of", format(options$asOfDate,"%m/%d/%Y"),  
                   "to", format(min(Net$Date), "%m/%d/%Y"), "-", format(max(Net$Date), "%m/%d/%Y"))))
    dev.off()  
  }
  xxx <- plotPositionsC(DOIX[[1]], DOIX[[2]], deltaValueDOI, DOIX[[4]], options)
  options$nDaysFut <- length(countBusinessDate(options$asOfDate, (options$MOI-1)))
    
  CashFutDistA(DiffLong, DiffShort, diff(log(abs(xxx[[5]])), lag = 1), diff(log(abs(xxx[[6]])), lag = 1), 
              xxx[[5]][length(xxx[[5]])], xxx[[6]][length(xxx[[6]])], options)

# write Delta              
  DeltaOut <- data.frame(c1 = colnames(dataCurveTableDOI)[2:length(colnames(dataCurveTableDOI))],
                         c2 = (deltaValueDOI)) 
  colnames(DeltaOut) <- c("Commodity", paste("Delta", format(options$MOI, "%b%Y"), "(Units)" ))                         
  write.csv(DeltaOut, file = paste(options$dir,"/", options$trader, "_Delta_", 
        format(options$DOI,"%b%Y"), "_AsOf_", format(options$asOfDate,"%d%b%Y"), ".csv", sep = ""),
        row.names = FALSE)                                  
}





