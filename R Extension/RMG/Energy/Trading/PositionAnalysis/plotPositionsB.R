plotPositionsB <- function(dataCurveTable, dataDOI, deltaValue, futurePriceTable, options) {
#-----------------------------------------------------------------------------------
# Plot: Position Historical View (Short, Long, Total)
#-----------------------------------------------------------------------------------
#  browser()
  pricingDate <- sort(unique(futurePriceTable$PRICING_DATE))
  futurePriceTableF <- data.frame(Date = pricingDate, array(NA, dim = c(length(pricingDate), length(deltaValue)))) 
  colnames(futurePriceTableF) <- colnames(dataCurveTable)[1:length(colnames(futurePriceTableF))]
  for (i in 1:length(pricingDate)) {
    indDate <- which(futurePriceTable$PRICING_DATE == pricingDate[i])
    for(j in 1:length(indDate)) {
      indCol <- which(colnames(futurePriceTableF) == futurePriceTable$COMMOD_CURVE_BUCKET[indDate[j]])
      if(length(indCol) == 1) 
        futurePriceTableF[i,indCol] <- futurePriceTable$PRICE[indDate[j]]  
    }    
  }
  NetAverage <- data.frame(Date =  futurePriceTableF[,1],
        t(array(deltaValue, dim = c(length(deltaValue), length(pricingDate)))) *
        futurePriceTableF[,2:length(futurePriceTableF[1,])])
                
  indLong  <- which(deltaValue > 0) + 1
  indShort <- which(deltaValue < 0) + 1
  NetShort <- array(NA, dim = length(pricingDate))
  NetLong  <- array(NA, dim = length(pricingDate))
  NetTotal <- array(NA, dim = length(pricingDate))
  
  for (i in 1:length(NetAverage[,1])) {
    NetShort[i] <- 1e-6*sum(NetAverage[i,indShort], na.rm = TRUE)
    NetLong[i]  <- 1e-6*sum(NetAverage[i,indLong], na.rm = TRUE)
    NetTotal[i] <- 1e-6*sum(NetAverage[i,2:length(NetAverage[1,])], na.rm = TRUE)
  }
  Net <- NULL
  for (i in 1:length(NetAverage[,1])) {
    xxx1 <- data.frame(Value = NetTotal[i], Date = NetAverage[i,1], Type = "Total", Year = "2006")
    xxx2 <- data.frame(Value =  NetLong[i], Date = NetAverage[i,1], Type = "Long", Year = "2006")
    xxx3 <- data.frame(Value = NetShort[i], Date = NetAverage[i,1], Type = "Short", Year = "2006")
    Net <- rbind(Net, xxx1, xxx2, xxx3)
  }
  save.image()
  fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b_%Y"), "_HistViewAsOf_",
                 format(options$asOfDate,"%b_%d_%Y"), ".pdf", sep = "")
  pdf(fName, width=8.0, heigh=11.0)
  print(xyplot(Value ~ Date| Type, data = Net, groups = Year, lwd = 3, type = "b", layout = c(1,3), 
  scales = list( x = list(tick.number = length(unique(format(Net$Date,"%b")))), y = "free"),
    main = paste("Hist View of", as.character(options$trader), format(options$asOfDate,"%m/%d/%Y"),  
                 "Positions for Month of", format(options$DOI,"%b,%Y"))))
  dev.off()  
}
