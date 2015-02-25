mainPositionReport <- function(options) {
  require(RODBC)
  require(reshape)
  require(lattice)
  source("H:/user/R/RMG/Energy/Trading/PositionReport/loadPortfolio.R")  
  source("H:/user/R/RMG/Energy/Trading/PositionReport/loadFutureCurves.R")  
  options$portfolio <- toupper(options$portfolio)  
  options$dir <- "S:/Risk/Portfolio Management/OptionData"
  dir.create(options$dirOut,  showWarnings = FALSE)
  # load positions
  data <- loadPortfolio(options) # load positions
  # load curves
  options$contract.dt.start <- min(as.Date(data$VALUATION_MONTH))   
  options$contract.dt.end   <- max(as.Date(data$VALUATION_MONTH))  
  options$curve.names       <- as.character(unique(data$LOCATION_NM))  
  futurePrices <- loadFutureCurves(options)  
  browser()                            
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
  
  Years <- unique(format(as.Date(sort(dataSubset$VALUATION_MONTH)),"%y"))
  nYears <- length(Years)
  Months <- c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")
  mmm.yy <- NULL
  for(y in 1:nYears) { mmm.yy <- c(mmm.yy, paste(Months, "-", Years[y], sep = "")) } 
  
  for(i in 1:length(locations)) {  
    dataSubsetX <- subset(dataSubset, LOCATION_NM == locations[i])   
    typeX <- array("Hist", dim = length(dataSubsetX$VALUATION_MONTH))
    indAsOfDate <- which(dataSubsetX$PRICING_DATE == as.character(options$asOfDate))
    typeX[indAsOfDate] <- "Real"
    PlotTable <- data.frame(Date = as.Date(dataSubsetX$VALUATION_MONTH), 
                            PricingDate = as.Date(dataSubsetX$PRICING_DATE),
                            Price = dataSubsetX$PRICE, 
                            Instrument = dataSubsetX$TYPE, Strike = dataSubsetX$STRIKE, 
                            Quantity = dataSubsetX$QUANTITY, Delta = dataSubsetX$DELTA, Type = typeX, 
                            value = array(NA, dim = length(dataSubsetX$VALUATION_MONTH))) 
    priceRange <- range(dataSubsetX$PRICE)
    priceRange[1] <- 0.7*floor(priceRange[1])
    priceRange[2] <- 1.5*ceiling(priceRange[2])    
    priceX <- seq(priceRange[1], priceRange[2], length = options$N)   
    nL <- length(indAsOfDate)*options$N                         
    PlotTableS <-data.frame(Date = rep(as.Date(dataSubsetX$VALUATION_MONTH[indAsOfDate]), each = options$N), 
                            PricingDate = as.Date(array(as.character(options$asOfDate +1), dim = nL)), 
                            Price = rep(priceX, length(indAsOfDate)),
                            Instrument = rep(dataSubsetX$TYPE[indAsOfDate], each = options$N), 
                            Strike = rep(dataSubsetX$STRIKE[indAsOfDate], each = options$N), 
                            Quantity = rep(dataSubsetX$QUANTITY[indAsOfDate], each = options$N), 
                            Delta = rep(dataSubsetX$DELTA[indAsOfDate], each = options$N), 
                            Type = array("Sim", dim = nL), value = array(NA, dim = nL))                            
    PlotTable <- rbind(PlotTable,PlotTableS)                                                                    
    PlotTable <- positionValue(PlotTable)    
    PlotTable$mmm.yy <- factor(format(as.Date(PlotTable$Date), "%b-%y"), 
                               levels=format(as.Date(unique(sort(as.character(PlotTable$Date)))), "%b-%y"))                                                          
    fileName <- paste(options$dirOut, "/", locations[i], "_", (options$asOfDate), ".pdf", sep = "")
    makeTrellisPlot(PlotTable, mmm.yy, nYears, fileName, options, locations[i])     
  }
  return() 
}
  
positionValue <- function(PlotTable) {
  indF <- which(PlotTable$Instrument == "FORWARD")
  if(length(indF) > 0) { 
    PlotTable$value[indF] <- PlotTable$Delta[indF]*PlotTable$Price[indF] 
  }
  indC <- which(PlotTable$Instrument == "CALL" | PlotTable$Instrument == "C")
  if(length(indC) > 0) { 
    PlotTable$value[indC] <- PlotTable$Quantity[indC]*pmax(PlotTable$Price[indC]-PlotTable$Strike[indC],0) 
  }
  indP <- which(PlotTable$Instrument == "PUT" | PlotTable$Instrument == "P")
  if(length(indP) > 0) { 
    PlotTable$value[indP] <- PlotTable$Quantity[indP]*pmax(PlotTable$Strike[indP]-PlotTable$Price[indP],0) 
  }
  PlotTable <- PlotTable[,-which(colnames(PlotTable) %in% c("Strike", "Quantity", "Delta", "Instrument"))]
  PlotTable <- cast(PlotTable, Date + PricingDate + Price +  Type ~ ., sum )
  PlotTable <- PlotTable[,-which(colnames(PlotTable)== "PricingDate")]
             
  indR <- which(PlotTable$Type == "Real")
  for(i in 1: length(indR)) {
    dateX <-  PlotTable$Date[indR[i]]
    valueX <- PlotTable$value[indR[i]] 
    indDate <- which(PlotTable$Date == dateX)
    PlotTable$value[indDate] <- PlotTable$value[indDate] - valueX
  }
  PlotTable$value <- 0.001 * PlotTable$value
  return(PlotTable)  
}  

makeTrellisPlot <- function(PlotTable, mmm.yy, nYears, fileName, options, locations) {
#  browser()
  PlotTableX <- NULL
  for (k in 1:length(mmm.yy)){
    indX <- which(PlotTable$mmm.yy == mmm.yy[k])
    if (length(indX) > 0) {
      PlotTableX <- rbind(PlotTableX, PlotTable[indX,])       
    } else {     
      PlotTableX <- rbind(PlotTableX, 
                            data.frame(Date = NA, Price = NA, Type = NA, value = NA, mmm.yy = mmm.yy[k]))        
    } 
  }
  indX <- which(is.na(PlotTableX$value) == FALSE)   
                                 
  if( length(indX) > 0 ) {
    if(max(abs(PlotTableX$value[indX])) > 0.5) {
#      PlotTableXX <- PlotTableX[indX,]
      PlotTableX$Price <- as.numeric(PlotTableX$Price)
      pdf(fileName, width=11.0, heigh=8.0)
      print(xyplot(value ~ Price| mmm.yy, data = PlotTableX, groups = Type,
      panel=function(x, y, subscripts, groups){  
        indR <-which(groups[subscripts] == "Real")
        indH <-which(groups[subscripts] == "Hist")
        indS <-which(groups[subscripts] == "Sim")
#        browser()
        panel.grid(h = -1, v = -1)          
        panel.abline(v = x[indR[1]], col = "brown")
        panel.abline(h = 0, col = "brown")
        panel.xyplot(x[indS],y[indS], type = "l", lwd = 3)
        xH <- x[indH]
        yH <- y[indH]                  
        xHS <- sort(xH, index.return = TRUE)
        yHS <- yH[xHS$ix]
        xHS <- xHS$x
        xHS <- as.numeric(xHS)
        panel.xyplot(xHS,yHS, type = "l", lwd = 3, col = "red")       
      }, 
      layout = c((length(mmm.yy)/nYears),nYears), 
      main = paste(as.character(options$varMapping), " ", as.character(options$asOfDate), ": ", 
                 as.character(locations), sep = ""), 
      xlab = "Price ($)", ylab = "Value ($K)" ))
      dev.off()    
    }
  }  
}  
