#  Example on how to manipulate data, etc.
#
#
#
#
  
#######################################################################
#
.get_hist_data <- function(startDt, endDt) {
  
  symbs <- 
    c("nepool_load_hist",       "load",
      "nepool_smd_da_4000_lmp", "hub_da")
  symbs <- matrix(symbs, byrow=TRUE, ncol=2)
  
  res <- FTR:::FTR.load.tsdb.symbols(symbs[,1], startDt, endDt)
  colnames(res) <- symbs[,2]
  head(res)  
  
  data.frame(time=index(res), res)    
}


#######################################################################
#######################################################################
#
.main <- function() {
  
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(plyr)
  require(reshape)
  
  source("H:/user/R/RMG/_Learning/2013-06-07_load_gym.R")
  
  startDt <- as.POSIXct("2010-01-01 01:00:00")
  endDt   <- as.POSIXct(paste(format(Sys.Date()), "00:00:00"))

  HH <- .get_hist_data(startDt, endDt)
  head(HH)
  
  
  HH$year <- as.numeric(format(HH$time-1, "%Y"))
  HH$mon <- format(HH$time-1, "%b")
  HH$month <- as.Date(format(HH$time-1, "%Y-%m-01"))
  HH$isWeekend <- ifelse(weekdays(HH$time-1) %in% c("Saturday", "Sunday"), TRUE, FALSE)
  HH$bucket <- PM:::addBucket(HH$time, buckets=c("5X16", "2X16H", "7X8"))  # add buckets
  
  # avg by month, weekday
  cast(HH, mon + isWeekend ~ ., mean, value="hub_da")
  cast(HH, mon ~ isWeekend, mean, value="hub_da")
  
  cast(HH, mon ~ isWeekend, function(x){round(mean(x),2)}, value="hub_da")
  
  MM <- cast(HH, month + bucket ~ ., mean, value="hub_da")
  colnames(MM)[3] <- "hub_da"     
  
  
  require(lattice)
  
  xyplot(hub_da ~ month | bucket, data=MM, type="l")
  
  xyplot(hub_da ~ month, groups=bucket, data=MM, type=c("g", "o"))
  
  
  aux <- ddply(HH, c("month", "bucket"), function(x){
    c(load=mean(x$load), hub_da=mean(x$hub_da))
    #browser()
  })
  
  
  
}


