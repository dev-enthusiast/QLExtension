# Calculate and plot the sensitivity of the spread to temperature
# for daily regressions by month. 
# 
# Written by Adrian Dragulescu on 31-Jan-2005


daily.spread.regression <- function(MM, options, save){

  source("C:/R/Work/MyLibrary/Graphics/label.outliers.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/bucket.ts.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/day.of.year.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/intersect.ts.R")
  
  fName <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.",
                 options$airport.name, ".Rdata",sep="")
  load(fName)
  fName <- paste("S:/All/Risk/Data/Weather/temp.hist.",
                 options$airport.name, ".csv",sep="")
  htemp <- read.csv(fName, header=F)
  htemp$temp <- (htemp[,4]+htemp[,5])/2
  htemp$date <- as.Date(paste(htemp[,1],htemp[,2],htemp[,3], sep="-"))
  
  ind <- which(colnames(MM) %in% options$shortNames)
  MM <- MM[,-ind]
  dMM <- bucket.ts(MM,"daily")
  dMM$date <- as.Date(paste(dMM$year,dMM$month,dMM$day, sep="-"))

  aux <- intersect.ts(dMM$date, dMM$spread, htemp$date, htemp$temp)
  dMM <- cbind(dMM[aux[[2]],], temp=htemp$temp[aux[[3]]])
  
  dMM$wn.temp  <- temp.day$mean[day.of.year(dMM$date)]
  wn.std       <- temp.day$sd[day.of.year(dMM$date)]
  dMM$temp.dev <- (dMM$temp - dMM$wn.temp)/wn.std
  dMM$temp.rel <- dMM$temp - dMM$wn.temp
  dMM[1:10,]

  require(nlme)  
  focus.months <- options$focus.months
  if (length(options$focus.months)!=0){
    for (focus.month in focus.months){      
      reg <- lm(spread ~ temp.rel, subset= month==focus.month, data=dMM)
      fileName <- paste(save$dir,"sumreg_daily_",month.abb[focus.month],
                        ".tex", sep="")
      sink(fileName)
      cat("\\begin{Verbatim}[baselinestretch=1, fontsize=\\footnotesize]\n")
      cat(paste("Regression output for the month of",
                month.abb[focus.month], "\n"))
      print(summary(reg))
      cat("Correlation matrix of coefficients: \n")
      print(round(cov2cor(vcov(reg)),3))
      cat("\\end{Verbatim}\n")
      sink()

      mMM <- subset(dMM, subset=month==focus.month, select=c("temp.rel","spread"))
      grid <- data.frame(temp.rel=seq(-20, 20, by=0.05))
      pred <- predict(reg, grid, se.fit=T)
      ci <- qt(0.975, as.numeric(reg$df.residual))

      fileName <- paste(save$dir,"daily_reg_",month.abb[focus.month],
                        ".pdf", sep="")
      if (save$Analysis$all){pdf(fileName, width=4.5, heigh=4.5)}
      plot(mMM$temp.rel, mMM$spread, col="blue",
           xlab="Daily temperature deviation", ylab="Daily spread, $",
           main=month.abb[focus.month], cex.main=1)
      lines(grid$temp.rel, pred$fit - ci*pred$se, lty=2, col="grey")
      lines(grid$temp.rel, pred$fit + ci*pred$se, lty=2, col="grey")              
      abline(reg, col="grey")
      if (save$Analysis$all){dev.off()}
    }
  }
   
  uYears <- unique(dMM$year)
  dMM$month.abb <- factor(month.abb[dMM$month], levels=month.abb)
  require(lattice)

  #--------  Trellis plot vs. temperature deviations -------------
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col <- c("blue", "red", "black", "green2")
  pplot <- xyplot( spread ~ temp.dev | month.abb, data=dMM, 
       panel=function(x,y, ...){
         panel.superpose(x,y, ...)
         panel.lmline(x,y, col="gray", type="l", lwd=1)
       },
       groups=year, 
       key=list(points=Rows(sym.col, 1:length(uYears)),
                       text=list(as.character(uYears)),
                       columns=length(uYears)),         
       ylab=paste("Daily spread (", options$spread, "), $", sep=""),
       xlab="Normalized temperature deviation",
       main=list(options$dateRange, cex=1),
       layout=c(4,3))
  fileName <- paste(save$dir,"trellis_daily.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8, heigh=6.5)}
  aux <- trellis.par.get()
  bkg.col <- trellis.par.get("background"); bkg.col$col <- "white"
  trellis.par.set("background", bkg.col)
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col <- c("blue", "red", "black", "green2")
  trellis.par.set("superpose.symbol", sym.col)
  print(pplot)
  if (save$Analysis$all){dev.off()}

}


#       list.Jul <- lmList(spread ~ temp.rel | year,
#                          subset= month==focus.month, data=dMM)
#       aux <- trellis.par.get()
#       bkg.col <- trellis.par.get("background"); bkg.col$col <- "white"
#       trellis.par.set("background", bkg.col)
#       sym <- trellis.par.get("dot.line"); sym$col <- "gray"; sym$lty <- 2
#       trellis.par.set("dot.line", sym)
#       sym <- trellis.par.get("dot.symbol"); sym$font <- 2
#       trellis.par.set("dot.symbol", sym)
#       plot(intervals(list.Jul), main=month.abb[focus.month], col="blue")


