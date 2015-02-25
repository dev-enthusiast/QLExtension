plotPositionsC <- function(dataCurveTable, dataDOI, deltaValue, futurePriceTable, options) {
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
    xxx1 <- data.frame(Value = NetTotal[i], Date = NetAverage[i,1], Comp = "Total", Type = "Hist")
    xxx2 <- data.frame(Value =  NetLong[i], Date = NetAverage[i,1], Comp = "Long",  Type = "Hist")
    xxx3 <- data.frame(Value = NetShort[i], Date = NetAverage[i,1], Comp = "Short", Type = "Hist")
    Net <- rbind(Net, xxx1, xxx2, xxx3)
  }

  # Heston & Historical variability
  vHT <- FitHeston(NetTotal,options)
  qHT <- c(0, qHeston(vHT[[1]], vHT[[2]], vHT[[3]], options$percentile, c(1:vHT[[4]])))
  vHL <- FitHeston(NetLong,options)
  qHL <- c(0, qHeston(vHL[[1]], vHL[[2]], vHL[[3]], options$percentile, c(1:vHL[[4]])))
  vHS <- FitHeston(NetShort,options)
  qHS <- c(0, qHeston(vHS[[1]], vHS[[2]], vHS[[3]], options$percentile, c(1:vHS[[4]])))  
  qT <- array(0, dim <- c((vHT[[4]]+1),2))
  qL <- array(0, dim <- c((vHL[[4]]+1),2))
  qS <- array(0, dim <- c((vHS[[4]]+1),2))
  for(i in 1:vHT[[4]]) {  
    qT[(i+1),] <- quantile(sort(diff(NetTotal, lag = i)), probs = c(0.01*options$percentile, 1-0.01*options$percentile)) 
    qL[(i+1),] <- quantile(sort(diff(NetLong,  lag = i)), probs = c(0.01*options$percentile, 1-0.01*options$percentile)) 
    qS[(i+1),] <- quantile(sort(diff(NetShort, lag = i)), probs = c(0.01*options$percentile, 1-0.01*options$percentile)) 
  }  
  daysX <- seq(as.Date(pricingDate)[length(pricingDate)]+1, 
               as.Date(pricingDate)[length(pricingDate)] + 1 +2*options$DT, by = 1)
  indWeekend <- which(weekdays(daysX) %in% c("Saturday", "Sunday"))
  projectionDays <- daysX[-indWeekend]
  projectionDays <- c(max(as.Date(pricingDate)), projectionDays[1:vHT[[4]]])

  for(i in 1:length(projectionDays)) {
    xxxTp <- data.frame(Value = NetTotal[length(NetTotal)]+abs(qHT[i]), Date = projectionDays[i], 
                       Comp = "Total", Type = "Heston_95")
    xxxTm <- data.frame(Value = NetTotal[length(NetTotal)]-abs(qHT[i]), Date = projectionDays[i], 
                       Comp = "Total", Type = "Heston_5")
    xxxLp <- data.frame(Value = NetLong[length(NetLong)]+abs(qHL[i]), Date = projectionDays[i], 
                       Comp = "Long", Type = "Heston_95")
    xxxLm <- data.frame(Value = NetLong[length(NetLong)]-abs(qHL[i]), Date = projectionDays[i], 
                       Comp = "Long", Type = "Heston_5")
    xxxSp <- data.frame(Value = NetShort[length(NetShort)]+abs(qHS[i]), Date = projectionDays[i], 
                       Comp = "Short", Type = "Heston_95")
    xxxSm <- data.frame(Value = NetShort[length(NetShort)]-abs(qHS[i]), Date = projectionDays[i], 
                       Comp = "Short", Type = "Heston_5")
    Net <- rbind(Net, xxxTp, xxxTm, xxxLp, xxxLm, xxxSp, xxxSm)                   
  }
    
  save.image()
  fName <- paste(options$dir,"/", options$trader, "_", format(options$DOI,"%b_%Y"), "_HistViewAsOf_",
                 format(options$asOfDate,"%b_%d_%Y"), ".pdf", sep = "")
  pdf(fName, width=8.0, heigh=11.0)
  print(xyplot(Value ~ Date| Comp, data = Net, groups = Type, lwd = 3, type = "b", layout = c(1,3), 
  scales = list( x = list(tick.number = length(unique(format(Net$Date,"%b")))), y = "free"),
    main = paste("Hist View of", as.character(options$trader), format(options$asOfDate,"%m/%d/%Y"),  
                 "Positions for Month of", format(options$DOI,"%b,%Y"))))
  dev.off() 
  return(list(qHT[2:length(qHT)],qHL[2:length(qHL)],qHS[2:length(qHS)], NetTotal, NetLong, NetShort)) 
}
