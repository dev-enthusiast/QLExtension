# Download the demand bids from nepool website.
# And archive them to RData files. 
# NEPOOL publishes the files on first day of the month!
#
# .cMonth - what to do with each file
# .download_month - download gz files only
# .getLinksInPage
# .process - trigger the whole archive process 
#
# Written by Adrian Dragulescu on 2010-10-04


################################################################################
# aggregate daily demand bid files into one montly data file
#
.cMonth <- function(file=fileName)
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
  aux <- cbind(datetime = (as.POSIXct(day, format="%Y%m%d") +
    3600*as.numeric(aux$Trading.Interval)), aux)
  aux$Trading.Interval <- NULL

  aux$participantId <- as.integer(aux$participantId)
  aux$nodeId <- as.integer(aux$nodeId)
  aux[, 3:33] <- sapply(aux[, 3:33], as.numeric)  # this takes 30 secs
  
  #aux <- subset(melt(aux, id=1:5), !is.na(value))

  return(aux)  
}


##################################################################
# Download files for one month only
#
.download_month <- function(month)
{
  rootSite <- "http://www.iso-ne.com/markets/hstdata/mkt_offer_bid/da_energy/"
  pattern  <- "WW_DAHBENERGY_ISO_"
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


################################################################################
# Nepool is really cool about changing the filenames based on "their" report
# time!  So I need to find them in the html page source ... 
#
.getLinksInPage <- function(mainLink, pattern="WW_DAHBENERGY_ISO_")
{
  con <- url(mainLink)
  LL  <- readLines(con)
  close(con)
  ind <- grep(pattern, LL)
  if (length(ind)==0)
    stop(paste("Could not find any files with the pattern", pattern))

  LL <- LL[ind]
  aux <- sapply(strsplit(LL, 'href=\"'), "[", 2)
  aux <- unique(sapply(strsplit(aux, '\"'), "[", 1))

  
  return(paste("http://www.iso-ne.com", aux, sep=""))
}


##################################################################
# Go backwards and find missing pieces to process
# If you miss something than you process the whole chain.
#
#
.process <- function(month)       
{
  yyyymm <- format(as.Date(month), "%Y%m")
  finalFile <- paste(outdir, "RData/DB_", yyyymm, ".RData", sep="")
  
  if (file.exists(finalFile)){
    rLog(paste("Final file already created for ", yyyymm, ".  Next.", sep=""))
    return()                       
  }
  
  gzFiles  <- list.files(paste(outdir, "Raw", sep=""), full.names=TRUE,
    pattern=paste(".*", yyyymm, ".*CSV\\.gz$", sep=""))
  if (length(gzFiles)==0){
    .download_month(month)
  }

  gzFiles  <- list.files(paste(outdir, "Raw", sep=""), full.names=TRUE,
    pattern=paste(".*", yyyymm, ".*CSV\\.gz$", sep=""))
  if (length(gzFiles)==0){
    stop("Download did not work")
  }
  # unzip them
  setwd(paste(outdir, "Raw/", sep=""))
  for (file in gzFiles)
    system(paste("I:/gnu/bin/gunzip.exe", shQuote(file)))

  csvFiles  <- list.files(paste(outdir, "Raw", sep=""), full.names=TRUE,
    pattern=paste(".*", yyyymm, ".*CSV$", sep=""))
  if (length(csvFiles)==0){
    stop("Unzipping did not work")
  }
  
  rLog("Aggregate to a monthly file ...")
  keys <- list(csvFiles)
  names(keys) <- yyyymm
  setwd(outdir)
  conquer(keys, .cMonth, preFileName="RData/DB_")

  if (file.exists(finalFile)){
    rLog("Zip the csv files back up ...")
    setwd(paste(outdir, "Raw/", sep=""))
    for (file in csvFiles)
      system(paste("I:/gnu/bin/gzip.exe", shQuote(file)))
  }

}



##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(DivideAndConquer)
Sys.setenv(tz="")

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_energyoffers_nepool.R")
rLog("Start at ", as.character(Sys.time()))
outdir <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/EnergyOffers/"

asOfDate <- Sys.Date()
month <- seq(as.Date(format(asOfDate, "%Y-%m-01")),
  by="-4 months", length.out=2)[2]

for (month in as.character(seq(as.Date("2008-01-01"), month, by="1 month"))){
  .process(as.Date(month))
}


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



## # zip all the files the Raw dir ... 
## dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/EnergyOffers/Raw/"
## files <- list.files(dir, full.names=TRUE, pattern="CSV$")
## for (file in files)
##   system(paste("I:/gnu/bin/gzip.exe", shQuote(file)))
