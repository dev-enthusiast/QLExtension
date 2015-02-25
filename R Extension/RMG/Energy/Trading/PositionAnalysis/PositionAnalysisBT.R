PositionAnalysisBT <- function(options) {

  options$dir <- paste(options$dirRoot, "/", "RatioBT", format(options$DOI, "%b%Y"), "_asof_", 
                     format(options$endDate, "%b%d%Y"), sep = "")
  dir.create(options$dir,  recursive = TRUE, showWarnings = FALSE)
  file = paste(options$dirRoot,"/", "RatioAnalysis_", as.character(options$pricing.dt.end), ".csv", sep = "")
  ratioArray <- read.csv( file )
  ind5STR <- which(ratioArray$MonthlyRanking == 1 | ratioArray$TotalRanking <= length(unique(ratioArray$FutureDate)))
  ratioArrayBest <- ratioArray[ind5STR,]
  ratioArrayBest <- data.frame(ratioArrayBest, relativeSpotRatio = array(NA, dim = (length(ind5STR))), 
                                               minRatio  = array(NA, dim = (length(ind5STR))),
                                               dateMin   = array(NA, dim = (length(ind5STR))),
                                               maxRatio  = array(NA, dim = (length(ind5STR))),
                                               dateMax   = array(NA, dim = (length(ind5STR))))      
  options$asOfDate0         <- options$asOfDate        
  options$asOfDate          <- as.Date("2006-08-31") # data to read positions (can not be future/weekend)
  options$endDate           <- as.Date("2006-08-31") # last data point to read historic data
  options$startDate         <- as.Date("2006-01-01") # first data point to read historic data

  options$dirX <- paste(options$dirRoot, "/", "RatioBT_", format(options$asOfDate0, "%b%Y"), "_to_", 
                     format(options$asOfDate, "%b%d%Y"), sep = "")
  dir.create(options$dirX,  recursive = TRUE, showWarnings = FALSE)

  data <- read.deltas.from.CPSPROD(options)   
  con    <- odbcConnectExcel(options$dirLimSymTable)
  sqlTables(con) 
  LimCommodCurveTable <- sqlFetch(con, "Sheet1")
  odbcCloseAll()    
  data <- dataAUX(data, LimCommodCurveTable)
  dataCurveTable <- ReadDataCurveTableB(data,options)
  dateYYYY_mm <- as.Date(paste(format(dataCurveTable$Date, "%Y"), format(dataCurveTable$Date, "%m"), "01", sep ="-"))
  aux <- data.frame(dateYYYYmm = dateYYYY_mm, dataCurveTable)
  dataCurveTableMonthlyAverage <- data.frame(Date = unique(dateYYYY_mm))
  for (i in 3:length(aux))
    dataCurveTableMonthlyAverage <- cbind(dataCurveTableMonthlyAverage,
                                          tapply(aux[,i], aux$dateYYYYmm, mean, na.rm = TRUE))
  colnames(dataCurveTableMonthlyAverage) <- colnames(dataCurveTable)
  for (i in 1:length(ind5STR)) {
    indX <- which(as.character(dataCurveTableMonthlyAverage$Date) == as.character(ratioArrayBest$FutureDate[i])) 
    if( length(indX) == 1) {
      ind1 <- which(colnames(dataCurveTableMonthlyAverage) == ratioArrayBest$Commodity1[i])
      ind2 <- which(colnames(dataCurveTableMonthlyAverage) == ratioArrayBest$Commodity2[i])
      if(length(ind1) == 1 & length(ind2) == 1)
        ratioArrayBest$relativeSpotRatio[i] <- 
          100*(dataCurveTableMonthlyAverage[indX,ind1]/dataCurveTableMonthlyAverage[indX,ind2] -
           ratioArrayBest$FutureRatio[i])/ratioArrayBest$FutureRatio[i]  
    }
  }  
  indFutureDate <- which(as.Date(unique(ratioArrayBest$FutureDate)) > options$startDate)
  uniqueFutureDate <- as.Date(unique(ratioArrayBest$FutureDate))[indFutureDate]
  ratioArrayX <- NULL
  for (i in 1:length(indFutureDate)) {
    options$contract.dt.start <- uniqueFutureDate[i] 
    options$contract.dt.end   <- options$contract.dt.start
    options$pricing.dt.start  <- options$startDate
    options$pricing.dt.end    <- min(uniqueFutureDate[i]-1,options$asOfDate)
    indX  <- which(as.character(ratioArrayBest$FutureDate) == as.character(uniqueFutureDate[i]))    
    CCB <- c(as.character(ratioArrayBest$Commodity1[indX]), as.character(ratioArrayBest$Commodity2[indX]))
    options$curve.names <- NULL
    for (k in 1:length(CCB)) {          
      if(length(grep("offpeak", CCB[k])) > 0 ) {
        coreSTR <- strsplit(CCB[k], split = "offpeak")
        indCCB <- which(as.character(data$COMMOD_CURVE_BUCKET) == paste(coreSTR,"7x8",sep = "") |
                        as.character(data$COMMOD_CURVE_BUCKET) == paste(coreSTR,"2x16H",sep = ""))
        options$curve.names <- c(options$curve.names, as.character(unique(data$COMMOD_CURVE[indCCB])))
        
      } else {
        indCCB <- which(as.character(data$COMMOD_CURVE_BUCKET) == CCB[k])
        options$curve.names <- c(options$curve.names, as.character(unique(data$COMMOD_CURVE[indCCB])))
      }  
    }  
    futurePriceTable <- read.prices.from.CPSPROD(options)
    futurePriceTableX <- futurePriceTableUpdate(futurePriceTable,data)    
    futurePriceTableF <- formatFuturePriceTable(futurePriceTableX,dataCurveTable)
    for( j in 1:length(indX)) {
      ind1 <- which(colnames(futurePriceTableF) == ratioArrayBest$Commodity1[indX[j]])
      ind2 <- which(colnames(futurePriceTableF) == ratioArrayBest$Commodity2[indX[j]])
      ratio12 <- futurePriceTableF[,ind1]/futurePriceTableF[,ind2]
      ratioArrayXX <- data.frame(ratio = ratio12, Date = futurePriceTableF$Date,  Type = "FUTURES",  
                          COMMODIITIES = paste(ratioArrayBest$Commodity1[indX[j]], "/", 
                                               ratioArrayBest$Commodity2[indX[j]], " ", 
                                               as.character(format(options$contract.dt.end, "%b-%d-%Y")), sep= "")) 
      ratioArrayX <- rbind(ratioArrayX,ratioArrayXX)
      if(is.na(ratioArrayBest$relativeSpotRatio[indX[j]])==  FALSE) {
        ratioX <- ratioArrayBest$FutureRatio[indX[j]]*(1+ratioArrayBest$relativeSpotRatio[indX[j]]/100)
        ratioArrayXXX <- data.frame(ratio = ratioX, Date = ratioArrayBest$FutureDate[indX[j]], Type = "SPOT",  
                             COMMODIITIES = paste(ratioArrayBest$Commodity1[indX[j]], "/", 
                                                  ratioArrayBest$Commodity2[indX[j]], " ", 
                                                  as.character(format(options$contract.dt.end, "%b-%d-%Y")), sep= ""))                                                                                                   
        ratioArrayX <- rbind(ratioArrayX,ratioArrayXXX)     
      }
      ratioArrayXXXY <- data.frame(ratio = ratioArrayBest$FutureRatio[indX[j]], Date = options$asOfDate0, Type = "SIGNAL",  
                            COMMODIITIES = paste(ratioArrayBest$Commodity1[indX[j]], "/", 
                                                 ratioArrayBest$Commodity2[indX[j]], " ", 
                                                 as.character(format(options$contract.dt.end, "%b-%d-%Y")), sep= ""))
      ratioArrayX <- rbind(ratioArrayX,ratioArrayXXXY)                                                          
      dRatio12 <- 100*(ratio12-ratioArrayBest$FutureRatio[indX[j]])/ratioArrayBest$FutureRatio[indX[j]]
      indMax <- which.max(dRatio12)
      indMin <- which.min(dRatio12)
      ratioArrayBest$minRatio[indX[j]] <- dRatio12[indMin]
      ratioArrayBest$maxRatio[indX[j]] <- dRatio12[indMax]
      ratioArrayBest$dateMin[indX[j]]  <- as.character(futurePriceTableF$Date[indMin])
      ratioArrayBest$dateMax[indX[j]]  <- as.character(futurePriceTableF$Date[indMax])      
    }
  }
  
  write.table(ratioArrayBest, 
  file = paste(options$dirX,"/", "BackTestingRatioAnalysisFrom", as.character(options$asOfDate0),"To",
  as.character(options$asOfDate), ".csv", sep = ""), append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  
  uniqueRatios <- unique(ratioArrayX$COMMODIITIES)
  nPlot <- ceiling(length(uniqueRatios)/3)
  for (i in 1:nPlot) {
    indUniqueRatios <- c((3*(i-1)+1):min(3*i,length(uniqueRatios)))
    indXXX <- NULL
    for (j in 1:length(indUniqueRatios))    
      indXXX <- c(indXXX, which(as.character(ratioArrayX$COMMODIITIES) == as.character(uniqueRatios[indUniqueRatios[j]])))
    ratioArrayXXXX <- ratioArrayX[indXXX,]
    fName = paste(options$dirX,"/", "BackTestingRatioAnalysisFrom", as.character(options$asOfDate0),"To",
    as.character(options$asOfDate), "Fig", as.character(i), ".pdf", sep = "")
    pdf(fName, width=8.0, heigh=11.0)
    print(xyplot(ratio ~ Date| COMMODIITIES, data = ratioArrayXXXX, groups = Type, lwd = 3, type = "b", scales = "free", 
            layout = c(1,length(indUniqueRatios)), auto.key = list(x = .8, y = .8,  space = "right"), 
            main = paste("Back Testing of Ratios from", format(options$asOfDate0, "%b-%d-%Y"))))
    dev.off()
  }
  
  save.image()
  upTH  <- 10
  lowTH <- -10
  
  PLTable <- NULL
  for (i in 1:length(uniqueRatios)) {
    indXXX <- which(as.character(ratioArrayX$COMMODIITIES) == as.character(uniqueRatios[i]))
    ratioArrayXXXX <- ratioArrayX[indXXX,] 
    indDate   <- which(ratioArrayXXXX$Type == "FUTURES" & is.na(ratioArrayXXXX$ratio) == FALSE)
    indSpot   <- which(ratioArrayXXXX$Type == "SPOT" & is.na(ratioArrayXXXX$ratio) == FALSE)
    indSignal <- which(ratioArrayXXXX$Type == "SIGNAL" & is.na(ratioArrayXXXX$ratio) == FALSE)
    EXEC = FALSE
    for (j in 1:length(indDate)) {
      ratioX <- 100*(ratioArrayXXXX$ratio[indDate[j]] -  ratioArrayXXXX$ratio[indSignal[1]])/
                    ratioArrayXXXX$ratio[indSignal[1]]
      if(is.na(ratioX) == FALSE) {               
        if(EXEC == FALSE & (ratioX < lowTH | ratioX > upTH)) {
          EXEC = TRUE
          PLTableX <- data.frame(COMMODIITIES = uniqueRatios[i], date = ratioArrayXXXX$Date[indDate[j]], 
                                 ratio = ratioX, status = "FUTURE_EXEC") 
          PLTable <- rbind(PLTable, PLTableX)                       
        }
      }  
    } 
    if(EXEC == FALSE & length(indSpot) == 1) {
      EXEC = TRUE
      ratioX <- 100*(ratioArrayXXXX$ratio[indSpot[1]] -  ratioArrayXXXX$ratio[indSignal[1]])/
                    ratioArrayXXXX$ratio[indSignal[1]]
      PLTableX <- data.frame(COMMODIITIES = uniqueRatios[i], date = ratioArrayXXXX$Date[indSpot[1]], 
                                 ratio = ratioX, status = "SPOT_EXEC")
      PLTable <- rbind(PLTable, PLTableX)                                                    
    }
    if(EXEC == FALSE) {
      ratioX <- 100*(ratioArrayXXXX$ratio[indDate[length(indDate)]] -  ratioArrayXXXX$ratio[indSignal[1]])/
                    ratioArrayXXXX$ratio[indSignal[1]]
      PLTableX <- data.frame(COMMODIITIES = uniqueRatios[i], date = ratioArrayXXXX$Date[indDate[length(indDate)]], 
                                 ratio = ratioX, status = "NO_EXEC") 
      PLTable <- rbind(PLTable, PLTableX)
    }                          
  }
  
    
  browser()
}





