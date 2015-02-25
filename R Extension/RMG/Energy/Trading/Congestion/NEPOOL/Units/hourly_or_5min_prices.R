# What's best, 5min prices or hourly prices for revenues?
#
#
#



#####################################################################
#
.get_hist_data <- function(startDt, endDt)
{
  symbols <- c(
   "nerpt_oi_unit_1691_curMW",         "mw5m",
   "nepool_smd_rt5m_1691_lmp",         "p5m"  
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH5m <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH5m) <- symbols[,"shortNames"]
  head(HH5m)

  HH5m <- data.frame(time=index(HH5m), HH5m)
  HH5m$hour <- as.POSIXct(format(HH$time-1, "%Y-%m-%d %H:00:00")) + 3600
  HH5m$pq <- HH5m$mw5m * HH5m$p5m
  rev5m <- cast(HH5m, hour ~ ., mean, na.rm=TRUE, value="pq")
  colnames(rev5m)[ncol(rev5m)] <- "rev5m"
  rev5m$month <- as.Date(format(rev5m$hour-1, "%Y-%m-%01"))
  agg5m <- cast(rev5m, month ~ ., sum, na.rm=TRUE, value="rev5m")
  colnames(agg5m)[ncol(agg5m)] <- "rev5m"

  
  HH1h <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt,
    aggHourly=c(TRUE, TRUE))
  colnames(HH1h) <- symbols[,"shortNames"]
  head(HH1h)

  HH1h <- data.frame(time=index(HH1h), HH1h)
  HH1h$month <- as.Date(format(HH1h$time-1, "%Y-%m-%01"))
  HH1h$pq <- HH1h$mw5m * HH1h$p5m
  rev1h <- cast(HH1h, month ~ ., sum, na.rm=TRUE, value="pq")
  colnames(rev1h)[ncol(rev1h)] <- "rev1h"
 

  rev <- merge(agg5m, rev1h)
  rev$diff <- rev$rev5m - rev$rev1h
  print(rev, row.names=FALSE)                 # <- this is the result

  
  aux <- melt(rev, id=1)
  require(lattice)
  xyplot(value/1000000 ~ month, data=aux, groups=aux$variable, type="o")

  
 
}




#####################################################################
#####################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Units/hourly_or_5min_prices.R")

  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  startDt  <- as.POSIXct("2011-01-01 00:00:01")   # for hist data
  endDt    <- Sys.time()                          # for hist data 



  
  
}
