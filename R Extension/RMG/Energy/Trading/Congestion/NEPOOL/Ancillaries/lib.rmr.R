# Move all the functions related to RMR to this library
# For specific RMR zonal curve marking/analysis see: NEPOOL/Ancillaries/analyze.rmr.R
#
#
# .check_seasonality
# calc_pnl_impact_rmr
# get_rmr_position
#
# .monthly_actuals_rmr
# .plot_rmr
# .pull_hist_rmr             - read actual rmr costs from tsdb
# .pull_marks_rmr            - read rmr marks from SecDb
# .report                    - make the historical/fwd spreadsheet output
# .simulate_rmr              - use the meboot package to get some variablity
# .update_xlsx_rmr           - for the NCPC spreadsheet with actuals




################################################################
# X a data.frame as returned by .report function (with hist+fwd info)
# show the RT + DA RMR together
#
.check_seasonality <- function(X, zone=4001)
{
  ind <- c(1, grep(zone, names(X)))
  Xz <- X[,ind]

  Xz[Xz == Inf] <- 0
  Xz$year <- format(as.Date(Xz$month), "%Y")
  Xz$mon <- factor(format(as.Date(Xz$month), "%b"), levels=month.abb)

  Xz$total.RMR <- apply(Xz[,grep(zone, names(Xz))], 1, sum)

  cast(Xz, mon ~ year, mean, fill=NA, value="total.RMR")  
}


################################################################
# Calculate PnL impact on marks change
#
calc_pnl_impact_rmr <- function(day1, day2, 
   endDate=as.Date("2017-12-01"))
{
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_rmr(day1, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_rmr(day1, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- subset(qq1, month >= nextMonth())

  if (day2 == Sys.Date()) {
    rLog("Using the same positions for ", format(day2))
    qq2 <- qq1
  } else {
    books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
    qqA   <- cbind(company="ccg", get_positions_rmr(day2, books))
    books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
    qqB   <- cbind(company="cne", get_positions_rmr(day2, books))
    qq2   <- rbind(qqA, qqB)
    qq2   <- subset(qq2, month >= nextMonth())
  }
  
  PP1 <- .pull_marks_rmr(day1,  startDate=day1)
  PP1$asOfDate <- NULL
  
  PP2 <- .pull_marks_rmr(day2, startDate=day2)
  PP2$asOfDate <- NULL

  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
}


################################################################
# get positions
#
get_positions_rmr <- function(asOfDate, books)
{
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
   fix=TRUE, service="RMR")
  
  qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
             value="notional")
  names(qq) <- c("month", "location", "value")

  data.frame(qq)
}



################################################################
# calculate monthly actuals, this is what you'll paste into
# the NCPC Tracker.xls in the Summary sheet. 
# @month is an R Date, bom
#
.monthly_actuals_rmr <- function( month )
{

  if (month < as.Date("2014-08-01")) {
    # get the historical hourly data
    startDt <- as.POSIXct(paste(format(month), "01:00:00"))
    endDt   <- as.POSIXct(paste(format(nextMonth(month)), "00:00:00"))
    HH <- .pull_hist_rmr(ptids=NULL, start.dt=startDt, end.dt=endDt,
                         long=FALSE)
  
    res <- aggregate(HH, as.Date(format(index(HH)-1, "%Y-%m-01")),
                    sum, na.rm=TRUE)
  } else {
    # get the daily data after the web redesign
    startDt <- month
    endDt   <- nextMonth( month ) - 1

    DD <-  .pull_hist_rmr2(ptids=NULL, start.dt=startDt, end.dt=endDt)
    if ( is.null(DD) )
      return( NULL )
    res <- aggregate(DD, as.Date(format(index(DD), "%Y-%m-01")),
                    sum, na.rm=TRUE)
  }

  
  MM <- zoo(NULL, index(res))
  MM$me.rt.lscp.cost   <- if (is.null(res$RT_4001_Dollars)) 0 else res$RT_4001_Dollars
  MM$ct.rt.lscp.cost   <- if (is.null(res$RT_4004_Dollars)) 0 else res$RT_4004_Dollars
  MM$ri.rt.lscp.cost   <- if (is.null(res$RT_4005_Dollars)) 0 else res$RT_4005_Dollars
  MM$sema.rt.lscp.cost <- if (is.null(res$RT_4006_Dollars)) 0 else res$RT_4006_Dollars
  MM$wma.rt.lscp.cost  <- if (is.null(res$RT_4007_Dollars)) 0 else res$RT_4007_Dollars
  MM$nema.rt.lscp.cost <- if (is.null(res$RT_4008_Dollars)) 0 else res$RT_4008_Dollars
  MM$nh.rt.lscp.cost   <- if (is.null(res$RT_4002_Dollars)) 0 else res$RT_4002_Dollars
  MM$vt.rt.lscp.cost   <- if (is.null(res$RT_4003_Dollars)) 0 else res$RT_4003_Dollars
  
  MM$me.da.lscp.cost   <- if (is.null(res$DA_4001_Dollars)) 0 else res$DA_4001_Dollars
  MM$ct.da.lscp.cost   <- if (is.null(res$DA_4004_Dollars)) 0 else res$DA_4004_Dollars
  MM$ri.da.lscp.cost   <- if (is.null(res$DA_4005_Dollars)) 0 else res$DA_4005_Dollars
  MM$sema.da.lscp.cost <- if (is.null(res$DA_4006_Dollars)) 0 else res$DA_4006_Dollars
  MM$wma.da.lscp.cost  <- if (is.null(res$DA_4007_Dollars)) 0 else res$DA_4007_Dollars
  MM$nema.da.lscp.cost <- if (is.null(res$DA_4008_Dollars)) 0 else res$DA_4008_Dollars
  MM$nh.da.lscp.cost   <- if (is.null(res$DA_4002_Dollars)) 0 else res$DA_4002_Dollars
  MM$vt.da.lscp.cost   <- if (is.null(res$DA_4003_Dollars)) 0 else res$DA_4003_Dollars

  MM$me.rt.rtlo   <- res$RT_4001_Load
  MM$ct.rt.rtlo   <- res$RT_4004_Load
  MM$ri.rt.rtlo   <- res$RT_4005_Load
  MM$sema.rt.rtlo <- res$RT_4006_Load
  MM$wma.rt.rtlo  <- res$RT_4007_Load
  MM$nema.rt.rtlo <- res$RT_4008_Load
  MM$nh.rt.rtlo   <- res$RT_4002_Load
  MM$vt.rt.rtlo   <- res$RT_4003_Load

  MM$me.rt.lscp.rate   <- MM$me.rt.lscp.cost/MM$me.rt.rtlo
  MM$ct.rt.lscp.rate   <- MM$ct.rt.lscp.cost/MM$ct.rt.rtlo
  MM$ri.rt.lscp.rate   <- MM$ri.rt.lscp.cost/MM$ri.rt.rtlo
  MM$sema.rt.lscp.rate <- MM$sema.rt.lscp.cost/MM$sema.rt.rtlo
  MM$wma.rt.lscp.rate  <- MM$wma.rt.lscp.cost/MM$wma.rt.rtlo
  MM$nema.rt.lscp.rate <- MM$nema.rt.lscp.cost/MM$nema.rt.rtlo
  MM$nh.rt.lscp.rate   <- MM$nh.rt.lscp.cost/MM$nh.rt.rtlo
  MM$vt.rt.lscp.rate   <- MM$vt.rt.lscp.cost/MM$vt.rt.rtlo

  # you calculate the rate this way so you can compare with the marks!
  MM$me.da.lscp.rate   <- MM$me.da.lscp.cost/MM$me.rt.rtlo
  MM$ct.da.lscp.rate   <- MM$ct.da.lscp.cost/MM$ct.rt.rtlo
  MM$ri.da.lscp.rate   <- MM$ri.da.lscp.cost/MM$ri.rt.rtlo
  MM$sema.da.lscp.rate <- MM$sema.da.lscp.cost/MM$sema.rt.rtlo
  MM$wma.da.lscp.rate  <- MM$wma.da.lscp.cost/MM$wma.rt.rtlo
  MM$nema.da.lscp.rate <- MM$nema.da.lscp.cost/MM$nema.rt.rtlo
  MM$nh.da.lscp.rate    <- MM$nh.da.lscp.cost/MM$nh.rt.rtlo
  MM$vt.da.lscp.rate    <- MM$vt.da.lscp.cost/MM$vt.rt.rtlo

  MM$me.total.lscp.rate   <- MM$me.da.lscp.rate + MM$me.rt.lscp.rate   
  MM$ct.total.lscp.rate   <- MM$ct.da.lscp.rate + MM$ct.rt.lscp.rate   
  MM$ri.total.lscp.rate   <- MM$ri.da.lscp.rate + MM$ri.rt.lscp.rate   
  MM$sema.total.lscp.rate <- MM$sema.da.lscp.rate + MM$sema.rt.lscp.rate   
  MM$wma.total.lscp.rate  <- MM$wma.da.lscp.rate + MM$wma.rt.lscp.rate   
  MM$nema.total.lscp.rate <- MM$nema.da.lscp.rate + MM$nema.rt.lscp.rate   
  MM$nh.total.lscp.rate   <- MM$nh.da.lscp.rate + MM$nh.rt.lscp.rate   
  MM$vt.total.lscp.rate   <- MM$vt.da.lscp.rate + MM$vt.rt.lscp.rate   

  MM
}


################################################################
# Plot RMR hist and marks side by side.  Each 
# @param DD is a data.frame with columns "month", and other variables
# @param columns is a vector of strings showing the variables to plot,
#    each variable in a different panel
# @param label a string to add to the ylab, for example the zone name  
#
.plot_rmr <- function( DD, columns=c("4001 RT"),
   maxDate=as.Date("2016-12-01"), label="")
{
  require(lattice)
  ind <- which( DD$month >= Sys.Date() )
  DD$type <- "Hist"
  DD$type[ind] <- "Fwd"
  DD <- subset(DD, month <= maxDate)
  
  ind <- c(1, ncol(DD), which(names(DD) %in% columns))
  aux <- melt(data.frame(DD[, ind]), id=1:2)
  aux$variable <- gsub("X", "", aux$variable)
  aux$month <- as.Date(aux$month)
  aux$value <- as.numeric(aux$value)

  xyplot(value ~ month|variable, data=aux, groups=type,
    type=c("g", "o"),
    ylab=paste(label, "RMR costs, $/MWh"),
    layout=c(1,length(unique(aux$variable))),
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(aux$type))),
    scales="free"
  )
}


################################################################
# plot RMR hist dollars by month
# aggregate DA and RT RMR
#
.plot_rmr_dollars <- function(HH, zones=c("NEMA", "SEMA", "RI"), ZZ)
{
  ind <- grep("_Dollars", colnames(HH))
  MM <- aggregate(HH[,ind], as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)

  aux <- melt(cbind(month=index(MM), as.data.frame(MM)), id=1)
  aux$ptid <- as.numeric(gsub(".*_(.*)_Dollars", "\\1", aux$variable))
  aux <- cast(aux, month + ptid ~ ., sum, fill=NA)
  colnames(aux)[3] <- "rmr_dollars"

  aux <- merge(aux, ZZ[,c("ptid", "zone")], by="ptid")
  aux <- aux[order(aux$zone, aux$month),]
  
  xyplot(rmr_dollars/1000000 ~ month|zone, data=aux,
    type=c("g", "o"),
    ylab="RMR DA+RT costs, M$",
    layout=c(1,length(zones)),
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(zones)),
    #scales="free",
    subset=aux$zone %in% zones     
  )
  
}



################################################################
# In tsdb folder /NEPOOL/Nepool Data/Nepool Transmission Congestion/
#
# Called LSCP (2nd contingency protection)
#
.pull_hist_rmr <- function(ptids=NULL,
  start.dt=as.POSIXct("2008-01-01 01:00:00"), end.dt=Sys.time(),
  long=TRUE)
{
  symbs <- matrix(c(
    "nepl_tc_da_conn_rmrmw",  "DA_4004_MW",
    "nepl_tc_da_maine_rmrmw", "DA_4001_MW",
    "nepl_tc_da_nema_rmrmw",  "DA_4008_MW",
    "nepl_tc_da_nh_rmrmw",    "DA_4002_MW",
    "nepl_tc_da_ri_rmrmw",    "DA_4005_MW",
    "nepl_tc_da_sema_rmrmw",  "DA_4006_MW",
    "nepl_tc_da_vt_rmrmw",    "DA_4003_MW",
    "nepl_tc_da_wcma_rmrmw",  "DA_4007_MW",

    "nepl_tc_rt_conn_rmrmw",  "RT_4004_MW",
    "nepl_tc_rt_maine_rmrmw", "RT_4001_MW",
    "nepl_tc_rt_nema_rmrmw",  "RT_4008_MW",
    "nepl_tc_rt_nh_rmrmw",    "RT_4002_MW",
    "nepl_tc_rt_ri_rmrmw",    "RT_4005_MW",
    "nepl_tc_rt_sema_rmrmw",  "RT_4006_MW",
    "nepl_tc_rt_vt_rmrmw",    "RT_4003_MW",
    "nepl_tc_rt_wcma_rmrmw",  "RT_4007_MW",

    "nepl_tc_da_conn_rmrncpc",  "DA_4004_Dollars",
    "nepl_tc_da_maine_rmrncpc", "DA_4001_Dollars",
    "nepl_tc_da_nema_rmrncpc",  "DA_4008_Dollars",
    "nepl_tc_da_nh_rmrncpc",    "DA_4002_Dollars",
    "nepl_tc_da_ri_rmrncpc",    "DA_4005_Dollars",
    "nepl_tc_da_sema_rmrncpc",  "DA_4006_Dollars",
    "nepl_tc_da_vt_rmrncpc",    "DA_4003_Dollars",
    "nepl_tc_da_wcma_rmrncpc",  "DA_4007_Dollars",

    "nepl_tc_rt_conn_rmrncpc",  "RT_4004_Dollars",
    "nepl_tc_rt_maine_rmrncpc", "RT_4001_Dollars",
    "nepl_tc_rt_nema_rmrncpc",  "RT_4008_Dollars",
    "nepl_tc_rt_nh_rmrncpc",    "RT_4002_Dollars",
    "nepl_tc_rt_ri_rmrncpc",    "RT_4005_Dollars",
    "nepl_tc_rt_sema_rmrncpc",  "RT_4006_Dollars",
    "nepl_tc_rt_vt_rmrncpc",    "RT_4003_Dollars",
    "nepl_tc_rt_wcma_rmrncpc",  "RT_4007_Dollars",

    "nepool_zonal_4001_load_rt", "RT_4001_Load",
    "nepool_zonal_4002_load_rt", "RT_4002_Load",
    "nepool_zonal_4003_load_rt", "RT_4003_Load",
    "nepool_zonal_4004_load_rt", "RT_4004_Load",
    "nepool_zonal_4005_load_rt", "RT_4005_Load",
    "nepool_zonal_4006_load_rt", "RT_4006_Load",
    "nepool_zonal_4007_load_rt", "RT_4007_Load",
    "nepool_zonal_4008_load_rt", "RT_4008_Load",
    "nepool_load_hist",          "RT_pool_Load"
   ), ncol=2, byrow=TRUE)
   colnames(symbs) <- c("symbol", "name")


  if (!is.null(ptids)){
    allptids <- as.numeric(gsub(".*_(.*)_.*", "\\1", symbs[,2]))
    symbs <- symbs[which(allptids %in% ptids), ]
  }
    
  DD <- FTR:::FTR.load.tsdb.symbols(symbs[,"symbol"],
    start.dt=start.dt, end.dt=end.dt)
  names(DD) <- symbs[, "name"]

  if (long) {
    DD <- melt(as.matrix(DD))
    names(DD)[1:2] <- c("time", "variable")
    DD <- subset(DD, value !=0)   # HUUGE reduction in size
  
    DD$ptid <- as.numeric(gsub(".*_(.*)_.*", "\\1", DD$variable))
    DD$type <- gsub(".*_.*_(.*)", "\\1", DD$variable)
    DD$dart <- gsub("(.*)_.*_.*", "\\1", DD$variable)
    DD$time <- as.POSIXct(as.character(DD$time))
  }
    
  DD
}


################################################################
# In tsdb folder /PM/NEPOOL
# need to update from lib.NEPOOL.webservices.R
# After the ISO redesigned the website (8/23/2014), the new data
# is daily comes from the webservices and it's late (one month late) 
# @param start.dt an R Date
# @param end.dt an R Date
#
.pull_hist_rmr2 <- function(ptids=NULL,
  start.dt=as.Date("2012-01-01"),
  end.dt=Sys.Date())
{
  symbs <- matrix(c(
    "pm_ne_dDaNcpcLscpCharge_9004",  "DA_4004_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9001",  "DA_4001_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9008",  "DA_4008_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9002",  "DA_4002_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9005",  "DA_4005_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9006",  "DA_4006_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9003",  "DA_4003_Dollars",
    "pm_ne_dDaNcpcLscpCharge_9007",  "DA_4007_Dollars",

    "pm_ne_dRtNcpcLscpCharge_9004",  "RT_4004_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9001",  "RT_4001_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9008",  "RT_4008_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9002",  "RT_4002_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9005",  "RT_4005_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9006",  "RT_4006_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9003",  "RT_4003_Dollars",
    "pm_ne_dRtNcpcLscpCharge_9007",  "RT_4007_Dollars"
   ), ncol=2, byrow=TRUE)
   colnames(symbs) <- c("symbol", "name")
                    
  if (!is.null(ptids)){
    allptids <- as.numeric(gsub(".*_(.*)_.*", "\\1", symbs[,2]))
    symbs <- symbs[which(allptids %in% ptids), ]
  }

  # daily data in HB format; need to pull more data because
  # many days don't have anything and the function call fails
  DD <- FTR:::FTR.load.tsdb.symbols(symbs[,"symbol"],
    start.dt=as.POSIXct(paste(format(start.dt-6*30), "00:00:00")), 
    end.dt=as.POSIXct(paste(format(end.dt), "23:59:59")))
  if ( is.null(DD) )
    return( NULL )     # you may have no data
  DD <- window(DD, start=start.dt)
  
  names(DD) <- symbs[, "name"][symbs[,"symbol"] %in% names(DD)]
  index(DD) <- as.Date(index(DD))
  DD <- -DD   # show it as a positive charges

  # and the Load (new symbols post 8/23/2014)
  symbs <- matrix(c(
    "nepool_smd_rt_4001_ld", "RT_4001_Load",
    "nepool_smd_rt_4002_ld", "RT_4002_Load",
    "nepool_smd_rt_4003_ld", "RT_4003_Load",
    "nepool_smd_rt_4004_ld", "RT_4004_Load",
    "nepool_smd_rt_4005_ld", "RT_4005_Load",
    "nepool_smd_rt_4006_ld", "RT_4006_Load",
    "nepool_smd_rt_4007_ld", "RT_4007_Load",
    "nepool_smd_rt_4008_ld", "RT_4008_Load",
    "nepool_load_hist",      "RT_pool_Load"
   ), ncol=2, byrow=TRUE)
   colnames(symbs) <- c("symbol", "name")
  
  if (!is.null(ptids)){
    allptids <- as.numeric(gsub(".*_(.*)_.*", "\\1", symbs[,2]))
    symbs <- symbs[which(allptids %in% ptids), ]
  }

  # hourly data is in HE format
  LL <- FTR:::FTR.load.tsdb.symbols(symbs[,"symbol"],
    start.dt=as.POSIXct(paste(format(start.dt), "01:00:00")),
    end.dt=as.POSIXct(paste(format(end.dt+1), "00:00:00")))    
  names(LL) <- symbs[, "name"]
  
  LL <- aggregate(LL, as.Date(format(index(LL)-1, "%Y-%m-%d")),
                  sum, na.rm=TRUE)
  LL <- window(LL, end = max(index(DD)))
  
  res <- merge(DD, LL, all=TRUE) 
    
  res
}



################################################################
# this is LSCP NCPC, still called RMR in SecDb
#
.pull_marks_rmr <- function(asOfDate=Sys.Date(),
  startDate=nextMonth(asOfDate), endDate=NULL, 
  zones=c("NEMA", "SEMA", "WCMA", "MAINE", "NH", "RI", "CT", "VT"),
  market=c("RT", "DA"))
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+6, 
       "-12-01", sep=""))
  
  location    <- as.character(outer(zones, market, paste, sep=" "))
  bucket      <- c("FLAT")
  serviceType <- c("RMR")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  fwd <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  
  fwd
}


################################################################
# Calculate monthly $/MWh RMR costs by region, RT and DA
# @param HH a zoo timeseries with either hourly or daily data
# @param FF the forward marks as returned by .pull_marks_rmr
#
#
.report <- function(HH, FF=NULL)
{
  ZZ <- data.frame(
    ptid = 4001:4008,
    delivPt = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WCMA", "NEMA"),
    zone = c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
  
  # RMR dollars by month
  idx <- grep("_Dollars$|_Load$", names(HH))

  if (class(index(HH))=="POSIXct") {
    MM  <- aggregate(HH[,idx], as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)
  } else if (class(index(HH))=="Date"){
    MM <- aggregate(HH[,idx], as.Date(format(index(HH), "%Y-%m-01")), sum, na.rm=TRUE)
  }

  
  zones <- as.character(4001:4008)
  TT <- zoo(matrix(0, nrow=nrow(MM), ncol=16), index(MM))
  cnames <- as.character(outer(zones, c("DA", "RT"), paste, sep="_"))
  colnames(TT) <- cnames
  for (col in zones){
    ind1 <- paste("DA", col, "Dollars", sep="_")
    ind2 <- paste("RT", col, "Dollars", sep="_")
    ind3 <- paste("RT", col, "Load", sep="_")
    TT[,paste(col, "DA", sep="_")] <- if (ind1 %in% names(MM)) {
      MM[,ind1]/MM[,ind3]
    } else {
      0           # no hist RMR charges for this location
    }
    TT[,paste(col, "RT", sep="_")] <- if (ind2 %in% names(MM)) {
      MM[,ind2]/MM[,ind3]
    } else {
      0           # no hist RMR charges for this location
    }
  }
  TT <- cbind(month=index(TT), as.data.frame(TT))
  rownames(TT) <- NULL

  if (!is.null(FF)) {
    aux <- strsplit(FF$location, " ")
    FF$market  <- sapply(aux, "[[", 2)
    FF$delivPt <- sapply(aux, "[[", 1)
    FF <- merge(FF, ZZ[,c("ptid", "delivPt")], all.x=TRUE)

    FF <- cast(FF, month ~ ptid + market, I, fill=NA)
    FF$month <- format(FF$month)

    TT <- TT[,colnames(FF)]
    
    res  <- rbind(TT, FF)
  } else {
    res <- TT
  }
  
  res
}


################################################################
# simulate rmr 
#  HH - the hourly historical data as returned by .pull_hist_rmr
#  
#
.simulate_rmr <- function( ptid, HH, months.back=c(12, 24, 36, 48, 60))
{
  require(meboot)
  ## HHH <- HH
  ## HH <- window(HHH, start=as.POSIXct("2012-08-01 01:00:00"),
  ##              end=as.POSIXct("2012-09-01 00:00:00"))

  rLog("aggregate daily and simulate")
  x <- na.omit(aggregate(HH[,paste("RT_", ptid, "_Dollars", sep="")],
                         as.Date(format(index(HH)-1)), sum))
  SS <- meboot(as.numeric(x))
  y  <- SS$ensemble
  y[which(y < 0)] <- 0
  y  <- zoo(y, index(x))

  # aggreagate from daily to monthly the simulated RMR dollars 
  YY <- aggregate(y, as.Date(format(index(y), "%Y-%m-01")),
    sum, na.rm=TRUE)
  YY <- window(YY, end = prevMonth() )
  LL <- aggregate(HH[,paste("RT_", ptid, "_Load", sep="")],
    as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)
  LL <- window(LL, end = prevMonth() )

  # calculate simulated RMR cost in $/MWh
  RR <- YY/LL
 
  
  # incorporate seasonality and calculate quantiles
  .get_quantiles <- function(RR)
  {
  #  browser()
    ZZ <- as.matrix(RR)
    rownames(ZZ) <- format(index(RR))
    z <- aaply(as.matrix(ZZ), 1, function(x){
      quantile(as.numeric(x), probs=c(0, 0.05, 0.10, 0.25, 0.50, 0.75, 0.90, 0.95,1))})
    names(dimnames(z))[1] <- ""

    zz <- melt(z)
    colnames(zz)[1:2] <- c("month", "quantile")
    zz$mon <- format(as.Date(zz$month), "%b")
    zz <- cast(zz, quantile ~ mon, function(x){round(mean(x),2)}, fill=0)
    zz$quantile <- as.character(zz$quantile)
    zz <- zz[order(as.numeric(gsub("%", "", zz$quantile))), ]
    zz <- zz[,c("quantile", month.abb)]
     
    zz
  }

  res <- vector("list", length=length(months.back))
  names(res) <- paste("last", months.back, sep="")
  for (i in seq_along(res)) {
    if (nrow(RR) < months.back[i])
      next
    res[[i]] <- .get_quantiles(tail(RR, months.back[i]))
  }
    
  res
}


################################################################
# Update the NCPC spreadsheet with the RMR info 
#
.update_xlsx_rmr <- function( months )
{
  res <- do.call(rbind,
    lapply(months, .monthly_actuals_rmr))

  aux <- as.data.frame(res[,c(
    "me.rt.lscp.cost", "ct.rt.lscp.cost", "ri.rt.lscp.cost",
    "sema.rt.lscp.cost", "wma.rt.lscp.cost", "nema.rt.lscp.cost",
    "nh.rt.lscp.cost",                      

    "me.da.lscp.cost", "ct.da.lscp.cost", "ri.da.lscp.cost",
    "sema.da.lscp.cost", "wma.da.lscp.cost", "nema.da.lscp.cost",
    "nh.da.lscp.cost",
                              
    "me.rt.rtlo", "ct.rt.rtlo", "ri.rt.rtlo",
    "sema.rt.rtlo", "wma.rt.rtlo", "nema.rt.rtlo",
    "nh.rt.rtlo"
  )])
  #colnames(aux) <- 1:ncol(aux)

  rLog("\n\n============================================================")
  print(aux, row.names=FALSE)
  rLog("Paste this on  Monthly Ancillary Updates/Actuals/NCPC Tracker.xls")
  rLog("sheet Summary, columns O:AI")

  aux
}



################################################################
################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.rmr.R")

  ####################################################
  # to update the NCPC tracker see .update_spreadsheets from lib.actuals.ancillaries.R

  
  ####################################################
  # my old tracker
  asOfDate  <- Sys.Date()
  startDate <- nextMonth()
  endDate   <- as.Date("2021-12-01")
  ZZ <- data.frame(
    ptid = 4001:4008,
    delivPt = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WCMA", "NEMA"),
    zone = c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))


  #HH <- .pull_hist_rmr(start.dt=as.POSIXct("2010-01-01 01:00:00"), long=FALSE)
  HH <- .pull_hist_rmr2()   # new one
  FF <- .pull_marks_rmr(asOfDate, startDate, endDate)
  # subset(FF, month >= as.Date("2020-01-01"))
  # print(cast(FF, month ~ location, I), row.names=FALSE)[, c("month", "NEMA RT")]
  
  out <- .report(HH, FF)
  print(out, row.names=FALSE)
  # paste into S:/All/Structured Risk/NEPOOL/Ancillary Services/RMR/Track RMR.xlsx
  

  out[,c("month", "4005_DA", "4005_RT")] 
  out[,c("month", "4008_DA", "4008_RT")]

  cols <- c("month", "4008_DA", "4008_RT")
  res  <- out[,cols]; res$total <- round(res[,cols[2]] + res[,cols[3]],2)
  .plot_rmr(res, columns="total", label="NEMA")
  
  print(res[,c("month", "total")], row.names=FALSE)
  aggregate(res$total, list(year=format(res$month, "%Y")), mean)
  
    
  .plot_rmr(out, columns=c("4006_RT", "4006_DA"))
  .plot_rmr(out, columns=c("4005_RT", "4005_DA"))
  .plot_rmr(out, columns=c("4001_RT", "4001_DA"))
  .plot_rmr(out, columns=c("4008_RT", "4008_DA"))

  .plot_rmr_dollars(HH, zones=c("NEMA", "SEMA", "RI"), ZZ)
  .plot_rmr_dollars(HH, zones=c("SEMA"), ZZ)
  # bucketizeByMonth(nepl_tc_rt_ri_rmrncpc, "FLAT", "NEPOOL", "3600", "sum")

  # look at seasonality (zone)
  print(.check_seasonality(out, zone=4008), row.names=FALSE)


  
  ####################################################
  # calculate PnL change
  #
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  
  day1 <- as.Date("2014-08-06")
  day2 <- as.Date("2014-08-07")
  pnl <- calc_pnl_impact_rmr(day1, day2, endDate=as.Date("2019-12-01"))
  pnl$gPnL
  
  print(cast(pnl$gPnL,  year ~ company, sum, fill=0, value="PnL.dP"),
        row.names=FALSE)
   
  cast(pnl$gPnL, company ~ location, sum, fill=0, value="PnL")

  cast(pnl$gPnL, company + location ~ year,
       function(x){round(sum(x))}, fill=0, value="PnL",
       subset=location %in% c("RI RT"))

  dP <- ddply(subset(pnl$PnL, company="cne"), c("location", "year"),
       function(x){round(mean(x$P2-x$P1),2)})
  cast(dP, location ~ year, fill=0, value="V1",
       subset=location %in% c("RI RT"))

  subset(pnl$PnL, location == "RI RT")
  

  ####################################################
  # simulate RMR
  #
  ptid <- 4005
  aux <- .simulate_rmr(ptid, HH, months.back=24)
  print(aux$last24, row.names=FALSE)
  
  # look at seasonality (zone)
  print(.check_seasonality(subset(out, month >= as.Date("2011-01-01")),
                           zone=ptid), row.names=FALSE)

  
  print(out[,c("month", "4008_RT")], row.names=FALSE)



  ####################################################
  # Look at SEMA RMR in Apr-2014
  #
  X <- window(HH[, grepl("4006", colnames(HH))],
              start=as.POSIXct("2013-01-01 01:00:00"))
    
  

  ####################################################
  # Estimate RI RMR cost once the curve was added 8/7/2012
  # so no service was available
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")

  asOfDate <- Sys.Date()-1
  x <- secdb.readDeltas(asOfDate, "NEPOOL Load Portfolio")
  x <- secdb.readDeltas(asOfDate, "CNE NewEngland Load Portfolio")
  
  qq <- subset(.format_one_pfolio(x, mw=FALSE), location=="RI")
  names(qq)[1] <- "month"
  qq$location  <- "RI RT"
  qq <- cast(qq, month + location ~ ., sum)
  names(qq)[3] <- "value"
  
  PP2 <- .pull_marks_rmr(day2, startDate=day2, endDate)
  PP2$asOfDate <- NULL
  PP1 <- PP2; PP1$value <- 0  # no marks
  
  aux <- calc_pnl_pq(PP1, PP2, qq, qq,
    groups=c("year", "location"))  # from lib.pnl.R
  gPnL <- aux[["gPnL"]]
  print(gPnL, row.names=FALSE)
  sum(gPnL$PnL)
  
  # for CCG -$11,359
  # for CNE -$63,000


  # show historical RMR dollars
  aux <- subset(HH, type="Dollars")
  aux$month <- as.Date(format(aux$time-1, "%Y-%m-01"))
  aux <- cast(aux, month + ptid ~ ., sum, fill=0)
  colnames(aux)[3] <- "value"
  aux$ptid <- as.character(aux$ptid)

  xyplot(value/100000 ~ month|ptid, data=aux, type=c("g", "h"),
    scales="free", ylab="RT RMR $(00,000)", 
    subset=ptid %in% c("4001", "4008", "4004", "4006"))

  
  
  
}







