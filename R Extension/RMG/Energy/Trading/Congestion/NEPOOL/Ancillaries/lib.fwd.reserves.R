#
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\LFRM Position\CER LFRM Position Nov 09 2011.xls
#
# Cost to load is in here ...
# S:\All\Structured Risk\NEPOOL\Ancillary Services\Reserves\LFRM\Auction Analyses\Long-Term\CT Marks Jul 2012 Adjusted.xlsx
#
# Last marks update I've done is:
#   S:\All\Structured Risk\NEPOOL\Ancillary Services\Reserves\LFRM\2012-2013 Winter\FwdResMarksUpdate_2012-09-04.xls
#
# See
#  - Congestion/lib.NEPOOL.LFRM.R for utilities to download/archive LFRM bids/results
#  - Congestion/NEPOOL/ISO_Data/ForwardReserveAuction/main_frm_auctions.R has the auction analysis
#
# 
# ForwardReserveAuction <-- the object from the auctionName
# order.FRA             <-- how to order the strings 
#
# calc_pnl_impact
# fwdres_auction        <-- from the startDate (2007-06-01), return auctionName "S07"
# .fwdres_get_months
# get_lrr_days          - the days used for the calculation of LRR
# get_all_positions   (all fwdres flavors)
# get_positions_fwdres_ccg
# get_positions_fwdres_cne
# get_positions_fwdres_units
# get_positions_fwdres_hedges
# make_new_marks        <-- what to send to External Data
# .process_SR_RSVCHARGE
# pull_marks_fwdres
# .pull_hist_fwdres_penalties
# read_fwdres_marks_file 
# .tlp_reservezone_load 
#


#########################################################################
# create an FRA object from a string
# @param auction the name of the auction.  NOT VECTORIZED.
#
ForwardReserveAuction <- function(auction="W12-13")
{
  months <- .fwdres_get_months(auction)
  season <- ifelse( grepl("^W", auction), "Winter", "Summer" )
  yy <- as.numeric(substring(auction, 2, 3))
  
  startDate <- as.Date(
    ifelse(season == "Summer",
      paste(2000+yy, "-06-01", sep=""),
      paste(2000+yy, "-10-01", sep="")))

  endDate <- as.Date(
    ifelse(season == "Summer",
      paste(2000+yy, "-09-30", sep=""),
      paste(2001+yy, "-05-31", sep="")))

  
  structure(list(auction = auction, 
                 months = months,
                 season = season,
                 startDate = startDate,
                 endDate   = endDate), class="ForwardReserveAuction")
}


#########################################################################
# how to order an FRA 
# where x is a vector of auctions c("S07", "S08", "W07-08")
# return an index with the chronological ordering
#
order.FRA <- function(x)
{
  aux <- lapply(x, ForwardReserveAuction)
  startDates <- as.Date(sapply(aux, "[[", "startDate"))
  order(startDates)
}




################################################################
# Calculate PnL impact on marks change
# Where qq is a data.frame with columns c("service", "month", "location", "qty")
#
calc_pnl_impact_fwdres <- function(day1, day2, qq)
{
  qq <- subset(qq, month >= nextMonth(day1))
  
  endDate <- max(qq$month)
  PP1 <- pull_marks_fwdres(day1, day1, endDate)
  PP1$asOfDate <- PP1$bucket <- NULL

  PP2 <- pull_marks_fwdres(day2, day2, endDate)
  #PP2 <- read_fwdres_marks_file(day2)
  PP2$asOfDate <- PP2$bucket <-  NULL

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  aux <- calc_pnl_pq(PP1, PP2, qq, qq, groups=c("id", "serviceType", "year"))
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
}


################################################################
# Get the fwd reserve auction name from a given month.
# Name is "S10" or "W11-12" 
# Where months is a begining of the month date.
#
fwdres_auction <- function(months)
{
  sapply(months, function(month) {
    m <- as.numeric(format(month, "%m"))
    yyyy <- format(month, "%Y")
    if (m %in% c(6,7,8,9)) {
      paste("S", substring(yyyy, 3, 4), sep="")
    } else if (m >= 10) {      
      paste("W", substring(yyyy, 3, 4), "-",
            substring(as.numeric(yyyy)+1, 3, 4), sep="")
    } else {
      paste("W", substring(as.numeric(yyyy)-1, 3, 4), "-",
            substring(yyyy, 3, 4), sep="")
    }
  })
}


################################################################
# Get the fwd reserve auction name from a given month (the opposite
# of fwdres_auction ).
# Name is "S10" or "W11-12" 
# Where months is a begining of the month date.
#
.fwdres_get_months <- function(auction="W12-13")
{
  auction <- toupper(auction)
  yrStart <- paste("20", substr(auction, 2, 3), sep="")
  if (substr(auction, 1, 1) == "W") {
    months  <- seq(as.Date(paste(yrStart, "-10-01", sep="")), by="1 month",
                   length.out=9)
  } else if (substr(auction, 1, 1) == "S") {
    months  <- seq(as.Date(paste(yrStart, "-06-01", sep="")), by="1 month",
                   length.out=4)
  } else {
    stop("Unknown auction", auction)
  }

  months
}


################################################################
# get the days used to calculate the Local Reserve Requirements
# @param auctionName, e.g. "S12"
# @return a vector of days
# function is not vectorized!
#
get_lrr_days <- function(auctionName)
{
  yyyy   <- 2000+as.numeric(substring(auctionName, 2, 3))
  season <- substring(auctionName, 1, 1)
    
  an1 <- if (season=="S") {
    paste(season, yyyy-2001, sep="")
  } else {
    paste(season, yyyy-2001, "-", yyyy-2000, sep="")
  }
  auction <- ForwardReserveAuction(an1)
  d1 <- seq(auction$startDate, auction$endDate, by="1 day")

  an2 <- if (season=="S") {
    paste(season, yyyy-2002, sep="")
  } else {
    paste(season, yyyy-2002, "-", yyyy-2001, sep="")
  }
  auction <- ForwardReserveAuction(an2)
  d2 <- seq(auction$startDate, auction$endDate, by="1 day")

  c(d2, d1)
}


################################################################
# get all related fwdres positions
#
# 
get_positions_fwdres_ccg <- function(asOfDate)
{
  # these are MWH, not the obligation
  portfolio <- "Nepool Load Portfolio"
  books <- secdb.getBooksFromPortfolio(portfolio)
  
  filter <- function(x){ x$`Service Type` == "FwdRes" }  
  res <- lapply(as.list(books), function(x){
    secdb.flatReport(asOfDate, x, filter=filter, to.df=TRUE)
  })
  res <- do.call(rbind, res)
  
  if (any(grepl("^ECSH ", rownames(res))))
    stop("Found some ECSH in the leaves ...")
  qq <- cast(res,  ServiceType + month + DeliveryPoint +
    SettlementType ~ ., sum, fill=0, value="Quantity")
  names(qq)[c(3,5)] <- c("location", "value")

  qq <- cbind(id = "ccg.load", qq)
  qq  
}


################################################################
#
# 
get_positions_fwdres_cne <- function(asOfDate)
{
  # these are MWH, not the obligation
  portfolio <- "CNE NewEngland Load Portfolio"
  books <- secdb.getBooksFromPortfolio(portfolio)

  filter <- function(x){ x$`Service Type` == "FwdRes" }  
  res <- lapply(as.list(books), function(x){
    secdb.flatReport(asOfDate, x, filter=filter, to.df=TRUE)
  })
  res <- do.call(rbind, res)
  
  if (any(grepl("^ECSH ", rownames(res))))
    stop("Found some ECSH in the leaves ...")
  qq <- cast(res,  ServiceType + month + DeliveryPoint +
    SettlementType ~ ., sum, fill=0, value="Quantity")
  names(qq)[c(3,5)] <- c("location", "value")

  qq <- cbind(id = "cne.load", qq)
  qq
}


################################################################
#
# 
get_positions_fwdres_units <- function(asOfDate)
{
 books <- c("XFRMGHM1", "XFRMGHM2", "XFRMGHM3", "XLSTREET",
            "XMEDWAY1", "XMEDWAY2", "XMEDWAY3", "MYSTIC7M")
 filter <- function(x){ x$`Service Type` %in% c("TMNSR", "TMOR") }  
 res <- secdb.flatReport(asOfDate, books, filter=filter, to.df=TRUE)

 agg <- cast(res, month + ServiceType + DeliveryPoint + SettlementType ~ ., sum,
             value="Quantity")
 colnames(agg)[ncol(agg)] <- "value"
 
 gen <- cbind(id="gen", data.frame(agg))
 colnames(gen)[which(colnames(gen)=="DeliveryPoint")] <- "location"
 
 gen 
}


################################################################
#
# 
get_positions_fwdres_hedges <- function(asOfDate)
{
  books <- c("MTMANCHG")
  filter <- function(x){ x$`Service Type` %in% c("FwdRes",
    "FwdResCP", "TMNSR", "TMOR") }  
  res <- secdb.flatReport(asOfDate, books, filter=filter, to.df=TRUE)
  res <- subset(res, grepl("^EPHY ", rownames(res)))
  qq <- cast(res,  ServiceType + month + DeliveryPoint  ~ ., sum, fill=0,
             value="Quantity")
  names(qq)[3:4] <- c("location", "value")
    
  qq <- cbind(id = "ccg.mtm", qq)

  qq
}


#########################################################################
# two files additional files that need to be sent to External Data.
#
make_new_marks <- function(fwdres, capacity, auction="W12-13")
{
  months <- .fwdres_get_months(auction)

  # get the peak hours
  res <- secdb.getHoursInBucket("NEPOOL", "Flat",
    as.POSIXct(paste(months[1], "01:00:00")),
    as.POSIXct(paste(nextMonth(months[length(months)]), "00:00:00")))
  res$month <- as.Date(format(res$time-1, "%Y-%m-01"))
  hrs <- cast(res, month ~ ., length, value="time")
  names(hrs)[2] <- "hrs"

  rLog("TMOR CT (7x24 product) file:")
  tmorct <- hrs
  tmorct$CT <- rep(fwdres$CT, nrow(hrs))/tmorct$hrs
  print(tmorct[,c("month", "CT")], row.names=FALSE)
  
  rLog("FwdResCP (5x16 product) file:")
  res <- secdb.getHoursInBucket("NEPOOL", "Peak",
    as.POSIXct(paste(months[1], "01:00:00")),
    as.POSIXct(paste(nextMonth(months[length(months)]), "00:00:00")))
  res$month <- as.Date(format(res$time-1, "%Y-%m-01"))
  hrs <- cast(res, month ~ ., length, value="time")
  names(hrs)[2] <- "hrs"

  fwdrescp <- hrs
  fwdrescp$CT  <- rep(fwdres$CT - capacity$ROP, nrow(hrs))/fwdrescp$hrs
  fwdrescp$ROP <- rep(fwdres$ROP- capacity$ROP, nrow(hrs))/fwdrescp$hrs
  print(fwdrescp[,c("month", "CT", "ROP")], row.names=FALSE)

}



#######################################################################
# Pull hist fwdres penalties
# from SR_RSVCHARGE 
# 
#
.pull_hist_fwdres_penalties <- function(startTime=NULL, endTime=Sys.time())
{
  if (is.null(startTime))
    startTime <- as.POSIXct(paste(as.numeric(format(Sys.Date(), "%Y"))-3, 
       "-01-01 01:00:00", sep=""))

  # NEED TO FIX
  
  symbols <- c(
   "NEPOOL_SMD_DA_4000_lmp",         "hub.da",
   "NEPOOL_SMD_RT_4000_lmp",         "hub.rt",            
   "NeRpt_rtCus_PoolLoadObl",        "load",               # pool load
   "NeRpt_rtCus_PoolMargLossRev",    "pool.lossrev.rt",    # pool RT marg loss revenue
   "NeRpt_rtCus_DAPoolMargLossRev",  "pool.lossrev.da",    # pool DA marg loss revenue
   "NERpt_RTCuS_MargLossRevAlloc",   "ccg.lossrev$",       # ccg marg loss revenue
   "NCRpt_RTCuS_MargLossRevAlloc",   "cne.lossrev$",       # cne marg loss revenue
   "NERpt_RTCuS_AdjLoadObl",         "ccg.rt.adjload",     # ccg adj load oblg
   "NCRpt_RTCuS_AdjLoadObl",         "cne.rt.adjload"      # ccg adj load oblg
               
 #  "NeRpt_rtCus_PoolEnergyStl",      "pool.engy.stl.rt",
 #  "NeRpt_rtCus_PoolLossRev",        "pool.loss.rev.rt",
 #  "NeRpt_rtCus_PoolExtInadCost",    "pool.ext.inad.cost",
 #  "NeRpt_rtCus_PoolEmerCost",       "pool.emer.cost.rt",               
 #  "NeRpt_rtCus_PoolEnergyStl",      "pool.ener.stl.da"
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startTime, endTime)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)

  ## HH <- cbind(time=index(HH), as.data.frame(HH))
  ## rownames(HH) <- NULL

  # discard NA obs
  HH <- HH[!is.na(HH$`cne.lossrev$`),]
  
  HH
}




#########################################################################
# pull fwd reserve marks from SecDb, in $/MWh
# In SecDb, it doesn't matter that the bucket is Peak or Flat for FwdRes,
# the price is the same (correct Peak price)
#
pull_marks_fwdres <- function(asOfDate=Sys.Date(),
   startDate=nextMonth(asOfDate),
   endDate=NULL,
   zones=c("CT", "NEMA", "ROP"),
   service=c("FWDRES", "TMNSR", "TMOR", "FWDRESCP"))
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(startDate, "%Y"))+6, "-12-01", sep=""))
  
  rLog("Pull FWDRES marks as of", format(asOfDate))
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- zones
  bucket      <- c("FLAT")
  serviceType <- service
  PP <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  PP$bucket[which(PP$serviceType == "FWDRES")] <- "PEAK"

  
  PP
}


#########################################################################
# read the new marks from the file you have created for submission!
#
read_fwdres_marks_file <- function( asOfDate=Sys.Date() )
{
  require(xlsx)
  filename <- paste("S:/All/Structured Risk/NEPOOL/Marks/Ancillaries/",
    "NEPOOL Zonal Ancillaries FwdRes_", asOfDate ,".xls", sep="")

  MM <- read.xlsx2(filename, sheetIndex=2,
    colClasses=c("Date", rep("numeric", 4)))
  MM <- melt(MM, id=1)
  names(MM)[1:2] <- c("month", "location")

  MM$serviceType <- "FWDRES"
  MM$bucket <- "FLAT"
  
  MM
}


#########################################################################
# get the tlp ONPEAK load forecast by reserve zone.
#
.tlp_reservezone_load <- function()
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  startDt <- as.POSIXct(paste(nextMonth(),  "01:00:00", sep=""))
  endDt   <- as.POSIXct(paste(as.numeric(format(Sys.Date(), "%Y" ))+5,
     "-01-01 00:00:00", sep=""))
  res <- tlp.nepool.load.fcst(startDt, endDt)

  #res$system <- apply(res, 1, sum)
  res$rop  <- res[,"4001"] + res[,"4002"] + res[,"4003"] + res[,"4005"] +
    res[,"4006"] + res[,"4007"]
  res$ct   <- res[,"X4004"]
  res$nema <- res[,"X4008"]

  # pick only the ONPEAK MWh
  res <- res[,c("ct", "nema", "rop")]
  res <- filterBucket(res, "5X16")
  res <- res[[1]]

  # the energy, MWh
  ene <- aggregate(res, as.Date(format(index(res)-1, "%Y-%m-01")),
    sum, na.rm=TRUE)

  EE <- cbind(month=index(ene), data.frame(ene))
  rownames(EE) <- NULL
  
  EE
}




#########################################################################
#########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)
  require(lattice)
  require(PM)

  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Ancillaries/lib.fwd.reserves.R")

  #====================================================================
  # get marks
  asOfDate <- Sys.Date()
  DD <- pull_marks_fwdres(asOfDate=asOfDate)
  dd <- cast(DD, asOfDate + bucket + serviceType + month ~ location, I, fill=NA)
  print(dd, row.names=FALSE)

  print(subset(dd, serviceType=="FWDRES"), row.names=FALSE)  # for the remarking

  
  xyplot(value ~ month, data=DD, groups=location, type=c("g", "b"),
    ylab="Forward Reserves price, $/MWh",
    subset=serviceType=="FWDRES",      
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(DD$location))))

  subset(dd[,c("month", "CT")],
    month >= as.Date("2013-06-01") & month <= as.Date("2014-05-01"))


  #====================================================================
  # make marks for the summer 2013 auction
  auction  <- "S13"
  fwdres   <- list(CT=5946, NEMA=5946, ROP=5946)  # $/MW-month
  capacity <- list(ROP=2951)                      # $/MW-month
  make_new_marks(fwdres, capacity, auction)

  
  #====================================================================
  # make marks for the winter 2012-2013 auction
  auction  <- "W12-13"
  fwdres   <- list(CT=3301, NEMA=3301, ROP=3301)  # $/MW-month
  capacity <- list(ROP=2951)                      # $/MW-month
  make_new_marks(fwdres, capacity, auction)
  

  
  #====================================================================
  # calc PnL after change in marks
  day1 <- as.Date("2014-08-27")
  day2 <- as.Date("2014-08-28")
  ccg <- get_positions_fwdres_ccg( day1 )
  cne <- get_positions_fwdres_cne( day1 )
  gen <- get_positions_fwdres_units( day1 )
  #hdg <- get_positions_fwdres_hedges( day1 )
  qq  <- rbind(ccg, cne) #, gen) #, hdg)
  names(qq)[2] <- "serviceType"
  qq$serviceType <- toupper(qq$serviceType)
  

  aux <- calc_pnl_impact_fwdres(day1, day2, qq)
  print(aux$gPnL, row.names=FALSE)
  cast(aux$gPnL, id ~ ., sum, value="PnL")
  cast(aux$gPnL, id ~ year, sum, fill=0, value="PnL")

  pnl.gen <- subset(aux$PnL, id == "gen")
  subset(pnl.gen, location=="NEMA" & serviceType=="TMNSR")
  
  

  cast(ccg, month ~ location, I, fill=0, value="value")
  

  #====================================================================
  # estimate FwdRes revenues from units
  clearingPriceNetCapacity <- 3000
  noMonths <- 12
  mw <- 72
  3000*noMonths*mw


  #====================================================================
  # fwdres penalties
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  reportName <- "SR_RSVCHARGE"; tab <- 1
  
  months <- seq(as.Date("2013-05-01"), nextMonth(), by="1 month")
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="CNE", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  # load archive 
  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)
  res <- cast(AA, reportDay ~ ., sum, na.rm=TRUE,
    value="Pool.Forward.Reserve.Market.Penalty")
  colnames(res)[2] <- "Pool.Forward.Reserve.Market.Penalty"
  res$reportDay <- as.Date( res$reportDay )
  
  plot(res$reportDay, res$Pool.Forward.Reserve.Market.Penalty,
       type="l", col="blue")
  
  
  #====================================================================
  auctions <- c("S07", "S08", "S09", "S10", "S11", "S12", "W07-08",
                "W08-09", "W09-10", "W10-11", "W11-12")
  auctions[order.FRA(auctions)] 

  auctions <- c("S07", "W08-09")
  ForwardReserveAuction(auctions)
  


  
  
  #====================================================================
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  
  months <- seq(as.Date("2011-06-01"), as.Date("2011-09-01"), by="1 month")
  for (month in as.character(months)) {
    rLog("working on month", month)
    .process_SR_RSVCHARGE(month)
  }


  
  

}









################################################################
# get fwdres positions for a given portfolio
#
## get_positions_fwdres <- function(asOfDate,
##   portfolio="PM East Retail Portfolio", service="FWDRES", from="PRISM")
## {
##   if (from == "SECDB") {
##     qq <- secdb.readDeltas(asOfDate, portfolio)
##     qq <- qq[grep(" 5X16 ", toupper(names(qq)))]
##     stop("Not Implemented!")
    
##   } else {   # PRISM
##     source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
##     source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

##     rLog("Pull", service, "positions for", portfolio, "as of", format(asOfDate))
##     aux <- get.positions.from.blackbird( asOfDate=asOfDate, fix=FALSE,
##       service=service, portfolio=portfolio)

##     qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
##       value="delta")
##     names(qq) <- c("month", "location", "qty")

##     cbind(service=service, as.data.frame(qq))
##   }
  
## }


## #########################################################################
## # SR_RSVCHARGE Summary of Pool Charges and Participant's Charges by
## # hour, issued upon settlement of Market Day.
## #
## # Did this for Syam.  Data is now in tsdb.  Which symbols ?! 
## #
## .process_SR_RSVCHARGE <- function(month)
## {
##   yyyymm <- format(as.Date(month), "%Y%m")
##   DIR <- paste("S:/Data/NEPOOL/RawData/OldZips/Rpt_", yyyymm, "/", sep="")

##   files <- list.files(DIR, full.names=TRUE, pattern="^SR_RSVCHARGE")
##   reportDate <- gsub(".*SR_RSVCHARGE.*_[[:digit:]]+_(.*)_.*", "\\1", files) 
##   uFiles <- unlist(lapply(split(files, reportDate), function(x){sort(x)[length(x)]}))
  
##   res1 <- vector("list", length=length(uFiles))
##   res2 <- vector("list", length=length(uFiles))
##   for (f in seq_along(uFiles)) {
##     aux <- .process_report(uFiles[f])
##     res1[[f]] <- aux[[3]]
##     res2[[f]] <- aux[[4]]
##   }
##   res1 <- do.call(rbind, res1)
##   res2 <- do.call(rbind, res2)

##   DIR_OUT <- "S:/All/Structured Risk/NEPOOL/Temporary/"
##   fname <- paste(DIR_OUT, "tab3_", month, ".csv", sep="")
##   write.csv(res1, fname, row.names=FALSE)

##   fname <- paste(DIR_OUT, "tab4_", month, ".csv", sep="")
##   write.csv(res2, fname, row.names=FALSE)
## }


## ################################################################
## # Get all the input data ready for the fwd reserves positions
## # spreadsheet
## #
## build_positions_xlsx_fwdres <- function(asOfDate=Sys.Date()-1)
## {
##   EE <- .tlp_reservezone_load()

##   # get the load positions
##   portfolio <- "Nepool Load Portfolio"
##   ccg <- get_positions_fwdres(asOfDate, portfolio=portfolio)
##   portfolio <- "PM East Retail Portfolio"
##   cne <- get_positions_fwdres(asOfDate, portfolio=portfolio)
##   LL <- rbind(ccg, cne)
##   LL <- cast(LL, month ~ location, sum, fill=0, value="qty")


##   # get the attrition positions  -- totally SCREWED numbers!!!
##   portfolio <- "Nepool Mixed Attr Portfolio"   
##   attr <- get_positions_fwdres(asOfDate, portfolio=portfolio, from="SECDB")
##   attr <- data.frame(cast(attr, month ~ location, sum, fill=0, value="qty"))
##   names(attr)[2:ncol(attr)] <- paste("attr.", names(attr)[2:ncol(attr)], sep="")

##   QQ <- merge(LL, attr, by="month", all.x=TRUE)
##   QQ <- merge(QQ, EE, by="month", all.x=TRUE)
##   QQ[is.na(QQ)] <- 0

##   rLog("Paste data below in 'CCG Deltas' sheet")
##   print(QQ, row.names=FALSE)
##    ## WRONG ATTRITION numbers pasted after the merge!
  
## }
