# Simple stats for DARTs
#
# .aggregate_month_bucket
# .format_mon_year_table
# .get_hist_data
# 
#


#######################################################################
# returns a list[bucket]
#
.aggregate_month_bucket <- function(HH)
{
  TT <- NULL
  
  flat <- aggregate(HH$dart, as.Date(format(index(HH), "%Y-%m-01")),
                    mean)
  TT[["7x24"]] <- flat


  # get the Onpeak bucket
  ind <- secdb.getBucketMask("NEPOOL", "5X16", startTime, endTime)
  ind <- ind[which(ind[,2]),]
  ind <- zoo(ind[,2], ind[,1])
  PP  <- merge(HH, ind, all=FALSE)
  
  peak <- aggregate(PP$dart, as.Date(format(index(PP), "%Y-%m-01")),
                    mean)
  TT[["5x16"]] <- peak
  
  # get the 7x8 bucket
  ind <- secdb.getBucketMask("NEPOOL", "7X8", startTime, endTime)
  ind <- ind[which(ind[,2]),]
  ind <- zoo(ind[,2], ind[,1])
  PP  <- merge(HH, ind, all=FALSE)
  
  overnight <- aggregate(PP$dart, as.Date(format(index(PP), "%Y-%m-01")),
                    mean)
  TT[["7x8"]] <- overnight


  TT  
}


#######################################################################
# x is a zoo time series
#
.email <- function(TT)
{
  out <- lapply(TT, .format_mon_year_table, digits=2)
  
  rLog("Email results")
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
   "NECash@constellation.com",
#  "adrian.dragulescu@constellation.com", 
    "Historical DART payoff by bucket", capture.output(print(out)))

}

#######################################################################
# x is a zoo time series
#
.format_mon_year_table <- function(x, digits=2)
{
  y <- data.frame(index=index(x), value=as.numeric(x))
  y$month  <- factor(format(y$index, "%b"), levels=month.abb)
  y$year   <- format(y$index, "%Y")
  y$value  <- round(y$value, digits)
  
  out <- cast(y, month ~ year, I, fill=NA)

  out
}

#######################################################################
#
.get_dart_hist <- function(startTime, endTime, ptid=c(4000))
{
  symbols <- NULL
  if (4000 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
      "NEPOOL_SMD_RT_4000_lmp",         "hub.rt"
    )
  if (4001 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4001_lmp",         "me.da",
      "NEPOOL_SMD_RT_4001_lmp",         "me.rt"
    )
  if (4002 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4002_lmp",         "nh.da",
      "NEPOOL_SMD_RT_4002_lmp",         "nh.rt"
    )
  if (4003 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4003_lmp",         "vt.da",
      "NEPOOL_SMD_RT_4003_lmp",         "vt.rt"
    )
  if (4004 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4004_lmp",         "ct.da",
      "NEPOOL_SMD_RT_4004_lmp",         "ct.rt"
    )
  if (4005 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4005_lmp",         "ri.da",
      "NEPOOL_SMD_RT_4005_lmp",         "ri.rt"
    )
  if (4006 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4006_lmp",         "sema.da",
      "NEPOOL_SMD_RT_4006_lmp",         "sema.rt"
    )
  if (4007 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4007_lmp",         "wma.da",
      "NEPOOL_SMD_RT_4007_lmp",         "wma.rt"
    )
  if (4008 %in% ptid)
    symbols <- c(symbols, 
      "NEPOOL_SMD_DA_4008_lmp",         "nema.da",
      "NEPOOL_SMD_RT_4008_lmp",         "nema.rt"
    )



  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startTime, endTime)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  HH <- na.omit(HH)
  HH$dart.hub <- HH$hub.da - HH$hub.rt
  
  HH
}




##########################################################################
##########################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(plyr)
  require(lattice)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.dart.R")
  #source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.reserves.R")

  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  startTime <- as.POSIXct("2006-01-01 01:00:00")   # for hist data
  #startTime <- as.POSIXct("2014-01-01 01:00:00")   # for hist data
  endTime   <- Sys.time()                          # for hist data 

  HH <- .get_dart_hist(startTime, endTime, ptid=4000:4008)
  print(tail(HH), row.names=FALSE)

  
  # dart by month 
  aux <- PM:::filterBucket(HH[, "dart.hub", drop=FALSE], buckets=c("5X16", "2X16H", "7X8"))
  MM <- lapply(aux,
    function(x){aggregate(x, as.Date(format(index(x)-1, "%Y-%m-01")), mean, na.rm=TRUE)}) 
  dart <- lapply(MM, .format_mon_year_table)
  dart <- melt(dart)
  colnames(dart)[ncol(dart)] <- "bucket"
  dart <- dart[,c("bucket", "year", "month", "value")]
  cast(dart, bucket + year ~ month, I, value="value", fill=NA)
  # interesting plots!
  plot(cumsum(aux[["7X8"]]), ylab="cumsum(7x8 dart)", col="blue")
  plot(cumsum(aux[["5X16"]]), ylab="cumsum(5x16 dart)", col="blue")
  plot(cumsum(aux[["2X16H"]]), ylab="cumsum(2x16H dart)", col="blue")


  # plot of the HE dart by hours ...
  aux <- data.frame(datetime=index(HH),
                    HE=as.POSIXlt(index(HH))$hour,
                    value=as.numeric(HH[,"dart.hub"]))
  aux$HE[which(aux$HE == 0)] <- 24                         # from HE 1 to HE 24
  aux$day <- as.Date(format(aux$datetime - 1, "%Y-%m-%d"))
  aux <- subset(aux, day >= as.Date("2014-12-03"))
  bux <- ddply(aux, c("HE"), function(x){
    data.frame(day=x$day, value=cumsum(x$value)) })  
  xyplot(value ~ day, data=bux, groups=bux$HE, type="l")   # interesting
  last <- bux[which(bux$day==as.Date("2014-12-27")),]
  last[order(last$value),]
  # in the long run it's a good strategy to inc on HE 23 and 24, dec HE 17


  

  # the big picture, dart by year!
  aux <- PM:::filterBucket(HH[, "dart.hub", drop=FALSE], buckets=c("7X24"))
  YY <- lapply(aux,
    function(x){aggregate(x, as.Date(format(index(x)-1, "%Y-01-01")), mean, na.rm=TRUE)}) 
  print(YY)

  
  
  res <- cast(subset(dart, month=="Aug"), bucket ~ year, I, fill=NA,
              subset=year!=2011)
  print(res, row.names=FALSE)


  

  
  #res <- .aggregate_month_bucket(HH)
  .format_mon_year_table(HH)
  
  
  

  
}


  
  ## aux <- mapply(function(x, y){
  ##   cbind(time=index(x), bucket=y, data.frame(x))
  ##   }, MM, names(MM), SIMPLIFY=FALSE)
  ## lapply(aux, head)
