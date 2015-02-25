plotPositions <- function(dataCurveTable, dataDOI, deltaValue, futurePriceTable, options) {
# browser()
#-----------------------------------------------------------------------------------
# Trellis Plot: daily historical prices for month of interest, classified by buckets
#-----------------------------------------------------------------------------------
#  trader <- unique(dataDOI$BOOK_OWNER)
  years <- unique(format(dataCurveTable$Date, "%Y"))
  buckets <- unique( dataDOI$QUANTITY_BUCKET_TYPE)
  indMOIall <- which(format(dataCurveTable$Date, "%m") == format(options$DOI,"%m"))
  for(i in 1:length(buckets)) {
    indB <- grep(as.character(buckets[i]), colnames(dataCurveTable))    
    dataCurveTableTemp <- dataCurveTable[indMOIall,indB]
    if(length(indB) > 1) {
      colnames(dataCurveTableTemp) <- strsplit(colnames(dataCurveTableTemp), paste("_",buckets[i],sep = ""))
      xyPlotData <- stack(dataCurveTableTemp)
      xyPlotData <- cbind(xyPlotData, rep(dataCurveTable$Tavg[indMOIall], length(indB)),
                          rep(format(dataCurveTable$Date[indMOIall],"%Y"), length(indB)))
      colnames(xyPlotData) <- c("Price", "CommodName", "Tavg", "Year")
    } else {
      xyPlotData <- data.frame( Price = dataCurveTableTemp, 
                                CommodName = as.factor(rep(colnames(dataCurveTable)[indB], length(dataCurveTableTemp))), 
                                Tavg = dataCurveTable$Tavg[indMOIall], 
                                Year = format(dataCurveTable$Date[indMOIall],"%Y"))    
    }
    fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b_%Y"), "_", buckets[i], ".pdf", sep = "")
    pdf(fName, width=11.0, heigh=8.0)
    print(xyplot(Price ~ Tavg| CommodName, data = xyPlotData, groups = Year, lwd = 5, scales = "free", 
    xlab = "T (F)", ylab = "Price ($)",
    main = paste("Historical Prices for Bucket: ", buckets[i], ", Month of", format(options$DOI,"%b")),
    auto.key = list(x = .8, y = .7, corner = c(0,0))))
    dev.off()
  } 
#-----------------------------------------------------------------------------------
# Plot: Position Historical View (Short, Long, Total)
#-----------------------------------------------------------------------------------
#  browser()
  dateYYYY_mm <- as.Date(paste(format(dataCurveTable$Date, "%Y"), format(dataCurveTable$Date, "%m"), "01", sep ="-"))
  aux <- data.frame(dateYYYYmm = dateYYYY_mm, dataCurveTable)
  dataCurveTableMonthlyAverage <- data.frame(Date = unique(dateYYYY_mm))
  for (i in 3:length(aux))
    dataCurveTableMonthlyAverage <- cbind(dataCurveTableMonthlyAverage,
                                          tapply(aux[,i], aux$dateYYYYmm, mean, na.rm = TRUE))                                         
  colnames(dataCurveTableMonthlyAverage) <- colnames(dataCurveTable) 
  forwardDate <- sort(unique(futurePriceTable$START_DATE), index = TRUE)$x
  dataCurveTableMonthlyAverageX <- data.frame(forwardDate, array(NA, 
       dim = c(length(forwardDate), length(colnames(dataCurveTableMonthlyAverage))-1)))
  for (j in 2:(length(colnames(dataCurveTableMonthlyAverage))-1)) {
    ind_x <- which(futurePriceTable$COMMOD_CURVE_BUCKET == colnames(dataCurveTableMonthlyAverage)[j])
    dataCurveTableMonthlyAverageX[1:length(ind_x),j] <- futurePriceTable$PRICE[ind_x]
  }
  colnames(dataCurveTableMonthlyAverageX) <- colnames(dataCurveTableMonthlyAverage)
  dataCurveTableMonthlyAverage <- rbind(dataCurveTableMonthlyAverage,dataCurveTableMonthlyAverageX)  
  NetMonthlyAverage <- data.frame(Date = dataCurveTableMonthlyAverage[,1],
        t(array(deltaValue, dim = c(length(deltaValue), length(dataCurveTableMonthlyAverage[,1])))) *
        dataCurveTableMonthlyAverage[,2:(length(dataCurveTableMonthlyAverage[1,])-1)])
  indLong  <- which(deltaValue > 0) + 1
  indShort <- which(deltaValue < 0) + 1
  NetMonthlyShort <- array(0, dim = length(NetMonthlyAverage[,1]))
  NetMonthlyLong  <- array(0, dim = length(NetMonthlyAverage[,1]))
  NetMonthlyTotal      <- array(0, dim = length(NetMonthlyAverage[,1]))
  for (i in 1:length(NetMonthlyAverage[,1])) {
    NetMonthlyShort[i] <- 1e-6*sum(NetMonthlyAverage[i,indShort], na.rm = TRUE)
    NetMonthlyLong[i]  <- 1e-6*sum(NetMonthlyAverage[i,indLong], na.rm = TRUE)
    NetMonthlyTotal[i] <- 1e-6*sum(NetMonthlyAverage[i,2:length(NetMonthlyAverage[1,])], na.rm = TRUE)
  }
  NetMonthly <- NULL
  for (i in 1:length(NetMonthlyAverage[,1])) {
    yearX <- format(NetMonthlyAverage[i,1], "%Y")
    if( NetMonthlyAverage[i,1] > options$pricing.dt.end )
      yearX <- paste(yearX,"_F",sep = "")
    xxx1 <- data.frame(Value = NetMonthlyTotal[i], Date = format(NetMonthlyAverage[i,1], "%b"),
                       Type = "Total", Year = yearX)
    xxx2 <- data.frame(Value =  NetMonthlyLong[i], Date = format(NetMonthlyAverage[i,1], "%b"),
                       Type = "Long", Year = yearX)
    xxx3 <- data.frame(Value = NetMonthlyShort[i], Date = format(NetMonthlyAverage[i,1], "%b"),
                       Type = "Short", Year = yearX)
    NetMonthly <- rbind(NetMonthly, xxx1, xxx2, xxx3)
  }
  fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b_%Y"), "_HistView.pdf", sep = "")
  pdf(fName, width=8.0, heigh=11.0)
  print(xyplot(Value ~ Date| Type, data = NetMonthly, groups = Year, lwd = 3, type = "o", layout = c(1,3), scales = "free",
    main = paste("Historical View of", as.character(options$trader), "Positions for Month of", format(options$DOI,"%b,%Y")),
    auto.key = list(x = .8, y = .8,  space = "right")))
   dev.off()
#-----------------------------------------------------------------------------------
# Plot: T-dependence of the Net(Gain/Loss)
#-----------------------------------------------------------------------------------
#  browser()
  idxMOI <- which(format(NetMonthlyAverage$Date, "%m") == format(options$DOI,"%m"))
  indMS  <- which(dataDOI$MARKET_SYMBOL == options$MARKET_SYMBOL)
  LCCMS  <- unique(dataDOI$COMMOD_CURVE_BUCKET[indMS])
  indLCCMS <- array(0, length(LCCMS))
  for(i in 1:length(LCCMS))
    indLCCMS[i] <- which(colnames(NetMonthlyAverage) == LCCMS[i])
  indLong  <- intersect(indLong,indLCCMS)
  indShort <- intersect(indShort,indLCCMS)
  for (i in 1:length(NetMonthlyAverage[,1])) {
    NetMonthlyShort[i] <- 1e-6*sum(NetMonthlyAverage[i,indShort], na.rm = TRUE)
    NetMonthlyLong[i]  <- 1e-6*sum(NetMonthlyAverage[i,indLong], na.rm = TRUE)
    NetMonthlyTotal[i] <- NetMonthlyShort[i] + NetMonthlyLong[i]
  }
  NetLongMOI  <- 100*(NetMonthlyLong[idxMOI]-NetMonthlyLong[idxMOI-1])/abs(NetMonthlyLong[idxMOI-1])
  NetShortMOI <- 100*(NetMonthlyShort[idxMOI]-NetMonthlyShort[idxMOI-1])/abs(NetMonthlyShort[idxMOI-1])
  NetTotalMOI <- 100*(NetMonthlyTotal[idxMOI]-NetMonthlyTotal[idxMOI-1])/abs(NetMonthlyTotal[idxMOI-1])
  dTempMOI    <- dataCurveTableMonthlyAverage$Tavg[idxMOI] - dataCurveTableMonthlyAverage$Tavg[idxMOI-1]
  TempMOI    <- dataCurveTableMonthlyAverage$Tavg[idxMOI]
  indHist <- which(is.na(dTempMOI) == FALSE) 
  fName <- paste(options$dir, "/", options$trader, "_", format(options$DOI,"%b_%Y"), "_HistNetDeltaT.pdf", sep = "")
  pdf(fName, width=11.0, heigh=8.0)
  plot( range(dTempMOI[indHist]), range(c(NetShortMOI[indHist], NetLongMOI[indHist], NetTotalMOI[indHist])), 
    type = "n", ylab = "Net (%)",
    xlab = paste("<T_", format(options$DOI,"%b"), "> - <T_",
                 format(NetMonthlyAverage$Date[idxMOI[1]-1],"%b"), "> (F)", sep = ""),
    main = paste(as.character(options$trader), " Historical Gain/Loss, ", options$MARKET_SYMBOL, ", ",
                 format(options$DOI,"%b"), " vs: <T_", format(options$DOI,"%b"), "> - <T_",
                 format(NetMonthlyAverage$Date[idxMOI[1]-1],"%b"), "> (F)", sep = ""))
  points(dTempMOI[indHist], NetLongMOI[indHist],   pch = 1, lwd = 3, col = "green")
  points(dTempMOI[indHist], NetShortMOI[indHist], pch = 2, lwd = 3, col = "red")
  points(dTempMOI[indHist], NetTotalMOI[indHist], pch = 3, lwd = 3, col = "black")
  legend("topleft", c("Total","Long","Short"), col = c("black","green","red"), pt.lwd = 3, pch = c(3,1,2))
  text(dTempMOI, NetTotalMOI-1, years, lwd = 3)
  grid()
  dev.off()  
  fName <- paste(options$dir, "/", options$trader, "_", format(options$DOI,"%b_%Y"), "_HistNetT.pdf", sep = "")
  pdf(fName, width=11.0, heigh=8.0)
  plot( range(TempMOI[indHist]), range(c(NetShortMOI[indHist], NetLongMOI[indHist], NetTotalMOI[indHist])), 
    type = "n", ylab = "Net (%)",
    xlab = paste("<T_", format(options$DOI,"%b"), ">  (F)", sep = ""),
    main = paste(as.character(options$trader), " Historical Gain/Loss, ", options$MARKET_SYMBOL, ", ",
                 format(options$DOI,"%b"), " vs: <T_", format(options$DOI,"%b"), "> (F)", sep = ""))
  points(TempMOI[indHist], NetLongMOI[indHist],   pch = 1, lwd = 3, col = "green")
  points(TempMOI[indHist], NetShortMOI[indHist], pch = 2, lwd = 3, col = "red")
  points(TempMOI[indHist], NetTotalMOI[indHist], pch = 3, lwd = 3, col = "black")
  legend("topleft", c("Total","Long","Short"), col = c("black","green","red"), pt.lwd = 3, pch = c(3,1,2))
  text(TempMOI, NetTotalMOI-1, years, lwd = 3)
  grid()
  dev.off()
#-----------------------------------------------------------------------------------
# Plot: Ratio of Prices for nB (nB = 3) Largest Long & Short Positions
#-----------------------------------------------------------------------------------
  nB <- 3
  indSort <- sort(deltaValue, index.return = TRUE)
  indShortB <- indSort$ix[1:nB] + 1
  indLongB <- indSort$ix[length(indSort$ix):(length(indSort$ix)-nB + 1)] + 1
  RatioTable <- NULL
  for (iL in 1:nB) {
    for (iS in 1:nB) {
      for (i in 1:length(dataCurveTableMonthlyAverage[,1])) {
        CurrentRatio <- NA
        if (is.numeric(dataCurveTableMonthlyAverage[i,indLongB[iL]]) == TRUE &
            is.numeric(dataCurveTableMonthlyAverage[i,indShortB[iS]]) == TRUE)
            CurrentRatio <- dataCurveTableMonthlyAverage[i,indLongB[iL]]/dataCurveTableMonthlyAverage[i,indShortB[iS]]
        yearX <- format(dataCurveTableMonthlyAverage[i,1], "%Y")
        if( dataCurveTableMonthlyAverage[i,1] > options$pricing.dt.end )
          yearX <- paste(yearX,"_F",sep = "")            
        RatioX <- data.frame(Ratio = CurrentRatio,
                             Type = paste("Long_", iL, "/", "Short_", iS , sep = ""),
                             Date = format(dataCurveTableMonthlyAverage[i,1], "%b"),
                             Year = yearX)
        RatioTable  <- rbind(RatioTable, RatioX)
      }
    }
  }
  fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b_%Y"), "_Ratios.pdf", sep = "")
  pdf(fName, width=11.0, heigh=8.0)
  print(xyplot(Ratio ~ Date| Type, data = RatioTable, groups = Year, type = "o", lwd = 2, scales = "free", layout = c(nB,nB),
    main = paste("Long: ", colnames(dataCurveTableMonthlyAverage)[indLongB[1]], ", ",
                         colnames(dataCurveTableMonthlyAverage)[indLongB[2]], ", ",
                         colnames(dataCurveTableMonthlyAverage)[indLongB[3]], sep = "") ,
    sub = paste("Short: ",colnames(dataCurveTableMonthlyAverage)[indShortB[1]], ", ",
                         colnames(dataCurveTableMonthlyAverage)[indShortB[2]], ", ",
                         colnames(dataCurveTableMonthlyAverage)[indShortB[3]], sep = ""),
  auto.key = list(x = .8, y = .7, space = "right")))
  dev.off()
#  browser()
#-----------------------------------------------------------------------------------
# Recomendations
#-----------------------------------------------------------------------------------  
}
