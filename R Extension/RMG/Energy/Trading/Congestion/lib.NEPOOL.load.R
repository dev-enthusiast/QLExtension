# Functions to deal with NEPOOL nodal load weights
#
# download.nodal.load.weights 
# extract.nodal.load.weights
# get_historical_zonal_load
# get_historical_pool_load
# get_load_deviation_pool           -- calculate DA vs. RT load difference
# get_load_deviation_cne
# get_load_deviation_exgen          
# .hist_fwd_view_load
# load_weights_monthly_avg
# nepool.load.fcst              
# tlp.nepool.load.fcst
#
# Other strats tsdb nepool load symbols: 
#   bucketizebymonth("NEPOOL_CT_tsdb_2010","peak","nepool","3600","sum");CT
#
# Nepool 3 day ahead load forcast:
#  isone_loadforeacst_native
#  isone_loadforeacst_2ndday
#  isone_loadforeacst_3rdday
#
#

#########################################################################
# Pick a node to extract, e.g. and apply a function to each file
#
apply.nodal.load.weights.files <- function(fun, startDate=as.Date("2007-01-01"),
  endDate=as.Date("2007-12-31"), ptids=4078, zone="4001")
{ 
  require(RODBC)
  dir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
     "Load/Nodal_Load_Weights/Raw/", sep="")
  days  <- seq(startDate, endDate, by="1 day")
  yyyymm <- unique(format(days, "%Y_%m"))
  res <- vector("list", length=length(yyyymm))
  for (i in 1:length(yyyymm)){
    fname <- paste(dir, yyyymm[i], "_zone_", zone, ".xls", sep="")
    rLog("Working on ", fname)
    con   <- odbcConnectExcel(fname)
    tables <- sqlTables(con)[1,]
    tname <- gsub("'|\\$", "", tables$TABLE_NAME)
    data  <- sqlFetch(con, tname)
    odbcCloseAll()
    #data  <- subset(data, `LOCATION ID` %in% ptids)
    data <- fun(data, ptids)  # apply your function
    res[[i]] <- data
  }
  return(res)
}


#########################################################################
# easy with this function because it downloads a lot of files
#
download.nodal.load.weights <- function(startDate=as.Date("2007-01-01"),
  endDate=as.Date("2007-12-31"), zone="4001")
{
  site   <- "http://www.iso-ne.com/markets/hstdata/hourly/nodal_load_wghts/"
  outdir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
     "Load/Nodal_Load_Weights/", sep="")

  days  <- seq(startDate, endDate, by="1 day")
  yyyymm <- unique(format(days, "%Y_%m"))
  for (i in yyyymm){
    year <- substring(i, 1, 4)
    mon  <- tolower(month.abb[as.numeric(substring(i, 6, 7))])
    rLog("Working on month:", as.character(i))
    fname <- paste(i, "_zone_", zone, ".xls", sep="")
    url <-  paste(site, year, "/", mon, "/", fname, sep="")
    fileOut <- paste(outdir, "Raw/", fname, sep="")

    if (!file.exists(fileOut)){
      command = paste( "i:\\util\\wget ",
        "--no-check-certificate ", "--output-document=",
        shQuote(gsub("/", "\\\\", fileOut)), " ", url, sep="" ) 
      errorCode <- system( command )
      #download.file(url, fileOut)  # does not download properly!
      rLog("Done")
    } else {
      rLog("File there already!")
    }
  }
}


################################################################
# Compare the historical and the fwd load
# 
# 
#
.hist_fwd_view_load <- function()
{
  require(lattice)
  
  LL.hist <- get_historical_pool_load(agg.month=TRUE)
  LL.hist$source <- "hist"
  
  startDt <- as.POSIXct(paste(nextMonth(), "01:00:00"))
  endDt   <- as.POSIXct("2021-01-01 00:00:00")

  # add the 2010 WN forecast  
  aux <- nepool.load.fcst(startDt, endDt, from="2010", agg.month=TRUE)
  aux <- zoo(apply(aux, 1, sum), index(aux))
  aux <- data.frame(month=index(aux), as.data.frame(aux))
  colnames(aux)[2] <- "pool.load.MWh"
  rownames(aux) <- NULL
  aux$source <- "WN.2010"
  LL <- rbind(LL.hist, aux)
  head(LL)

  # add the tlp WN forecast  
  aux <- nepool.load.fcst(startDt, endDt, from="tlp", agg.month=TRUE)
  aux <- zoo(apply(aux, 1, sum), index(aux))
  aux <- data.frame(month=index(aux), as.data.frame(aux))
  colnames(aux)[2] <- "pool.load.MWh"
  rownames(aux) <- NULL
  aux$source <- "WN.tlp"
  LL <- rbind(LL, aux)
  head(LL)
  
  xyplot(pool.load.MWh/1000000 ~ month, data=LL,
         groups=LL$source,
         type=c("g", "o"),
          auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=length(unique(LL$source))),
         ylab="Pool load, MMWh")


  

}



################################################################
# Get the NEPOOL pool load
# 
# @return a data.frame
#
get_historical_pool_load <- function(start.dt=as.POSIXct("2008-01-01 01:00:00"),
  end.dt=Sys.time(), agg.month=FALSE)
{
  #symbs <- "nepool_load_hist"        # this is so much smaller ... 
  symbs <- "neRpt_rtCus_PoolLoadObl"  # RT Pool Load obligation from SR_RTCUSTSUM reports

  if (agg.month) {
    LL <- PM:::ntplot.bucketizeByMonth(symbs, "FLAT",
      as.Date(start.dt), as.Date(prevMonth(end.dt)), stat="Sum")
    LL <- LL[,c("month", "value")]
    
  } else {
    LL <- FTR:::FTR.load.tsdb.symbols(symbs,
      start.dt=start.dt, end.dt=end.dt)
    LL <- data.frame(datetime=index(LL), pool.load.MWh=as.numeric(LL))
  }

  colnames(LL)[2] <- "pool.load.MWh"
  rownames(LL) <- NULL
  LL$pool.load.MWh <- - LL$pool.load.MWh
  
  LL  
}


################################################################
# git them from tsdb
#
get_historical_zonal_load <- function(ptids=NULL,
  start.dt=as.POSIXct("2008-01-01 01:00:00"), end.dt=Sys.time())
{
  symbs <- matrix(c(
    # old symbols, retired after 8/23/2014                
    ## "nepool_zonal_4001_load_da", "DA_4001_Load",  
    ## "nepool_zonal_4001_load_rt", "RT_4001_Load",
                     
    "nepool_smd_da_4001_ld", "DA_4001_Load",
    "nepool_smd_da_4002_ld", "DA_4002_Load",
    "nepool_smd_da_4003_ld", "DA_4003_Load",
    "nepool_smd_da_4004_ld", "DA_4004_Load",
    "nepool_smd_da_4005_ld", "DA_4005_Load",
    "nepool_smd_da_4006_ld", "DA_4006_Load",
    "nepool_smd_da_4007_ld", "DA_4007_Load",
    "nepool_smd_da_4008_ld", "DA_4008_Load",

    "nepool_smd_rt_4001_ld", "RT_4001_Load",
    "nepool_smd_rt_4002_ld", "RT_4002_Load",
    "nepool_smd_rt_4003_ld", "RT_4003_Load",
    "nepool_smd_rt_4004_ld", "RT_4004_Load",
    "nepool_smd_rt_4005_ld", "RT_4005_Load",
    "nepool_smd_rt_4006_ld", "RT_4006_Load",
    "nepool_smd_rt_4007_ld", "RT_4007_Load",
    "nepool_smd_rt_4008_ld", "RT_4008_Load"
   ), ncol=2, byrow=TRUE)
   colnames(symbs) <- c("symbol", "name")

  LL <- FTR:::FTR.load.tsdb.symbols(symbs[,"symbol"],
    start.dt=start.dt, end.dt=end.dt)
  names(LL) <- symbs[, "name"]

  LLL <- melt(as.matrix(LL))
  names(LLL)[1:2] <- c("time", "variable")
  
  LLL$ptid <- as.numeric(gsub(".*_(.*)_.*", "\\1", LLL$variable))
  LLL$type <- gsub(".*_.*_(.*)", "\\1", LLL$variable)
  LLL$dart <- gsub("(.*)_.*_.*", "\\1", LLL$variable)
  LLL$time <- rep(index(LL), 16)  
    
  LLL
}


#########################################################################
# Pull the DA/RT load deviation for CNE or EXGEN
# 
# @param startDt a POSIXct
# @param company, either CNE or EXGEN
# @return a zoo aggregated by day (summed over the zones and hours).
#
get_load_positive_deviation <- function(startDt, endDt=Sys.now(), company="EXGEN")
{
  # get the cleared demand bids
  db <- get_cleared_demandbids(startDt, endDt, company=company)
  ZZ <- structure(c("me", "nh", "vt", "ct", "ri", "sema", "wma", "nema"),
    names=4001:4008)
  colnames(db) <- ZZ[colnames(db)]
  head(db)
  db.pool <- zoo(apply(db, 1, sum, na.rm=TRUE), index(db))
  
  # get rt loads
  if (tolower(company) == "exgen") {
    rt <- get_hist_ccg_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
  } else if (tolower(company) == "cne") {
    rt <- get_hist_cne_loads_RTLOCSUM(startDt, endDt, as.matrix=FALSE)
  } else {
    stop("unknown company: ", company)
  }
  rt.pool <- zoo(apply(rt, 1, sum, na.rm=TRUE), index(rt))
  head(rt.pool)
  
  dart <- merge(db.pool, rt.pool)
  dart$dart <- -dart$db.pool + dart$rt.pool
  #plot(index(dart),  dart$dart)

  dart.day <- aggregate(dart$dart, as.Date(format(index(dart)-1, "%Y-%m-%d")),
    function(x) { sum(x[x>0]) })
  
  head(dart.day)
  #plot(index(dart.day),  as.numeric(dart.day))
 
  dart.day
}



#########################################################################
# Calculate avg. by month
#
load_weights_monthly_avg <- function(files)
{
  require(RODBC)
  DIR <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
     "Load/Nodal_Load_Weights/Raw/", sep="")
  files <- list.files(DIR, pattern="2012_.*_zone_4008.*")

  res <- ldply(files, function(fname) {
    rLog("Working on ", fname)
    con   <- odbcConnectExcel(paste(DIR, fname, sep=""))
    tables <- sqlTables(con)[1,]
    tname <- gsub("'|\\$", "", tables$TABLE_NAME)
    data  <- sqlFetch(con, tname)
    odbcCloseAll()

    x <- cast(data, location_id ~ ., mean, value="mw_factor")
    colnames(x)[2] <- "mw_factor"
    cbind(month=substring(fname, 1, 7), x)
  })
  
  write.csv(res, file="c:/temp/nema_nodal_weights_2012.csv",
            row.names=FALSE)
  
}


#########################################################################
# Get a nepool load foreacst from various sources 
# should extend it maybe
# the "2010" is the WN load we've been using for all the ancillaries.
#   prior to 2/2014.  Changed to the "tlp" since.
# the "tlp" is the WN load did by strats a while ago.  It seem to be
#   a bit low for Jul/Aug, but is better than "2010" load.  Use this one
#   now (2/1/2014 - AAD).   
#
#
nepool.load.fcst <- function(startDt=as.POSIXct("2012-01-01 01:00:00"),
  endDt=as.POSIXct("2014-01-01 00:00:00"), zones=4001:4008,
  from=c("2010", "tlp", "celt2012"), extend=TRUE, agg.month=FALSE)
{
  ZZ <- structure(c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
    names=4001:4008)

  if (from == "2010") {
    symbs <- paste("nepool_", ZZ[as.character(zones)], "_tsdb_2010", sep="")
    rLog("Consider using the tlp wn load forecast!")
  } else if (from == "tlp") {
    symbs <- paste("nepool_", ZZ[as.character(zones)], "_simload", sep="")
    
  } else if (from == "celt2012") {

  } else {
    stop(from, "load forecast not implemented!")
  }
  
  HH <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(HH) <- as.character(zones)
  head(HH)

  if (extend & (endDt > index(HH)[length(index(HH))])) {
    rLog("Extending the load forecast to", format(endDt))
    last.dt  <- index(HH)[length(index(HH))]  
    lastYear <- as.numeric(format(last.dt, "%Y"))
    ind <- seq(as.POSIXct(paste(lastYear-1, "-01-01 01:00:00", sep="")),
               last.dt, by="1 hour")
    fullYearHH <- HH[ind,]

    maxYr <- as.numeric(format(endDt, "%Y"))
    for (i in 1:(maxYr-lastYear+1)) {
      ind <- index(fullYearHH) + 8760*3600*i
      HH  <- rbind(HH, zoo(as.matrix(fullYearHH), ind))
    }

    HH <- window(HH, end = endDt)
  }

  if (agg.month) {
    HH <- aggregate(HH,
      as.Date(format(index(HH)-1, "%Y-%m-01")), sum, na.rm=TRUE)
  }
  
  HH
}



#########################################################################
# Get the latest tlp nepool load foreacst
#
tlp.nepool.load.fcst <- function(startDt=as.POSIXct("2012-01-01 01:00:00"),
  endDt=as.POSIXct("2014-01-01 00:00:00"), zones=4001:4008)
{
  rLog("DEPRECATED!  please use nepool.load.fcst from the same lib!")
  ZZ <- structure(c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
    names=4001:4008)

  symbs <- paste("nepool_", ZZ[as.character(zones)], "_simload", sep="")
  
  HH <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
  colnames(HH) <- as.character(zones)
  head(HH)

  HH
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

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  
  startDt <- as.POSIXct("2014-01-01 01:00:00")
  endDt   <- as.POSIXct("2014-03-01 00:00:00")
  
  
  res     <- tlp.nepool.load.fcst()


  startDt <- as.POSIXct("2014-01-01 01:00:00")
  endDt   <- as.POSIXct("2015-01-01 00:00:00")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  LL <- nepool.load.fcst(startDt, endDt, from="tlp")

    # get the peak hours, monthly totals for FwdRes
    require(PM)
    pk <- filterBucket(LL, buckets="5x16")[[1]]
    mth <- aggregate(pk, format(index(pk)-1, "%Y-%m-01"), sum)
    agg <- data.frame(
      month = index(mth),
      ct    = as.numeric(mth[,"4004"]),
      nema  = as.numeric(mth[,"4008"]),
      ros   = as.numeric(apply(mth[,c("4001", "4002", "4003", "4005", "4006", "4007")], 1, sum)))
     

  


  

  .hist_fwd_view_load()
  

  download.nodal.load.weights(startDate=as.Date("2012-01-01"),
                              endDate=as.Date("2012-12-31"),
                              zone="4008")

  
  

}
