# Three functions to deal with detrending.  The estimation of the
# trend is done on monthly quantities.  After detrending, the load
# is leveled to the first historical month (not to the last).  It
# should not matter because it is always done in pairs remove/add.trend. 
#
# Given a dataframe with columns:
#      data = [year month day hour x]     column "hour" is optional
#
# Written by Adrian Dragulescu on 13-Sep-2004

#---------------------------------------------------------
remove.trend <- function(data, save, options){
# Works on the historical data
  
 R <- nrow(data)
 aux1 <- paste(data$year[1],data$month[1],data$day[1],sep="-")
 aux2 <- paste(data$year[R],data$month[R],data$day[R],sep="-")
 rangeMths <- seq(as.Date(aux1), as.Date(aux2), "month")

 aux <- as.Date(paste(data$year,data$month,"1", sep="-"))
 histMths  <- unique(aux)
 ind <- which(is.element(rangeMths,histMths)==FALSE)
 rangeInd <- 1:length(rangeMths)
 if (length(ind)!=0){rangeInd <- rangeInd[-ind]} # if missing months

 options$one.column <- 1
 mth.Load    <- summarize.monthly(data, options)
 options$one.column <- 0
 reg.DT <- lm(mth.Load$x ~ rangeInd)  # detrending regression

 no.Months   <- length(histMths)
 slope       <- as.numeric(coefficients(reg.DT)[2])
 x.detrended <- data$x
 aux         <- as.character(aux)
 for (mth in 1:no.Months){
   str <- paste(substr(as.character(histMths[mth]),1,7),"-01",sep="")
   ind <- which(aux==str)
   x.detrended[ind] <- data$x[ind]-slope*rangeInd[mth]/length(ind)
 }
 if (save$Analysis$all){plot.hist.trend(reg.DT,save,options)}
 
 return(list(reg.DT, x.detrended))
} 

#---------------------------------------------------------
add.trend <- function(data, reg.DT, options){
# add a trend to a weather normalized forecasted load
  rangeMths <- seq(as.Date(options$historical$startDate),
                   as.Date(options$forecast$endDate), by="month")
  forecastMths <- seq(as.Date(options$forecast$startDate),
                   as.Date(options$forecast$endDate), by="month")
  rangeInd <- 1:length(rangeMths)
  ind <- which(is.element(rangeMths,forecastMths)==TRUE)
  rangeInd <- rangeInd[ind]     # pick only the forecast months

  no.Months    <- length(forecastMths) 
  if (tolower(options$forecast$loadgrowth)=="historical"){
     slope <- as.numeric(coefficients(reg.DT)[2])
  } else {
     slope <- options$forecast$loadgrowth/12  # monthly
  } 
  adder <- array(0, dim=nrow(data$x))
  aux   <- as.Date(paste(data$year,data$month,"1", sep="-"))
  aux   <- as.character(aux)

  for (mth in 1:no.Months){
   str <- paste(substr(as.character(forecastMths[mth]),1,7),"-01",sep="")
   ind <- which(aux==str)
   eom <- seq(forecastMths[mth], by="month", length=2)[2] 
   no.days.in.mth <- as.numeric(eom - forecastMths[mth]) # if missing data
   if (length(intersect(colnames(data),"hour"))==1){
      no.days.in.mth <- 24*no.days.in.mth}   # for hourly time series
   adder[ind] <- slope*rangeInd[mth]/no.days.in.mth
  }
  data$x <- data$x + adder
  data   <- cbind(data, adder)
  return(data)
}

#---------------------------------------------------------
plot.hist.trend <- function(reg.DT, save, options){

  mth.Load <- reg.DT$model$mth.Load
  rangeInd <- reg.DT$model$rangeInd
  dateMth <-  seq(as.Date(options$historical$startDate),
                  as.Date(options$historical$endDate), "month")
  dateMthPOS <- strptime(as.character(dateMth), format="%Y-%m-%d")
  L <- length(as.character(dateMthPOS))

  main=paste("Historical ",options$loadName,sep="")
  fileName <- paste(save$dir$plots,"histload_trend.pdf", sep="")
  if (save$Analysis$all){pdf(fileName, width=7.25, heigh=3.5)}
  plot(dateMthPOS, mth.Load, xlab="", xaxt="n", col="blue",
       ylab="Monthly Load, MWh")
  title(main)
  axis.POSIXct(1, dateMthPOS, at=dateMthPOS[seq(1,L, by=2)],
               format="%b%y", las=2)
  lines(dateMthPOS, predict(reg.DT, data.frame(rangeInd=rangeInd)), col="gray")
  if (save$Analysis$all){dev.off()}

  caption <- "Linear regression of monthly load data vs.\\ index of month: "
  formula     <- "lm(formula = mth.Load\\$x \$\\sim \$ rangeInd)"
  caption <- paste(caption, formula,".  The load growth ", sep="")
  aux <- summary(reg.DT)$coefficients; aux <- aux["rangeInd","Pr(>|t|)"]
  if (aux>0.05){yesno <- "is not"}else{yesno <- "is"}
  growthRate <- 100*12*coefficients(reg.DT)[2]/coefficients(reg.DT)[1]
  growthRate <- signif(as.numeric(growthRate),2)
  caption <- paste(caption, yesno, " statistically significant at the 95\\% significance level.  The annualized load growth rate is ", growthRate, "\\%.", sep="")
  if (!options$detrend){yesno <- "not"}else{yesno <- ""}
  caption <- paste(caption, "For RMG analysis, historical data has", yesno, "been detrended.")
  fileName <- paste(save$dir$Rtables,"trendReg.tex", sep="")
  print.xtable(xtable(reg.DT, caption=caption), file=fileName)

}



