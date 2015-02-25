# Decide how to price the seller's choice node.
# Original trade: PWS64E1
# IBT trade: PZP92Y4
#
# .algo_daily_lookback
# .algo_yesterday_min
#
# .calc_pnl_impact
# .calc_sc_marks       - use the hub and the discount to calc the marks
# ..discount_for_presentation  - for the pricing meetings <--           
# .get_CL8_ptids
# .get_sc_positions
# .get_sc_marks
# .hist_sellerschoice  - find out the SC discount in %
# ..load_with_ptid_filter 
# .make_sc_marks
# .mark_cl8            - for the offer
# .pull_hist_prices  read tsdb prices
# .strategy_cost               - estimate cost of strategy
# .SC_strategy  - pick the best node on a daily basis
# .SC_for_rt_floorprice - what is the SC discount when rt hits the floor?
#


#################################################################
# Calculate the daily sc.discount using perfect lookback
# @param x an hourly zoo object (for a given bucket) with all the LMP prices 
# @return a data.frame with columns [day, ind, sc.discount]
#
.algo_daily_lookback <- function( x )
{
  # calculate daily mean
  DD <- aggregate(x, as.Date(format(index(x)-1, "%Y-%m-%d")),
                  FUN=mean, na.rm=TRUE)
    
  # pick the lowest price
  res <- do.call(rbind, apply(DD, 1, function(x){
    ind <- which.min(x)
    data.frame(ind=names(x[ind]), sc.discount=x[ind]/x[1])
  }))

  cbind(day=index(DD), res)
}


#################################################################
# Calculate the daily sc.discount using the yesterday's min rule.
# @param x an hourly zoo object (for a given bucket) with all the LMP prices 
# @return a data.frame with columns [day, ind, sc.discount]
#
.algo_yesterday_min <- function( x )
{
  DD <- aggregate(x, as.Date(format(index(x)-1, "%Y-%m-%d")),
                 FUN=mean, na.rm=TRUE)
  rownames(DD) <- NULL
 
  # pick the lowest price today and offer it for tomorrow
  ind <- apply(DD, 1, function(x){ which.min(x) }) 
  sc.discount <- rep(NA, length(ind))
  
  # loop over days (can't do first day of the year with this strategy)
  for (i in 2:length(ind)) {
     sc.discount[i] <- DD[i,ind[i-1]]/DD[i,1]   # ratio of price to hub price (index = 1)
  }
        
  data.frame(day=index(DD), ind=names(DD)[ind], sc.discount=sc.discount)
}




#################################################################
# Get positions, new and old marks and calculate PnL
# SC.new - the new SC marks by Peak/Offpeak bucket
#
.calc_pnl_impact <- function(PP1, PP2, QQ)
{
  sc1 <- PP1
  sc2 <- PP2
  colnames(sc1)[which(colnames(sc1)=="SC")] <- "SC.price.1"
  colnames(sc2)[which(colnames(sc2)=="SC.price")] <- "SC.price.2"
  sc21 <- merge(sc1[,c("bucket", "month", "SC.price.1")],
                sc2[,c("bucket", "month", "SC.price.2")])
  
  
  PnL <- merge(sc21, QQ, all.y=TRUE)
  PnL$PnL <- round(PnL$delta * (PnL$SC.price.2 - PnL$SC.price.1))
  sum(PnL$PnL)  

  PnL$year <- format(PnL$month, "%Y")
  print(cast(PnL, year ~ bucket, sum, value="PnL"))
  
  PnL
}


#################################################################
# Use the hub price and the SC discount to construct the SC mark
#
.calc_sc_marks <- function(SC.discount, asOfDate=Sys.Date(),
                           endDate=as.Date("2018-12-01"))
{
  startDate   <- nextMonth(asOfDate)
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- c("HUB")
  bucket      <- c("PEAK", "OFFPEAK")
  serviceType <- c("ENERGY")
  hub <- PM:::secdb.getElecPrices( as.Date(asOfDate), startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  hub$mon <- as.numeric(format(hub$month, "%m"))
  colnames(hub)[which(colnames(hub)=="value")] <- "hub.price"
  hub <- hub[, c("month", "bucket", "hub.price", "mon")]
  
  SC <- merge(hub, SC.discount[, c("mon", "SC.discount", "bucket")])
  SC$SC.price <- SC$hub.price * SC$SC.discount
  SC <- SC[order(SC$bucket, SC$month), ]
  SC$spread <- SC$SC.price - SC$hub.price

  SC
}

#################################################################
# Numbers for the presentation  
# Paste into xls from
#   H:\user\R\RMG\Energy\Trading\Congestion\NEPOOL\Deals\CMP_Entitlements\2015
#
..discount_for_presentation <- function()
{  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.sellers.choice.R")
  
  #--------------------------------------------- CL8 (marked as CL8)
  ptids.CL8 <- .get_CL8_ptids()
  s <- .SC_strategy(ptids=ptids.CL8, mkt="RT", rule="LOOKBACK", startYear=2010)
  write.csv(s, file="C:/temp/cl8_lookback.csv", row.names=FALSE)
  s <- .SC_strategy(ptids=ptids.CL8, mkt="RT", rule="YESTERDAY MIN", startYear=2010)
  write.csv(s, file="C:/temp/cl8_yesterdaymin.csv", row.names=FALSE)

  # negative prices DOES affect the min price 
  s <- .SC_strategy_yesterdays_min(ptids=ptids.CL8, mkt="RT", startYear=2010,
                                   floorPrice=-100)


  #-------------------------------------------- CincapV (marked as Maine-SC)
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.sellers.choice.R")
  s <- .SC_strategy(mkt="DA", rule="LOOKBACK", startYear=2010)
  s <- .SC_strategy(mkt="DA", rule="YESTERDAY MIN", startYear=2010)

  

  s$ptid <- gsub("nepool_smd_da_([[:digit:]]+)_lmp", "\\1", s$ind)
  aux <- rev(sort(table(s$ptid)))
  aux <- data.frame(rank=1:length(aux), ptid.lookback = names(aux), no.days=aux)
  rownames(aux) <- NULL
  print(aux, row.names=FALSE)

  write.csv(s, file="C:/temp/sellers_choice_lookback.csv", row.names=FALSE)
  write.csv(s, file="C:/temp/sellers_choice_yesterdaymin.csv", row.names=FALSE)
  

  
  SC.discount <- .make_sc.discount_from_strategy(s, fromDate=as.Date("2005-01-01"))

  

}




#################################################################
#
.get_CL8_ptids <- function()
{
  require(xlsx)
  fname <- paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/",
    "Deals/CMP_Entitlements/2014/cl8.xlsx", sep="")

  ptids <- as.numeric(read.xlsx2(fname, 1, startRow=4)$ptid)

  ptids
}



#################################################################
#
.get_sc_positions <- function(asOfDate="2011-03-10")
{
  # get SC positions
  rLog("Only the load part from NPCNCP")
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  load(paste(dir, "positionsAAD_", format(asOfDate), ".RData", sep=""))

  qq <- subset(QQ, contract.dt <= as.Date("2018-12-01") &
                   contract.dt >= as.Date(asOfDate) &
                   trading.book %in% c("NPCNCP") &
             #      portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
             #        "NEPOOL MTM HEDGES OF ACCRUAL", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )
  qq <- subset(qq, grepl(" SC ", qq$curve))
  qsc <- cast(qq, contract.dt + bucket ~ ., sum, value="delta")
  colnames(qsc)[3] <- "value"
  qsc[,3] <- round(qsc[,3])
  qsc <- PM:::combineBuckets(qsc, weight="none", buckets=c("2X16H", "7X8"),
              multiplier=c(1,1), name="OFFPEAK")
  qsc$bucket <- ifelse(qsc$bucket=="5X16", "PEAK", qsc$bucket)
  colnames(qsc)[which(colnames(qsc)=="value")] <- "delta"
  colnames(qsc)[which(colnames(qsc)=="contract.dt")] <- "month"

  data.frame(qsc)
}

#################################################################
#
.get_sc_marks <- function(asOfDate=Sys.Date(),
                          endDate=NULL,
                          bucket=c("PEAK", "OFFPEAK"))
{
  startDate   <- as.Date(asOfDate)
  if (is.null(endDate))
    endDate <- as.Date( format(Sys.Date()+3*365, "%Y-12-01"))
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- c("SC", "HUB")
  bucket      <- bucket
  serviceType <- c("ENERGY")
  SC1 <- PM:::secdb.getElecPrices( as.Date(asOfDate), startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  SC <- cast(SC1, asOfDate + bucket + month ~ location, I)
  SC$`SC/HUB` <- SC$SC - SC$HUB
  
  SC
}



#################################################################
# Calculate historical discount to hub of the lowest node in the
# pool by bucket by month. 
#
.hist_sellerschoice <- function()
{
  files <- list.files(DIROUT, pattern="hPrices_DA_.*RData",
    full.names=TRUE)

  .oneFileFun <- function(file=file, n=10){
    load(file)
    # pick the lowest n points from each hour 
    minN <- t(apply(PP, 1, function(x, n){sort(as.numeric(x))[1:n]}, n))

    # calculate the ratio of lowest n nodes to hub price
    RR <- minN/matrix(PP[,1], nrow=nrow(PP), ncol=n)  

    time <- rownames(RR)
    rownames(RR) <- NULL
    cbind(time=time, data.frame(RR))
  }
  
  res <- finalize(files, .oneFileFun)
  res$time <- as.POSIXct(res$time)
  
  # add the Onpeak bucket flag
  ind <- secdb.getBucketMask("NEPOOL", "5X16",
    as.POSIXct("2005-01-01 01:00:00"), Sys.time())
  ind$bucket <- ifelse(ind$`5X16`, "PEAK", "OFFPEAK")
  res <- merge(res, ind[, c("time", "bucket")], by="time", all.x=TRUE)


  res$yyyymm <- format(res$time-1, "%Y-%m-01")
  res$year   <- format(res$time-1, "%Y")
  res$mon    <- as.numeric(format(res$time-1, "%m"))

  
  # look at the difference between X1 and X10
  summary(res$X10-res$X1)
##      Min.   1st Qu.    Median      Mean   3rd Qu.      Max. 
## 0.0002793 0.0148000 0.0219700 0.0431100 0.0343200 2.9720000 

  
  #plot(res$time, res$V10)
  #subset(res, res$V1 < 0.1)  # a lot of negative prices!

  # let's use the 2010 percentages forward from "V10" with a 1% discount  
#  DD <- cast(res, bucket + yyyymm ~ ., mean, value="V10", subset=year=="2010")
  DD <- cast(res, bucket + mon ~ ., mean, value="X5", subset=year %in% c(
     "2010", "2011", "2012", "2013"))
  names(DD)[3] <- "SC.discount"
  #DD$mon <- as.numeric(substr(DD$yyyymm, 6, 7))
  # to account for using the 10th lowest vs. the lowest node 
  # DD$SC.discount <- DD$SC.discount - 0.01  
  
  SC.discount <- data.frame(DD)

  SC.discount
}


#################################################################
# Load an RData file with historical prices and filter it if
# nedeed. 
# @return a zoo objects with prices
#
..load_with_ptid_filter <- function(filename, ptids=NULL)
{
  year <- gsub("hPrices_DA_(.*).RData", "\\1", basename(filename))
  rLog("Loading", year)
  load(filename)

  if (!is.null(ptids)) {
    all.ptids <- gsub(".*_([[:digit:]]+)_.*", "\\1", colnames(PP)) 
    PP <- PP[,which(all.ptids %in% ptids)] 
  }
      
  PP    
}


#################################################################
# Calculate the Seller's Choice discount from a strategy result. 
# @s a strategy, a data.frame with: day, tsdb.symb, sc.discount
#
#
.make_sc.discount_from_strategy <- function(s, fromDate=as.Date("2010-01-01"))
{
  s <- subset(s, day >= fromDate)
  s$mon <- as.numeric(format(s$day, "%m"))

  X <- ddply(s, c("mon"), function(x) {summary(na.omit(x$sc.discount))})
   
  
  SC.discount <- data.frame(mon = rep(X$mon,2),
                            bucket = rep(c("PEAK", "OFFPEAK"), each=12),
                            SC.discount = rep(X$Mean,2) )
  
  SC.discount
}


#################################################################
# Calculate the Seller's Choice discount for CL8
# Save the results in a spreadsheet for meeting 
#
.mark_cl8 <- function()
{

  ## #---------------------------------- CL8
  ## ptids.CL8 <- .get_CL8_ptids()  
  ## strat <- .SC_strategy_yesterdays_min( ptids=ptids.CL8 )
  ## strat <- .SC_strategy_yesterdays_min( ptids=ptids.CL8, startYear=2010 )
  ## strat <- .SC_strategy_yesterdays_min( ptids=ptids.CL8, startYear=2010,
  ##                                      floorPrice=-50)
  ## strat <- .SC_strategy_daily_lookback( ptids=ptids.CL8 )
  
  
  #---------------------------------- CL8
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.sellers.choice.R")
  
  
}


#################################################################
# Calculate historical discount to hub of the lowest node in the
# pool by bucket by month. 
# @param market a string either "DA" or "RT"
.pull_hist_prices <- function(ptids, years=2013:2014, mkt="DA")
{
  symbs <- paste("nepool_smd_", mkt, "_", ptids, "_lmp", sep="")
  currentYear <- as.numeric(format(Sys.Date(), "%Y"))
  for (year in years) {
    rLog("Working on year:", year)
    startDt <- ISOdatetime(year, 1, 1, 1, 0, 0)
    endDt <-  min(Sys.time(), ISOdatetime(year+1, 1, 1, 0, 0, 0))
    PP <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)

    DIROUT <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/" 
    fname <- paste(DIROUT, "hPrices_", mkt, "_", year, ".RData", sep="")
    save(PP, file=fname)
    rLog("Wrote file", fname)
  }
}


#################################################################
# Calculate the cost of a strategy, so you can compare them.
# Calculate the PnL of selling Seller's Choice and buying Hub, 1 MW,
# every hour. 
# @param X is a data.frame with (day, ind, sc.discount)
# @return a data.frame with the PnL by year
#
.strategy_cost <- function(X)
{
  startDate <- min(X$day)
  endDate <- max(X$day)
  
  hub <- PM::ntplot.bucketizeByDay("nepool_smd_da_4000_lmp", "7x24",
    startDate, endDate,
    region="NEPOOL", SamplingPeriod=3600, stat="Mean")

  hub <- merge(hub[,c("day", "value")], X, by="day")
  hub$year <- format(hub$day, "%Y")
  res <- ddply(hub, c("year"), function(x) {
    24*sum(x$value * x$sc.discount)
  })
  colnames(res)[2] <- "PnL"
  
  as.data.frame(res)
}



#################################################################
# Pick the Seller's choice node by looking at the min node the 
# previous day.
# @param ptids vector of ptids
# @param mkt "DA" or "RT"
# @return a strategy, a data.frame with: day, tsdb.symb, sc.discount
#
.SC_strategy <- function(ptids=NULL,
                         rule = c("YESTERDAY MIN", "LOOKBACK"),
                         mkt="DA", 
                         floorPrice=0,
                         startYear=2010,
                         bucket = c("7X24", "7X16", "7X8"),
                         do.plot=TRUE)
{
  DIROUT <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/" 
  files <- list.files(DIROUT, pattern=paste("hPrices_", mkt, "_.*RData", sep=""),
    full.names=TRUE)

  year <- as.numeric(gsub(".*_([[:digit:]]{4})\\.RData", "\\1", basename(files)))
  ind  <- which(year >= startYear)
  rLog("History from:", year[ind[1]])
  files <- files[ind]
  
  res <- ldply(files, function(fname, ptids, bucket, floorPrice) {

    PP <- ..load_with_ptid_filter(fname, ptids=ptids)
    rLog("loaded ", fname)
 
    if (floorPrice != 0) {
      ind <- which(PP < 1)
      if (length(ind)>0) {
        rLog(paste("Setting", length(ind), "hours to the min floor", floorPrice))
        aux <- as.matrix(PP)
        aux[ind] <- floorPrice      # cannot do this on PP directly!
        PP <- zoo(aux, index(PP))   
      }
    }

    # filter by bucket
    pp <- filterBucket(PP, buckets=bucket)
    #lapply(pp, head)

    algoFun <- if (rule == "YESTERDAY MIN") {
      .algo_yesterday_min
    } else if (rule == "LOOKBACK") {
      .algo_daily_lookback
    } else {
      stop("Unknown rule!")
    }
    
    # calculate avg price by day, ptid, bucket
    res <- lapply(pp, function(x) {
      algoFun(x)
    })
    
    aux <-  mapply(function(x,y){cbind(x,bucket=y)}, res, names(pp), SIMPLIFY=FALSE)
    do.call(rbind, aux)
  }, ptids, bucket, floorPrice)
  
  if (do.plot) {
    aux <- res
    aux$year <- format(aux$day, "%Y")
    aux$dayOfYear <- as.numeric(format(aux$day, "%j"))
    aux$mon <- factor(format(aux$day, "%b"), levels=month.abb)

    aux <- subset(aux, day>=as.Date("2010-01-01"))
    print(xyplot(sc.discount ~ dayOfYear|bucket, data=aux, groups=year,  
           type=c("g", "l"), subset=day>=as.Date("2010-01-01"),
           auto.key=list(space="top", points=FALSE, lines=TRUE,
             columns=length(unique(aux$year))), layout=c(3,1)))
  }
  
  res$year <- format(res$day, "%Y")
  res$mon <- factor(format(res$day, "%b"), levels=month.abb)
  res$yyyymm <- as.Date(format(res$day, "%Y-%m-01"))
  print(ddply(res, c("bucket", "year"), function(x) {
     summary(na.omit(x$sc.discount))}), row.names=FALSE)

  # average by month
  X <- ddply(res, c("bucket", "mon"), function(x) {
    summary(na.omit(x$sc.discount))})
  print(X, row.names=FALSE)
    
  #plot(X$mon, X$Mean, type="o")
  #cast(res, year ~ mon, mean, value="sc.discount")
  #res$year <- res$mon <- res$yyyymm <- res$dayOfYear <- NULL

  res$ptid <- gsub("nepool_smd_(.*)_([[:digit:]]+)_lmp", "\\2", res$ind)
  
  res  
}


#################################################################
# How is the sc discount when rt prices hit the floor?
# @return a zoo with  
#
.SC_for_rt_floorprice <- function(ptids, mkt="RT")
{
  DIROUT <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/" 
  files <- list.files(DIROUT, pattern=paste("hPrices_", mkt, "_.*RData", sep=""),
    full.names=TRUE)

  year <- as.numeric(gsub(".*_([[:digit:]]{4})\\.RData", "\\1", basename(files)))
  ind  <- which(year >= 2009)
  rLog("History from:", year[ind[1]])
  files <- files[ind]
  
  res <- ldply(files, function(fname, ptids, bucket, floorPrice) {

    PP <- ..load_with_ptid_filter(fname, ptids=ptids)
    rLog("loaded ", fname)

    browser()
    ind <- which(PP < 1, arr.ind=TRUE)
    ZZ <- PP[sort(unique(ind[,"row"])), ]
    
    
    
    
  })

  

}



#################################################################
#################################################################
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(PM)
  require(zoo)
  require(plyr)
  require(reshape)
  require(DivideAndConquer)
  require(lattice)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.sellers.choice.R")

  #load NEPOOL MAP
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

  # refresh the historical data set (every month or so) -- takes about 2 min
  refresh <- TRUE  
  if (refresh) {
   ptids <- c(4000, subset(MAP, PTF.status=="EPTF")$ptid)
   .pull_hist_prices(ptids, years=2014:2014, mkt="DA")
  }

  # Use this strategy for marks.  That's what we do too ...
  s1 <- .SC_strategy_yesterdays_min()
  .strategy_cost(s1)
  SC.discount <- .make_sc.discount_from_strategy(s1, fromDate=as.Date("2011-01-01"))
  
  write.csv(s1, file="c:/temp/sc.csv", row.names=FALSE)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Marks/lib.sellers.choice.R")
  s1_a <- .SC_strategy_yesterdays_min(floorPrice=-100)
  
  xyplot(SC.discount ~ mon, data=SC.discount, groups=bucket,
         type=c("g", "o"))

  
  
  # Let's NOT use this strategy for marks
  #s2 <- .SC_strategy_daily_lookback()
  #.strategy_cost(s2)
  #SC.discount <- .make_sc.discount_from_strategy(s2, fromDate=as.Date("2010-01-01"))

  
 
  QQ  <- .get_sc_positions(asOfDate="2014-01-10")
  QQ$year <- format(QQ$month, "%Y")
  cast(QQ, year ~ ., sum, value="delta")
  
  PP1 <- .get_sc_marks(asOfDate=Sys.Date(), endDate=as.Date("2019-01-01"))

  PP2 <- .calc_sc_marks(SC.discount, asOfDate=Sys.Date())
  print(cast(data.frame(year=format(PP2$month, "%Y"), PP2), bucket + year ~ mon, I,
       fill=NA, value="spread"), row.names=FALSE)
  
  PnL <- .calc_pnl_impact(PP1, PP2, QQ)
  sum(PnL$PnL)

  # the new marks
  PP2$year <- format(PP2$month, "%Y")
  spread <- cast(PP2, bucket + year ~ mon, I, value="spread", fill=NA)
  print(spread, row.names=FALSE)

  #=================================================================
  # the marks
  PP1 <- .get_sc_marks(asOfDate=Sys.Date(), bucket=c("2X16H", "5X16", "7X8"))
  print(cast(melt(PP1, id=1:3), month ~ location + bucket, I,
             fill=NA, value="value"), row.names=FALSE)
  
  #=================================================================
  # the CL8 marks.  Look at the location
  # H:\user\R\RMG\Energy\Trading\Congestion\NEPOOL\Deals\CMP_Entitlements\2015
  # check function ..discount_for_presentation in this file
  #


  



  
  #s4 <- .SC_strategy_daily_lookback(ptids=ptids.CL8)

  

}
