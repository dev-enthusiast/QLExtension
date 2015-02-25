RatioAnalysis <- function(dataCurveTable, futurePriceTable, options) {
#  browser()
  dataCurveTableMonthlyAverage <- MonthlyAverage(dataCurveTable,futurePriceTable)
  ind7x8  <- grep("7x8", colnames(dataCurveTableMonthlyAverage))
  dataCurveTableMonthlyAverage <- dataCurveTableMonthlyAverage[,-ind7x8]
  ind2x16 <- grep("2x16", colnames(dataCurveTableMonthlyAverage))
  dataCurveTableMonthlyAverage <- dataCurveTableMonthlyAverage[,-ind2x16]
  indForward <- which(dataCurveTableMonthlyAverage$Date > options$pricing.dt.end)
  forwardDate <- sort(dataCurveTableMonthlyAverage$Date[indForward]) 
  arrayLength <- length(indForward)*(length(colnames(dataCurveTable))-2)^2
  ratioArray <- array(NA, dim = (c(arrayLength,9)))
  m <- 0
  for (iT in 1:length(forwardDate)) {
    idxM <- which(dataCurveTableMonthlyAverage$Date <= options$pricing.dt.end &
                  format(dataCurveTableMonthlyAverage$Date, "%m") == format(forwardDate[iT], "%m"))
    idxT <- which(dataCurveTableMonthlyAverage$Date == forwardDate[iT])
    for (i1 in 2:(length(colnames(dataCurveTableMonthlyAverage))-1)) {
      for (i2 in 2:(length(colnames(dataCurveTableMonthlyAverage))-1)) {
        m <- m +1
        ratioArray[m,1] <- colnames(dataCurveTableMonthlyAverage)[i1]
        ratioArray[m,2] <- colnames(dataCurveTableMonthlyAverage)[i2]
        ratioArray[m,3] <- as.character(forwardDate[iT])
        ratioX <- dataCurveTableMonthlyAverage[idxM,i1]/dataCurveTableMonthlyAverage[idxM,i2]
        ratioArray[m,4] <- max(ratioX, na.rm = TRUE)
        ratioArray[m,5] <- mean(ratioX, na.rm = TRUE)
        ratioArray[m,6] <- dataCurveTableMonthlyAverage[idxT,i1]/dataCurveTableMonthlyAverage[idxT,i2]
        ratioArray[m,7] <- as.numeric(ratioArray[m,6])/as.numeric(ratioArray[m,5])
      }
    }
  }

  idxExclude <- which(is.na(as.numeric(ratioArray[,7])) == TRUE | as.numeric(ratioArray[,7]) == 1)
  ratioArray <- ratioArray[-idxExclude,]
  for (iT in 1:length(forwardDate)) {
    idxDate <- which(ratioArray[,3] == as.character(forwardDate[iT]))
    idxSort <- sort(as.numeric(ratioArray[idxDate,7]), decreasing = TRUE, index = TRUE)$ix + idxDate[1] - 1
    for (i in 1:length(idxSort))
      ratioArray[idxSort[i],8] <- i
  }
  idxSort <- sort(as.numeric(ratioArray[,7]), decreasing = TRUE, index = TRUE)$ix
  for (i in 1:length(idxSort))
    ratioArray[idxSort[i],9] <- i
  idxWorst <- which(as.numeric(ratioArray[,8]) > 5 & as.numeric(ratioArray[,9]) > 25)
  ratioArray <- ratioArray[-idxWorst,]
  colnames(ratioArray) <- c("Commodity1", "Commodity2", "FutureDate", "MaxHistRatio", "AvgHistRatio", "FutureRatio",
                            "Future/AvgHist", "MonthlyRanking", "TotalRanking")                         
  write.table(ratioArray, 
    file = paste(options$dirRoot,"/", "RatioAnalysis_", as.character(options$pricing.dt.end), ".csv", sep = ""), 
    append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  idxBest <- which(as.numeric(ratioArray[,8]) == 1 | as.numeric(ratioArray[,9]) <= length(indForward))            
  Com1Com2 <- unique(paste(ratioArray[idxBest,1],ratioArray[idxBest,2], sep ="XYZ")) 
  Com1Com2Split <- strsplit(Com1Com2,"XYZ")
  ranking <- array(0,  dim = length(Com1Com2))
  nMax <- length(Com1Com2) 
  com12    <- array(NA, dim = c(nMax, 3)) 
  for (i in 1:length(Com1Com2)) {
    indx <- which(ratioArray[,1] == Com1Com2Split[[i]][1] & ratioArray[,2] == Com1Com2Split[[i]][2])
    ranking[i] <- min(as.numeric(ratioArray[indx,9]))
  }
  idxSort <- sort(ranking, index = TRUE)$ix 
  for  (i in 1:nMax) {
    com12[i,1] <- Com1Com2Split[[idxSort[i]]][1]      
    com12[i,2] <- Com1Com2Split[[idxSort[i]]][2]      
    com12[i,3] <- ranking[idxSort[i]]      
  } 
  nPlots <- ceiling(nMax/3)
  par0   <- trellis.par.get()  
  for(k in 1:nPlots) {
    jMax <- 3
    if(k == nPlots) 
      jMax = nMax-(k-1)*3      
    RatioBest  <- NULL 
    for(j in 1:jMax) {  
      jx <- j+(k-1)*3 
      i1 <- which(colnames(dataCurveTableMonthlyAverage) == com12[jx,1])  
      i2 <- which(colnames(dataCurveTableMonthlyAverage) == com12[jx,2])
      TypeX <- paste("Rank # ", com12[jx,3], "; ", com12[jx,1], "/", com12[jx,2], sep = "")
      for (i in 1:length(dataCurveTableMonthlyAverage[,1])) {
        yearX <- format(dataCurveTableMonthlyAverage[i,1], "%Y")
        if( dataCurveTableMonthlyAverage[i,1] > options$pricing.dt.end )
          yearX <- paste(yearX,"_F",sep = "")
        xxx <- data.frame(Ratio = dataCurveTableMonthlyAverage[i,i1]/dataCurveTableMonthlyAverage[i,i2], 
                          Date = format(dataCurveTableMonthlyAverage[i,1], "%b"),Type = TypeX, Year = yearX)
        RatioBest <- rbind(RatioBest, xxx)
      }
      idxRA <- which(ratioArray[,1] == com12[jx,1] & ratioArray[,2] == com12[jx,2])
      for (i in 1:length(idxRA)) {
        signal <- "***"
        if(as.numeric(ratioArray[idxRA[i],8]) == 1 | as.numeric(ratioArray[idxRA[i],9]) <= length(indForward))
          signal <- "*****"
        xxx <- data.frame(Ratio = as.numeric(ratioArray[idxRA[i],6]), Date = format(as.Date(ratioArray[idxRA[i],3]), "%b"), 
        Type = TypeX, Year = signal)
        RatioBest <- rbind(RatioBest, xxx)
      }
    } 
    indxxx <- which(RatioBest$Year == "***")
    if(length(indxxx) > 0) {
      arrayXXX <- RatioBest[indxxx,]
      RatioBest <- rbind(RatioBest[-indxxx,],arrayXXX)
    }  
    fName <- paste(options$dirRoot,"/", "RatioAnalysisFig", as.character(k), "_", as.character(options$pricing.dt.end), ".pdf", sep = "")
    pdf(fName, width=8.0, heigh=11.0)
    sym.col <- trellis.par.get("superpose.symbol")
    sym.col$col <- c(sym.col$col, "black", "black")
    sym.col$pch <- c(array(1, dim = (length(unique(RatioBest$Year))-2)), 7, 5)
      
    trellis.par.set("superpose.symbol", sym.col)
    plot.line <- trellis.par.get("add.line")
    plot.line$lwd <- 3
    plot.line$lty <- 3
    trellis.par.set("add.line",plot.line)  
    print(xyplot(Ratio ~ Date| Type, data = RatioBest, groups = Year, 
      panel = function(x,y,subscripts,groups){
      groupsX <- unique(groups)
        for (i in 1:length(groupsX)) {
          ind <- which(groups[subscripts]==groupsX[i])
          if(groupsX[i] == "***" | groupsX[i] == "*****") { 
              panel.xyplot(x[ind], y[ind], pch = sym.col$pch[i], col = sym.col$col[i])
          } else {  
            panel.xyplot(x[ind],y[ind], lwd = 3, type="o", col = sym.col$col[i])
          }
        }
      }, scales = "free", layout = c(1,j),
    main = paste("Ratios as of", as.character(options$pricing.dt.end)), 
    key=list(points=Rows(sym.col, 1:length(unique(RatioBest$Year))),
            text=list(as.character(unique(RatioBest$Year))),
             columns=1, space = "right", border =1 )))
    dev.off()
  }
  trellis.par.set(par0)
  dev.off()
}