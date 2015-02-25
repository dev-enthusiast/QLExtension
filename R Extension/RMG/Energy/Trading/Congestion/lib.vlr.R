# Calculate vlr
#
# calculate_stage1_vlr
# calculate_stage2_vlr
# cost_vlr2_load
#


###########################################################################
# Calculate stage 1 VLR
# @param startDt a POSIXct
# @param endDt a POSIXct
# 
#
calculate_stage1_vlr <- function(startDt, endDt=Sys.Date())
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  
  this.month <- as.Date("2013-07-01")
  days <- seq(as.Date("2013-07-15"), as.Date("2013-07-20"), by="1 day")
  
  if (is.null(days))
    days <- seq(this.month, nextMonth(this.month)-1, by="1 day")
  
  asOfDate <- as.Date("2013-06-19") # some day in the middle of the prev month
  QQ1 <- .get_booked_loads_ccg( asOfDate )  # booked before bom
  QQ1 <- .get_booked_loads_cne( asOfDate )  # booked before bom
  QQ1 <- subset(QQ1, month == this.month)
  head(QQ1)
  QQ1 <- subset(QQ1, bucket %in% c("2X16H", "5X16", "7X8"))
  qq <- cast(QQ1, bucket ~ ., sum, value="value")
  
  hrs <- PM:::ntplot.bucketHours(c("2X16H", "5X16", "7X8"),
     this.month, this.month, region="NEPOOL", period="Month")
  colnames(hrs)[3] <- "hrs"
  QQ1 <- merge(QQ1, hrs[,c("bucket", "hrs")])
  head(QQ1)
  QQ1$mw <- QQ1$value/QQ1$hrs
  qq <-  merge(qq, hrs[,c("bucket", "hrs")])
  qq$mw <- qq$`(all)`/qq$hrs
  hubQQ <- PM:::expandMonthlyBucketsHourly(this.month,
    structure(qq$mw, names=qq$bucket),
    region="NEPOOL", agg=TRUE, bucketMask=NULL)

  
  PP1 <- .pull_marks(asOfDate=as.Date(secdb.dateRuleApply(this.month, "-1b")),
    deliveryPt=unique(QQ1$location), startDate=this.month, endDate=this.month)
  colnames(PP1)[ncol(PP1)] <- "mark"
  hub <- subset(PP1, location=="HUB DA")
  hubHH <- PM:::expandMonthlyBucketsHourly(this.month,
    structure(hub$value, names=hub$bucket),
    region="NEPOOL", agg=TRUE, bucketMask=NULL)

  
  
  # get da LMPs
  symbs <- paste("nepool_smd_da_", 4000:4008, "_lmp", sep="")
  PP2 <- FTR:::FTR.load.tsdb.symbols(symbs, as.POSIXct(paste(days[1], "01:00:00")),
    as.POSIXct(paste(days[length(days)]+1, "00:00:00")))
  colnames(PP2) <- toupper(c("hub da", "me", "nh", "vt", "ct", "ri",
                             "sema", "wma", "nema"))
  
  # IT IS NOT DONE ----------------------------------------------

  
  
  
}



###########################################################################
# Calculate stage 2 VLR for each company
# @param startDt a POSIXct
# @param endDt a POSIXct
# 
#
calculate_stage2_vlr <- function(startDt, endDt=Sys.time())
{
  # get da/rt LMPs
  symbs <- c(paste("nepool_smd_da_", 4001:4008, "_lmp", sep=""),
             paste("nepool_smd_rt_", 4001:4008, "_lmp", sep=""))
  #symbs <- c("nepool_smd_da_4000_lmp", "nepool_smd_rt_4000_lmp")
  PP <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  zones <- c("me", "nh", "vt", "ct", "ri", "sema", "wma", "nema")
  res <- vector("list", length=8)
  for (i in 1:8) {
    res[[i]] <- data.frame(time=index(PP), zone=zones[i],
      dart=(PP[, paste("nepool_smd_da_", 4000+i, "_lmp", sep="")]
            -PP[, paste("nepool_smd_rt_", 4000+i, "_lmp", sep="")]))              
  }
  dart <- do.call(rbind, res)
  rownames(dart) <- NULL
  head(dart)
  
  LL1 <- demandbids_vs_rtload(startDt, endDt, company="exgen")
  LL1 <- LL1[, c("time", "zone", "bid.da", "actual")]
  LL1$company <- "exgen"
  head(LL1)
  LL2 <- demandbids_vs_rtload(startDt, endDt, company="cne")
  LL2 <- LL2[, c("time", "zone", "bid.da", "actual")]
  LL2$company <- "cne"
  head(LL2)
  LL <- rbind(LL1, LL2)
  LL$zone <- as.character(LL$zone)
  colnames(LL)[3:4] <- c("load.bid.da", "load.rt")

  VLR <- merge(LL, dart, by=c("time", "zone"))
  VLR$vlr <- (VLR$load.rt-VLR$load.bid.da)*VLR$dart
  VLR <- na.omit(VLR)
  
  VLR  
}


###########################################################################
# Estimate cost to load
# Should use the short term forecast vs rt load.  (not demand bids) <---
# @param company a string, "cne" or "exgen"
# @param startDt a POSIXct
# @param endDt a POSIXct
# 
#
cost_vlr2_cne_load <- function(company, startDt, endDt=Sys.time())
{
  # get da/rt LMPs
  symbs <- c(paste("nepool_smd_da_", 4001:4008, "_lmp", sep=""),
             paste("nepool_smd_rt_", 4001:4008, "_lmp", sep=""))
  #symbs <- c("nepool_smd_da_4000_lmp", "nepool_smd_rt_4000_lmp")
  PP <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  zones <- c("me", "nh", "vt", "ct", "ri", "sema", "wma", "nema")
  res <- vector("list", length=8)
  for (i in 1:8) {
    res[[i]] <- data.frame(time=index(PP), zone=zones[i],
      dart=(PP[, paste("nepool_smd_da_", 4000+i, "_lmp", sep="")]
            -PP[, paste("nepool_smd_rt_", 4000+i, "_lmp", sep="")]))              
  }
  dart <- do.call(rbind, res)
  rownames(dart) <- NULL
  head(dart)
  
  # get the load
  if (company == "cne") {
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
    colnames(LL)[2] <- "zone"

    
  } else if (company == "exgen") {
    #

    
  } else {
    stop("What?!")
  }
  
  VLR <- merge(LL, dart, by=c("time", "zone"))
  VLR$vlr2 <- (VLR$actual.load-VLR$fcst.load)*VLR$dart

  VLR$month <- as.Date(format(VLR$time-1, "%Y-%m-01"))
  VLR <- na.omit(VLR)
  
  
  res <- ddply(VLR, c("month"), function(x){
    sum(x$vlr2)/sum(x$actual)
  })
  colnames(res)[2] <- "vlr2.rate.$/MWh" 
  
  res
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
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.vlr.R")

  
  startDt <- as.POSIXct("2013-07-15 01:00:00")
  endDt   <- Sys.time()
  LL <- demandbids_vs_rtload(startDt, endDt, company="exgen")
  #LL <- demandbids_vs_rtload(startDt, endDt, company="cne")
  tail(LL)


  # calculate monthly vlr cost
  startDt <- as.POSIXct("2010-01-01 01:00:00")
  endDt   <- Sys.time()
  company <- "exgen"
  x <- cost_vlr2_load(startDt, endDt, company=company)
  #x <- cost_vlr2_load(startDt, endDt, company="cne")
  x <- subset(x, month < currentMonth())
  xyplot(`vlr2.rate.$/MWh` ~ month, data=x,
         type=c("g", "o"),
         main=paste(toupper(company),"VLR stage 2 cost"))

  cast(v)
  
  xyplot(vlr2 ~ day, data=vlr.day,
         type=c("g", "o"),
         subset=company == "exgen", 
         main=paste("VLR stage 2 cost"))

  
  
  # for Kate
  vlr2 <- calculate_stage2_vlr(startDt=as.Date("2013-01-01"), endDt=Sys.Date())
  write.csv(vlr.day, file="S:/All/Structured Risk/NEPOOL/Temporary/vlr2_2014-09-17.csv",
            row.names=FALSE) 


  
  
}

