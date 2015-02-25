# Calculate and plot the sensitivity of the spread to temperature
# for daily regressions by month.  Include other variables:
# - into Cinergy prices
# - NYISO flows
# 
# Written by Adrian Dragulescu on 24-Feb-2005


daily.spread.regression.other <- function(MM, options, save){

  source("C:/R/Work/MyLibrary/Graphics/label.outliers.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/bucket.ts.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/day.of.year.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/intersect.ts.R")

#  options$tsdb.refresh  <- 0
  options$hdata.symbols <- "NYPP_HAM_ENERGY_DNI_PJM"
  options$hdata.filename <- "hdata.NYISO.csv"
  ny.imp <- tsdbRead(options,save)
  colnames(ny.imp) <- c("year", "month", "day", "hour", "ny.imp",)

#browser()  
  options$hdata.symbols <- c("MWDaily_16HrDly_CinergyInto",
                             "MWDaily_OPDly_CinergyInto")
  options$hdata.filename <- "hdata.CIN.csv"; options$HS <- 0
  cin.price <- tsdbRead(options,save); options$HS <- 1
  colnames(cin.price) <- c("year", "month", "day", "Cin.16H","Cin.Off")
  
  fName <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.",
                 options$airport.name, ".Rdata",sep="")
  load(fName)
  fName <- paste("S:/All/Risk/Data/Weather/temp.hist.",
                 options$airport.name, ".csv",sep="")
  htemp <- read.csv(fName, header=F)
  htemp$temp <- (htemp[,4]+htemp[,5])/2
  htemp$date <- as.Date(paste(htemp[,1],htemp[,2],htemp[,3], sep="-"))
  
  source("C:/R/Work/MyLibrary/TimeFunctions/make.onpeak.offpeak.flat.R")
  aux <- make.onpeak.offpeak.flat(subset(MM, select=c("year","month","day",
                                               "hour", "spread")))
  dMM <- bucket.ts(MM,"daily")
  dMM <- cbind(dMM, spread.Pk=aux$onpeak)
  dMM <- cbind(dMM, spread.Off=aux$offpeak)  
  dMM$date <- as.Date(paste(dMM$year,dMM$month,dMM$day, sep="-"))

  aux <- intersect.ts(dMM$date, dMM$spread, htemp$date, htemp$temp)
  dMM <- cbind(dMM[aux[[2]],], temp=htemp$temp[aux[[3]]])
  
  dMM$wn.temp  <- temp.day$mean[day.of.year(dMM$date)]
  wn.std       <- temp.day$sd[day.of.year(dMM$date)]
  dMM$temp.dev <- (dMM$temp - dMM$wn.temp)/wn.std
  dMM$temp.rel <- dMM$temp - dMM$wn.temp

  #-------------------- add into CIN prices
  cin.price$date <- as.Date(paste(cin.price$year, cin.price$month,
                                  cin.price$day, sep="-"))
  aux <- intersect.ts(dMM$date, dMM$spread, cin.price$date, cin.price$Cin.16H)
  dMM <- cbind(dMM[aux[[2]],], Cin.16H=cin.price$Cin.16H[aux[[3]]],
               Cin.Off=cin.price$Cin.Off[aux[[3]]])

  #-------------------- add NYISO flows
  dny.imp <- bucket.ts(ny.imp, "daily")
  dny.imp$date <- as.Date(paste(dny.imp$year, dny.imp$month,
                                  dny.imp$day, sep="-"))
  aux <- intersect.ts(dMM$date, dMM$spread, dny.imp$date, dny.imp$ny.imp)
  dMM <- cbind(dMM[aux[[2]],], ny.imp=dny.imp$ny.imp[aux[[3]]])
  dMM[1:10,]

  ######################################################################
  plot(dMM$spread, dMM$Cin.16H-dMM$WHub)

  uYears <- unique(dMM$year)
  dMM$month.abb <- factor(month.abb[dMM$month], levels=month.abb)
  require(lattice)
  
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col <- c("blue", "red", "black", "green2")
  #--------------------  Trellis plot vs. Cin prices -------------
  pplot <- xyplot( spread ~ I(Cin.16H-WHub) | month.abb, data=dMM, 
       panel=function(x,y, ...){
         panel.superpose(x,y, ...)
         panel.lmline(x,y, col="gray", type="l", lwd=1)
       },
       groups=year, 
       key=list(points=Rows(sym.col, 1:length(uYears)),
                       text=list(as.character(uYears)),
                       columns=length(uYears)),         
       ylab=paste("Daily spread ", options$spread, ", $", sep=""),
       xlab="Daily spread Cin.16H - WHub, $",
       main=list(options$dateRange, cex=1),
       layout=c(4,3))
  fileName <- paste(save$dir,"trellis_daily_Cin.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8, heigh=6.5)}
  aux <- trellis.par.get()
  bkg.col <- trellis.par.get("background"); bkg.col$col <- "white"
  trellis.par.set("background", bkg.col)
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col <- c("blue", "red", "black", "green2")
  trellis.par.set("superpose.symbol", sym.col)
  print(pplot)
  if (save$Analysis$all){dev.off()}

  #--------------------  Trellis plot vs. NY imports -------------
  pplot <- xyplot( spread ~ ny.imp | month.abb, data=dMM, 
       panel=function(x,y, ...){
         panel.superpose(x,y, ...)
         panel.lmline(x,y, col="gray", type="l", lwd=1)
       },
       groups=year, 
       key=list(points=Rows(sym.col, 1:length(uYears)),
                       text=list(as.character(uYears)),
                       columns=length(uYears)),         
       ylab=paste("Daily spread ", options$spread, ", $", sep=""),
       xlab="Daily NYISO imports, MW",
       main=list(options$dateRange, cex=1),
       layout=c(4,3))
  pplot
  fileName <- paste(save$dir,"trellis_daily_imports.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=8, heigh=6.5)}
  aux <- trellis.par.get()
  bkg.col <- trellis.par.get("background"); bkg.col$col <- "white"
  trellis.par.set("background", bkg.col)
  sym.col <- trellis.par.get("superpose.symbol")
  sym.col$col <- c("blue", "red", "black", "green2")
  trellis.par.set("superpose.symbol", sym.col)
  print(pplot)
  if (save$Analysis$all){dev.off()}

  focus.months <- options$focus.months
  if (length(options$focus.months)!=0){
    for (focus.month in focus.months){      
      reg   <- lm(spread ~ temp.rel + I(Cin.16H-WHub) + ny.imp + WHub + Cin.Off,
                  subset= month==focus.month, data=dMM)
      
      fileName <- paste(save$dir,"sumreg_daily_other_",month.abb[focus.month],
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
    }
  }
   
}




