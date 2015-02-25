# Functions to deal with CNE loads
# All CNE reports are in S:\Data\NEPOOL_CNE\RawData\OldZips\
#
# check_temperature_dependence   # how does the cne load depends on temperature
# cne_load_forecast_accuracy
# .get_booked_loads_cne            -- what we have booked
# get_ixrt_deviation_costs         -- 
# get_cne_load_positions_secdb     
# get_cne_shortterm_zonal_load     -- when spreadsheet for nmarket has issues
# get_fwd_cne_loads_nepool         -- RACM CNE load forecast by zone
# get_fwd_zonal_loads_racm         -- RACM load forecast by zone 
# get_hist_cne_ARRAWDSUM           -- from cne settlements in RKD
# get_hist_cne_loads_RTLOCSUM      -- get the cne hist loads from RKD
# get_hist_cne_loads_tsdb          -- get the cne hist loads from tsdb
# get_hist_cne_PEAKCONTRIBUTIONDLY -- has the cap tags 
# .get_cne_settlement_load_tsdb_symbols -- read the CNE tsdb load symbols
#
#
# CNE RT Load obligation tsdb symbols: NCRpt_RTLoS_4001_LoadObl
#


##########################################################################
# To compare day on day booked load in SecDb in case there are big changes
# 
# See also Ancillaries/lib.ncpc.R function deviation_charges_load
#
check_temperature_dependence <- function(startDt=Sys.Date()-30, 
  endDt=Sys.Date() )
{
  startDt <- as.POSIXct(paste(format(startDt), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(endDt), "00:00:00")) 
  
  # all actual CNE load 
  LL.actual <- get_hist_cne_loads_RTLOCSUM( startDt, endDt, as.matrix=FALSE)
  head(LL.actual)
  actual <- zoo(-apply(LL.actual, 1, sum), index(LL.actual))


  LL.fcst <- get_cne_load_forecast(startDt, endDt, type="lt", 
    aggregate=TRUE)
  fcst <- zoo(apply(LL.fcst, 1, sum), index(LL.fcst))
  head(fcst)

  HH <- merge(actual, fcst)
  HH <- cbind(datetime=index(HH), as.data.frame(HH))
  rownames(HH) <- NULL
  head(HH)
  HH$day <- as.Date(format(HH$datetime-1, "%Y-%m-%d"))
  
  # get the temperature
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.temperature.R")
  startDt <- as.Date("2000-01-01")
  endDt   <- Sys.Date()                      # for hist data 
  TT <- .get_hist_temperature_and_departures(startDt, endDt,
    location="bos")

  HH <- merge(HH, TT, all.x=TRUE)
  head(HH)

  PK <- ddply(HH, "day", function(x){
    x[which.max(x$actual),]
  })
  
  
  
  plot(PK$bos.max, PK$actual, col="blue")
  points(PK$bos.max, PK$fcst, col="red")

  
  
  
  

  LL
  
}


##########################################################################
# To compare day on day booked load in SecDb in case there are big changes
# 
# See also Ancillaries/lib.ncpc.R function deviation_charges_load
#
cne_load_forecast_accuracy <- function(start.dt=as.Date("2009-01-01"), 
  end.dt=Sys.Date() )
{
  symbols <- c(
    "NCRpt_RTORS_Dev",                "cne.rt.devMWh"         # cne rt deviation           
    )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  DD <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], start.dt, end.dt)
  colnames(DD) <- symbols[,"shortNames"]
  DD <- cbind(day=index(DD), as.data.frame(DD))
  

  LL <- PM:::ntplot.bucketizeByDay("NCRpt_RTCuS_LoadObl", "FLAT", start.dt, end.dt,
    stat="sum")
  colnames(LL)[4] <- "cne.rt.load"
  
  DD <- merge(DD, LL[,c("day", "cne.daily.load")])
  DD$load.fcst.error <- -DD$cne.rt.devMWh/DD$cne.daily.load

  MM <- aggregate(DD$load.fcst.error, list(month=as.Date(format(DD$day, "%Y-%m-01"))), mean)
  
  xyplot(x ~ month, data=MM, type=c("g","o"), ylab="CNE load deviation/CNE load")


  
}


###########################################################################
# Calculate the true historical cost of IXRT for CNE customers 
# and make a report
#
#
get_ixrt_deviation_premium <- function()
{
  require(SecDb)
  require(zoo)
  require(reshape)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.temperature.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  DD <- .pull_hist_economic_NCPC(start.dt=as.POSIXct("2013-01-01 01:00:00"))
  DD <- DD[,!grepl("^ccg", colnames(DD))]
  DD <- DD[which(!is.na(DD[,2])),]
  DD$cne.rt.econ.ncpc <- DD$pool.rt.econ.ncpc * DD$cne.rt.devMWh/DD$pool.rt.devMWh

  # get the cne load forecast 
  startDt <- as.POSIXct(paste(format(DD$day[1]), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(DD$day[nrow(DD)]), "00:00:00"))
  fcst <- get_cne_load_forecast(startDt, endDt,
    market=c("FP", "FP_MTM", "IXDA", "IXRT"), type="st",
    aggregate=FALSE, aggregateZone=TRUE)
  head(fcst)  
  fcstL <- aggregate(fcst,
    list(as.Date(format(index(fcst)-1, "%Y-%m-%d"))), sum)
  #fcstL$fcst.load.total <- apply(fcstL, 1, sum)
  #fcstL$ratio.ixrt.load <- fcstL$ixrt/fcstL$fcst.load.total 
  fcstL <- cbind(day=index(fcstL), as.data.frame(fcstL))

  DD2 <- merge(DD, fcstL, by="day")

  # aggregate by month ...
  MM <- aggregate(DD2[,-1],
    list(as.Date(format(DD2$day, "%Y-%m-01"))), sum, na.rm=TRUE)
  MM$pool.da.econ.ncpc.rate <- MM$pool.rt.econ.ncpc.rate <- NULL
  colnames(MM)[1] <- "month" 

  # how does CNE deviation compare with the pool deviation 
  MM$cne.deviation.ratio <- MM$cne.rt.devMWh/MM$pool.rt.devMWh
  MM$cne.load.ratio <- MM$cne.rt.load/MM$pool.rtlo
  

  # paste MM in the spreadsheet Congetion/NEPOOL/CNE/Presentations/2014_NCPC costs/



  

  require(lattice)
  aux <- melt(MM[,c("month", "cne.deviation.ratio", "cne.load.ratio")], id=1)
  xyplot(value ~ month, data=aux, groups=variable,
         type=c("g", "o"),
         ylab="Ratio",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2))


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
  




  
  
  portfolio <- "CNE NewEngland Load Portfolio"
  books <- secdb.getBooksFromPortfolio(portfolio)
  books <- "CBOSMECO"

  asOfDate <- Sys.Date()-1
  filter <- function(x){ x$`Service Type` == "OpRes" }  
  y <- PM:::secdb.flatReport(asOfDate, books, filter=filter,
                             variables=NULL, to.df=FALSE)

  res <- subset(res, month >= nextMonth())
  res2 <- cast(res, month + DeliveryPoint ~ SettlementType, sum, value="Quantity")
  
  
  ## rt <- get_hist_cne_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
  ## rt <- data.frame(time=index(rt), as.matrix(rt))
  ## rtL <- melt(rt, id=1)
  ## colnames(rtL)[3] <- "actual.load"
  ## rtL$actual.load <- -rtL$actual.load  # make it +

  ## LL <- merge(fcstL, rtL, by=c("time", "variable"))
  ## LL.all <- aggregate(LL[,c("fcst.load", "actual.load")],
  ##   by=list(time=LL$time), sum)

  

  
  
  DD$cne.econ.ncpc.charge <- 0;

}



###########################################################################
# Use SecDb delta archives.  Return the load by zone, month, bucket
#
.get_booked_loads_cne <- function(asOfDate)
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")

  QQ1 <- .get_zonal_positions_SecDb(asOfDate, "CNE NewEngland Load Portfolio")
  QQ1 <- subset(QQ1, market == "PWX")
  head(QQ1)

  QQ1
}




##########################################################################
# To compare day on day booked load in SecDb in case there are big changes
#
get_cne_load_positions_secdb <- function(days, by="book|year")
{
  maxDate <- seq(as.Date(format(Sys.Date(), "%Y-12-01")), by="5 years", length.out=2)[2]
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"

  res <- NULL
  for (i in 1:length(days)){
    fname <- paste(dir, "positionsAAD_", days[i], ".RData", sep="")
    rLog("Loading ", fname)
    load(fname)

    aux <- subset(QQ, contract.dt <= maxDate &
                  contract.dt >= Sys.Date() &
                  portfolio %in% c("PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )
    aux$year <- format(aux$contract.dt, "%Y")
    if (by=="book|year") {
      aux <- cast(aux, trading.book + year ~ ., sum, fill=NA, value="delta")
      aux[,3] <- round(aux[,3])
      names(aux)[3] <- "delta"
    }
    res[[i]] <- cbind(day=days[i], aux)
  }
  res <- do.call(rbind, res)
  
  res
}



##########################################################################
#
# @param startDt, endDt are POSIXct times
# @param aggregate if you want to sum over the products
# @param type, short term or long term forecast
# @param aggregate if you want to aggregate by zone (sum over the products)
# @param aggregateZone if you want to aggregate by product (sum over the zones)
#
get_cne_load_forecast <- function( startDt, endDt,
  market=c("FP", "FP_MTM", "IXDA", "IXRT"), type="st",
  aggregate=TRUE, aggregateZone=FALSE)
{
  symbs <- matrix(
    c(
      "csg_lt_isone_maine_ixda",           "me_ixda",
      "csg_lt_isone_maine_ixrt",           "me_ixrt",
      "csg_lt_isone_maine_fp",             "me_fp",
      "csg_lt_isone_maine_fp_mtm",         "me_fpmtm",

      "csg_lt_isone_newham_ixda",          "nh_ixda",
      "csg_lt_isone_newham_ixrt",          "nh_ixrt",
      "csg_lt_isone_newham_fp",            "nh_fp",
      "csg_lt_isone_newham_fp_mtm",        "nh_fpmtm",
      
      "csg_lt_isone_connec_ixda",          "ct_ixda",
      "csg_lt_isone_connec_ixrt",          "ct_ixrt",
      "csg_lt_isone_connec_fp",            "ct_fp",
      "csg_lt_isone_connec_fp_mtm",        "ct_fpmtm",
      
      "csg_lt_isone_rhodei_ixda",          "ri_ixda",
      "csg_lt_isone_rhodei_ixrt",          "ri_ixrt",
      "csg_lt_isone_rhodei_fp",            "ri_fp",
      "csg_lt_isone_rhodei_fp_mtm",        "ri_fpmtm",

      "csg_lt_isone_semass_ixda",          "sema_ixda",
      "csg_lt_isone_semass_ixrt",          "sema_ixrt",
      "csg_lt_isone_semass_fp",            "sema_fp",
      "csg_lt_isone_semass_fp_mtm",        "sema_fpmtm",
       
      "csg_lt_isone_wcmass_ixda",          "wma_ixda",
      "csg_lt_isone_wcmass_ixrt",          "wma_ixrt",
      "csg_lt_isone_wcmass_fp",            "wma_fp",
      "csg_lt_isone_wcmass_fp_mtm",        "wma_fpmtm",

      "csg_lt_isone_nemass_ixda",          "nema_ixda",
      "csg_lt_isone_nemass_ixrt",          "nema_ixrt",
      "csg_lt_isone_nemass_fp",            "nema_fp",
      "csg_lt_isone_nemass_fp_mtm",        "nema_fpmtm"       
    ), ncol=2, byrow=TRUE)
  
  if (type == "st")
    symbs[,1] <- gsub("_lt_", "_st_", symbs[,1])

  # select the market you want
  ind <- which( gsub(".*_.*_.*_.*_(.*)?$", "\\1", symbs[,1]) %in% tolower(market) )
  symbs <- symbs[ind,]
  
  res <- FTR:::FTR.load.tsdb.symbols(symbs[,1], startDt, endDt)
  colnames(res) <- gsub("csg_.*_isone_(.*)", "\\1", colnames(res))
  head(res)

  if (aggregate) {
    uZones <- unique(gsub("(.*)_(.*)?", "\\1", colnames(res)))
    ZZ <- structure(c("me", "nh", "ct", "ri", "sema", "wma", "nema"),
      names=c("maine", "newham", "connec", "rhodei", "semass", "wcmass", "nemass"))

    # faster to sum them directly ...
    aux <- zoo(matrix(0, ncol=length(uZones), nrow=length(index(res))), index(res))
    colnames(aux) <- uZones
    for (zone in uZones) {
      #if (zone == "semass") browser()
      aux[,zone] <- as.numeric(apply(res[,grep(zone, colnames(res)), drop=FALSE], 1,
                                     sum, na.rm=TRUE))
    }
    
    res <- aux
  }

  if (aggregateZone) {
    #browser()
    aux <- zoo(matrix(0, ncol=4, nrow=nrow(res)), index(res))
    products <- c("fp", "fp_mtm", "ixda", "ixrt")
    colnames(aux) <- products
    for (product in products) {
      ind <- which(grepl(paste(product, "$", sep=""), colnames(res)))
      aux[,product] <- as.numeric(apply(res[,ind], 1, sum, na.rm=TRUE))
    }
    
    res <- aux
  }

  
  res
}




##########################################################################
# startDt, endDt are POSIXct times
# short term have _st_
# long term have  _lt_      has it all the way to 
#
#  _FP_MTM, _IXDA, _IXRT are all the categories.  Should write a better
#     function
# 
get_cne_shortterm_zonal_load <- function( startDt, endDt,
  scaleFactorMW=structure(rep(1, 8), names = c("ct", "me", "nema",
    "nh", "ri", "sema", "vt", "wma")), type="st" )
{
  symbs <- c("csg_lt_isone_connec_ixda",
             "csg_lt_isone_maine_ixda",
             "csg_lt_isone_nemass_ixda",
             "csg_lt_isone_newham_ixda",
             "csg_lt_isone_rhodei_ixda",
             "csg_lt_isone_semass_ixda",
             "csg_lt_isone_wcmass_ixda",
             
             "csg_lt_isone_connec_fp",
             "csg_lt_isone_maine_fp",
             "csg_lt_isone_nemass_fp",
             "csg_lt_isone_newham_fp",
             "csg_lt_isone_rhodei_fp",
             "csg_lt_isone_semass_fp",
             "csg_lt_isone_wcmass_fp",
             
             "csg_lt_isone_connec_fp_mtm",
             "csg_lt_isone_maine_fp_mtm",
             "csg_lt_isone_nemass_fp_mtm",
             "csg_lt_isone_newham_fp_mtm",
             "csg_lt_isone_rhodei_fp_mtm",
             "csg_lt_isone_semass_fp_mtm",
             "csg_lt_isone_wcmass_fp_mtm")

  if (type == "st")
    symbs <- gsub("_lt_", "_st_", symbs)
  
  res <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(res) <- gsub("csg_.*_isone_(.*)_(.*)?", "\\1", colnames(res))

  require(reshape)
  aux <- melt(as.matrix(res))
  aux$X2 <- factor(aux$X2, levels=c("connec", "maine", "nemass", "newham",
    "rhodei", "semass", "vt", "wcmass"))
  out <- cast(aux, X1 ~ X2, sum, fill=0, add.missing=TRUE)
  colnames(out) <- c("datetime", "ct", "me", "nema", "nh", "ri", "sema",
                     "vt", "wma")

  out <- data.frame(out)
  if (any(scaleFactorMW != 1)){
    scaleFactorMW <- expand.grid.df(data.frame(id=1:nrow(out)),
      as.data.frame(t(scaleFactorMW)))
    out[,2:ncol(out)] <- out[,2:ncol(out)] * scaleFactorMW[,-1]
  }
  
  out
}


###########################################################################
# The RACM forecast for the CNE loads, by product and zone
# got these symbols from Dinkar
#
get_fwd_cne_loads_nepool <- function(startTime, endTime, zone=c("CT", "ME",
  "NEMA", "SEMA", "RI", "NH", "WMA"), agg.zone=TRUE)
{
  stop("DEPRECATED!!!")
  DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/"
  symbs <- read.csv(paste(DIR, "cne_tsdb_symbols.csv", sep=""))
  symbs$zone <- NULL
  symbs <- subset(symbs, participant=="cne")
  symbs$longzone <- gsub("CSG_LT_ISONE_(.*)_.*", "\\1", symbs$symbol)

  ZZ <- data.frame(longzone=c("CONNEC", "MAINE", "NEMASS",
    "SEMASS", "RHODEI","NEWHAM", "WCMASS"), zone=c("CT", "ME",
    "NEMA", "SEMA", "RI", "NH", "WMA"))
  
  symbs <- merge(symbs, ZZ, all.x=TRUE)
  symbs <- symbs[which(symbs$zone %in% zone), ]
  symbs <- split(symbs$symbol, symbs$zone)
  
  LL <- vector("list", length=length(symbs))
  for (r in 1:length(LL)){
    rLog("Working on zone", names(symbs)[r]) 
    LL[[r]] <- FTR:::FTR.load.tsdb.symbols(symbs[[r]], startTime, endTime)
  }
  if (agg.zone){
    LL <- lapply(LL, function(x){zoo(apply(x, 1, sum, na.rm=TRUE), index(x))})
    names(LL) <- names(symbs)
  }

  LL <- do.call(cbind, LL)
  
  ind <- which(duplicated(rownames(LL)))
  if (length(ind)>0){
    rLog("Some duplicated entries!  Probably daylight savings... I remove them")
    LL <- LL[-ind,]
    LL <- zoo(LL, as.POSIXct(rownames(LL)))
  }
  
  LL 
}



###########################################################################
# The ARRAWDSUM report from cne settlements system. 
#
get_hist_cne_ARRAWDSUM <- function(startTime, endTime)
{
  require(RODBC)
  con    <- odbcDriverConnect("FileDSN=H:/user/R/RMG/Utilities/DSN/cnestl.dsn")
  tables <- sqlTables(con) 
  data   <- sqlFetch(con, "tbl_SD_ARRAWDSUM", max=4)

  query <- paste("select A.FlowDate, A.MarketName, A.ClassType, A.LocationID, ",
    "A.LocationName, A.PeakHourLoad, A.AdjustedPeakHourLoad, ",
    "A.LoadShareDollars, A.ZonalPeakHourLoad, A.ZonalAdjustedPeakHourLoad, ",
    "A.ZonalLoadShareDollars ", 
    "from tbl_SD_ARRAWDSUM A, tbl_Files B ",
    "where A.File_ID = B.File_ID and B.File_LatestVersion='Y' ",
    "and A.FlowDate >= '", format(startTime, "%Y-%m-%d"), "'",
    "and A.FlowDate <= '", format(endTime, "%Y-%m-%d"), "'", sep="")

  #query <- "select * from tbl_RT_LOCSUM where FlowDate >= '2010-01-01'"
  data <- sqlQuery(con, query)
  odbcCloseAll()

  return(data)
}


###########################################################################
# The historical load we served from RTLOCSUM report by ZONE
# @return a zoo object, with hourly data one column for each zone.
#
get_hist_cne_loads_RTLOCSUM <- function(startTime, endTime, as.matrix=TRUE)
{
  ZZ <- data.frame(ptid=4001:4008, zone=c("ME", "NH", "VT",
    "CT", "RI", "SEMA", "WMA", "NEMA"))

  ZZ$symbs <- paste("NCRpt_RTLoS_", ZZ$ptid, "_RevMeteredLd", sep="")
  
  LL <- FTR:::FTR.load.tsdb.symbols(ZZ$symbs, startTime, endTime)
  colnames(LL) <- tolower(structure(ZZ$zone, names=ZZ$symbs)[colnames(LL)])

  if (as.matrix) {
    rLog("why do you need as matrix?!")
    out <- matrix(0, nrow=nrow(LL), ncol=8, dimnames=list(format(index(LL)),
      c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma")))
    out[, colnames(LL)] <- LL
  } else {
    out <- LL
  }
  
  out
}


###########################################################################
# Load the RTLOCSUM report from the CNE settlements system
# apparently this data is in tsdb too.  see NEPOOL/NEPCNE Data/ 
#
get_hist_cne_loads_RTLOCSUM.db <- function(startTime, endTime, zone=c("CT", "ME",
  "NEMA", "SEMA", "RI", "NH", "WMA"), as.zoo=TRUE)
{
  require(RODBC)
  
  ZZ <- data.frame(locationName=c(".Z.MAINE", ".Z.NEWHAMPSHIRE", ".Z.VERMONT",
      ".Z.CONNECTICUT", ".Z.RHODEISLAND", ".Z.SEMASS", ".Z.WCMASS",
      ".Z.NEMASSBOST"),
    zone=c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
    ptid=4001:4008)
 
  con    <- odbcDriverConnect("FileDSN=H:/user/R/RMG/Utilities/DSN/cnestl.dsn")
  tables <- sqlTables(con) 
  data   <- sqlFetch(con, "tbl_RT_LOCSUM", max=4)

  query <- paste("select A.FlowDate, A.TradingInterval, A.LocationID, ",
    "A.LocationName, A.RT_LoadObligation, A.RT_AdjustedLoadObligation ",
    "from tbl_RT_LOCSUM A, tbl_Files B ",
    "where A.File_ID = B.File_ID and B.File_LatestVersion='Y' ",
    "and A.FlowDate >= '", format(startTime, "%Y-%m-%d"), "'",
    "and A.FlowDate <= '", format(endTime, "%Y-%m-%d"), "'", sep="")

  #query <- "select * from tbl_RT_LOCSUM where FlowDate >= '2010-01-01'"
  rLog("Reading historical cne load data from db ... ")
  data <- sqlQuery(con, query)
  rLog("Done.")
  odbcCloseAll()
  LL <- cast(data, FlowDate + TradingInterval ~ LocationName, I, fill=NA,
               value="RT_LoadObligation")

  if (as.zoo){  # return a zoo object?
    LL <- data.frame(LL[, c("FlowDate", "TradingInterval",
               names(LL)[names(LL) %in% ZZ$locationName])])
    suppressWarnings(timestamp <- as.POSIXct(paste(LL$FlowDate, " ",
      as.character(as.numeric(LL$TradingInterval)-1), ":59:59", sep=""),
      format="%Y-%m-%d %H:%M:%S"))
    ind <- which(is.na(timestamp))
    if (length(ind)>0){
      rLog("Removed daylight savings hours.")
      timestamp <- timestamp[-ind]
      LL <- LL[-ind,]
    }
  
    LL <- zoo(LL[,-c(1,2)], timestamp+1)  # back to Hour Ending
    aux <- merge(data.frame(locationName=colnames(LL)), ZZ, sort=FALSE)
    colnames(LL) <- aux$zone
    LL <- LL[, zone, drop=FALSE]

    LL <- -LL
  }
  
  return(LL)  
}


###########################################################################
# I suppose this are RT
#
get_hist_cne_loads_tsdb <- function(startTime, endTime, zone=c("CT", "ME",
  "NEMA", "SEMA", "RI", "NH", "WMA"), agg.zone=TRUE)
{
  aux <- .get_cne_settlement_load_tsdb_symbols(region="ISONE")
  aux <- aux[which(aux[,"zone"] %in% zone), ]
  symbs <- strsplit(aux[,"loadsymbol"], ",")
  
  LL <- vector("list", length=nrow(aux))
  for (r in 1:nrow(aux)){
    rLog("Working on zone", aux[r,"zone"]) 
    LL[[r]] <- FTR:::FTR.load.tsdb.symbols(symbs[[r]], startTime, endTime)
  }
  if (agg.zone)
    LL <- lapply(LL, function(x){x <- apply(x, 1, sum, na.rm=TRUE)})
  
  LL <- do.call(cbind, LL)
  
  ind <- which(duplicated(rownames(LL)))
  if (length(ind)>0){
    rLog("Some duplicated entries!  Probably daylight savings... I remove them")
    LL <- LL[-ind,]
    LL <- zoo(LL, as.POSIXct(rownames(LL)))
  }
  
  if (agg.zone)
    colnames(LL) <- aux[, "zone"]

  LL
}


###########################################################################
# from the share drive
#
get_hist_cne_PEAKCONTRIBUTIONDLY <- function(startDate=NULL,
  endDate=Sys.Date())
{
  root <- "S:/Data/NEPOOL_CNE/RawData/OldZips/Rpt_"
  if (is.null(startDate))
    startDate <- as.Date(format(endDate, "%Y-%m-01"))

  mths <- seq(startDate, endDate, by="1 month")
  res <- vector("list", length=length(mths))
  for (m in seq_len(length(mths))){
    rLog("Reading month:", format(mths[m]))
    mth <- format(mths[m], "%Y%m")
    file <- sort(list.files(paste(root, mth, sep=""),
      pattern="PEAKCONTRIBUTIONDLY", full.names=TRUE), decreasing=TRUE)[1]
    if (is.na(file)){
      rLog("No report found!  Next...")
      next
    }
    aux <- read.csv(file, skip=4)
    aux <- aux[-c(1, nrow(aux)),]
    aux <- aux[,-1]
    res[[m]] <- aux
  }
  res <- do.call(rbind, res)

  return(res)
}
  
###########################################################################
# Dinkar keeps this info in a matlab file!
# this rudimentary parser works only for this file!
# These are the cne settlement 
#
.get_cne_settlement_load_tsdb_symbols <- function(region="ISONE")
{
  MM  <- readLines("I:/ja/prod/matlab/applications/cne_vlr/cnelfpmap.m")
  MM <- gsub("\\.\\.\\.", "", MM)
  
  ind <- grep("^mapCell", MM)
  MM  <- MM[-(1:ind)]

  ind <- grepl("%", MM)
  if (any(ind))
    stop("You have comments!  Pay attention")
  
  # reduce multilines to one line
  LL <- ""; aux <- ""
  for (i in 1:length(MM)){
   aux <- paste(aux, MM[i])
   if (grepl("(.*);.*", MM[i])){   # one statetement per line
      LL[length(LL)]  <- paste(LL[length(LL)], aux)
      aux <- ""
      LL <- c(LL, "")
    } 
  }
  LL <- LL[grepl("ISONE", LL)]    # grab only NEPOOL now

  # extract the list elements, separated by "|"
  aux <- gsub(" *\\{(.*)\\}.*\\{(.*)\\}.*\\{(.*)\\}.*\\{(.*)\\}.*",
              "\\1|\\2|\\3|\\4", LL)
  aux <- strsplit(aux, "|", fixed=TRUE)

  if (any(sapply(aux, length) != 4))
    stop("Wrong number of columns!  Please check.")

  aux <- do.call(rbind, aux)
  symbols <- gsub(" *", "", aux[,3])
  symbols <- tolower(gsub("'", "", symbols))
  aux[,3] <- symbols
  aux <- aux[,-1]
  colnames(aux) <- c("longzone", "loadsymbol", "pricesymbol")
  aux[,1] <- gsub("'", "", aux[,1])
  aux[,1] <- gsub(" *", "", aux[,1])

  # check if loads are not properly separated ...
  symbs <- strsplit(aux[,"loadsymbol"], ",")
  symbs <- lapply(symbs,
    function(x){
      ind <- grep("^nepl_.*(nepl_).*", x)
      if (length(ind)==1){  # allow only one fuckup per line...
        y <- paste("nepl_", strsplit(x[ind], "nepl_")[[1]][-1], sep="")
        x <- x[-ind]
        x <- c(x, y)
      }
      x
    })
   symbs <- sapply(symbs, function(x){paste(x, sep="", collapse=",")})
   aux[,2] <- symbs
  
  ZZ <- data.frame(longzone=c("CONNECTICUT", "MAINE", "NEMASSBOST",
    "SEMASS", "RHODEISLAND","NEWHAMPSHIRE", "WCMASS"), zone=c("CT", "ME",
    "NEMA", "SEMA", "RI", "NH", "WMA"))

  aux <- merge(ZZ, aux)

  return(aux)
}


##########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  
  startDt <- as.POSIXct(paste(format(Sys.Date()+1), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()+2), "00:00:00"))

  scaleFactorMW <- structure(
            c(1,  1,       1,    1,    1,      1,    1,   1),
    names = c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma"))
  res <- get_cne_shortterm_zonal_load(startDt, endDt, scaleFactorMW)
  res
  
  #res <- get_cne_shortterm_zonal_load(startDt, endDt)
  res

  t(t(apply(res[,-1], 1, sum)))

  #=====================================================================
  days <- as.Date(c("2011-08-19", "2011-08-22",
                    "2011-08-23", "2011-08-24"))

  day <- as.Date("2012-02-22")
  res <- get_cne_load_positions_secdb(days, by="book|year")
  aux <- cast(res, trading.book + year ~ day, I, fill=NA, value="delta")
  

  #=====================================================================
  # get CNE hist loads  
  res <- get_hist_cne_loads_RTLOCSUM(as.POSIXct("2010-01-01 01:00:00"),
    as.POSIXct("2011-10-01 01:00:00"), zone="CT")

  
  
  #=====================================================================
  # compare CNE load forecast accuracy
  # see lib.loads.compare.actuals.R
  loadAssets <- .get_load_ids(company="CNE")

  
  #=====================================================================
  # get CNE hist loads
  #
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
 
  
  
}



























## ###########################################################################
## # The RACM forecast for the NEPOOL zonal loads.  -- ONLY FOR CAL 11 --
## # Got symbols by Long Han, done on 2/26/2010
## # 
## get_fwd_zonal_loads_racm <- function(startTime, endTime, zone=c("CT", "ME",
##   "NEMA", "SEMA", "RI", "NH", "WMA"))
## {
##   DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/"
##   symbs <- read.csv(paste(DIR, "cne_tsdb_symbols.csv", sep=""))
##   symbs$zone <- NULL
##   symbs <- subset(symbs, participant=="pool")
  
##   symbs$longzone <- gsub("NEPOOL_FCST_(.*)_DEMAND", "\\1", symbs$symbol)

##   ZZ <- data.frame(longzone=c("CT", "ME", "NEMA",
##     "SEMA", "RI","NH", "WCMA"), zone=c("CT", "ME",
##     "NEMA", "SEMA", "RI", "NH", "WMA"))
  
##   symbs <- merge(symbs, ZZ, all.x=TRUE)
##   symbs <- symbs[which(symbs$zone %in% zone), ]
##   symbs <- split(symbs$symbol, symbs$zone)

##   LL <- vector("list", length=length(symbs))
##   for (r in 1:length(LL)){
##     rLog("Working on zone", names(symbs)[r]) 
##     LL[[r]] <- FTR:::FTR.load.tsdb.symbols(symbs[[r]], startTime, endTime)
##   }
##   LL <- do.call(cbind, LL)
##   names(LL) <- names(symbs)

##   LL
## }
