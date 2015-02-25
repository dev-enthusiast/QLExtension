# Various utilities to use when downloading data from NEPOOL
#
# .download_month -- download all the files from one month, gz files only
# .process -- trigger the archiving, etc. (in a smart way)
#
#

################################################################################
# Aggregate daily ImportExport files into one monthly data file.
# Each file type has a different aggregator. 
#
.cMonth.IE <- function(file=fileName)
{
  day <- strsplit(file, "_|\\.")[[1]][4]
  aux <- read.csv(file, skip=3)
  aux <- aux[-1,]           # remove the units line
  aux <- aux[-nrow(aux),]   # remove the last comment line
  if (ncol(aux) != 12)
    stop("File format changed!")
  aux <- aux[,-1]           # remove first column, just descriptive
  aux <- cbind(datetime = (as.POSIXct(day, format="%Y%m%d") +
    3600*as.numeric(aux$Trading.Interval)), aux)
  aux$Trading.DateTime <- PM:::isotimeToPOSIXct(data.frame(
    Date=aux[,"Trading.Date"], Hour.Ending=aux[,"Trading.Interval"]))
  
  aux$Trading.Interval <- NULL

  aux$participantId <- as.integer(aux$participantId)
  aux$nodeId <- as.integer(aux$nodeId)
  aux[, 3:33] <- sapply(aux[, 3:33], as.numeric)  # this takes 30 secs
  
  return(aux)  
}


################################################################################
# Aggregate daily EnergyOffer files into one monthly data file.
# Each file type has a different aggregator. 
#
.cMonth.EO <- function(file=fileName)
{
  day <- strsplit(file, "_|\\.")[[1]][4]
  aux <- read.csv(file, skip=3)
  aux <- aux[-1,]           # remove the units line
  aux <- aux[-nrow(aux),]   # remove the last comment line
  if (ncol(aux)!=34)
    stop("File format changed!")
  names(aux)[c(3,4)] <- c("participantId", "nodeId")
  aux <- aux[,-1]           # remove first column, just descriptive
  names(aux)[12:31] <- paste(c("P.", "MW."), rep(1:10, each=2), sep="")
  aux <- cbind(Trading.Date = as.Date(day, format="%Y%m%d"), aux)
  aux <- cbind(PM:::isotimeToPOSIXct(data.frame(
    Date=aux[,"Trading.Date"], Hour.Ending=aux[,"Trading.Interval"])), aux)
  aux$Trading.Date <- aux$Trading.Interval <- NULL
  aux$Date <- aux$Hour.Ending <- NULL
 
  
  aux$participantId <- as.integer(aux$participantId)
  aux$nodeId <- as.integer(aux$nodeId)
  aux[, 3:33] <- sapply(aux[, 3:33], as.numeric)  
  
  return(aux)  
}



##################################################################
# Download files for one month only
# dump them in the Raw/ subdirectory
#
.download_month <- function(month, outdir, 
   rootSite="http://www.iso-ne.com/markets/hstdata/mkt_offer_bid/da_regulation/",
   pattern="WW_DAHBREGASM_ISO_")
{
  month <- as.Date(month)
  yearMon  <- tolower(format(month, "%Y/%b"))   # "2008/oct"
  yyyymm   <- format(month, "%Y%m")
  mainLink <- paste(rootSite, yearMon, "/index.html", sep="")

  curdir <- getwd()
  setwd(paste(outdir, "Raw/", sep=""))

  # get the links to the files that need to be downloaded
  allFiles <- .getLinksInPage(mainLink, pattern=pattern)
  outFiles <- basename(allFiles)  # what I save
  outFiles <- strsplit(outFiles, "_")
  outFiles <- sapply(outFiles, function(x){
    x[4] <- paste(substr(x[4], 1, 8), ".CSV.gz", sep="")
    x <- paste(x[1:4], sep="", collapse="_")
    return(x)})

  rLog("Found ", length(outFiles), "files")
  for (f in 1:length(allFiles)){
    if (file.exists(outFiles[f]))
      next
    download.file(allFiles[f], outFiles[f])
  }
  setwd(curdir)  # no side effects please!
}


##################################################################
# Go backwards and find missing pieces to process
# If you miss something than you process the whole chain.
#
#
.process <- function(month, outdir, rootSite, pattern, fun=.cMonth.EO)       
{
  yyyymm <- format(as.Date(month), "%Y%m")
  finalFile <- paste(outdir, "RData/DB_", yyyymm, ".RData", sep="")
  
  if (file.exists(finalFile)){
    rLog(paste("Final file already created for ", yyyymm, ".  Next.", sep=""))
    return()                       
  }
  
  gzFiles <- list.files(paste(outdir, "Raw", sep=""), full.names=TRUE,
    pattern=paste(".*", yyyymm, ".*CSV\\.gz$", sep=""))
  if (length(gzFiles)==0) {
    .download_month(month, outdir, rootSite=rootSite, pattern=pattern)
  }

  gzFiles  <- list.files(paste(outdir, "Raw", sep=""), full.names=TRUE,
    pattern=paste(".*", yyyymm, ".*CSV\\.gz$", sep=""))
  if (length(gzFiles)==0){
    stop("Download did not work")
  }

  rLog("Aggregate to a monthly file ...")
  keys <- list(gzFiles)
  names(keys) <- yyyymm
  setwd(outdir)
  conquer(keys, fun, preFileName="RData/DB_")

}


##################################################################
##################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(SecDb)
  require(zoo)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.download.bidoffer.R")


  # ENERGY_OFFERS
  {
    rootSite <- "http://www.iso-ne.com/markets/hstdata/mkt_offer_bid/da_energy/"
    pattern <- "WW_DAHBENERGY_ISO_"
    outdir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/EnergyOffers/"
  }

   #month <- as.Date("2011-11-01") 
   #months <- seq(as.Date("2012-09-01"), by="1 month", length.out=7)
   months <- seq(as.Date("2012-09-01"), as.Date("2013-05-01"), by="1 month")
  
   lapply(months, function(month) { 
     .process(month, outdir, rootSite, pattern, fun=.cMonth.EO)
   })


  

}
