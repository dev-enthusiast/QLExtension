mainPositionReportAggregate <- function(options) {
#  browser()
  require(RODBC)
  require(reshape)
  require(lattice)
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/loadPortfolio.R")  
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/loadFutureCurves.R") 
  source("H:/user/R/RMG/Finance/Heston/FitHeston.R")   
  source("H:/user/R/RMG/Finance/Heston/qHeston.R")  
  options$portfolio <- toupper(options$portfolio)  
  options$dir <- "H:/user/R/RMG/Energy/Trading/DailyOptionReport/Data/"
  dir.create(options$dirOut,  showWarnings = FALSE)
  # load positions
  data <- loadPortfolio(options) # load positions
  # load curves
  options$contract.dt.start <- min(as.Date(data$VALUATION_MONTH))   
  options$contract.dt.end   <- max(as.Date(data$VALUATION_MONTH))  
  options$curve.names       <- as.character(unique(data$LOCATION_NM))  
  futurePrices <- loadFutureCurves(options)                              
  # merge data & futures
  colnames(futurePrices)[which(colnames(futurePrices) == "COMMOD_CURVE")] <- "LOCATION_NM"
  colnames(futurePrices)[which(colnames(futurePrices) == "START_DATE")]   <- "VALUATION_MONTH" 
  dataSubset <- subset(data, VAR_MAPPING == options$varMapping)
  locations <- unique(dataSubset$LOCATION_NM)
  valMonths <- unique(as.Date(dataSubset$VALUATION_MONTH))
  futurePricesSubset  <- subset(futurePrices, LOCATION_NM %in% locations)
  futurePricesSubset <- subset(futurePricesSubset, VALUATION_MONTH %in% valMonths)
  # retain only last nHist days 
  indNhist <- which(unique(futurePrices$PRICING_DATE) <=  options$pricingAsOfDate) 
  if( length(indNhist) < length(unique(futurePrices$PRICING_DATE)) | length(indNhist) > options$nHist) {
    pricingDates <- sort(unique(futurePricesSubset$PRICING_DATE)[indNhist], decreasing = TRUE)[1:min(length(indNhist),options$nHist)]
    futurePricesSubset <- subset(futurePricesSubset, PRICING_DATE %in% pricingDates)    
  }  
  futurePricesSubset$VALUATION_MONTH <- as.character(futurePricesSubset$VALUATION_MONTH)
  futurePricesSubset$PRICING_DATE <- as.character(futurePricesSubset$PRICING_DATE)
  dataSubset <- merge(dataSubset, futurePricesSubset)
  
  DataTable1 <- data.frame( PRICING_DATE = as.Date(dataSubset$PRICING_DATE), 
                            VALUATION_MONTH = as.Date(dataSubset$VALUATION_MONTH),
                            PRICE = dataSubset$PRICE, 
                            Instrument = dataSubset$TYPE, 
                            Strike = dataSubset$STRIKE, 
                            Quantity = dataSubset$QUANTITY, 
                            Delta = dataSubset$DELTA,
                            LOCATION_NM = dataSubset$LOCATION_NM,  
                            value = array(NA, dim = length(dataSubset$VALUATION_MONTH))) 
  indF <- which(DataTable1$Instrument == "FORWARD")
  if(length(indF) > 0) { DataTable1$value[indF] <- DataTable1$Delta[indF]*DataTable1$PRICE[indF] }
  indC <- which(DataTable1$Instrument == "CALL" | DataTable1$Instrument == "C")
  if(length(indC) > 0) { 
    DataTable1$value[indC] <- DataTable1$Quantity[indC]*pmax(DataTable1$PRICE[indC]-DataTable1$Strike[indC],0) 
  }
  indP <- which(DataTable1$Instrument == "PUT" | DataTable1$Instrument == "P")
  if(length(indP) > 0) { 
    DataTable1$value[indP] <- DataTable1$Quantity[indP]*pmax(DataTable1$Strike[indP]-DataTable1$PRICE[indP],0) 
  } 
  DataTable1 <- DataTable1[,-which(colnames(DataTable1) %in% c("Strike", "Quantity", "Delta", "Instrument"))]     
  DataTable1 <- cast(DataTable1, PRICING_DATE + VALUATION_MONTH + LOCATION_NM + PRICE  ~ ., sum )
  valMonthLocation <- tapply(DataTable1$value, list(DataTable1$VALUATION_MONTH, DataTable1$LOCATION_NM), 
                             function(x){max(x, na.rm = TRUE) - min(x, na.rm = TRUE)})
  valMonthLocation <- valMonthLocation[,which(colnames(valMonthLocation) %in% unique(DataTable1$LOCATION_NM))]
  indC <- NULL
  for(i in 1:length(colnames(valMonthLocation))) {
    indR <- which(is.na(valMonthLocation[,i]) == FALSE)
    if( max(valMonthLocation[indR, i]) == 0 ) {indC <- c(indC, i) }
  }
  if(length(indC) > 0 ) {
    DataTable1 <- DataTable1[-which(DataTable1$LOCATION_NM %in% colnames(valMonthLocation)[indC]),]
    valMonthLocation <- valMonthLocation[,-indC]
  }
#  browser()
  valMonths <- sort(unique(DataTable1$VALUATION_MONTH))
  Win07 <- which(valMonths < as.Date("2007-03-01"))
  Spr07 <- which(valMonths > as.Date("2007-02-01") & valMonths < as.Date("2007-07-01"))
  Sum07 <- which(valMonths > as.Date("2007-06-01") & valMonths < as.Date("2007-09-01"))
  Sep07 <- which(valMonths == as.Date("2007-09-01"))
  Fal07 <- which(valMonths > as.Date("2007-09-01") & valMonths <= as.Date("2007-12-01"))
  Win08 <- which(valMonths > as.Date("2007-12-01") & valMonths < as.Date("2008-03-01"))
  Rem08 <- which(valMonths > as.Date("2008-02-01") & valMonths < as.Date("2009-01-01"))
  Cal07 <- which(format(valMonths, "%Y") == "2007")
  Cal08 <- which(format(valMonths, "%Y") == "2008")
  Cal09 <- which(format(valMonths, "%Y") == "2009")
  All <- c(1:length(valMonths))
  
  Groups <- list(1,2, c(1,2,3), Win07, Spr07, Sum07, Sep07, Fal07, Win08, Rem08, Cal07, Cal08, Cal09, All)
  for (i in 1:length(Groups)) {                                                           
    classFactor <- list(timeGroup = valMonths[Groups[[i]]], locGroup = unique(DataTable1$LOCATION_NM))  
    aggregateByCF(DataTable1, classFactor, valMonthLocation, options)                                                 
  }  
  
#  # by months                                                                                                         
#  for (i in 1:length(unique(DataTable1$VALUATION_MONTH))) {                                                           
#    classFactor <- list(timeGroup = unique(DataTable1$VALUATION_MONTH)[i], locGroup = unique(DataTable1$LOCATION_NM))  
#    aggregateByCF(DataTable1, classFactor, valMonthLocation, options)                                                 
#  }                                                                                                                   
#  # by years
#  Years <- unique(format(DataTable1$VALUATION_MONTH, "%Y")) 
#  for (i in 1:length(Years)) {
#    indMonths <- which(format(DataTable1$VALUATION_MONTH, "%Y") == Years[i])
#    classFactor <- list(timeGroup = unique(DataTable1$VALUATION_MONTH[indMonths]), locGroup = unique(DataTable1$LOCATION_NM))  
#    aggregateByCF(DataTable1, classFactor, valMonthLocation, options)  
#  }
#  # total 
#  classFactor <- list(timeGroup = unique(DataTable1$VALUATION_MONTH), locGroup = unique(DataTable1$LOCATION_NM))
#  aggregateByCF(DataTable1, classFactor, valMonthLocation, options)
  return()
}

aggregateByCF <- function(DataTable, classFactor, valMonthLocation, options) {
#  browser()
  DataTable <- DataTable[which(DataTable$VALUATION_MONTH %in% classFactor$timeGroup),]
  DataTable <- DataTable[which(DataTable$LOCATION_NM %in% classFactor$locGroup),]
  valMonthLocation <- valMonthLocation[which(as.character(rownames(valMonthLocation)) %in% 
    as.character(classFactor$timeGroup)), which(colnames(valMonthLocation) %in% classFactor$locGroup)]                                       
  DataTableX <- aggregate(DataTable$value, list(PRICING_DATE = DataTable$PRICING_DATE), sum)
  DataTableX$x <- 1e-6*DataTableX$x
  
  xxx <- paste(DataTable$LOCATION_NM, DataTable$VALUATION_MONTH, sep = "_")
  PriceX     <- cbind(DataTable[,-which(colnames(DataTable) %in% c("VALUATION_MONTH", "LOCATION_NM", "value"))],xxx)
  Price <- tapply(PriceX$PRICE, list(PRICING_DATE = PriceX$PRICING_DATE, COMMOD = PriceX$xxx), I)
  PriceW <- Price
  # construct weight function
  w <- array(NA, dim = length(colnames(Price)))
  for (i in 1:length(colnames(Price))) {
    aux <- strsplit(xxx[i],"_")
    if( is.array(valMonthLocation) == FALSE) {
      w[i] <- valMonthLocation[which(names(valMonthLocation) == aux[[1]][1])]     
    } else {
      iC <- which(colnames(valMonthLocation) %in% aux[[1]][1])
      iR <- which(rownames(valMonthLocation) %in% aux[[1]][2])
      w[i] <- valMonthLocation[iR,iC]
    }  
  }
  wS <- sort(w, decreasing = TRUE, index = TRUE)
  indTop <- wS$ix[1:min(options$nWeight,length(w))]
  WW <- matrix(rep(w[indTop], times = length(Price[,1])), nrow = length(Price[,1]), 
        ncol = length(indTop), byrow = TRUE)
  PriceW <- Price[,indTop] * WW
  PriceW <- PriceW/max(w)
  z <- prcomp(na.omit(PriceW))
  x1 <- as.matrix(PriceW) %*%  z$rotation[,1] 
  # add spread from Heston Fit
#  browser()   
  vH <- FitHeston(DataTableX$x,options)
  qH <- qHeston(vH[[1]], vH[[2]], vH[[3]], options$percentile, c(1:vH[[4]]))
  quantiles <- array(0, dim <- c(vH[[4]],2))
  for(i in 1:vH[[4]]) {  
    quantiles[i,] <- quantile(sort(diff(DataTableX$x, lag = i)), 
                              probs = c(0.01*options$percentile, 1-0.01*options$percentile)) 
  }                                
  daysX <- seq(as.Date(DataTableX$PRICING_DATE)[length(x1)]+1, 
               as.Date(DataTableX$PRICING_DATE)[length(x1)] + 1 +2*options$DT, by = 1)
  indWeekend <- which(weekdays(daysX) %in% c("Saturday", "Sunday"))
  projectionDays <- daysX[-indWeekend]
  projectionDays <- projectionDays[1:vH[[4]]]  
  fileName <- paste(options$dirOut, "/PCA_", options$varMapping, "_", 
                    as.character(format(classFactor$timeGroup[1],"%b-%y")), "_", 
                    as.character(format(classFactor$timeGroup[length(classFactor$timeGroup)],"%b-%y")), "_",
                    (options$asOfDate), ".pdf", sep = "")
  pdf(fileName, width=11.0, heigh=8.0)
  nf <- layout(c(3,2,1))
  dateStr <- paste("Date (", format(min(as.Date(DataTableX$PRICING_DATE)), "%m/%d/%Y"), " -- ", 
                             format(max(as.Date(DataTableX$PRICING_DATE)), "%m/%d/%Y"), " )", sep = "")                             
  dateRange <- c(min(as.Date(DataTableX$PRICING_DATE)), projectionDays[vH[[4]]])   
  valueRange <- c(min(c(DataTableX$x, DataTableX$x[length(x1)] - abs(qH[vH[[4]]]), 
                        DataTableX$x[length(x1)] + min(quantiles))), 
                  max(c(DataTableX$x, DataTableX$x[length(x1)] + abs(qH[vH[[4]]]), 
                        DataTableX$x[length(x1)] + max(quantiles)))) 
                        
  plot(dateRange, range(na.omit(x1)), type = "n", ylab = "PC1 ($)", xlab = dateStr)                                            
  lines(as.Date(DataTableX$PRICING_DATE), x1, type = "o") 
  grid()
  plot(x1, DataTableX$x, type = "o", xlab = "PC1 ($)", ylab = "Value (M$)" )
  points(x1[1], DataTableX$x[1], pch = 23, lwd = 3, col = "blue")
  lines(x1[(length(x1)-2):length(x1)], DataTableX$x[(length(x1)-2):length(x1)], type = "o", col = "green", lwd = 3)
  points(x1[length(x1)], DataTableX$x[length(x1)], pch = 23, lwd = 3, col = "red")
  grid()                                                        
  plot(dateRange, valueRange, type = "n", xlab = dateStr, ylab = "Value (M$)" , 
    main = paste(options$varMapping, ", ", as.character(format(classFactor$timeGroup[1],"%m/%y")), " - ", 
                 as.character(format(classFactor$timeGroup[length(classFactor$timeGroup)],"%m/%y")), ", PC1 var = ", 
                 round(100*(z$sdev[1]*z$sdev[1])/sum(z$sdev*z$sdev), 2), " (%), ", 
                 length(indTop), " variables.", sep = ""))
  lines(as.Date(DataTableX$PRICING_DATE), DataTableX$x, type = "o")
  lines(c(as.Date(DataTableX$PRICING_DATE[length(x1)]),as.Date(projectionDays)), 
        c(DataTableX$x[length(x1)],(DataTableX$x[length(x1)] + qH)), type = "o", lwd = 2, col = "red" ) 
  lines(c(as.Date(DataTableX$PRICING_DATE[length(x1)]),as.Date(projectionDays)), 
        c(DataTableX$x[length(x1)],(DataTableX$x[length(x1)] - qH)), type = "o", lwd = 2, col = "red" ) 
  lines(c(as.Date(DataTableX$PRICING_DATE[length(x1)]),as.Date(projectionDays)), 
        c(DataTableX$x[length(x1)],(DataTableX$x[length(x1)] + quantiles[,1])), type = "o", lwd = 2, col = "green" ) 
  lines(c(as.Date(DataTableX$PRICING_DATE[length(x1)]),as.Date(projectionDays)), 
        c(DataTableX$x[length(x1)],(DataTableX$x[length(x1)] + quantiles[,2])), type = "o", lwd = 2, col = "green" )                                 
  grid()                                                                                     
  dev.off()  
  return()
}

  
  
