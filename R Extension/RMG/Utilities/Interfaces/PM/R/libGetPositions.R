# A set of utilities to get positions from PRISM for PM
#
# .cleanDeliveryPt
# .compareTwoPositionsFile
# .getNYPPPositions
# plotPositionsZone
#
#

#################################################################
# Convert delivery_point info as you see it in BlackBird or the
# trade screen to the associated location name.
#
.cleanDeliveryPt <- function(x, region="NEPOOL")
{
  z <- x     # make a copy

  if (region=="NEPOOL"){
    # remove MCC prefixes
    z <- gsub("^MCC ", "", z)
    # remove trailing RT or DA strings
    z <- gsub(" RT| DA$", "", z)
  }
  if (region=="NYPP"){
    # remove leading ART, DAM
    z <- gsub("^(ART|DAM) ", "", z)
    # remove leading MCC
    z <- gsub("^MCC ", "", z)
  }
    
  z
}

#################################################################
# Compare two position files from two different days, and produce
# a simple report for emailing.  Good to track day-to-day changes,
# silly mappings issues, etc. 
# You can pass on what portfolio/books you want to compare!
#
.compareTwoPositionsFiles <- function(file1, file2, portfolio.book=NULL,
   sink=FALSE, by=c("eti", "portfolio"), service=NULL)
{
  out <- NULL
  
  day1 <- as.Date(substr(file1, nchar(file1)-15, nchar(file1)-6))
  load(file1)
  QQ1 <- QQ
  names(QQ1) <- gsub("_", ".", tolower(names(QQ1)))
  QQ1$asOfDate1 <- day1
  if (!is.null(service)){
    this.service <- service
    QQ1 <- subset(QQ1, service == this.service)
  }

  day2 <- as.Date(substr(file2, nchar(file2)-15, nchar(file2)-6))
  load(file2)
  QQ2 <- QQ
  names(QQ2) <- gsub("_", ".", tolower(names(QQ2)))
  QQ2$asOfDate2 <- day2
  if (!is.null(service)){
    this.service <- service
    QQ2 <- subset(QQ2, service == this.service)
  }
  
  if (!is.null(portfolio.book)){
    QQ1 <- subset(QQ1, portfolio %in% portfolio.book)
    QQ2 <- subset(QQ2, portfolio %in% portfolio.book)
  }

  eti1 <- unique(QQ1[, c("asOfDate1", by)])
  eti2 <- unique(QQ2[, c("asOfDate2", by)])
  eti <- merge(eti1, eti2, all=TRUE, by=by)

  if (sink){
    if (file.exists("msg.txt")) unlink("msg.txt")
    sink("msg.txt")
  }
  
  rLog("Comparing days:", as.character(day1), "to", as.character(day2), "\n")
  ind <- which(is.na(eti$asOfDate1))
  if (length(ind)>0){
    rLog("\n--------------------------------------------------")
    rLog(length(ind), "trades came in:", eti$eti[ind])
    etis <- eti$eti[ind]
    aux  <- subset(QQ2, eti %in% etis)
    print(unique(aux[,c(by, "curve", "trade.start.date",
      "trade.end.date", "delivery.point", "notional")]), row.names=FALSE)
  }
  
  ind <- which(is.na(eti$asOfDate2))
  if (length(ind)>0){
    rLog("\n--------------------------------------------------")
    rLog(length(ind), "trades went away:", eti$eti[ind])
    etis <- eti$eti[ind]
    aux  <- subset(QQ1, eti %in% etis)
    print(unique(aux[,c(by, "curve", "trade.start.date",
      "trade.end.date", "delivery.point", "notional")]), row.names=FALSE)
  }
  
  if (sink){
    sink()
    out <- readLines("msg.txt") 
  }
  
  return(invisible(out))
}

#################################################################
# Get NYPP positions from NEPOOL ACCRUAL PORTFOLIO
#
.getNYPPpositions <- function(asOfDate=Sys.Date()-1)
{
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  fileIn <- paste(dir, "positionsAAD_", asOfDate, ".RData", sep="")
  if (file.exists(fileIn)){
    load(fileIn)
  } else {
    stop("File", fileIn, "does not exist!")
  }

  filter <- ( QQ$mkt=="PWY"
             & QQ$contract.dt >= as.Date("2009-05-01")
             & QQ$contract.dt <= as.Date("2009-11-01")
             & QQ$portfolio == "NEPOOL ACCRUAL PORTFOLIO"
             & QQ$trading.book %in% c("CONGIBT", "NPFTRMT1")
            )
             
  TT  <- subset(QQ, filter)             
  TT  <- aggregate(TT$delta, list(TT$delivery.point, TT$contract.dt), sum)
  names(TT) <- c("deliv.pt", "contract.dt", "value")
  TT$deliv.pt <- .cleanDeliveryPt(TT$deliv.pt, region="NYPP")
  TT <- subset(TT, abs(value)>1)

  
  load("S:/All/Risk/Data/FTRs/NYPP/ISO/DB/RData/map.RData")

  aux <- merge(TT, MAP[,c("deliv.pt", "zone")], all.x=TRUE)
  aux <- cast(aux, zone + deliv.pt ~ contract.dt,
              function(x){round(sum(x))}, fill=NA)

  
  aux <- cast(aux, contract.dt + zone ~ ., sum)
  names(aux)[3] <- "value"
  aux <- subset(aux, abs(value)>0)
  aux <- cast(aux, contract.dt ~ zone, function(x){round(sum(x))})
  
  
}



#################################################################
# Aggregate a dataframe to reporting months:
# Keep the first 4 months, and then Cal Years
aggregateByReportingPeriod <- function(X, variable="contract.dt",
  asOfDate=Sys.Date())
{
  X[,variable] <- as.Date(X[,variable])  # in case you pass POSIX
  aux <- X
  aux$period <- format(X[, variable], "Cal%y")
  aux[, variable] <- NULL
  resCal <- cast(aux, ... ~ ., sum, fill=NA)
  ind <- resCal$period == paste("Cal", format(asOfDate, "%y"), sep="")
  resCal[ind,"period"] <- paste("Bal", format(asOfDate, "%y"), sep="")
  names(resCal)[ncol(resCal)] <- "value"
  
  maxMonth <- seq(as.Date(format(asOfDate, "%Y-%m-01")), by="4 months",
                  length.out=2)[2]
  aux <- subset(X, get(variable) <= maxMonth)
  resMon <- aux
  resMon$period <- format(aux[,variable], "%Y-%m")
  resMon[,variable] <- NULL
  
  return(rbind(resMon, resCal))    
}

##############################################################################
# Low level view, of longs & shorts by delivery point
#
getPositionsDeliveryPoint <- function(QQ, subset=TRUE, buckets="peak/offpeak",
  mw=FALSE)
{
  if (!("value" %in% names(QQ))){
    rLog("No value column.  Will use delta column for aggregation.")
    if (!("delta" %in% names(QQ)))
      stop("Cannot find column delta in positions data.frame!")
    QQ$value <- QQ$delta
  }
  if (!all(c("mkt", "delivery.point", "contract.dt", "bucket") %in%
     names(QQ)))
    stop("Missing either mkt, delivery.point, contract.dt, or bucket from QQ.")

  # aggregate deltas by delivery point.
  res <- cast(QQ, mkt + delivery.point + contract.dt + bucket  ~., sum,
              subset=subset, fill=NA)
  
  res$delivery.point <- .cleanDeliveryPt(res$delivery.point)
  res$delivery.point[res$delivery.point == "PTF"] <- "HUB"
  
  # do one more aggregation ...
  res <- cast(res, mkt + delivery.point + contract.dt + bucket  ~., sum)
  names(res)[ncol(res)] <- "value"

  if (buckets[1] == "peak/offpeak"){
    res <- combineBuckets(res, weight="none", buckets=c("2X16H", "7X8"),
                          name="OFFPEAK")
    res[regexpr("5X16", res$bucket)>0,"bucket"] <- "PEAK"
  }

  if (mw){
    HRS <- ntplot.bucketHours(unique(res$bucket),
      startDate=min(res$contract.dt), endDate=max(res$contract.dt))
    names(HRS)[2:3] <- c("contract.dt", "hours")
    res <- merge(res, HRS, all.x=TRUE)
    res$mwh   <- res$value              # return it for reference
    res$value <- res$value/res$hours    # the mw
  }

  res <- subset(res, value != 0)
  
  return(data.frame(res))
}

##############################################################################
# High-level view, by mkt, zone, bucket
# Change the buckets to "PEAK" and "OFFPEAK"
# MAP is the NEPOOL$MAP or NYPP$MAP
#
getPositionsZone <- function(QQ, MAP, subset=TRUE, buckets="peak/offpeak",
  mw=FALSE, showDeliveryPoint=FALSE)
{
  res <- getPositionsDeliveryPoint(QQ, subset=subset, buckets=buckets, mw=mw)

  aux <- merge(res, MAP[,c("deliv.pt", "zone.id", "ptid")], by.x="delivery.point",
               by.y="deliv.pt", all.x=TRUE)
  aux[is.na(aux$zone.id), "zone.id"] <- aux[is.na(aux$zone.id), "delivery.point"]

  ZN <- data.frame(zone.id=as.character(c(4000:4008, 4010, 4011, 4013, 4017)), 
    zone.name=c("Hub", "MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA",
                 ".Sal", ".Ros", ".Hig", ".Nrt"))
  aux <- merge(aux, ZN, all.x=TRUE)
  aux[is.na(aux$zone.name), "zone.name"] <- aux[is.na(aux$zone.name), "zone.id"]

  # this is how delivery points are mapped in SecDb; these are some custom nodes
  # I should pull this from SecDb ... TODO!
  if (any(aux$zone.name=="CL8"))
    aux[aux$zone.name=="CL8", "zone.name"] <- "RI"   
  if (any(aux$zone.name=="MAINE-SC"))
    aux[aux$zone.name=="MAINE-SC", "zone.name"] <- "SC"
  if (any(aux$zone.name=="KLN"))
    aux[aux$zone.name=="KLN", "zone.name"] <- "CT"
  if (any(aux$zone.name=="RUMFORD.AZIS"))
    aux[aux$zone.name=="RUMFORD.AZIS", "zone.name"] <- "MAINE"
  

  if (showDeliveryPoint){
    aux <- cast(aux[, c("zone.name", "delivery.point", "contract.dt", "bucket",
      "value")], ... ~ ., sum, fill=NA)
  } else {
    aux <- cast(aux[, c("zone.name", "contract.dt", "bucket", "value")], ... ~ .,
              sum, fill=NA)
  }
  names(aux)[ncol(aux)] <- "value"

  return(data.frame(aux))
}

##############################################################################
# 
plotPositionsZone <- function(QQ, plotName="", maxPlotsPage=8, scale=1000, ...)
{

  if (!("package:lattice" %in% search()))
    require(lattice)
  
  if ("contract.dt" %in% names(QQ))
    QQ$period <- QQ$contract.dt
  
  if (!all(c("zone.name", "bucket", "period", "value") %in% names(QQ)))
    stop("Missing either zone.id, period, bucket, or value from QQ.")

  
  aux <- QQ
  aux$zone.id <- factor(aux$zone.name)
  aux$sign    <- sign(aux$value)
  
  noPlots <- nrow(unique(aux[,c("period", "bucket")]))  # how many panels 
  noPages <- ceiling(noPlots/maxPlotsPage)
  
  filename <- paste(getwd(), "/", plotName, "NepoolPositions.pdf", sep="")
  # open the device
  trellis.device(pdf, file=filename, width=11, height=8.5)  
  print(barchart(zone.id ~ value/scale | bucket*period,
    groups=sign, data=aux,
#    subset=period <= as.Date("2009-05-01"), 
    stack=TRUE, panel = function(...){
      panel.grid(h=-length(unique(aux$zone.id)), v=0)
      panel.barchart(...)},
    layout=c(maxPlotsPage/2,2),        
    col=c("red", "green"), scales="free",
    xlab=paste(scale,"'s MWh", sep=""), ... 
  ))
  cat(paste("Wrote the plots to", filename, "\n"))
  dev.off()
  
}


## #################################################################
## # Next 4 months, BalYear, Cal
## # a similar version is in H:\user\R\RMG\Energy\VaR\PE\PEUtils.R
## .makeReportingMonths <- function(x, asOfDate=Sys.Date())
## {
##   bom       <- as.Date(format(asOfDate, "%Y-%m-01"))   # beginning of the month
##   boy       <- as.Date(format(asOfDate, "%Y-%01-01"))  # beginning of the year
##   start.qtr <- seq(boy, by="year", length.out=4)[4]
##   start.cal <- seq(boy, by="year", length.out=6)[6]
##   pricing.dts.qtr <- pricing.dts.cal <- NULL

##   next.mo <- seq(bom, by="month", length.out=2)[2]
##   pricing.dts.mo <- seq(next.mo, start.qtr, by="month")
##   pricing.dts.mo <- pricing.dts.mo[-length(pricing.dts.mo)]

##   # go by quarter after the first 3 years, quarterly for the next 2
##   if (maxDate >= start.qtr){
##     pricing.dts.qtr <- seq(start.qtr, by="3 months", length.out=8)
##     pricing.dts.qtr <- pricing.dts.qtr[pricing.dts.qtr <= maxDate]    
##   }
##   # go by year after the first 5 years
##   if (maxDate >= start.cal){
##     pricing.dts.cal <- seq(start.cal, maxDate, by="year")
##     pricing.dts.cal <- pricing.dts.cal[pricing.dts.cal <= maxDate]    
##   }
  
##   return(c(pricing.dts.mo, pricing.dts.qtr, pricing.dts.cal))  
## }
