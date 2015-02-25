# Monitor changes in the FI_UNITOPER reports in NEPOOL
#
# Written by Adrian Dragulescu on 2011-01-14


################################################################################
# Prepare a side by side comparison of changes in load forecast
# Have to run this before archiving ...
#
.email <- function(out)
{
   
  rLog("Email results")
  to <- paste(c("NECash@constellation.com", "steven.wofford@constellation.com",
    "robert.pleiss@constellation.com", "david.w.butrow@constellation.com",
    "robert.kline@constellation.com", "michael.michalek@constellation.com",
    "michael.boyd@constellation.com", "james.r.young@constellation.com",
    "EastDesk@constellation.com"),  # <-- this is CCG 24 Hour East Desk
              sep="", collapse=",")
  #to <-  "adrian.dragulescu@constellation.com"
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
    paste("FI_UNITOPER changes for day:", format(asOfDate)),
    c(out,
      paste("\n\n\nPlease contact Adrian Dragulescu for questions regarding",
        "this procmon job."),
       paste("Procmon job:",
        "RMG/R/Reports/Energy/Trading/Congestion/nepool_monitor_fi_unitoper")))

}

##################################################################
#
.format_one <- function(X)
{
  out <- cast(X, name + Hour.End ~ version, value="MWh", fill=NA)
  out <- capture.output(print(out, row.names=FALSE))
  out <- c(out, " ",
    "==============================================================================",
    " ")
  
  out
}


##################################################################
# 
.get_filenames <- function(asOfDate)
{
  yyyymm <- format(asOfDate, "%Y%m")
  DIR <- paste("S:/Data/NEPOOL_EXGN/RawData/OldZips/Rpt_", yyyymm,
    "/FI_UNITOPER/FI_UNITOPER_", format(asOfDate, "%Y%m%d"), sep="")
  list.files(DIR, full.names=TRUE)
}

##################################################################
# look in the file that keeps the state 
#
.has_updated <- function(asOfDate)
{
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/",
    "ISODatabase/NEPOOL/Units/FI_UNITOPER/no_reports.csv", sep="")

  if (!file.exists(fname)) {
    FALSE
  } else {
    noReports <- subset(read.csv(fname), date==format(asOfDate, "%Y%m%d"))$noFiles
    if (length(noReports)==0) noReports <- 0
    files <- .get_filenames(asOfDate)  # current files
    if (length(files) != noReports) {
      write.csv(data.frame(date=format(asOfDate, "%Y%m%d"),
                           noFiles=length(files)), file=fname, row.names=FALSE)
      TRUE
    } else {
      FALSE
    }
  }
}


##################################################################
# read the csv files and prepare the output
#
.process <- function( units )
{
  files <- .get_filenames(asOfDate)

  res <- lapply(files, .read_one)
  res <- do.call(rbind, res)
  res <- subset(res, Number %in% names(units))
  res$MWh <- sprintf("%.1f", res$MWh)
  colnames(res)[2] <- "PTID"
  res$name <- units[res$PTID]
  res$PTID <- NULL  
  
  res <- split(res, res$name)
  res <- res[as.character(units)]
  out <- unlist(lapply(res, .format_one))

  out
}

##################################################################
# put the most recent data into tsdb
# It's already there! nepool_rpt_fi_unit_1616_tdy
#
.publish_tsdb <- function(asOfDate)
{  
  files <- .get_filenames(asOfDate)
  lastFile <- sort(files)[length(files)]

  res <- .read_one(lastFile)

  symbols <- paste("nepool_fi_unitoper_")
  
  
  
}
 

##################################################################
# read one report csv file,
# reports are stored on the S:/Data/Nepool/RawData/OldZips/ folder
#
.read_one <- function(filename)
{  
  #version <- substr(filename, nchar(filename)-17, nchar(filename)-4)
  version <- gsub(".*_(.*)\\.CSV.*", "\\1", basename(filename) )
  version <- as.POSIXct(version, format="%Y%m%d%H%M%S") - 5*3600
  version <- format(version, "%m/%d %H:%M")
  aux <- read.csv(filename, skip=5)
  aux <- aux[,-1]
  aux <- aux[-nrow(aux),]
  aux <- cbind(version=version, aux)

  aux
}
 


##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(SecDb)
#Sys.setenv(tz="")

returnCode <- 99   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_monitor_fi_unitoper.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate <<- Sys.Date()
ptids <- c("1478",     "1616",     "40327",          "40328",  "14614", "502",
  "417", "418", "419", "466", "625", "626", "627", "642", "1625")
units <- c("Mystic 8", "Mystic 9", "ForeRiver1", "ForeRiver2", "Kleen", "Mystic 7",
  "FrJet1", "FrJet2", "FrJet3", "LSt.", "WMed1", "WMed2", "WMed3", "Yarmouth4", "GraniteRidge")
names(units) <- ptids

# check if the asOfDate folder has a new file inside
res <- .has_updated(asOfDate)
rLog("Has file updated?", res)
if (res) {
  out <- .process(units=units) 
  .email(out)
}

rLog("Done at ", as.character(Sys.time()))

if (as.numeric(format(Sys.time(), "%H"))==23)
  returnCode <- 0    # not sure why procmon complains if this is not here


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


