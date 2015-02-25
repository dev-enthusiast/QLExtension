# Deal with single-source contingency in NEPOOL
#
#
# .agg_csv_files
# analysis_ssc
# .archive_ssc
# .get_filenames(month) 
# .get_sscontingency
# .get_sscontingency_trim
# .read_ssc_csv
# .write_ssc_tsdb
#

################################################################################
# files can be csv or gz
#
.agg_csv_files <- function(files, yyyymm,
   DIR="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/")
{
  .readOne <- function(file=fileName) {
    #browser()
    aux <- read.csv(file, skip=8)
    aux <- aux[-nrow(aux),]
    aux[,1] <- gsub(".*_(.*)\\.csv", "\\1", basename(file))
    colnames(aux) <- c("timestamp", "Interface", "time", "actual.margin",
      "authorized.margin", "base.limit", "single.source.contingency",
      "lowest.limit", "phaseII.RTflow")
    aux
  }

  keys <- list(files)
  names(keys) <- yyyymm
  
  setwd(DIR)
  conquer(keys, .readOne, preFileName="RData/ssc_")
}


################################################################################
# Look at historical patterns
#
.analysis_ssc <- function()
{
  require(zoo)
  require(PM)

  startDt <- as.POSIXct("2010-01-01 01:00:00")
  endDt <- Sys.time()
  symb <-  "nepool_ssc_limit"
  DD <- tsdb.readCurve(symb, startDt, endDt)

  # average hourly 
  HH <- aggregate(DD$value, list(time=as.POSIXct(format(DD$time-1,
    "%Y-%m-%d %H:00:00"))), mean)
  colnames(HH)[2] <- "limit"
  HH$mm    <- as.numeric(format(HH$time-1, "%m"))
  HH$year  <- as.numeric(format(HH$time-1, "%Y"))
  HH$month <- as.Date(format(HH$time-1, "%Y-%m-01"))
  HH$day <- as.Date(format(HH$time-1, "%Y-%m-%d"))
  HH$limit <- -HH$limit

  MM <- ddply(HH, c("month"), function(x){summary(x$limit)})
  aux <- melt(MM, id=1)
  
  require(lattice)

  # add buckets
  buckets <- c("5X16", "2X16H", "7X8")
  HH$bucket <- addBucket(HH$time)

  # daily average by day, bucket
  DD <- ddply(HH, c("day", "bucket"), function(x){
    data.frame(x[1,c("year", "month", "mm")], value=mean(x$limit))})

  xyplot(value ~ month|bucket, data=DD,
    groups=bucket, type=c("g", "p"),
    layout=c(1,3),
    ylab="SSC limit, MW")
 

  # median by month, bucket
  lim <- ddply(DD, c("month", "bucket"), function(x) {
    data.frame(q25=quantile(x$value, 0.25), median=median(x$value),
               q75=quantile(x$value, 0.75))})

  round_to <- function(x, to=25)
  {
    xMin <- to*(x %/% to)
    xMax <- to*(x %/% to + 1)
    ifelse(x - xMin > to/2, xMax, xMin)
  }

  res <- cast(lim, month ~ bucket, function(x){round_to(x, 25)},
    fill=NA, value="median")
  print(res, row.names=FALSE)
  
  
  


  round_to(c(1412.1, 1430, 1445, 1455), to=25)
  
  
  
  
  
  
  
}


################################################################################
# read the csv files, then gzip them 
# save all them files in the month of interest into the RData file
#
.archive_ssc <- function(month,
   DIR="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/")
{
  yyyymm <- format(as.Date(month), "%Y%m")
  prevMth <- format(as.Date(month-1), "%Y%m")
  file_arch <- paste(DIR, "RData/ssc_", prevMth, ".RData", sep="")
  if ((format(Sys.Date(), "%d")=="01") & !file.exists(file_arch)){
    csvFiles <- list.files(paste(DIR, "Raw", sep=""), full.names=TRUE,
      pattern=paste(".*ssc_iso_", prevMth, ".*", sep=""))
    if (length(csvFiles) > 0){
      rLog("Aggregating past month files") 
      .agg_csv_files(csvFiles, yyyymm)
    }
  }
    
  # Gzip all csv files other than the ones in current month
  csvFiles <- list.files(paste(DIR, "Raw", sep=""), full.names=TRUE)
  ind <- grepl(paste(".*ssc_iso_", yyyymm, ".*csv$", sep=""), csvFiles)
  csvFiles <- csvFiles[!ind]
  if (length(csvFiles)==0){
    setwd(paste(DIR, "Raw/", sep=""))
    for (file in csvFiles)
      system(paste("I:/gnu/bin/gzip.exe", shQuote(file)))
    setwd(DIR)
  }
}



################################################################################
# @param an R Date, e.g. 2011-02-01
#
.get_filenames <- function(month)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/Raw/"
  fnames <- list.files(DIR, full.names=TRUE,
    pattern=paste("ssc_iso_", format(month, "%Y%m"), ".*", sep=""))

  fnames
}


################################################################################
# download the file
#
.get_sscontingency <- function(
   DIR="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/")
{
  #link <- "http://www.iso-ne.com/trans/ops/limits/ssc/ssc.do?submit=csv"  # before 24-Aug-2014
  link <- "http://www.iso-ne.com/transform/csv/ssc?start="

  timestamp <- format(Sys.time(), "%Y%m%d%H%M%S")
  date <- format(Sys.Date(), "%Y%m%d")
  
  fname <- paste(DIR, "Raw/ssc_iso_", date, "_", timestamp,
    ".csv", sep="")
  download.file(link, fname)

  if (file.info(fname)$size > 0) {1} else 0
}


################################################################################
# download the file
# @param day the day you want
#
.get_sscontingency_trim <- function(day=Sys.Date()-1,
   DIR="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/")
{

  timestamp <- format(Sys.time(), "%Y%m%d%H%M%S")

  # download only the required day
  yyyymmdd <- format(day, "%Y%m%d")
  link <- paste("http://www.iso-ne.com/transform/csv/ssc?start=", yyyymmdd, sep="")
  
  fname <- paste(DIR, "Raw/ssc_iso_", yyyymmdd, "_", timestamp,
    ".csv", sep="")
 
  try(aux <- read.csv(link, nrows=8*5+1, skip=4))
  if (class(aux) != "try-error"){
    aux <- aux[-1,]  # get rid of first line
    
    write.csv(aux, file=fname, row.names=FALSE)
    rLog("Wrote file", fname)
    1
  } else {
    0
  }  
}


################################################################################
# read the scc reports
#
.read_ssc_csv <- function(fnames)
{
  res <- ldply(fnames, function(fname){
    rLog("Working on", fname)
    aux <- read.csv(fname)
    ## if (basename(fname) == "ssc_iso_20140824_20140825101207.csv")
    ##   browser()
    
    bux <- unique(aux[,c("Local.Time", "Lowest.Limit", "Phase.II.RT.Flow")])
    
    if (basename(fname) < "ssc_iso_20140824_20140825101207.csv") {
       bux$Local.Time <- as.POSIXct(bux$Local.Time)
    } else {
      bux$Local.Time <- as.POSIXct(bux$Local.Time, format="%m/%d/%Y %H:%M:%S")
    }
      
    bux
  })
  
  res
}

################################################################################
# Write the webservice data to an RData file.
# @param ssc the data.frame returned from .ws_get_singlesrccontingencylimits
#
.write_ssc_RData <- function( ssc )
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/Raw/"

  day <- format(ssc$BeginDate[1], "%Y-%m-%d")
  filename <- paste(DIR, "scc_iso_", day, ".RData", sep="")

  save(ssc, file=filename)
  rLog("Wrote", filename)
} 


################################################################################
# update to tsdb
# @param X data.frame with columns Local.Time, Lowest.Limit, Phase.II.RT.Flow
#
.write_ssc_tsdb <- function( X )
{
  symbol <- "nepool_ssc_limit"
  if ( !tsdbExists(symbol) ) {
    path <- paste("T:/Data/pm/nepool/", symbol, ".bin", sep="")
    description  <- "Nepool single source contingency limit"
    dataSource   <- "http://www.iso-ne.com/trans/ops/limits/ssc/ssc.do" 
    realTimeFlag <- TRUE
    
    tsdbCreate(symbol, path, description, dataSource, realTimeFlag=realTimeFlag)
  } 
  x <- data.frame(time=X$Local.Time, value=X$Lowest.Limit)
  if (nrow(x) > 0)
    tsdbUpdate(symbol, x)

  
  symbol <- "nepool_ssc_phase2_rt"
  if ( !tsdbExists(symbol) ) {
    path <- paste("T:/Data/pm/nepool/", symbol, ".bin", sep="")
    description  <- "Nepool Phase II RT flow from the SSC web page"
    dataSource   <- "http://www.iso-ne.com/trans/ops/limits/ssc/ssc.do" 
    realTimeFlag <- TRUE
    
    tsdbCreate(symbol, path, description, dataSource, realTimeFlag=realTimeFlag)
  } 
  x <- data.frame(time=X$Local.Time, value=X$Phase.II.RT.Flow)
  if (nrow(x) > 0)
    tsdbUpdate(symbol, x)
  
  
  0
}



########################################################################
# Get the largest single source contingency for one day
# @param day an RDate 
# @result a data.frame
#
.ws_get_singlesrccontingencylimits <- function( day, ws_version = "1.1")
{
  URL <- tolower(paste("https://webservices.iso-ne.com/api/v",
    ws_version, "/singlesrccontingencylimits/day/",
    format(day, "%Y%m%d"), sep=""))

  r <- .request( URL )
  x <- content(r, "text")

  # contents should start here
  ind <- gregexpr("SingleSrcContingencyLimits", x)[[1]][1]
  y <- substring(x, ind-2, nchar(x))  
  y <- fromJSON(y)

  z <- y$SingleSrcContingencyLimits$SingleSrcContingencyLimit
##                       BeginDate RtFlowMw LowestLimitMw DistributionFactor      InterfaceName ActMarginMw AuthorizedMarginMw SingleSrcContingencyMw BaseLimitMw
## 1 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3    NY Central-East     370.346               9999              -1234.487          NA
## 2 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3   NY Oakdale 230KV      22.074                  0              -2000.000        2000
## 3 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3   NY Oakdale 345KV      20.904                  1              -2020.000        1855
## 4 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3 NY Rochester 345KV       7.734                  2              -2035.000        1725
## 5 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3      PJM 5004/5005     330.364                  0              -1400.000        1400
## 6 2015-01-01T00:03:58.000-05:00  -1385.1         -1400                0.3        PJM Central    1891.360                  0              -1400.000        1400

  z$BeginDate <- .convert_timestamp(z$BeginDate, fmt="%Y-%m-%dT%H:%M:%OS%z")  # from lib.NEPOOL.webservices.R 

  
  z
}




################################################################################
################################################################################
#
.test <- function()
{
  require(CEGbase)
  require(plyr)
  require(reshape)
  require(DivideAndConquer)
  require(SecDb)
  require(Tsdb)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.NEPOOL.sscontingency.R")

  ####################################################################
  # download from webservices
  library(httr)
  library(XML)
  library(jsonlite)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.webservices.R")

  
  days <- seq(as.Date("2015-02-01"), as.Date("2015-02-15"), by="1 day")
  lapply(format(days), function(day) {
    rLog("Working on ", day)
    ssc <- .ws_get_singlesrccontingencylimits( as.Date(day) )
    .write_ssc_RData( ssc )
  })

  
  #day <- as.Date("2015-01-01")
  days <- seq(as.Date("2015-02-01"), as.Date("2015-02-15"), by="1 day")
  lapply(format(days), function(day) {
    rLog("Working on ", day)
    filename <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/SingleSourceContingency/Raw/",
                      "scc_iso_", day, ".RData", sep="")
    load(filename)
    X <- data.frame(Local.Time = ssc$BeginDate,
                    Lowest.Limit = ssc$LowestLimitMw,
                    Phase.II.RT.Flow = ssc$RtFlowMw)
    .write_ssc_tsdb( X )
  })

  
         
  
  

  ####################################################################
  # write to tsdb a bunch of months 
  months <- seq(as.Date("2014-09-01"), as.Date("2015-01-01"), by="1 month")  
  lapply(as.character(months), function(month)
  {
    rLog("Working on month", month)
    fnames <- .get_filenames(as.Date(month))
    X <- .read_ssc_csv(fnames)
    #plot(X[,1], X[,2], type="l")
    #browser()
    .write_ssc_tsdb(X)
  })
  

  
  
}


  ## startDt <- Sys.Date()-50
  ## endDt <- Sys.Date()
  ## res <- tsdb.readCurve("NEPOOL_IntChg_RT_PHActFlow", startDt, endDt)

