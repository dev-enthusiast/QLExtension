mainPriceAnalysisReport <- function(options, TYPE) {

  source("H:/user/R/RMG/Energy/Trading/PriceAnalysis/makePDF.PriceAnalysisReport.R")
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R") 
#  source("H:/user/R/RMG/TimeFunctions/countBusinessDate.R")
#  source("H:/user/R/RMG/TimeFunctions/add.date.R")
  
  require(reshape)
  require(RODBC)
  require(xtable)  
  options$plot <- 0       # plot the results
  options$type <- "raw"   # or "corrected"
  # power
  options$curve.names      <- options$curve.power
  resPower <- read.prices.from.CPSPROD(options)
  options$curve.shortNames <- options$curve.powerShortNames
  classListPower <- c(list(c(1:4), "East/Central"), list(c(5:8), "West"))  
  classSeasPower <- list(c("Jun", "Jul", "Aug", "Sep"), "Summmer", c("Nov", "Dec", "Jan", "Feb"), "Winter")
  outPower <- tablesGraphsOut(resPower, options, classListPower, classSeasPower, "(\\$/MWh)", "Price ($/MWh)", 
                              "Power On-Peak FP")
  # NG 
  options$curve.names      <- options$curve.NG  
  resNG <- read.prices.from.CPSPROD(options)                       
  options$curve.shortNames <- options$curve.NG.ShortNames               
  classListNG <- c(list(c(1:5), "East/Central"), list(c(6:9), "West")) 
  classSeasNG <- list(c("Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct"), "Summer", 
                      c("Nov", "Dec", "Jan", "Feb", "Mar"), "Winter")  
  unitNG <- "(\\$/MMBtu)"  
  ylabNG <- "Price ($/MMBtu)"                  
  outNG <- tablesGraphsOut(resNG, options, classListNG, classSeasNG, unitNG, ylabNG, "NG FP")
  # HR
  colnames(resNG)[which(colnames(resNG) == "COMMOD_CURVE")] <- "COMMOD_CURVE_NG"
  colnames(resNG)[which(colnames(resNG) == "PRICE")] <- "PRICE_NG"  
  colnames(resPower)[which(colnames(resPower) == "PRICE")] <- "PRICE_POWER"               
  matchTable <- data.frame(COMMOD_CURVE = options$curve.power, COMMOD_CURVE_NG = options$HR.Power2NG)  
  resHR <- merge(merge(resPower, matchTable), merge(matchTable, resNG))    
  resHR <- data.frame(resHR, PRICE = (resHR$PRICE_POWER/resHR$PRICE_NG))
  resHR <- resHR[, c("COMMOD_CURVE", "PRICING_DATE", "START_DATE", "PRICE")]
  options$curve.shortNames <- options$curve.powerShortNames   
  options$curve.names      <- options$curve.power                                                                                     
  unitHR <- "(MMBtu/MWh)"
  ylabHR <- "Heat Rate (MMBtu/MWh)"                                                                                  
  outHR <- tablesGraphsOut(resHR, options, classListPower, classSeasPower, unitHR, ylabHR, "Forward HR")
  table2 <- data.frame(LOCATION_NG = options$curve.NG.ShortNames, COMMOD_CURVE_NG = options$curve.NG)  
  table2 <- table2[-1,]                                                                              
  table1 <- data.frame(LOCATION_POWER = options$curve.powerShortNames, COMMOD_CURVE_POWER = options$curve.power, 
                  LOCATION_NG = table2$LOCATION_NG, COMMOD_CURVE_NG = matchTable$COMMOD_CURVE_NG) 
  caption <- "Power and NG Curves and Locations as Matched for the Heat Rate"                 
  file <- paste(options$dirOut, "tableHeader.tex", sep = "")                                       
  print.xtable(xtable(table1, caption = caption), file = file, size = "scriptsize")
  makePDF.PriceAnalysisReport(options)  
}

tablesGraphsOut <- function(res, options, classList, classSeas, unitX, ylabX, COMMOD) {
#  browser()
  res$START_DATE <- as.Date(paste(format(res$START_DATE, "%Y-%m"), "-01", sep = ""))
  COMMODx <- gsub(" ", "-", COMMOD)
  curveNames <- data.frame(COMMOD_CURVE = options$curve.names, LOCATION = options$curve.shortNames)
  res <- merge(res,curveNames)
  res <- res[,-which(colnames(res) == "COMMOD_CURVE")]
  colnames(res)[which(colnames(res) == "PRICE")]  <- "value"
  dates <- sort(unique(res$START_DATE))
#  busDates <- countBusinessDate(dates[1], add.date(dates[length(dates)], "0 e"))
#  nB <- array(NA, dim = length(dates))
#  for (i in 1:length(nB))  
#    nB[i] <- length(which( busDates >= dates[i] & busDates <= add.date(dates[i], "0 e")))
#  data.frame(START_DATE = dates, BUS_DATES = nB) 
  xxx <- timeFrame(dates, classSeas)
  timeFrameList <- xxx[[1]]
  timeFrameName <- xxx[[2]]    
  MMM <- NULL
  formula <- paste(paste(colnames(res)[-which(colnames(res) %in% c("START_DATE", "value"))],
                         collapse = " + ", sep = ""), " ~ .", sep = "")
#  browser()                       
  for (i in 1:length(timeFrameName)) {
    timeFrame <- data.frame(START_DATE = dates[timeFrameList[[i]]])
    MM <- merge(res, timeFrame)
    MMM <- rbind(MMM, data.frame(cast(MM, eval(formula), mean), TIME_FRAME = timeFrameName[i]))
  }
  MMM$LOCATION <- factor(MMM$LOCATION, levels =  options$curve.shortNames)
  pricingDates <- sort(unique(MMM$PRICING_DATE))
  firstDate <- as.character(format(pricingDates[1], "%m/%d/%y"))  
  lastDate  <- as.character(format(pricingDates[length(pricingDates)], "%m/%d/%y"))
  # last
  MMMlast <- subset(MMM, PRICING_DATE == pricingDates[length(pricingDates)])
  MMMlast <- MMMlast[,-which(colnames(MMMlast) == "PRICING_DATE")]
  tableLast <- cast(MMMlast, LOCATION ~ TIME_FRAME, function(x) round(x, digits = 2))
  # months
  tableM  <- tableLast[,1:13]
  captionM <- paste(COMMOD, " (Rolling 12 Months) as of ", lastDate, " ", unitX, sep = "")
  fileM <- paste(options$dirOut, "tableM", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableM, caption = captionM), file = fileM, size = "tiny", caption.placement = "top")
  # QY  
  lengthQY <- max(grep("CAL", colnames(tableLast))) 
  tableQY <- cbind(LOCATION = tableLast[,1], tableLast[,(14:lengthQY)])
  captionQY <- paste(COMMOD, " (Quarterly and Calendar Strips) as of ", lastDate, " ", unitX, sep = "")
  fileQY <- paste(options$dirOut, "tableQY", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableQY, caption = captionQY), file = fileQY, size = "tiny", caption.placement = "top")
  # Seas
  tableS <- cbind(LOCATION = tableLast[,1], tableLast[,((lengthQY+1):length(colnames(tableLast)))])                                            
  captionS <- paste(COMMOD, " (", classSeas[[2]], " and ", classSeas[[4]], ") as of ", lastDate, " ", unitX, sep = "")                     
  fileS <- paste(options$dirOut, "tableS", COMMODx, ".tex", sep = "")                                            
  print.xtable(xtable(tableS, caption = captionS), file = fileS, size = "tiny", caption.placement = "top")   
  # diff
  MMMprev <- subset(MMM, PRICING_DATE == pricingDates[(length(pricingDates)-1)])
  MMMprev <- MMMprev[,-which(colnames(MMMprev) == "PRICING_DATE")]
  MMMprev$value <- (-1)*MMMprev$value
  MMMdiff <- rbind(MMMlast,MMMprev)
  MMMdiff <- aggregate(MMMdiff$value, list(LOCATION = MMMdiff$LOCATION, TIME_FRAME = MMMdiff$TIME_FRAME), sum)
  colnames(MMMdiff)[which(colnames(MMMdiff) == "x")] <- "value"
  MMMdiff$LOCATION <- factor(MMMdiff$LOCATION, levels =  options$curve.shortNames)
  MMMdiff$TIME_FRAME <- factor(MMMdiff$TIME_FRAME, levels = timeFrameName)
  tableDiff <- cast(MMMdiff, LOCATION ~ TIME_FRAME, function(x) round(x, digits = 2))
#  browser()
  # months
  tableMd  <- tableDiff[,1:13]
  captionMd <- paste(COMMOD, "Changes (Rolling 12 Months) as of", lastDate, unitX)                   
  fileMd <- paste(options$dirOut, "tableMd", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableMd, caption = captionMd), file = fileMd, size = "tiny", caption.placement = "top")
  # QY 
  tableQYd <- cbind(LOCATION = tableDiff[,1], tableDiff[,(14:lengthQY)])
  captionQYd <- paste(COMMOD, "Changes (Quarterly and Calendar Strips) as of", lastDate, unitX)                       
  fileQYd <- paste(options$dirOut, "tableQYd", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableQYd, caption = captionQYd), file = fileQYd, size = "tiny", caption.placement = "top")
  # Seas                                                                                                           
  tableSd <- cbind(LOCATION = tableDiff[,1], tableDiff[,((lengthQY+1):length(colnames(tableDiff)))])                
  captionSd <- paste(COMMOD, " Changes (", classSeas[[2]], " and ", classSeas[[4]], ") as of ", 
                     lastDate, " ", unitX, sep = "")        
  fileSd <- paste(options$dirOut, "tableSd", COMMODx, ".tex", sep = "")                                             
  print.xtable(xtable(tableSd, caption = captionSd), file = fileSd, size = "tiny", caption.placement = "top")
  # min & max 
  glueMinMax <- function(x) 
    paste(as.character(round(min(x), digits =2)), as.character(round(max(x), digits =2)), sep = "-")
  tableMinMax <- cast(MMM, LOCATION~TIME_FRAME, glueMinMax)
  # months
  tableMinMaxM  <- tableMinMax[,1:13]
  captionMinMaxM <- paste(COMMOD, "Min-Max (Rolling 12 Months) from", firstDate, "to", lastDate, unitX)                   
  fileMinMaxM <- paste(options$dirOut, "tableMinMaxM", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableMinMaxM, caption = captionMinMaxM), file = fileMinMaxM,   
    size = "tiny", caption.placement = "top")
  # QY 
  tableMinMaxQY <- cbind(LOCATION = tableMinMax[,1], tableMinMax[,(14:lengthQY)])
  captionMinMaxQY <- paste(COMMOD, "Min-Max (Quarterly and Calendar Strips) from", firstDate, "to", lastDate, unitX)                       
  fileMinMaxQY <- paste(options$dirOut, "tableMinMaxQY", COMMODx, ".tex", sep = "")
  print.xtable(xtable(tableMinMaxQY, caption = captionMinMaxQY), file = fileMinMaxQY, 
    size = "tiny", caption.placement = "top")
  # Seas                                                                                                           
  tableMinMaxS <- cbind(LOCATION = tableMinMax[,1], tableMinMax[,((lengthQY+1):length(colnames(tableMinMax)))])                
  captionMinMaxS <- paste(COMMOD, " Min-Max (", classSeas[[2]], " and ", classSeas[[4]], ") from ", firstDate, " to ",
                     lastDate, " ", unitX, sep = "")        
  fileMinMaxS <- paste(options$dirOut, "tableMinMaxS", COMMODx, ".tex", sep = "")                                             
  print.xtable(xtable(tableMinMaxS, caption = captionMinMaxS), file = fileMinMaxS, 
    size = "tiny", caption.placement = "top") 
  # Figs
  dataFig <- subset(res, PRICING_DATE == pricingDates[length(pricingDates)] )
  dataFig <- dataFig[,-which(colnames(dataFig) == "PRICING_DATE")]
  dataFig$LOCATION <- factor(dataFig$LOCATION, levels =  options$curve.shortNames)  
  dataFig <- tapply(dataFig$value, list(as.character(dataFig$START_DATE), dataFig$LOCATION), mean)
  cols <- c("red", "blue", "green", "black", "gold", "purple", "cyan", "brown")
  for(i in 1:(length(classList)/2)) {
    dataFigX <- dataFig[, classList[[(2*i-1)]]]
    file = paste(options$dirOut,"fig", COMMODx, "-", i, ".pdf", sep = "")
    pdf(file, width=11, height=8)
    matplot(dataFigX, ylab = ylabX, xaxt="n", xlab="", type="l", lwd = 3, lty="solid", col = cols,
      main = paste(COMMOD, " ", classList[[(2*i)]], " as of ", lastDate, sep = ""))
    cdates <- as.Date(rownames(dataFigX)) 
    if (max(cdates)-min(cdates)>3*365){    # if more than 3 years, label Jan and Jul
      aux <- min(which(as.numeric(format(as.Date(rownames(dataFigX)), "%m"))%in% c(1,7)))
      ind.labx <- seq(aux,nrow(dataFigX), by=6)
    } else {
      ind.labx <- 1:nrow(dataFigX)
    }
    labx <- format(cdates, "%m-%Y")[ind.labx]
    axis(1, at=ind.labx, labels=labx, las=2, cex.axis=1.0)
    abline(v = ind.labx, col = "grey", lty = "dotted")
    grid(nx = NA, ny = NULL)
    legend(x="topleft", y=NULL, legend=colnames(dataFigX), bty="n", text.col=cols, cex = 1.1, ncol = 4)    
    dev.off()      
  }
  return(MMMlast)
}

# define time frames
timeFrame <- function(dates, classSeas) {
  indDates <- which(is.na(dates) == FALSE)
  timeFrameList <- NULL
  timeFrameName <- NULL
  qEnd   <- c("Mar", "Jun", "Sep", "Dec")
  # first 12 months
  for ( i in 1:12) {
    timeFrameList <- c(timeFrameList, list(indDates[i]))
    timeFrameName <- c(timeFrameName,  as.character(format(dates[i], "%b.%y")))
  }
  # staring from the next year by quarters
  currentYear <- as.numeric(format(dates[indDates[1]], "%Y"))
  indNotCurrentYear <- which(as.numeric(format(dates, "%Y")) != currentYear)
  # 2 years by quarters
  for(i in 1:24) {
    if(format(dates[indNotCurrentYear[i]], "%b") %in% qEnd) {
      timeFrameList <- c(timeFrameList, list((indNotCurrentYear[i] -2):indNotCurrentYear[i]))
      nameX <- paste("Q", as.character(which(format(dates[indNotCurrentYear[i]], "%b") == qEnd)), ".",
                     as.character(format(dates[indNotCurrentYear[i]], "%y")), sep = "")
      timeFrameName <- c(timeFrameName, nameX)
    }
  }
  years <- unique(format(dates, "%y"))
  for (i in 1:length(years)) {
    indYear <- which((format(dates, "%y")) == years[i])
    if(length(indYear) > 3) {
      timeFrameList <- c(timeFrameList, list(indYear))
      timeFrameName <- c(timeFrameName, paste("CAL.", years[i], sep = ""))
    }
  }
  # Seasons  
#  browser()
  indI <- sort(c(which(format(dates,"%b") == classSeas[[1]][1]), which(format(dates,"%b") == classSeas[[3]][1])))
  indF <- sort(c(which(format(dates,"%b") == classSeas[[1]][length(classSeas[[1]])]), 
                which(format(dates,"%b") == classSeas[[3]][length(classSeas[[3]])])))
  if(indI[1] > indF[1]) {indF <- indF[-1]}  
  if(length(indI) > length(indF)) { indI <- indI[1: length(indF)] } 
  for (i in 1:length(indI)) {
    timeFrameList <- c(timeFrameList, list(c(indI[i]:indF[i]))) 
    yearsX <- paste(unique(format(dates[indI[i]:indF[i]], "%y")), sep = "", collapse = "-")
    timeFrameName <- c(timeFrameName, 
                       paste(format(dates,"%b")[indI[i]], format(dates,"%b")[indF[i]] , yearsX, sep = "-"))
  }
  return(list(timeFrameList,timeFrameName))
}