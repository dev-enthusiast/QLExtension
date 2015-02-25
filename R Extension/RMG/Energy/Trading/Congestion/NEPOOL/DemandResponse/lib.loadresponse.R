# Library to deal with loadresponse program in nepool
#
# analysis_druplift
# backast_costs         - use the regression to see how it does
# calculate_marks       - using current fwd curve and new regression
# calc_pnl_impact      
# forecast_costs         
# .pull_hist_druplift   
# .pull_marks_druplift  - pull SecDb mark for DRUplift
# .get_hist_energy    - pool mwh by month
# .extract_table_F    - from the pdf
#
# read_SD_LRSTLDETAIL - read the settlement detail report, from ISO
#
#
# The new ISO reports:
#   SD_DRSTLDETAIL and SD_DRCHDDETAIL
# 
# Summary pool data is in the "Monthly Markets Reports"
# Keep track of the model performance in 
# S:\All\Structured Risk\NEPOOL\Ancillary Services\DemandResponse/cost_tracker.xlsx
#
#

#######################################################################
# Do the loess regression between log(payment) ~ log(hub.rt)
# @param startDt an R Date
# @param endDt an R Date
# @return reg the loess regression
# 
analysis_druplift <- function(startDt, endDt)
{
  require(lattice)
  
  DR <- .pull_hist_druplift(startDt, endDt)
  DR$date <- as.Date(format(DR$datetime-1, "%Y-%m-%d"))

  # aggregate by day
  DD <- ddply(DR, .(date), function(x){
    c(apply(x[,c("hub.da", "hub.rt")], 2, mean),
      apply(x[,c("Pool.Total.Payment"), drop=FALSE], 2, sum )) 
  })

  DD <- subset(DD, Pool.Total.Payment > 0) 

  rLog("Regress daily total pool costs vs. peak rt price")
  reg <- loess( log(Pool.Total.Payment, 10) ~ log(hub.rt, 10),
    data=DD, span=2/3, degree=1, family="symmetric")
  

  logTicks <- function (lim, loc = c(1, 5)) {
      ii <- floor(log10(range(lim))) + c(-1, 2)
      main <- 10^(ii[1]:ii[2])
      r <- as.numeric(outer(loc, main, "*"))
      r[lim[1] <= r & r <= lim[2]]
  }
  
  xscale.components.log10 <- function(lim, ...) {
    ans <- xscale.components.default(lim = lim, ...)
    tick.at <- logTicks(10^(lim+0.1), loc = 1:9)
    tick.at.major <- logTicks(10^lim, loc = 1)
    major <- tick.at %in% tick.at.major
    ans$bottom$ticks$at <- log(tick.at, 10)
    ans$bottom$ticks$tck <- ifelse(major, 1.5, 0.75)
    ans$bottom$labels$at <- log(tick.at, 10)
    ans$bottom$labels$labels <- as.character(tick.at)
    #ans$bottom$labels$labels[!major] <- ""
    ans$bottom$labels$check.overlap <- FALSE
    ans
  }

  yscale.components.log10 <- function(lim, ...) {
    ans <- yscale.components.default(lim = lim, ...)
    tick.at <- logTicks(10^(lim+0.1), loc = 1:9)
    tick.at.major <- logTicks(10^lim, loc = 1)
    major <- tick.at %in% tick.at.major
    ans$left$ticks$at <- log(tick.at, 10)
    ans$left$ticks$tck <- ifelse(major, 1.5, 0.75)
    ans$left$labels$at <- log(tick.at, 10)
    ans$left$labels$labels <- as.character(tick.at)
    ans$left$labels$labels[!major] <- ""
    ans$left$labels$check.overlap <- FALSE
    ans
  }  

  windows()
  print(xyplot(log10(Pool.Total.Payment) ~ log10(hub.rt), data=DD,
    panel = function(x, y, ...) {
      panel.grid()
      panel.rug(x, y, ...)
      panel.points(x, y, ..., cex=1.1)
      panel.loess(x, y, ..., lwd=2)
    },      
    col=c("steelblue"),
    col.line="gray", main=paste(""),
    xlab="RT Onpeak, $/MWh",
    ylab="Pool DR Uplift payment, $",
    xscale.components=xscale.components.log10,
    yscale.components=yscale.components.log10               
  ))

  
  reg
}


#######################################################################
# Backcast the costs
#
backcast_costs <- function( reg, startDt=as.Date("2012-06-01"),
  endDt=Sys.Date() )
{
  DR <- .pull_hist_druplift(startDt, endDt)
  DR$date <- as.Date(format(DR$datetime-1, "%Y-%m-%d"))

  # aggregate by day
  DD <- ddply(DR, .(date), function(x){
    c(apply(x[,c("hub.da", "hub.rt")], 2, mean),
      apply(x[,c("Pool.Total.Payment"), drop=FALSE], 2, sum )) 
  })

  
  # predict the cost using the regression
  yP <- predict(reg, data.frame(hub.rt=DD$hub.rt), se=TRUE)
  DD$predictedPoolPayment <- 10^(yP$fit)
  DD$predictedPoolPaymentBandUp   <- 10^(yP$fit + 1.95*yP$se.fit)
  DD$predictedPoolPaymentBandDown <- 10^(yP$fit - 1.95*yP$se.fit)

  
  rLog("Aggregate by month")
  DD$month <- as.Date(format(DD$date, "%Y-%m-01"))
  MM <- ddply(DD, .(month), function(x){
      apply(x[,c("Pool.Total.Payment", "predictedPoolPaymentBandDown",
        "predictedPoolPayment", "predictedPoolPaymentBandUp"),
         drop=FALSE], 2, sum )
  })

  MM
}

#######################################################################
# 
#
calculate_marks <- function( reg, endDt=as.Date("2017-12-01") )
{
  asOfDate <- Sys.Date()
  startDt  <- nextMonth()
  
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- "Hub"       
  bucket      <- c("5X16", "2X16H")
  serviceType <- "Energy"
  useFutDates <- FALSE  
  
  PP <- PM:::secdb.getElecPrices( asOfDate, startDt, endDt,
    commodity, location, bucket, serviceType, useFutDates=FALSE)
  names(PP)[ncol(PP)] <- "hub.rt"
  
  # add the number of PEAK days in a month
  start.dt <- as.POSIXct(paste(startDt, "01:00:00"))
  end.dt <- as.POSIXct("2018-01-01 00:00:00")
  hrs <- FTR:::FTR.bucketHours(start.dt=start.dt, end.dt=end.dt,
    buckets=c("5X16", "2X16H"))
  hrs$days <- hrs$hours/16
  names(hrs) <- c("month", "bucket", "hours", "days")
  PP <- merge(PP, hrs[,c("month", "bucket", "days")],
              by=c("month", "bucket"))
  
  # predict the cost using the loess, remember it's log10
  yP <- predict(reg, data.frame(hub.rt=PP$hub.rt), se=TRUE)
  PP$predictedDailyCost <- 10^(yP$fit + 1.95*yP$se.fit)
  PP$monthlyDRcost <- PP$predictedDailyCost * PP$days
  fwd <- data.frame(cast(PP, month ~ ., sum, value="monthlyDRcost"))
  names(fwd)[2] <- "monthlyDRcost"
  
  windows()
  plot(fwd[,1], fwd[,2], type="o", col="blue", xlab="",
       ylab="Monthly DR cost")
  
  # get the forecasted energy by month
  rLog("Reading the 2010 tlp weather norm load forecast")
  LL <- nepool.load.fcst(startDt, end.dt, from="2010")
  aux <- data.frame(dt=index(LL),
    month=format(index(LL)-1, "%Y-%m-01"),
    load=as.numeric(apply(LL, 1, sum)))     
  
  EE <- data.frame(cast(aux, month ~ ., sum, na.rm=TRUE, value="load"))
  names(EE)[2] <- "energy"
  EE$month <- as.Date(EE$month)
  
  fwd <- merge(fwd, EE, all.x=TRUE)
  fwd$druplift.mark <- fwd$monthlyDRcost/fwd$energy

  rLog("For jul/aug, set it at least 0.05 $/MWh ...")
  ind <- which(format(fwd$month, "%m") %in% c("07", "08"))
  fwd$druplift.mark[ind] <- max(fwd$druplift.mark[ind], 0.05)
  
  fwd
}


#######################################################################
# for now from the MIS reports, but eventually will be in tsdb
# SD_DRCHGDETAIL_
# @param startDt an R Date
# @param endDt an R Date
# @return HH historical data as a data.frame
#
.pull_hist_druplift <- function( startDt=as.Date("2012-06-01"),
   endDt=Sys.Date() )
{
  startDt <- as.POSIXct(paste(startDt, "01:00:00"))
  endDt   <- as.POSIXct(endDt)
  symbols <- c(
   "NERPT_SD_DR_CHG_POOL_TOT_PMT",   "Pool.Total.Payment",
   "NERPT_SD_DR_CHG_CUST_CHG_ALLOC", "ccg.mw",        # ccg rt load
   "NERPT_SD_DR_CHG_POOL_CHG_ALLOC", "pool.mw",       # pool rt load
   "NERPT_SD_DR_CHG_CUST_CHG",       "ccg.charge.$",  # dollars
   "NERpt_RTCuS_LoadObl",            "ccg.load",
   "NCRpt_RTCuS_LoadObl",            "cne.load",               
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt"
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))
  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH) <- symbols[,"shortNames"]
  HH <- HH[!is.na(HH$Pool.Total.Payment),]
  
  HH <- cbind(datetime=index(HH), as.data.frame(HH))
  HH
}


#######################################################################
#
calc_pnl_impact_druplift <- function(day1, day2, qq,
   endDate=as.Date("2017-12-01") )
{
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  qq <- get_positions_druplift(asOfDate, company)
  qq <- qq[order(qq$location, qq$month),]
  
  PP1 <- .pull_marks_rmr(day1,  startDate=day1, endDate)
  PP1$asOfDate <- NULL
  
  PP2 <- .pull_marks_rmr(day2, startDate=day2, endDate)
  PP2$asOfDate <- NULL

  aux <- calc_pnl_pq(PP1, PP2, qq, qq,
    groups=c("year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
  
}

################################################################
# get positions
#
get_positions_druplift <- function(asOfDate, books)
{
  rLog("doesn't work well, don't use it!")
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
   fix=TRUE, service="ENERGY")
  ## bux <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
  ##   value="notional", subset=eti=="PZS3VXD")
  
  qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
             value="notional")
  names(qq) <- c("month", "location", "value")

  qq
}


#######################################################################
#
.pull_marks_druplift <- function(asOfDate, startDt, endDt)
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- " "       # <-- cannot be "" crashes R!
  bucket      <- "7x24"
  serviceType <- "DRUplift"
  useFutDates <- FALSE  
  
  fwd <- PM:::secdb.getElecPrices( asOfDate, startDt, endDt,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  fwd
}


#######################################################################
# Pool energy usage by month
#
.get_hist_energy <- function(startTime, endTime)
{
  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt",            
#   "NeRpt_rtCus_PoolLoadObl",        "load"              # hourly
   "nepool_load_hist",               "load"            
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startTime, endTime)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  HH <- cbind(time=index(HH), as.data.frame(HH))
  HH$month <- as.Date(format(HH$time-1, "%Y-%m-01"))
    
  rownames(HH) <- NULL

  HH
}


#######################################################################
# MWh interruptions and payments for all Load response programs by
# zone and month from the ISO FERC biannual filings.
#
.extract_table_F <- function(TT)
{
  ind <- grep("\f.*Table F", TT)  # table starts on new page
  aux <- TT[ind[1]:length(TT)]
  ind <- grep("\f", aux)
  aux <- aux[ind[1]:(ind[2]-1)]     # assume table F is on one page only

  zones <- c("ME", "NH", "VT", "CT", "RI", "SEMA", "WCMA", "NEMA")

  # find where each zone starts
  indStart <- which(gsub(" ", "", substr(aux, 1, 4)) %in% zones)
  indEnd   <- grep("^ *Total", aux)
  if (length(indStart) != length(indEnd))
    stop("Problem identifying start/end block for zone")
  
  res <- NULL
  for (i in 1:length(indStart)) {
    block <- aux[indStart[i]:indEnd[i]]
    block <- strsplit(block, " +")
    block[length(block)] <- NULL
    block <- do.call("rbind", block)
    colnames(block) <- c("zone", "mon", "year", "MWh interrupted",
                         "Payment", "$/MWh")
    block <- as.data.frame(block)
    block$zone <- block$zone[1]

    res[[i]] <- block
  }
  res <- do.call(rbind, res)

  res$Payment <- as.numeric(gsub("\\$|,", "", res$Payment))
  res$`MWh interrupted` <- as.numeric(gsub("\\$|,", "", res$`MWh interrupted`))
  res$month <- as.Date(paste("1", res$mon, res$year), format="%d %b %Y")
  
  res <- res[,c("zone", "month", "MWh interrupted", "Payment")]

  res
}






#######################################################################
#######################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  #require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/DemandResponse/lib.loadresponse.R")

  startDt <- as.Date("2012-06-01")
  endDt   <- as.Date("2013-02-28")
  reg <- analysis_druplift(startDt, endDt)

  hist <- backcast_costs(reg)
  print(hist, row.names=FALSE)

  fwd <- calculate_marks(reg, endDt=as.Date("2018-12-01"))
  PP2 <- fwd[,c("month", "druplift.mark")]
  names(PP2)[2] <- "value"
  print(PP2, row.names=FALSE)
  
  
  ####################################################################
  asOfDate <- Sys.Date()-2
  startDt  <- nextMonth()
  endDt    <- as.Date("2017-12-01")

  PP1 <- .pull_marks_druplift(asOfDate, startDt, endDt)
  PP1 <- PP1[,c("month", "value")]

  ####################################################################
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")

  x <- secdb.readDeltas(asOfDate, "NEPOOL Load Portfolio")  
  qq <- .format_one_pfolio(x, mw=FALSE)
  qq1 <- cbind(company="ccg", data.frame(cast(qq, contract.dt ~ ., sum)))
  names(qq1)[3] <- "value"
  
  x <- secdb.readDeltas(asOfDate, "CNE NewEngland Load Portfolio")
  qq <- .format_one_pfolio(x, mw=FALSE)
  qq2 <- cbind(company="cne", data.frame(cast(qq, contract.dt ~ ., sum)))
  names(qq2)[3] <- "value"

  qq <- rbind(qq1, qq2)
  names(qq)[2] <- "month"

  aux <- calc_pnl_pq(PP1, PP2, qq, qq,
    groups=c("year", "company"))  # from lib.pnl.R
  gPnL <- aux[["gPnL"]]
  print(gPnL, row.names=FALSE)
  sum(gPnL$PnL)
   


  
  ####################################################################
  # look at CNE load response program performance
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  
  months <- seq(as.Date("2011-01-01"), by="1 month", length.out=12)
  read_SD_LRSTLDETAIL(months, org="CNE")






  
  ####################################################################
  # read historical DR data from the pdf report NEPOOL sends to FERC
  #
  require(DivideAndConquer)
  source("H:/user/R/RMG/Utilities/read.pdf.R")
  asOfDate  <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  startTime <- as.POSIXct("2009-10-01 01:00:00")   # for hist data
  endTime   <- Sys.time()                          # for hist data 
    
  HH  <- .get_hist_energy(startTime, endTime)
  mLL <- cast(HH, month ~ ., sum, na.rm=TRUE, value="load")  # monthly energy
  names(mLL)[2] <- "Energy"
  
  DIR_IN <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/LoadResponse/Raw/"
  files <- list.files(DIR_IN, full.name=TRUE)

  res <- NULL
  for (f in 1:length(files)){
    rLog("Working of file", basename(files[f]))
    TT <- read.pdf(files[f])
    res[[f]] <- .extract_table_F(TT)
  }
  res <- do.call("rbind", res)
  DR <- cast(res, month ~ zone, I, fill=0, value="Payment")

  # add the Energy
  DR <- merge(DR, mLL, by="month", all.x=TRUE)
  
  print(DR, row.names=FALSE)
  




  
}






  ## windows()
  ## plot(DD$hub.da, DD$Pool.Total.Payment, log="xy",
  ##   col="blue",
  ##   xlab="DA Onpeak, $/MWh",
  ##   ylab="Pool DR Uplift payment, $")
  ## xP <- seq(round(min(DD$hub.da)), round(max(DD$hub.da)), by=1)
  ## yP <- predict(reg, data.frame(hub.da=xP), se=TRUE)
  ## lines(xP, 10^(yP$fit), col="gray", lwd=2)
  ## lines(xP, 10^(yP$fit+1.95*yP$se.fit), col="gray", lwd=1, lty=2)
  ## lines(xP, 10^(yP$fit-1.95*yP$se.fit), col="gray", lwd=1, lty=2)


## #######################################################################
## #
## read_SD_LRSTLDETAIL <- function(months, org="CNE")
## {
##   # pick only the latest one for the month
##   filenames <- sapply(months, function(month){
##     aux <- .get_report_csvfilenames(org=org, month=month,
##                              reportName="SD_LRSTLDETAIL")
##     sort(aux)[length(aux)]
##   })

##   .readOne <- function(file=file) .process_report(file)[[1]]

##   res <- finalize(filenames, .readOne)

##   # deviation by asset
##   cast(res, Asset.Name ~ ., sum, value="LR.Deviation")

##   # profit by asset
##   cast(res, Asset.Name ~ ., sum, value="LR.Deviation.Settlement")


  
  
## }

  ## # from MIS reports until data is in SecDb ...
  ## source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

  ## months <- seq(as.Date("2012-06-01"), nextMonth(), by="1 month")
  ## fnames <- unlist(sapply(months, function(m){
  ##   .get_report_csvfilenames(org="CCG", month=m,
  ##                            reportName="SD_DRCHGDETAIL")
  ## }))

  ## aux <- lapply(fnames, .process_report)
  ## DR <- do.call(rbind, lapply(aux, "[[", 1))

  ## aux <- data.frame(Date=format(as.Date(DR$Trading.Date, "%m/%d/%Y")),
  ##                   Hour.Ending=as.character(DR$Trading.Interval))
  ## DR$datetime <- PM:::isotimeToPOSIXct(aux)$datetime
