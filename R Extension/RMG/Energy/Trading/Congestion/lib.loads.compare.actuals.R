# Tools to look at our load forecast accuracy.
#
# analysis_load_deviation
# calculate_stage2_vlr
# check_load_forecast_ccg_assetId  -- compare with settlements
# check_load_forecast_cne
# demandbids_vs_rtload
#

###########################################################################
# Look at our deviation and see if we can pinpoint it to something
# @param startDt a POSIXct
# @param endDt a POSIXct
# @param company either exgen or cne
# @return a data.frame with columns time, zone, bid, actual, abs.error, rel.error
#
analysis_load_deviation <- function(startDt, endDt=Sys.time(), company="exgen")
{

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.temperature.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  DD <- .pull_hist_economic_NCPC()
  DD$cne.loaddev.fraction <- -DD$cne.rt.loaddevMWh/DD$cne.rt.load
  DD$ccg.loaddev.fraction <- -DD$ccg.rt.loaddevMWh/DD$ccg.rt.load
  tail(DD)
  DD <- DD[which(!is.na(DD$pool.rt.econ.ncpc)),]
  
  DD$month <- as.Date(format(DD$day, "%Y-%m-01"))
  DD$mon <- factor(format(DD$day, "%b"), levels=month.abb)

  # it was wild before, with Kleen and a lot more virtuals
  DD <- subset(DD, day >= startDt)
  TT <- .get_hist_temperature_and_departures(startDt, endDt,
   location="bos")
  head(TT)
  DD <- merge(DD, TT, by="day")
  
  xyplot(ccg.loaddev.fraction ~ day, data=DD, type=c("g", "o"),
         ylab="Ratio of deviation to load, CEG")

  summary(DD$ccg.loaddev.fraction)
##    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
## 0.01237 0.04744 0.07073 0.08161 0.10410 0.82030
  
  summary(DD$cne.loaddev.fraction)
##     Min.  1st Qu.   Median     Mean  3rd Qu.     Max.     NA's 
## 0.007303 0.019660 0.030240 0.039440 0.049010 0.901200       29 

  DD <- subset(DD, ccg.loaddev.fraction < 0.4)
  DD$daytype <- ifelse(weekdays(DD$day) %in% c("Saturday", "Sunday"),
                       "weekend", "weekday" )
  DD$temp <- (DD$bos.max + DD$bos.min)/2
  DD$temp.departure <- (DD$bos.max + DD$bos.min)/2 - DD$bos.10yr
  DD$temp.departure.cut <- cut(DD$temp.departure, 7)
  
  aux <- cast(DD, daytype + temp.departure.cut ~ ., mean, value="ccg.loaddev.fraction")

  xyplot(ccg.loaddev.fraction ~ temp.departure|daytype, data=DD)
  xyplot(ccg.loaddev.fraction ~ temp.departure|mon, groups=daytype,
         data=DD, type=c("g", "p"),
         auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(DD$daytype))))

  
  
  
  MM <- cast(DD, month ~ ., mean, value="ccg.loaddev.fraction")
  
  MM 
}




###########################################################################
# Compare actuals and forecasts for one load asset
# 
#
check_load_forecast_ccg_assetId <- function(startDt, endDt, assetId=2481,
  plot=FALSE)
{
  symbols <- c(
    paste("NERpt_SD_LOAD_", assetId, "_ProdObl", sep=""),    "Actuals",
    paste("NEPOOL_Ld_Fcst_assetid_", assetId, sep=""),       "Forecast")

   symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  if (ncol(HH) != 2) {
    rLog("Cannot find forecast for assetId ", assetId)
    return(NULL)
  }
  colnames(HH) <- symbols[,"shortNames"]
  HH$Actuals <- -HH$Actuals

  HH <- na.omit(HH)
  HH$`fcst-act` <- HH$Forecast - HH$Actuals
  if (plot) {
    aux <- melt(as.matrix(HH))
    colnames(aux) <- c("datetime", "forecast-actual", )

    # bla-bla
  }

  totals <- as.data.frame(t(as.matrix(apply(HH, 2, sum))))
  
  cbind(Asset.ID=assetId, totals)
}


###########################################################################
# Compare actuals and forecasts for CNE by zone
# 
check_load_forecast_cne <- function(startDt, endDt=Sys.time(), zone=NULL)
{
  fcst <- get_cne_load_forecast(startDt, endDt,
    market=c("FP", "FP_MTM", "IXDA", "IXRT"), type="st", aggregate=TRUE)
  colnames(fcst) <- gsub("maine", "me", colnames(fcst))
  colnames(fcst) <- gsub("newham", "nh", colnames(fcst))
  colnames(fcst) <- gsub("connec", "ct", colnames(fcst))
  colnames(fcst) <- gsub("rhodei", "ri", colnames(fcst))
  colnames(fcst) <- gsub("semass", "sema", colnames(fcst))
  colnames(fcst) <- gsub("wcmass", "wma", colnames(fcst))
  colnames(fcst) <- gsub("nemass", "nema", colnames(fcst))
  fcst <- data.frame(time=index(fcst), as.matrix(fcst))
  fcstL <- melt(fcst, id=1)
  colnames(fcstL)[3] <- "fcst.load"
  
  rt <- get_hist_cne_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
  rt <- data.frame(time=index(rt), as.matrix(rt))
  rtL <- melt(rt, id=1)
  colnames(rtL)[3] <- "actual.load"
  rtL$actual.load <- -rtL$actual.load  # make it +

  LL <- merge(fcstL, rtL, by=c("time", "variable"))
  LL.all <- aggregate(LL[,c("fcst.load", "actual.load")],
    by=list(time=LL$time), sum)

  

  
  xyplot(actual.load - fcst.load ~ time, data=LL.all,
         type=c("g", "l"),
         xlab="",
         ylab="Actual - Forecasted, MWh",
         subset = time >= as.POSIXct("2013-07-01"))

  hist(LL.all$actual.load - LL.all$fcst.load, breaks=32)
  

  
  
}


###########################################################################
# Compare ccg short term load forecast with settlements by ZONE
# @param startDt a POSIXct
# @param endDt a POSIXct
# @param company either exgen or cne
# @param adjust.rt.idx, should you substract the rt.idx customers from rt load?
# @return a data.frame with columns time, zone, bid, actual, abs.error, rel.error
#
demandbids_vs_rtload <- function(startDt, endDt=Sys.now(), company=c("exgen", "cne"), 
  adjust.rt.idx=TRUE)
{
  # get the cleared demand bids
  db <- get_cleared_demandbids(startDt, endDt, company=company)
  ZZ <- structure(c("me", "nh", "vt", "ct", "ri", "sema", "wma", "nema"),
   names=4001:4008)
  colnames(db) <- ZZ[colnames(db)]
  #head(db)
  db <- melt(data.frame(time=index(db), db), id=1)
  colnames(db)[3] <- c("bid.da")
  db$bid.da <- -db$bid.da
  head(db)
  # cast(db, time ~ ., sum, value="bid.da")
  
  # get rt loads
  if (company == "exgen") {
    rt <- get_hist_ccg_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
  } else if (company == "cne") {
    rt <- get_hist_cne_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
    if (adjust.rt.idx) {
      # I don't have actuals, just use the forecasted index rt customers until
      # a better solution ...
      rt.idx <- get_cne_load_forecast(startDt, endDt, market="IXRT",
                                      type="st", aggregate=TRUE)
      rt <- rt + rt.idx  # the load is negative, the rt.idx is positive    
    }
    
  } else {
    stop("unknown company!")
  }
  rt <- melt(data.frame(time=index(rt), rt), id=1)
  colnames(rt)[3] <- c("actual")
  rt$actual <- -rt$actual
  head(rt)
  # cast(rt, time ~ ., sum, value="actual")

  
  # put them together 
  LL <- merge(db, rt)
  head(LL)
  colnames(LL)[2] <- c("zone")
  LL$abs.error <- LL$actual - LL$bid
  LL$rel.error <- (LL$actual - LL$bid)/LL$actual

  
  LL
}



##########################################################################
##########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.demandbids.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.loads.compare.actuals.R")

  startDt <- as.POSIXct("2013-01-01 01:00:00")
  endDt   <- Sys.time()
  LL <- demandbids_vs_rtload(startDt, endDt, company="exgen")
  tail(LL)


  
  startDt <- as.POSIXct("2013-07-15 01:00:00")
  endDt   <- Sys.time()
  LL <- demandbids_vs_rtload(startDt, endDt, company="exgen")
  #LL <- demandbids_vs_rtload(startDt, endDt, company="cne")
  tail(LL)
  
  xyplot(abs.error ~ time | zone, data=LL,
    subset=zone %in% c("ct", "nema", "sema", "me"),
    type=c("g", "b"),      
    scale="free",
    ylab="Actual-Forecast (MW)")
  


  # aggregate over all the zones
  AA <- ddply(LL, "time", function(x){
    #browser()
    c(bid.da=sum(x$bid.da, na.rm=TRUE), actual=sum(x$actual, na.rm=TRUE))
  })
  head(AA)
  AA$abs.error <- AA$actual - AA$bid
  AA$abs.rel.error <- abs(AA$actual - AA$bid)/AA$actual
  AA$month <- as.Date(format(AA$time-1, "%Y-%m-01"))
  MM <- cast(AA, month ~ ., mean, value="abs.rel.error")
  
  
  #######################################################################
  loadAssets <- .get_load_ids(company="EXGN")
  loadAssets <- loadAssets[order(loadAssets$Share.of.Load.Reading),]

  startDt <- as.POSIXct("2013-05-01 01:00:00")
  endDt   <- Sys.time()
  check_load_forecast_ccg_assetId(startDt, endDt, assetId=2481)


  
  #######################################################################
  startDt <- as.Date("2011-07-01")
  endDt <- Sys.Date()
  analysis_load_deviation(startDt, endDt)



}






  ## #######################################################################
  ## # for MECO industrials look at the deviations
  ## #

  ## require(xlsx)
  ## HH <- read.xlsx2("c:/temp/Initial vs Final Energy MWh_AAD.xlsx", sheetIndex=2,
  ##   colIndex=c(2,3,6,9), startRow=3, colClasses=c("POSIXct", rep("numeric", 3)))
  ## head(HH)
  ## colnames(HH)[2:4] <- c("nema", "sema", "wma")
  ## HH$month <- as.Date(format(HH$Date-1, "%Y-%m-01"))
  ## MM <- ddply(HH, "month", function(x) {
  ##   c(nema=mean(abs(x$nema)), sema=mean(abs(x$sema)), wma=mean(abs(x$wma)))
  ## })
  ## MM




