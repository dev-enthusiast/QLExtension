#
# Keep the historicals in the file:
#   S:/All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates/Ancillary Mark updates.xls
# Supporting files are in:
#   S:\All\Structured Risk\NEPOOL\Ancillary Services\Monthly Ancillary Updates\Actuals
#
#
# 2014_new_rtncpc_allocation - calculate the hist cost of the proposed allocation
# calc_pnl_impact_opres
# deviation_allocation       - to gen, load, other(virtuals)
# deviation_charges_load     - plot/analyze the deviation charges to cne/ccg
# get_positions_opres
# hist_fwd_view              - compare the actuals and the marks
# .monthly_actuals_opres     - calculate the actuals like in the spreadsheet
# .pull_hist_economic_NCPC   - read actual 1st contingency ncpc costs from tsdb
# .pull_hist_pool_ncpc_type  - get the hist pool NCPC costs by type
# .pull_marks_opres          - read opres marks from SecDb
# .update_xlsx_ncpc          - keep track of actuals  
# .update_xlsx_opres
#
# For the ixrt cne costs see lib.cne.loads.R
#





################################################################
# Calculate pnl impact
#
calc_pnl_impact_opres <- function( day1, day2,
   endDate=as.Date("2017-12-01") )
{
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_opres(day1, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_opres(day1, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- subset(qq1, month >= nextMonth())

  if (day2 == Sys.Date()) {
    rLog("using the same positions qq2=qq1 ... ")
    qq2 <- qq1
  } else {
    books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
    qqA   <- cbind(company="ccg", get_positions_opres(day2, books))
    books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
    qqB   <- cbind(company="cne", get_positions_opres(day2, books))
    qq2   <- rbind(qqA, qqB)
    qq2   <- subset(qq2, month >= nextMonth())
  }
  
  PP1 <- .pull_marks_opres(day1,  startDt=day1, endDt=endDate)
  PP1$asOfDate <- NULL
  
  PP2 <- .pull_marks_opres(day2, startDt=day2, endDt=endDate)
  PP2$asOfDate <- NULL

  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
}

################################################################
# Show the allocation of deviations by bucket (generation, load, other)
# for exgen and cne.
# @months a list of months
# @return data.frame with the cost allocated by month
#
deviation_allocation <- function( months )
{
  months <- months[order(months)]
  startDt <- as.Date(format(months[1], "%Y-%m-01"))
  endDt   <- nextMonth(months[length(months)]) - 1
  
  DD <- .pull_hist_economic_NCPC(start.dt=startDt, end.dt=endDt)

  # get the gen deviations too
  symbols <- c(
    #"NERpt_RTORS_IncImpDev",          "ccg.rt.incimpMWh",  # empty in tsdb
    "NERpt_RTORS_GenDev",             "ccg.rt.gendevMWh",  
    #"NCRpt_RTORS_IncImpDev",          "cne.rt.incimpMWh",  # empty in tsdb
    "NCRpt_RTORS_GenDev",             "cne.rt.gendevMWh")
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)
  HH <- data.frame(day=index(HH), HH)
  rownames(HH) <- NULL
  DD <- merge(DD, HH)
  DD$ccg.rt.incimpdevMWh <- DD$ccg.rt.devMWh - DD$ccg.rt.loaddevMWh - DD$ccg.rt.gendevMWh
  DD$cne.rt.incimpdevMWh <- DD$cne.rt.devMWh - DD$cne.rt.loaddevMWh - DD$cne.rt.gendevMWh

  
  RR <- DD[, c("day", "pool.rt.econ.ncpc", "pool.rt.devMWh",
    "pool.rtlo", "cne.rt.load", "ccg.rt.load",  
    "ccg.rt.devMWh", "ccg.rt.loaddevMWh", "ccg.rt.gendevMWh", "ccg.rt.incimpdevMWh",
    "cne.rt.devMWh", "cne.rt.loaddevMWh", "cne.rt.gendevMWh", "cne.rt.incimpdevMWh")]
  
  RR <- cbind(month=as.Date(format(DD$day, "%Y-%m-01")), RR)

  res <- ddply(RR, c("month"), function(x) {
    data.frame(
      exgen.loaddev.cost = sum(x$pool.rt.econ.ncpc * x$ccg.rt.loaddevMWh/x$pool.rt.devMWh),         
      exgen.gendev.cost = sum(x$pool.rt.econ.ncpc * x$ccg.rt.gendevMWh/x$pool.rt.devMWh),
      exgen.incimpdev.cost = sum(x$pool.rt.econ.ncpc * x$ccg.rt.incimpdevMWh/x$pool.rt.devMWh),
      cne.loaddev.cost = sum(x$pool.rt.econ.ncpc * x$cne.rt.loaddevMWh/x$pool.rt.devMWh),         
      cne.gendev.cost = sum(x$pool.rt.econ.ncpc * x$cne.rt.gendevMWh/x$pool.rt.devMWh),
      cne.incimpdev.cost = sum(x$pool.rt.econ.ncpc * x$cne.rt.incimpdevMWh/x$pool.rt.devMWh),
               
      # and the costs in $/MWh
      pool.dev.cost.unit = -sum(x$pool.rt.econ.ncpc)/sum(x$pool.rtlo),
      exgen.dev.cost.load.unit = -sum(x$pool.rt.econ.ncpc * x$ccg.rt.loaddevMWh/x$pool.rt.devMWh)/sum(x$ccg.rt.load),
      exgen.dev.cost.all.unit = -sum(x$pool.rt.econ.ncpc * x$ccg.rt.devMWh/x$pool.rt.devMWh)/sum(x$ccg.rt.load),         
      cne.dev.cost.load.unit = -sum(x$pool.rt.econ.ncpc * x$cne.rt.loaddevMWh/x$pool.rt.devMWh)/sum(x$cne.rt.load)   
    )
  })

  
  res
}


################################################################
# Look at deviation charges
#
deviation_charges_load <- function( )
{
  HH1 <- .pull_hist_economic_NCPC()  # daily data
  head(HH1)

  HH1$year <- format(HH1$day, "%Y")
  HH1$month <- as.Date(format(HH1$day, "%Y-%m-01"))
  HH1$our.devMWh <- HH1$ccg.rt.devMWh + HH1$cne.rt.devMWh

  xyplot(log(our.devMWh) ~ log(HH1$pool.rt.devMWh), data=HH1,
         groups=year)
  
  xyplot(log(our.devMWh) ~ log(HH1$pool.rt.devMWh)|year, data=HH1,
         )
  
  xyplot(our.devMWh/HH1$pool.rt.devMWh ~ day, data=HH1,
         type=c("g", "h"))

  # our ratio of the pool deviation
  ddply(HH1, c("year"), function(x) {
    summary(na.omit(x$our.devMWh/x$pool.rt.devMWh))}) 

  
  
  plot(log(HH1$pool.rt.devMWh), log(HH1$our.devMWh))
  
  MM1 <- ddply(HH1, c("month"), function(x){
    pool.rt.econ.ncpc = sum(x$pool.rt.econ.ncpc)   
  })
  

  #==============================================================
  #
  cast(HH1, . ~ year, sum, na.rm=NA, value="pool.rt.econ.ncpc")
  #  value     2008     2009     2010     2011     2012     2013
  #  (all) 33317973 27481002 73444849 50345018 48497481 55699565

  ddply(HH1, c("year"), function(x) {
    data.frame(pool.rt.econ.ncpc = sum(x$pool.rt.econ.ncpc))
  })

  
  # bad deviation days
  HH1$cne.dev.fraction <- -HH1$cne.rt.devMWh/HH1$cne.rt.load
  HH1$ccg.dev.fraction <- -HH1$ccg.rt.devMWh/HH1$ccg.rt.load

  # it is random in log space!
  plot(HH1$pool.rt.econ.ncpc, HH1$cne.dev.fraction,
    col="blue", log="xy",
    ylab="")

  

  
  plot(HH1$day, HH1$cne.load.error, col="blue", type="l")
  
  
  plot(HH1$pool.rtlo, HH1$cne.rt.devMWh, col="blue",
       log="y")

  
  hh <- melt(HH1, id=1)

  # cne deviation
  xyplot(value ~ date | variable,
    data=subset(hh, variable %in% c("cne.rt.devMWh",
      "pool.rt.devMWh", "pool.rt.econ.ncpc")),  
    subset=date >= as.Date("2012-11-01"),      
    type=c("g", "o"), scales="free", 
    xlab="",
    ylab="CNE daily rt deviation, MWh",
    layout=c(1,3))
  
  
  
}


################################################################
# get positions
#
get_positions_opres <- function( asOfDate, books )
{
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="OPRES")
  
  qq <- cast(aux, contract.dt + delivery.point ~ ., sum, fill=NA,
             value="notional")
  names(qq) <- c("month", "location", "value")

  data.frame(qq)
}


################################################################
# get positions for OpRes from the flat report
#
get_positions_opres_flatrpt <- function( asOfDate, books )
{
  rLog("NOT FINISHED")
  filter <- function(x){ x$`Service Type` == "OpRes" && x$`Delivery Point` == "DA"}
  res <- lapply(as.list(books), function(x){
    PM:::secdb.flatReport(asOfDate, x, variables = c("Quantity", "Today Value", "Active Contracts",
    "Delivery Point", "Service Type", "Settlement Type", "Load Estimate Base Name"),
                          filter=filter, to.df=TRUE)
  })
  aux <- do.call(rbind, res)
  aux <- subset(aux, month >= nextMonth())
  bux <- cast(aux, month ~ ., sum, value="Quantity",  
    subset=DeliveryPoint=="DA" & SettlementType=="Physical" & month >= nextMonth())
  sort(rownames(aux))

  
  

  
  unique(sapply(aux, function(x){x$`Load Estimate Base Name`}))
  
  
  
  res5 <- lapply(as.list(books), function(x){
    PM:::secdb.flatReport(asOfDate, x, filter=filter, to.df=TRUE)
  })
  df <- do.call(rbind, res5)
  df2 <- subset(df, DeliveryPoint=="DA" & SettlementType=="Physical" & month >= nextMonth())

  
  res2 <- cast(df, month ~ ., sum, value="Quantity",  
    subset=DeliveryPoint=="DA" & SettlementType=="Physical" & month >= nextMonth())


  res
}


################################################################
# Compare the actuals NCPC pool cost to the costs implied by the
# marks.  For OpRes RT it's harder because we embed the RT reserves
# costs into the marks.
#
hist_fwd_view <- function()
{
  require(xlsx)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  
  HH <- .pull_hist_economic_NCPC()
  HH$month <- as.Date(format(HH$day, "%Y-%m-01"))

  # monthly sums of historical cost
  MM <- ddply(HH, c("month"), function(x){
    data.frame(pool.rt.econ.ncpc = sum(x$pool.rt.econ.ncpc, na.rm=TRUE),
               pool.rt.devMWh    = sum(x$pool.rt.devMWh, na.rm=TRUE),
               pool.da.econ.ncpc = sum(x$pool.da.econ.ncpc, na.rm=TRUE))
  })
  
  # get the fwd marks
  FF <- .pull_marks_opres()

  # get hist/fwd load
  LL.hist <- get_historical_pool_load(agg.month=TRUE)
  LL.hist$source <- "hist"
  startDt <- as.POSIXct(paste(nextMonth(), "01:00:00"))
  endDt   <- as.POSIXct(paste(nextMonth(max(FF$month)),"00:00:00"))
  LL <- nepool.load.fcst(startDt, endDt, from="tlp", agg.month=TRUE)
  LL <- zoo(apply(LL, 1, sum), index(LL))
  LL.fwd <- data.frame(month=index(LL), as.data.frame(LL))
  colnames(LL.fwd)[2] <- "pool.load.MWh"
  rownames(LL.fwd) <- NULL
  LL.fwd$source <- "fcst"
  LL <- rbind(LL.hist, LL.fwd)
  head(LL)
  xyplot(pool.load.MWh/1000000 ~ month, data=LL, groups=LL$source,
         type=c("g", "o"))

  #----------------------------------------------------------------
  # look at the DA NCPC 
  aux <- subset(FF, serviceType=="OPRES" & location == "DA")
  aux <- aux[,c("month", "value", "source")]
  aux$type <- "mark"
  ncpc_da <- aux
  head(ncpc_da)

    # add historical cost DA NCPC marks
    aux <- merge(MM[, c("month", "pool.da.econ.ncpc")],
                 subset(LL, type="hist"))
    aux$value <- aux$pool.da.econ.ncpc/aux$pool.load.MWh
    aux$type <- "mark"
    ncpc_da <- rbind(ncpc_da, aux[,c("month", "value", "source", "type")])

    # add the new marks for DA NCPC
    fname <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/2014-02-17_winter recap.xlsx"
    aux <- read.xlsx2(fname, sheetName="OpRes DA", colIndex=16:17,
      startRow=4, colClasses=c("Date", "numeric"))
    colnames(aux) <- c("month", "value")
    aux <- aux[!is.na(aux$month),]
    aux$source <- "fwd.new"
    aux$type <- "mark"
    ncpc_da <- rbind(ncpc_da, aux)
  
  xyplot(value ~ month, data=ncpc_da, groups=ncpc_da$source,
         type=c("g", "o"),
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=3), 
         ylab="OpRes DA, $/MWh")

  #----------------------------------------------------------------
  # look at the RT NCPC
  cast(FF, month ~ location, I, subset=location %in% c("RT", "CTRT", "NEMART"))
  aux <- subset(FF, serviceType=="OPRES" & location == "RT")
  aux <- aux[,c("month", "value", "source")]
  aux$type <- "mark"
  ncpc_rt <- aux
  head(ncpc_rt)
  

    # add the historical costs from the spreadsheet until a better solution
    filename <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
      "Monthly Ancillary Updates/Actuals/NCPC Tracker.xls", sep="")
    aux <- read.xlsx2(filename, sheetName="Summary", colIndex=c(1, 11:14),
      startRow=2,
      endRow=10+round(as.numeric((nextMonth()-as.Date("2003-04-01"))/30.5)),
      header=TRUE, colClasses=c("Date", rep("numeric", 4)))
    colnames(aux)[2:5] <- c("RT", "DA", "CTRT", "NEMART")
    aux <- subset(aux, !is.na(month))
    aux$type <- "mark"
    aux$source <- "hist"
    colnames(aux)[2] <- "value"
    ncpc_rt <- rbind(ncpc_rt, aux[,c("month", "value", "source", "type")])
  
    # add the new marks for RT NCPC
    fname <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/2014-02-18_winter recap.xlsx"
    aux <- read.xlsx2(fname, sheetName="OpRes RT", colIndex=c(22,25),
      startRow=4, colClasses=c("Date", "numeric"))
    colnames(aux) <- c("month", "value")
    aux <- aux[is.finite(aux$value),]
    aux$source <- "fwd.new"
    aux$type <- "mark"
    ncpc_rt <- rbind(ncpc_rt, aux)
  
  xyplot(value ~ month, data=ncpc_rt, groups=ncpc_rt$source,
         type=c("g", "o"),
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=3), 
         ylab="OpRes RT, $/MWh")

  # seasonal             
  aux <- subset(ncpc_rt, source %in% c("fwd.new", "hist"))
  aux$mm <- as.numeric(format(aux$month, "%m"))
  aux$year <- as.numeric(format(aux$month, "%Y"))
  cast(aux, mm ~ year, I, value="value", fill=NA)
  
              
              
  
}


################################################################
# Calculate historical monthly economic ncpc costs, the same as
# in the spreadsheet
#
# @param a zoo object with monthly total economic ncpc $
#
.monthly_actuals_economic_ncpc <- function( month )
{
  startDt <- as.Date(format(month, "%Y-%m-01"))
  endDt   <- nextMonth(month) - 1
  rLog("  working on month", format(startDt))
  
  DD <- .pull_hist_economic_NCPC(start.dt=startDt, end.dt=endDt)
  
  MM <- aggregate(DD[,c("pool.da.econ.ncpc", "pool.rt.econ.ncpc", "pool.rtlo")],
    list(month=as.Date(format(DD$day, "%Y-%m-01"))), sum, na.rm=TRUE)
  
  zoo(MM[,-1], MM[,1])
}


################################################################
# Calculate historical opres costs, the same as in the
# spreadsheet.  You can compare this with the marks!
# @param an R Date beginning of the month
# @return a monthly zoo object with the opres rates
#
.monthly_actuals_opres <- function( month )
{
  # get economic NCPC costs
  EC <- .monthly_actuals_economic_ncpc(month)
  EC$pool.da.econ.ncpc.rate <- -EC$pool.da.econ.ncpc/EC$pool.rtlo
  EC$pool.rt.econ.ncpc.rate <- -EC$pool.rt.econ.ncpc/EC$pool.rtlo

  # get the RT reserve costs
  RR <- .monthly_actuals_rtreserves( month )

  OpRes <- merge(EC, RR[,c("rop.rtreserve.rate", "ct.rtreserve.rate",
                           "nema.rtreserve.rate")])
  OpRes$opres.rt.rop.rate  <- OpRes$pool.rt.econ.ncpc.rate + OpRes$rop.rtreserve.rate
  OpRes$opres.rt.ct.rate   <- OpRes$pool.rt.econ.ncpc.rate + OpRes$ct.rtreserve.rate
  OpRes$opres.rt.nema.rate <- OpRes$pool.rt.econ.ncpc.rate + OpRes$nema.rtreserve.rate
  OpRes$opres.da.rate      <- OpRes$pool.da.econ.ncpc.rate 

  # return just the rates, in $/MWh  -- allocated all to RTLO!
  OpRes <- OpRes[, c("opres.rt.rop.rate", "opres.rt.ct.rate",
                     "opres.rt.nema.rate", "opres.da.rate")]
  OpRes
}


################################################################
# git them from tsdb
#
.pull_hist_load <- function(ptids=NULL,
  start.dt=as.POSIXct("2008-01-01 01:00:00"), end.dt=Sys.time())
{
  rLog("DEPRECATED.  Use lib.NEPOOL.load.R, get_historical_zonal_load")
  get_historical_zonal_load(ptids=ptids, start.dt=start.dt, end.dt=end.dt)
}


#######################################################################
# Economic NCPC + RT reserve costs is called Service="OpRes" in SecDb 
#
.pull_hist_economic_NCPC <- function(
   start.dt=as.POSIXct("2008-01-01 01:00:00"), end.dt=Sys.time())
{
  # Total RT Economic Operating Reserves credits from SR RTOPRESSTL report
  # report is currently called: SR RTNCPCSTL !!!
  # Total pool RT deviation from SR RTOPRESSTL  
  # pool deviation charge $/MWh = symb1/symb2

  symbols <- c(
    "NERpt_RTORS_EconOpResCred",      "pool.rt.econ.ncpc",    # column 1 SR_RTNCPCSTL
    "NERpt_RTORS_PoolDev",            "pool.rt.devMWh",       # column 6 SR_RTNCPCSTL
    "NERpt_RTORS_EconOpResChg",       "ccg.rt.econ.ncpc",          
    "NERpt_RTORS_Dev",                "ccg.rt.devMWh",        # ccg rt deviation
    "NERpt_RTORS_LoadOblDev",         "ccg.rt.loaddevMWh",    # ccg rt load deviation
    "NERpt_DAORS_TotEconOpResCred",   "pool.da.econ.ncpc",    # from OI_ncpc_iso  
    "NERpt_DAORS_TotPoolLoadObl",     "pool.dalo",            # from SR
    "NERpt_DAORS_EconOpResChg",       "ccg.da.econ.ncpc",     # ccg econ ncpc charges
    "NERpt_DAORS_LoadObl",            "ccg.dalo",             # ccg load
    "NCRpt_RTORS_Dev",                "cne.rt.devMWh",        # cne rt deviation           
    "NCRpt_RTORS_LoadOblDev",         "cne.rt.loaddevMWh"     # cne rt load deviation
    )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], start.dt, end.dt)
  colnames(HH) <- symbols[,"shortNames"]
  head(HH)
  HH <- data.frame(day=index(HH), HH)
  rownames(HH) <- NULL
  
  # add the rtlo for pool, ccg, cne
  LL <- PM:::ntplot.bucketizeByDay(c("NERpt_RTCuS_PoolLoadObl", "NCRpt_RTCuS_LoadObl",
                                     "NERpt_RTCuS_LoadObl"),
    "FLAT", as.Date(start.dt), as.Date(end.dt), stat="sum")
  LL <- cast(LL, day ~ symbol, I, fill=NA)
  colnames(LL)[2:4] <- c("pool.rtlo", "cne.rt.load", "ccg.rt.load")
  HH <- merge(HH, LL, all=TRUE)

  # pool.dalo has a lot of NA's, so aggregate it from the hourly ...
  ## source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  ## pool <- get_historical_zonal_load(start.dt=start.dt, end.dt=end.dt)
  ## pool$day <- as.Date(format(pool$time - 1, "%Y-%m-%d"))
  ## pool <- cast(pool, day ~ dart, sum, na.rm=TRUE, value="value")
  ## colnames(pool)[1:2] <- c("day", "pool.dalo2")
  ## HH <- merge(HH, pool[,c("day", "pool.dalo2")], all.x=TRUE)
  ## HH$pool.dalo <- ifelse(is.na(HH$pool.dalo), -HH$pool.dalo2, HH$pool.dalo)
  
  
  # calculate the rates  
  # HH <- na.omit(HH)
  HH$`pool.da.econ.ncpc.rate` <- -HH$`pool.da.econ.ncpc`/HH$pool.dalo
  HH$`pool.rt.econ.ncpc.rate` <- HH$`pool.rt.econ.ncpc`/HH$pool.rt.devMWh

  
  HH
}



#######################################################################
# this is Economic NCPC, for some reason called OpRes in SecDb
#
.pull_marks_opres <- function(asOfDate=Sys.Date(),
   startDt=nextMonth(asOfDate),
   endDt=NULL,
   market=c("RT", "DA", "CTRT", "NEMART"),
   adjustOilRFP=FALSE)
{
  if (is.null(endDt))
    endDt <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+6, 
       "-12-01", sep=""))
  
  
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- c("ROP")
  bucket      <- c("FLAT")
  location    <- market
  serviceType <- c("OPRES")
  FF <- PM:::secdb.getElecPrices( asOfDate, startDt, endDt,
    commodity, location, bucket, serviceType, useFutDates)  
  FF$source   <- "fwd"
  FF$asOfDate <- NULL

  if (adjustOilRFP) {
    oil_adder <- expand.grid.df(
      data.frame(
        month=as.Date(c("2013-12-01", "2014-01-01", "2014-02-01")),
        adder=c(2.29, 2.19, 2.50)),
      data.frame(location=c("RT", "CTRT", "NEMART")))
    
    FF <- merge(FF, oil_adder, all.x=TRUE)
    FF$adder[is.na(FF$adder)] <- 0
    FF$value <- FF$value - FF$adder
    FF$adder <- NULL
  } 
  
  FF
}




#######################################################################
# the ncpc tracker spreadsheet
#
.update_xlsx_ncpc <- function(months)
{
  res <- do.call(rbind,
    lapply(months, .monthly_actuals_economic_ncpc))

  rLog("\n\n============================================================")
  print(res[, c("pool.da.econ.ncpc", "pool.rt.econ.ncpc", "pool.rtlo")])
  rLog("Paste this on Monthly Ancillary Updates/Actuals/NCPC Tracker.xls")
  rLog("sheet Summary, columns A:C")
  
  res
}


#######################################################################
# the opres tracker spreadsheet
# this calculation corresponds to columns K:N in spreadsheet NCPC Tracker.xlsx
# I don't need to paste it in the spreadsheet as it gets calculated ...
#
.update_xlsx_opres <- function(months)
{
  res <- do.call(rbind,
    lapply(months, .monthly_actuals_opres))

  res
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
  require(lattice)
  require(xlsx)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")

  # to calculate OpRes actuals, see lib.actuals.ancillaries.R

  # to compare hist vs. fwd see hist_fwd_view()
  
  ###########################################################################
  asOfDate  <- Sys.Date()
  startDate <- as.Date(format(asOfDate, "%Y-%m-01"))
  endDate   <- as.Date("2019-12-01")
  ZZ <<- data.frame(
    ptid = 4001:4008,
    delivPt = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WCMA", "NEMA"),
    zone = c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))

  # compare the OpRes marks with history
  hist_fwd_view()  

  # get the OpRes marks
  FF <- .pull_marks_opres()
  cast(FF, month ~ location, I, fill=NA, value="value",
       subset=location %in% c("RT", "CTRT", "NEMART"))
  
  
  ########################################################################
  # look at Economic NCPC allocations between different buckets
  # (gen,load,virtuals)
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  months <- seq(as.Date("2013-01-01"), as.Date("2014-01-01"), by="1 month")
  AA <- deviation_allocation( months )


  
  ########################################################################
  # look at Economic NCPC 
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  HH <- .pull_hist_economic_NCPC()
  print(subset(HH, day >= as.Date("2014-01-01")), row.names=FALSE)
  
  HH1 <- zoo(HH[,-1], HH[,1])
  
  mths <-  as.Date(format(as.Date(index(HH1)), "%Y-%m-01"))
  MM1 <- aggregate(HH1, mths, mean, na.rm=TRUE)       
  MM <- cbind(date=index(MM1), as.data.frame(MM1))
  rownames(MM) <- NULL
  MM$month <- format(MM$date, "%Y-%m-01")
  MM$year  <- format(MM$date, "%Y")
  MM$mmm   <- factor(format(MM$date, "%b"), levels=month.abb)
  
  windows()
  xyplot(pool.rt.econ.ncpc/1000000 ~ month, data=MM,
         type=c("g", "o"),
         ylab="Pool RT Economic NCPC, M$")
  windows()
  xyplot(pool.da.econ.ncpc/1000000 ~ month, data=MM,
         type=c("g", "o"),
         ylab="Pool DA Economic NCPC, M$")
  
  # monthly rt economic ncpc rate
  xyplot(pool.rt.econ.ncpc.rate ~ date, data=MM,
    type=c("g", "o"), xlab="", ylab="Economic NCPC, $/MWh Deviation")

  # monthly cne deviation
  xyplot(cne.rt.devMWh ~ date, data=MM,
    type=c("g", "o"), xlab="", ylab="CNE daily rt deviation, MWh")

  print(cast(MM, mmm ~ year, mean, fill=NA,
    value="pool.rt.econ.ncpc.rate"), row.names=FALSE)

  # agggregate by year
  HH1$year <-  as.numeric(format(index(HH1), "%Y"))
  ddply(as.data.frame(HH1), c("year"), function(x) {
    data.frame(pool.rt.econ.ncpc=sum(x$pool.rt.econ.ncpc), pool.rtlo=sum(x$pool.rtlo))
  })
  

  
  ########################################################################
  # calculate PnL impact
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")

  day1 <- as.Date("2014-06-02")
  day2 <- as.Date("2014-06-03")  
  pnl <- calc_pnl_impact_opres(day1, day2, endDate=as.Date("2019-12-01"))
  sum(pnl$gPnL$PnL.dP)
  
  print(cast(pnl$gPnL,  company ~ year, sum, fill=0, value="PnL.dP"),
        row.names=FALSE)

  print(cast(pnl$gPnL,  company + location ~ year, sum, fill=0, value="PnL.dP"),
      row.names=FALSE)

  print(pnl$gPnL, row.names=FALSE)
  
  # price change
  aux <- subset(pnl$PnL, location %in% c("DA", "RT"))[, c("year", "P1", "P2", "location")]
  ddply(aux, c("location", "year"), function(x){round(mean(x$P2-x$P1),2)})


  ########################################################################
  # Allocation to load for cne, ccg, pool
  #
  months <- seq(as.Date("2013-01-01"), by="1 month", length.out=14)
  AA <- deviation_allocation(months)
  aux <- melt(AA[,c("month", "pool.dev.cost.unit",
    "cne.dev.cost.load.unit", "exgen.dev.cost.load.unit")], id=1)
  aux$variable <- gsub("\\.dev\\.cost\\.unit", "", aux$variable)
  aux$variable <- gsub("\\.dev\\.cost\\.load\\.unit", "", aux$variable)
  xyplot(value ~ month, data=aux, groups=variable,
         type=c("g", "o"),
         ylab="Load deviation cost, $/MWh",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=3))
  
   

  

  
  ########################################################################
  # get OpRes positions
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  day1 <- as.Date("2013-11-29")
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_opres(day1, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_opres(day1, books))
  qq1   <- rbind(qqA, qqB)

  winterMths <- c(as.Date("2013-12-01"), as.Date("2014-01-01"), as.Date("2014-02-01"))
  qq1   <- subset(qq1, month %in% winterMths & location != "DA")

  cost <- data.frame(month=winterMths, adder=c(2.29, 2.19, 2.50))
  qq2  <- merge(qq1, cost)
  print(qq2, row.names=FALSE)
  
  

  
  ########################################################################
  # 2013-08 Allocate the new unit testing costs to the opres
  #
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  PP <- .pull_marks_opres()
  PPP <- cast(PP, month ~ location, I, fill=NA)
  print(PPP[,c("month", "DA", "CTRT", "NEMART", "RT")], row.names=FALSE)
  print(PPP[,c("month", "CTRT", "NEMART", "RT")], row.names=FALSE)

  LL <- nepool.load.fcst(startDt=as.POSIXct("2012-10-01 01:00:00"),
    endDt=as.POSIXct("2018-01-01 00:00:00"), from="2010")
  aux <- data.frame(dt=index(LL),
    month=format(index(LL)-1, "%Y-%m-01"),
    load=as.numeric(apply(LL, 1, sum)))     
  
  EE <- data.frame(cast(aux, month ~ ., sum, na.rm=TRUE, value="load"))
  names(EE)[2] <- "energy"
  EE$month <- as.Date(EE$month)

  
  

  


  
}




















  ## ########################################################################
  ## # look at OpRes, this is the pool costs 
  ## source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  ## HH <- .pull_hist_opres()

  ## MM <- aggregate(HH, as.Date(format(index(HH), "%Y-%m-01")), sum, na.rm=TRUE)  
  ## MM <- cbind(date=index(MM), as.data.frame(MM))
  ## rownames(MM) <- NULL
  ## xyplot(pool.rt.econ.ncpc/1000000 ~ date, data=MM,
  ##   type=c("g", "o"), xlab="",
  ##   ylab="Pool Economic NCPC costs, M$")
  
  ## print(MM, row.names=FALSE)
  












#######################################################################
# Pull the $ costs for the pool by NCPC type 
#  NOT FINISHED !!!
# the weekly report has good data on the split!
# 
## .pull_hist_pool_ncpc_type <- function(
##    start.dt=as.POSIXct("2013-01-01 01:00:00"), end.dt=Sys.time())
## {
##   # Total RT Economic Operating Reserves credits from SR RTOPRESSTL report
##   # report is currently called: SR RTNCPCSTL !!!
##   # Total pool RT deviation from SR RTOPRESSTL  
##   # pool deviation charge $/MWh = symb1/symb2

##   symbols <- c(
##     "NERpt_RTORS_EconOpResCred",      "pool.rt.econ.ncpc",    # column 1 SR_RTNCPCSTL
##     "NERpt_DAORS_TotEconOpResCred",   "pool.da.econ.ncpc",    # from OI_ncpc_iso  
##     "NERpt_DAORS_TotPoolLoadObl",     "pool.dalo",
##     )
##   symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
##     c("tsdb", "shortNames")))

##   HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], start.dt, end.dt)
##   colnames(HH) <- symbols[,"shortNames"]
##   head(HH)
##   HH <- data.frame(day=index(HH), HH)
##   rownames(HH) <- NULL
## }



  ## FF <- .pull_marks_opres()

  ## # pull the historical costs from the spreadsheet until a better solution
  ## filename <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
  ##   "Monthly Ancillary Updates/Actuals/NCPC Tracker.xls", sep="")
  ## HH <- read.xlsx2(filename, sheetName="Summary", colIndex=c(1, 11:14),
  ##   startRow=2,
  ##   endRow=10+round(as.numeric((nextMonth()-as.Date("2003-04-01"))/30.5)),
  ##   header=TRUE, colClasses=c("Date", rep("numeric", 4)))
  ## colnames(HH)[2:5] <- c("RT", "DA", "CTRT", "NEMART")
  ## HH <- melt(HH, id=1)
  ## colnames(HH)[2] <- "location"
  ## HH$serviceType <- "OPRES"
  ## HH$source <- "hist"
  ## HH$bucket <- "FLAT"
  ## HH <- subset(HH, !is.na(month))

  ## DD <- rbind(FF, HH)
  ## xyplot(value ~ month | location, data=DD,
  ##   group=source, type=c("g", "o"),
  ##   ylab="OpRes costs, $/MWh",
  ##   layout=c(1,2),      
  ##   subset=location %in% c("DA", "RT"),
  ##   scales="free")

  ## windows()
  ## xyplot(value ~ month | location, data=DD,
  ##   group=source, type=c("g", "o"),
  ##   ylab="OpRes costs, $/MWh",
  ##   layout=c(1,2),      
  ##   subset=location %in% c("CTRT", "NEMART"),
  ##   scales="free")
   
  
  ## aux <- DD #FF
  ## aux$mon  <- format(aux$month, "%m")
  ## aux$year <- format(aux$month, "%Y")
  ## print(cast(FF, month ~ location, I, fill=NA), row.names=FALSE)
  ## print(tail(cast(HH, month ~ location, I, fill=NA)), row.names=FALSE)
  
  ## # seasonal table, by location
  ## print(cast(aux, mon ~ year, function(x){round(x, 2)}, fill=NA,
  ##   subset=location=="RT"), row.names=FALSE)
  
  ## print(cast(aux, mon ~ year, function(x){round(x, 2)}, fill=NA,
  ##   subset=location=="CTRT"), row.names=FALSE)

  ## print(cast(aux, mon ~ year, function(x){round(x, 2)}, fill=NA,
  ##   subset=location=="NEMART"), row.names=FALSE)
  


## ################################################################
## # New proposal for RT Economic NCPC cost allocation.
## # Positive load deviations allocated at the NCPC RT Load rate (lower)
## # Rest of deviations allocated at the NCPC Deviation Rate.
## #
## #
## .new_rtncpc_allocation_2014 <- function()
## {
##   require(CEGbase)
##   require(SecDb)
##   require(zoo)
##   require(reshape)
##   require(lattice)
##   require(xlsx)
  
##   source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
##   source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
##   source("H:/user/R/RMG/Energy/Trading/Congestion/lib.demandbids.R")
##   source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")

  
##   source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
##   DD <- .pull_hist_economic_NCPC()

##   DD <- DD[,c("day", "pool.rt.econ.ncpc", "pool.rt.devMWh", "ccg.rt.econ.ncpc",
##               "ccg.rt.devMWh", "cne.rt.devMWh", "pool.rtlo")]

##   DD$ncpc.deviation.rate <- DD$pool.rt.econ.ncpc/DD$pool.rt.devMWh 
##   DD <- subset(DD, day >= as.Date("2012-01-01") & day <= as.Date("2013-12-31"))
##   head(DD)
  
  
##   startDt <- as.POSIXct("2012-01-01 01:00:00")
##   endDt   <- as.POSIXct("2014-01-01 00:00:00")

##   cne.positive.devMWh <- get_load_positive_deviation(startDt, endDt, company="CNE")
##   head(cne.positive.devMWh)
##   cne.positive.devMWh <- data.frame(day=index(cne.positive.devMWh),
##                                     cne.positive.devMWh=as.numeric(cne.positive.devMWh))
  
##   ccg.positive.devMWh <- get_load_positive_deviation(startDt, endDt, company="EXGEN")
##   head(ccg.positive.devMWh)
##   ccg.positive.devMWh <- data.frame(day=index(ccg.positive.devMWh),
##                                     ccg.positive.devMWh=as.numeric(ccg.positive.devMWh))

##   DD <- merge(DD, cne.positive.devMWh, by="day")
##   DD <- merge(DD, ccg.positive.devMWh, by="day")

##   # load the rates from the ISO
##   filename <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/Monthly Ancillary Updates/OpRes/2014-04-17_rt_ncpc_allocation_change.xlsx"
##   RR <- read.xlsx2(filename, sheetIndex=2, endRow=732, header=TRUE,
##     colIndex=1:3, colClasses=c("Date", "numeric", "numeric"))
##   colnames(RR)[1] <- "day"

##   res <- merge(DD, RR)
##   head(res)
##   res$year <- format(res$day, "%Y")

##   res$ccg.cost.old <- res$ccg.rt.devMWh * res$RT.First.Contingency.NCPC.Deviation.Charge.Rate
##   res$cne.cost.old <- res$cne.rt.devMWh * res$RT.First.Contingency.NCPC.Deviation.Charge.Rate

##   res$ccg.cost.new <- res$ccg.positive.devMWh * res$RT.Frist.Contingency.NCPC.RTLO.Charge.Rate + 
##     (res$ccg.rt.devMWh - res$ccg.positive.devMWh)* res$RT.First.Contingency.NCPC.Deviation.Charge.Rate
##   res$cne.cost.new <- res$cne.positive.devMWh * res$RT.Frist.Contingency.NCPC.RTLO.Charge.Rate + 
##     (res$cne.rt.devMWh - res$cne.positive.devMWh)* res$RT.First.Contingency.NCPC.Deviation.Charge.Rate

  
  
##   out <- ddply(res, "year", function(x) {
##      data.frame(exgen.new = sum(x$ccg.cost.new),
##                cne.new    = sum(x$cne.cost.new),
##                exgen.old  = sum(x$ccg.cost.old),
##                cne.old    = sum(x$cne.cost.old))
##   })

##   # not a big change see the spreadsheet: Monthl Ancillary Updates/OpRes/2014-04-17_rt_ncpc_allocation_change.xlsx
  
##   cast(res, year ~ ., sum, value="pool.rt.econ.ncpc")
## }
