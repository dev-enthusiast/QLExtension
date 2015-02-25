# Construct historical load simulations using historical temperatures.
# Where: temp.historical.scenarios - matrix of historical temperatures,
#                                    size [no.years 366]
#        reg.LT - the regression coefficients from the load vs. temp
#        reg.DT - the linear regression for load detrending 
# Returns: hs.load - load by day, for all 40+ historical sims
#
# Last modified by Adrian Dragulescu on 19-Feb-2004

hist.simulate.load <- function(temp.hist.scenarios, reg.LT, reg.DT, options){

  startDate <- posix2chron(options$forecast$startDate)
  endDate   <- posix2chron(options$forecast$endDate)

  yrDD <- substr(as.character(years(startDate)),3,4)
  start.year  <- chron(paste("1/1/",yrDD,sep=""))
  end.year    <- chron(paste("12/31/",yrDD,sep=""))
  start.years <- seq.dates(start.year, endDate, by="years")
  end.years   <- seq.dates(end.year, by="year", length=length(start.years))  
  
  f.days <- seq.dates(startDate,endDate, by="days") # forecast days
  wd  <- which(as.numeric(is.weekend(f.days))==0)
  we  <- which(as.numeric(is.weekend(f.days))==1)
  YMD <- ymd(as.Date(f.days), rep=1)    # table of year month day
  no.days <- length(f.days)    # number of deal days 
  uYears  <- unique(YMD$year)  # unique years

  #-------------Select which days are needed in each year--------
  days.in.year=NULL
  if (length(uYears)==1){      # for deals shorter than 1 year
    aux <- (startDate-start.year+1):(endDate-start.year+1)
    days.in.year[[1]] <- aux
  } else {                     # for deals longer  than 1 year
    aux <- (startDate-start.year[1]+1):(end.years[1]-start.year[1]+1)
    days.in.year[[1]] <- aux
    if (length(uYears)>2){
      for (yr in 2:(length(uYears)-1)){
        aux <- 1:(end.years[yr]-start.years[yr]+1)
        days.in.year[[yr]] <- aux
      }  
    }
    aux <- 1:(endDate-start.years[length(uYears)]+1)
    days.in.year[[length(uYears)]] <- aux     
  }

  #-------Construct the historical temperature simulations-------
  no.hist.years <- dim(temp.hist.scenarios)[1]
  no.hist.sim <- no.hist.years-length(uYears)+1
  hT=NULL
  for (n in (1:no.hist.sim)){
    aux=NULL
    for (yr in 1:length(uYears)){
      aux <- c(aux, temp.hist.scenarios[n+yr-1,days.in.year[[yr]]])
    }
    hT <- cbind(hT, aux)
  }
  colnames(hT) <- 1:no.hist.sim

  #------Calculate historically simulated loads------------------
  hs.load=matrix(NA, nrow=no.days, ncol=no.hist.sim)
  uMonths=unique(YMD$month)
  for (m in uMonths){
    ind <- which(YMD$month==m)
    indWD <- intersect(wd,ind)
    indWE <- intersect(we,ind)
    for (s in 1:no.hist.sim){
      hs.load[indWD,s] <- 24*predict(reg.LT$WD[[m]],
                                 data.frame(temp=hT[indWD,s]))
      hs.load[indWE,s] <- 24*predict(reg.LT$WE[[m]],
                       data.frame(temp=hT[indWE,s]))      
    }  
  } # end loop over month
  hs.load <- cbind(YMD, hs.load)

  if (options$detrend){
    data  <- hs.load[,c("year","month","day",4)]
    colnames(data)[4] <- "x"
    data  <- add.trend(data, reg.DT, options)  # add the trend
    adder <- data$adder       # adder identical for all historical sims
    for (s in 1:no.hist.sim){hs.load[,3+s] <- hs.load[,3+s] + adder}
  }
  
  return(hs.load)
} # end of function




# monthF <- factor(hs.load$month, labels=month.abb[unique(hs.load$month)])
# xyplot( hs.load$"1" ~ hs.load$day | monthF,
#        panel=function(x,y){
#          hF <- factor(x)
#          my <- tapply(y,hF,mean)
#          panel.xyplot(x,y, col="gray")
#          panel.xyplot(levels(hF),my, type="l", col="blue", lwd=2)
#        }, 
#        col="grey", pch=1, bg="white", 
#        xlab="Hour", ylab="Weekday Load, MW",
#        main=paste("Forecasted",options$loadName,"load"))
