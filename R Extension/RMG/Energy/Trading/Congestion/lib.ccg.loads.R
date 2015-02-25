# Historical tsdb symbols for a given load asset id:
#   NERpt_SD_LOAD_7705_ProdObl;Actuals
#   NEPOOL_Ld_Fcst_assetid_7705;Forecast
#   showload("MECOHistBK","28jun13","MECODSSEMARes DS")
#
# NEPOOL_Ld_Fcst_AssetID_42343 <-- for Mass-E Gov Agg DA forecasting
#
# Wethr_SSI_BOS_TEMP_Forc  - the temps used for the load forecast!
#
# For demand bids see lib.demandbids.R
# For comparison with actuals see: lib.loads.compare.actuals.R
#
# MX load data (what we get from them on a day ahead):
#  - mx_nepool_ld_fcst_ct , 
#
# Sensitivity to Temperature:
#   nep_fct_ctP2;  nepool load forecast for CT, plus 2F above normal
#   nepool_ld_fcst_ct;  baseline pool ccg load forecast
#   nep_fct_ctM2;  nepool load forecast for CT, minus 2F below normal
#
# .get_load_ids
# .get_booked_loads_ccg
# .get_loads_zone         # forecasted CCG + CNE loads from hqfcst
# .get_loads_zone_rtrhq
# check_temperature_dependence   # how does the cne load depends on temperature
# find_load_objects_SecDb  -- get a list of load objects in SecDb with positions
# find_load_objects_SecDb_xlxs -- 
# get_deviation_discount   -- see if ccg load can get a discount on RT NCPC
# get_hist_ccg_loads_RTLOCSUM   -- ccg historical load
# get_ccg_shortterm_zonal_load  -- when you have issues with nmarket!
# get_forecasted_load    
# get_mx_shortterm_zonal_load   -- pull the mx load forecast
# .hist_load_stats_ccg
# read_secdb_load_objects -- read the load objects one by one, and filter for dates
# 
#

secdb.evaluateSlang('link("_lib ntplot addin");')

###########################################################################
# Get current load IDs from the SD_RTLOAD report
# Needs:
#  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
#
# to find the zones this assets are in, you need to see
#   lib.NEPOOL.iso.R, function NEPOOL$customer_and_asset_info
#
#
.get_load_ids <- function(asOfDate=Sys.Date()-5, company="CCG")
{
  yyyymm   <- format(asOfDate, "%Y%m")
  yyyymmdd <- format(asOfDate, "%Y%m%d")

  file <- if (company == "CCG") {
    files <- list.files(paste("S:/Data/NEPOOL/RawData/OldZips/Rpt_", yyyymm,
      sep=""), full.names=TRUE, pattern="^SD_RTLOAD")
    grep(paste("SD_RTLOAD_000050027_", yyyymmdd, sep=""), files, value=TRUE)
    
  } else if (company == "CNE") {
    files <- list.files(paste("S:/Data/NEPOOL_CNE/RawData/OldZips/Rpt_", yyyymm,
      sep=""), full.names=TRUE, pattern="^SD_RTLOAD")
    grep(paste("SD_RTLOAD_000050017_", yyyymmdd, sep=""), files, value=TRUE)
    
  } else if (company == "EXGN") {
    files <- list.files(paste("S:/Data/NEPOOL_EXGN/RawData/OldZips/Rpt_", yyyymm,
      sep=""), full.names=TRUE, pattern="^SD_RTLOAD")
    grep(paste("SD_RTLOAD_000050428_", yyyymmdd, sep=""), files, value=TRUE)
  }
  

  res <- .process_report(file)[[1]]

  hr1 <- subset(res, Trading.interval == 1)
  hr1 <- hr1[, -(1:2)]

  # load the load asset map
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/loadAssets.RData")
  colnames(loadAssets)[1] <- "Asset.ID"
  loadAssets$Asset.ID <- as.integer(loadAssets$Asset.ID)
  aux <- merge(hr1, loadAssets[,c("Asset.ID", "Load.Zone.Id")], by=c("Asset.ID"),
               all.x=TRUE)
  
  # not sure why this is not in the table
  ind <- which(aux$Asset.ID == 40055 & is.na(aux$Load.Zone.Id))
  if (length(ind) > 0) aux$Load.Zone.Id[ind] <- 4001
  
  aux
}

###########################################################################
# Use SecDb delta archives.  Return the load by zone, month, bucket
#
.get_booked_loads_ccg <- function(asOfDate)
{
  QQ1 <- .get_zonal_positions_SecDb(asOfDate, "NEPOOL Load Portfolio")
  QQ1 <- subset(QQ1, market == "PWX")
  head(QQ1)

  QQ1
}


###########################################################################
# Forecasted loads.  Best try!  Used by the FTR presentation. 
#
.get_loads_zone <- function(startDt, endDt, zone)
{
  rLog("BAD FUNCTION NAME, should use fwd in the name")
  if (toupper(zone) == "NEMA") {
    symbols <- c(
      "hqfcst_nstards_nema",
      "hqfcst_nsmecod_nema",
      "hqfcst_nenepool_nema",       # CNE loads (not hourly!)
      "hqfcst_ibtattr_nema"         # attrition 
    )
  } else if (toupper(zone) == "WMA") {
    symbols <- c(
      "hqfcst_nsmecod_wma",
      "hqfcst_cpwmeco_wma",           
      "hqfcst_nenepool_wma",       # CNE loads (not hourly!)
      "hqfcst_ibtattr_wma"         # attrition 
    )
  } else if (toupper(zone) == "SEMA") {
    symbols <- c(
      "hqfcst_nstards_sema",
      "hqfcst_nsnantuc_sema",
      "hqfcst_nsmecod_sema",                 
      "hqfcst_nenepool_sema",       # CNE loads (not hourly!)
      "hqfcst_ibtattr_sema"         # attrition 
    )
  } else if (toupper(zone) == "CT") {
    symbols <- c(
      "hqfcst_nsuiload_ct",
      "hqfcst_nscthed1_ct",
      "hqfcst_npclpnps_ct",
      "hqfcst_nenepool_ct",       # CNE loads (not hourly!)
      "hqfcst_ibtattr_ct"         # attrition 
    )
  } else {
    stop("Unimplemented zone ", zone)
  }
  res <- FTR:::FTR.load.tsdb.symbols(symbols, startDt, endDt)
  res <- apply(res, 1, sum)
  res <- data.frame(zone=toupper(zone), time=as.POSIXct(names(res)), value=as.numeric(res))

  res
}


###########################################################################
# Forecasted loads from rtrhq.  Used by the FTR presentation. 
# 
#
.get_loads_zone_rtrhq <- function(startDt, endDt, zone)
{
  portfolios <- c("nepool load portfolio", "cne newengland load portfolio")
  filter <- paste("Region=NEPOOL;Zone=", toupper(zone), sep="")
  res <- ntplot.rtrhq(portfolios, filter=filter)

  subset(res, time >= startDt & endDt <= endDt)
}





###########################################################################
# Return a list of load objects in SecDb with positions
# Goes to PRISM to find the load names, etc. 
#
find_load_objects_SecDb_prism <- function( asOfDate=Sys.Date()-1,
  pfolio="NEPOOL Load Portfolio")
{
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  books <- PM:::secdb.getBooksFromPortfolio( pfolio )

  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="ENERGY") 

  lobj <- unique(aux[,c("counterparty", "eti", "load.name", "load.class",
    "trading.book", "volume", "delivery.point", "trade.start.date", "trade.end.date")])
  lobj <- lobj[order(lobj$load.name),]
  names(lobj)[names(lobj)=="volume"] <- "share"

  ## lobj$zone <- sapply(strsplit(lobj$delivery.point, " "), "[[", 1)
  ## lobj$delivery.point <- NULL
  ## lobj <- unique(lobj[,c("counterparty", "eti", "load.name", "load.class",
  ##    "trading.book", "share", "zone")])
  ## rownames(lobj) <- NULL
  
  lobj

}


###########################################################################
# Return a list of load objects in SecDb with positions
# Goes to PRISM to find the load names, etc. 
#
find_load_objects_SecDb_xlxs <- function( asOfDate=Sys.Date()-1)
{
  require(xlsx)

  filename <- "S:/All/Structured Risk/DR/Wholesale Load Summary/Wholesale Load Summary V9.xlsx"
  
}


###########################################################################
# Calculate if we can give a discount to ccg loads vs. the pool on RT NCPC
# See the sister function in lib.cne.load.R/get_ixrt_deviation_premium
# Last updated on 2/28/2014
#
get_deviation_discount <- function()
{
  require(SecDb)
  require(zoo)
  require(reshape)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.temperature.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R")
  DD <- .pull_hist_economic_NCPC(start.dt=as.POSIXct("2013-01-01 01:00:00"))
  DD <- DD[,!grepl("^cne", colnames(DD))]
  DD <- DD[which(!is.na(DD[,2])),]
  DD$ccg.rt.econ.ncpc <- DD$pool.rt.econ.ncpc * DD$ccg.rt.devMWh/DD$pool.rt.devMWh

  # aggregate by month ...
  MM <- aggregate(DD[,-1],
    list(as.Date(format(DD$day, "%Y-%m-01"))), sum, na.rm=TRUE)
  MM$pool.da.econ.ncpc.rate <- MM$pool.rt.econ.ncpc.rate <- NULL
  colnames(MM)[1] <- "month" 

  # how does CCG deviation compare with the pool deviation 
  MM$ccg.deviation.ratio <- MM$ccg.rt.loaddevMWh/MM$pool.rt.devMWh
  MM$ccg.load.ratio <- MM$ccg.rt.load/MM$pool.rtlo

  MM$ccg.load.accuracy <- -MM$ccg.rt.loaddevMWh/MM$ccg.rt.load
  MM$pool.deviation.ratio.toLoad <- -MM$pool.rt.devMWh/MM$pool.rtlo
  
  
  require(lattice)
  aux <- melt(MM[,c("month", "ccg.deviation.ratio", "ccg.load.ratio")], id=1)
  xyplot(value ~ month, data=aux, groups=variable,
         type=c("g", "o"),
         ylab="Ratio",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2))
  
  aux <- melt(MM[,c("month", "ccg.load.accuracy", "pool.deviation.ratio.toLoad")], id=1)
  xyplot(value ~ month, data=aux, groups=variable,
         type=c("g", "o"),
         ylab="Ratio",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2))
  


}




###########################################################################
# The historical load we served from RTLOCSUM report
# Why is this guy returning a matrix?!
#
# @param startTime a POSIXct
# @param endTime a POSIXct
# @return a matrix with one column for each zone
#
get_hist_ccg_loads_RTLOCSUM <- function(startTime, endTime, as.matrix=TRUE)
{
  ZZ <- data.frame(ptid=4001:4008, zone=c("ME", "NH", "VT",
    "CT", "RI", "SEMA", "WMA", "NEMA"))

  ZZ$symbs <- paste("NERpt_RTLoS_", ZZ$ptid, "_RevMeteredLd", sep="")
  
  LL <- FTR:::FTR.load.tsdb.symbols(ZZ$symbs, startTime, endTime)
  colnames(LL) <- tolower(structure(ZZ$zone, names=ZZ$symbs)[colnames(LL)])

  if (as.matrix) {
    rLog("why do you need as matrix?!")
    out <- matrix(0, nrow=nrow(LL), ncol=8, dimnames=list(format(index(LL)),
      c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma")))
    out[, colnames(LL)] <- LL
    out[which(is.na(out))] <- 0   # somehow still returns NA for vt
  } else {
    out <- LL
  }
  
  out
}


##########################################################################
# there's a 1 day short term forecast and a 10 day short term forecast
# startDt is a POSIXct
# endDt   is a POSIXct
#
get_ccg_shortterm_zonal_load <- function(startDt, endDt, oneDayForecast=TRUE,
  includeMxLoad=FALSE, asZoo=FALSE)
{
  symbs <- c("nepool_ld_fcstXX_ct",
             "nepool_ld_fcstXX_me",
             "nepool_ld_fcstXX_nema",
             "nepool_ld_fcstXX_nh",
             "nepool_ld_fcstXX_ri",
             "nepool_ld_fcstXX_sema",
             "nepool_ld_fcstXX_vt",
             "nepool_ld_fcstXX_wma")
  if (oneDayForecast){
    symbs <- gsub("XX", "_1d", symbs)
  } else {
    symbs <- gsub("XX", "", symbs)
  }
  
  res <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(res) <- gsub(".*_(.*)$", "\\1", colnames(res))
  
  if (!asZoo) {
    out <- matrix(0, nrow=nrow(res), ncol=8, dimnames=list(format(index(res)),
      c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma")))
    out[, colnames(res)] <- res
  } else { out <- res }
  
  if (includeMxLoad) {
    stop("no MX pull implemented yet")
  }
  
  out
}


##########################################################################
# Get the forecasted load by assetId
# startDt is a POSIXct
# endDt   is a POSIXct
# assetId a vector of numeric asset ids
#
#
get_forecasted_load <- function(startDt, endDt, assetId, shortTerm=FALSE)
{
  symbs <- if (shortTerm) {
    paste("nepool_ld_fcst_1d_assetid_", assetId, sep="")
  } else {
    paste("nepool_ld_fcst_assetid_", assetId, sep="")
  }

  res <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(res) <- gsub(".*_(.*)$", "\\1", colnames(res))

}


##########################################################################
# Read MX files
#   PROD: \\ceg\nmarketprod\New England\auditpath\load
#   TEST: \\ceg\nmarkettest\New England\auditpath\load\MX_DEMANDBID_20110801_NEISO.csv
#
# This is MX short term load forecast
#
# For the actual/settlement mx loads, see function .get_mx_rtload from /lib.mx.R
#
get_mx_demandbids <- function(startDt, endDt, from="CSV", format=c("long", "short"))
{
  ZZ <- data.frame(zone=c("me", "nh", "vt", "ct", "ri", "sema",  "wma", "nema"),
    ptid=4001:4008)
  if (from == "CSV") {
    DIR.archive <- "S:/All/nMarket/NESMD/MXEnergy/"
    DIR.audit <- "//ceg/nmarketprod/New England/auditpath/load/"

    sDate <- as.Date(format(startDt, "%Y-%m-%d"))
    eDate <- as.Date(format(endDt-1, "%Y-%m-%d"))
    days  <- seq(sDate, eDate, by="1 day")
    
    res <- vector("list", length=length(days))
    for (d in 1:length(days)) {
      fileName <- paste(DIR.archive, "MX_DEMANDBID_", format(days[d], "%Y%m%d"),
        "_NEISO.csv", sep="")
      if (!file.exists(fileName)) {
        fileName <- paste(DIR.audit, "MX_DEMANDBID_", format(days[d], "%Y%m%d"),
          "_NEISO.csv", sep="")
        if (!file.exists(fileName)) {
          next
        }      
      }
      aux <- read.csv(fileName, header=FALSE)
      names(aux) <- c("Date", "Hour.Ending", "ptid", "load")
      res[[d]] <- aux
    }
    res <- do.call(rbind, res)
  } else if (from == "TSDB") {
    stop("See function .get_mx_load_forecast in Congestion/lib.mx.R" )
  } else {
    stop("Unknown from argument!")
  }
  
  res <- merge(res, ZZ, by="ptid")
  res$Date <- format(as.Date(as.character(res$Date), format="%Y%m%d"))
  res$datetime <- PM:::isotimeToPOSIXct(res)$datetime

  if (format=="short") {
    #browser()
    res$zone <- factor(res$zone, levels=c("ct", "me", "nema", "nh", "ri",
                                   "sema", "vt", "wma"))
    res <- cast(res, datetime ~ zone, fill=0, add.missing=TRUE, value="load")
    res <- as.data.frame(res)
    ## rnames <- format(res$datetime)
    ## cnames <- colnames(res)[-1]
    ## res1 <- as.matrix(as.data.frame(res[,-1]))
    ## rownames(res1) <- rnames
  }
  res
}



###########################################################################
# lobj is a data.frame as returned by find_load_objects_SecDb
# 
read_secdb_load_objects <- function(lobj, asOfDate=Sys.Date(),
  startDt=Sys.time(), endDt=as.POSIXct("2012-01-01 01:00:00"), agg=FALSE)
{
  if (!all(c("load.name", "load.class", "share") %in% names(lobj)))
    stop("Incorect columns in lobj.")

  res <- vector("list", length=nrow(lobj))
  for (r in 1:nrow(lobj)){
    rLog("Working on", lobj$load.name[r], lobj$load.class[r])
    ld <- PM:::secdb.getLoadObject(lobj$load.name[r], lobj$load.class[r], asOfDate)
    ld$value <- lobj$share[r]*ld$value
    ld <- subset(ld, time >= startDt & time <= endDt)
    res[[r]] <- cbind(load.name=lobj$load.name[r],
                      load.class=lobj$load.class[r],
                      ld)
  }
  res <- do.call("rbind", res)

  if (agg) {
    rLog("Aggregate load")
    res <- cast(res, time ~ ., sum, fill=0)
    colnames(res)[2] <- "value"
  } 

  res
}



##########################################################################
#####################################
#####################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.mx.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

  startDt <- as.POSIXct(paste(format(Sys.Date()+1), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()+2), "00:00:00"))  # 2


  ## startDt <- as.POSIXct("2014-11-02 01:00:00")
  ## endDt   <- as.POSIXct("2014-11-03 00:00:00") 
  ## seq(startDt, endDt, by="1 hour")
  

  ########################################################################
  # get the ccg + mx load forecast
  res.ccg <- get_ccg_shortterm_zonal_load(startDt, endDt, oneDayForecast=TRUE)


  
  t(t(apply(res.ccg, 1, sum)))


  
  sum(res.ccg)



  ########################################################################
  # issues with SEMA load for 5/1/2014  Pick it from the long term. 
  zone <- "SEMA"
  portfolios <- c("nepool load portfolio")
  filter <- paste("Region=NEPOOL;Zone=", toupper(zone), sep="")
  res <- ntplot.rtrhq(portfolios, filter=filter)

  plot(as.POSIXlt(res$time), res$value, type="l")

  aux <- subset(res, time >= startDt & time <= endDt)
  print(aux[,c("time", "value")], row.names=FALSE)
  
  
  
  ########################################################################
  # get all the booked load object names
  # Example: NECO residential load PAS9CE4
  #
  {
    lobj <- find_load_objects_SecDb_prism(asOfDate=Sys.Date()-1, pfolio="NEPOOL Load Portfolio")
    lobj[order(lobj$trade.start.date),]
    
    lobj <- unique(lobj[,c("load.name", "load.class", "zone", "eti")])
    lobj <- subset(lobj, zone %in% c("SC", "CT", "WMA", "SEMA", "NEMA", "RI", "MAINE", "NH") &
                   load.name != "NULL")
    
    #lobj <- subset(lobj, zone == "CT")  # to pick only some zones
    LL <- read_secdb_load_objects(lobj, asOfDate=Sys.Date(),
      startDt=Sys.time(), endDt=as.POSIXct("2012-09-01 01:00:00"), agg=TRUE)


    # by zone
    zLO <- split(lobj, lobj$zone)
    zLL <- lapply(zLO, read_secdb_load_objects, asOfDate=Sys.Date(),
      startDt=Sys.time(), endDt=as.POSIXct("2012-09-01 01:00:00"), agg=TRUE)
    lapply(zLL, head)

    res <- NULL
    for (i in 1:length(zLL)){
      if (i==1) {
        res <- as.data.frame(zLL[[i]])
        colnames(res)[2] <- names(zLL)[[i]]
      } else {
        aux <- as.data.frame(zLL[[i]])
        colnames(aux)[2] <- names(zLL)[[i]]
        res <- merge(res, aux)
      }
    }
    write.csv(res, file="c:/temp/ccg_loads.csv", row.names=FALSE)    
  }


      
  ########################################################################
  #  CNE loads are scaled by reserve margin!
  #
  {
    lobj <- find_load_objects_SecDb_prism(asOfDate=Sys.Date()-1,
      pfolio="CNE NewEngland Load Portfolio")
    lobj <- unique(lobj[,c("load.name", "load.class", "share", "zone")])
    lobj <- subset(lobj, zone %in% c("SC", "CT", "WMA", "SEMA", "NEMA", "RI", "MAINE", "NH") &
                 load.name != "NULL")
 
    # by zone
    zLO <- split(lobj, lobj$zone)
    zLL <- lapply(zLO, read_secdb_load_objects, asOfDate=Sys.Date(),
      startDt=Sys.time(), endDt=as.POSIXct("2012-09-01 01:00:00"), agg=TRUE)
    lapply(zLL, head)

    res <- NULL
    for (i in 1:length(zLL)){
      if (i==1) {
        res <- as.data.frame(zLL[[i]])
        colnames(res)[2] <- names(zLL)[[i]]
      } else {
        aux <- as.data.frame(zLL[[i]])
        colnames(aux)[2] <- names(zLL)[[i]]
        res <- merge(res, aux)
      }
    }

    aux <- res
    aux$time <- format(aux$time)
    write.csv(aux, file="c:/temp/cne_loads.csv", row.names=FALSE)
  
  }


  
  loadName  <- "CSGLCONNEC LOAD"
  loadClass <- "6064"
  res <- PM:::secdb.getLoadObject(loadName, loadClass, Sys.Date())
  res$time <- as.POSIXlt(res$time)
  aux <- subset(res, time > Sys.time() & time <= as.POSIXct("2012-09-01 01:00:00") )
  
  plot(as.POSIXlt(aux$time), aux$value, type="l")
  
  
  ########################################################################
  #
  startDt <- as.POSIXct("2012-10-01 01:00:00")
  endDt   <- Sys.time()

  check_load_forecast_ccg(startDt, endDt)
  
  # there is something funny with CT!

  loadAssets <- .get_load_ids(company="CNE")
  write.csv(loadAssets, file="C:/Temp/cne_load_assets.csv", row.names=FALSE)

  
  loadAssets <- .get_load_ids(company="EXGN")
  loadAssets <- loadAssets[order(loadAssets$Share.of.Load.Reading),]

  check_load_forecast_ccg_assetId(startDt, endDt, assetId=2481)

  # performance by assetID
  res <- ddply(loadAssets, .(Asset.ID), function(x){
    check_load_forecast_ccg_assetId(startDt, endDt, assetId=x$Asset.ID)
  })

  res <- merge(loadAssets[, c("Asset.ID", "Asset.Name", "Load.Zone.Id")], res, 
    by=c("Asset.ID"))
  res <- res[order(-res$Actuals),]


  cast(res, Load.Zone.Id ~ ., sum, value="fcst-act")
  
  

  
  ids <- .get_load_ids(company="CNE")
  write.csv(ids, file="c:/temp/cne_load_assets.csv", row.names=FALSE)

  ids <- .get_load_ids(company="CCG")
  write.csv(ids, file="c:/temp/ccg_load_assets.csv", row.names=FALSE)

  ##########################################################################
  # get historical load by zone
  LL <- get_hist_ccg_loads_RTLOCSUM(as.POSIXct("2008-01-01 01:00:00"), Sys.time())
  TT <- apply(LL, 1, sum, na.rm=TRUE)
  
  write.csv(LL, file="c:/temp/ccg_hist_load.csv", row.names=FALSE)


  ##########################################################################
  # look at historical ccg forecast error
  #  see lib.loads.compare.actuals 

  startDt <- as.POSIXct(paste(format(Sys.Date()+1), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()+3), "00:00:00"))  # 2




  
}





## ##########################################################################
## #  not finished
## .read_demandbids_xls <- function(fname=NULL)
## {
##   if (is.null(fname)){
##     tomorrow <- tolower(format(Sys.Date()+1, "%d%b%y"))
##     tomorrow <- gsub("^0", "", tomorrow)
##     dir <- "S:/All/nMarket/NESMD/Update bids Price Sensitive/"
##     allfiles <- tolower(list.files(dir, full.names=TRUE))
##     idx <- grep(tomorrow, allfiles)
##     if (length(idx)!=1)
##       stop("Cannot find the file for tomorrow!")
        
##     fname <- allfiles[idx]
##   }

##   wb <- loadWorkbook(fname)
##   sheets <- getSheets(wb)
##   sheet  <- sheets[["Day 1"]]

##   val <- getMatrixValues(sheet, 7:31, 2:9)
##   colnames(val) <- c("ct", "me", "nema", "nh", "ri", "sema", "vt", "wma")
##   val <- data.frame(na.omit(val))

  
  
## }
