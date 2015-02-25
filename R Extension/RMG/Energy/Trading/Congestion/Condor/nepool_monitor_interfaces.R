# Monitor the lines that make the nepool interfaces if they
# will be out in the next couple of days.
# You can manually add other lines that influence the interfaces in
# .getOutagesOnInterfaces
#
# SEE NEPOOL/lib.NEPOOL.transmission.R for more functionality
#
# .readLatestOutageFile   - read short term outage csv file
# .getOutagesOnInterface  - merge the outages with the interface definition
# .makeTextOutages        - create text output
# .plotOutages            - experimental yet
# .saveAndCompare         - compare now with prev
#
# Written by Adrian Dragulescu on 20-May-2008


############################################################################
# Compare two ST Outages files 
#
.readLatestOutageFile <- function()
{
  dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages/"
  setwd(dirOut)
  files  <- sort(list.files(dirOut, pattern="csv$", full.names=TRUE))

  # pick the today's file that is the biggest.  Noticed that the 8AM is 2KB,
  # 7AM is 80KB!
  finfo <- file.info(files[grep(as.character(Sys.Date()), files)])
  lastFile <- rownames(finfo)[which.max(finfo$size)]
  if (length(lastFile)==0)
    return("Cannot find an outage file for today.")
  
  #lastFile <- files[length(files)]
  if (length(grep(as.character(Sys.Date()), lastFile))==0)
    return("Cannot find an outage file for today!")

  rLog("Reading file:", lastFile)
  OO <- read.csv(lastFile)                      # all outages
  lineOO <- subset(OO, equipment.type=="Line")  # line outages
  if (nrow(lineOO)==0)
    return("Empty outage file!")
  
  lineOO <- lineOO[,c("station", "equipment.description", "voltage",
    "planned.start", "planned.end", "actual.start",
    "actual.end", "status")]

  # replace MM:SS with 00:00 in the outage times.
  lineOO$planned.start <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$planned.start)
  lineOO$planned.end <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$planned.end)
  lineOO$actual.start <- gsub("[[:digit:]]{2}:[[:digit:]]{2}$",
    "00:00", lineOO$actual.start)

  # convert to POISXct
  lineOO$planned.start <- as.POSIXct(lineOO$planned.start,
                                          "%m/%d/%Y %H:%M:%S", tz="")
  lineOO$planned.end <- as.POSIXct(lineOO$planned.end,
                                          "%m/%d/%Y %H:%M:%S", tz="")
  lineOO$actual.start <- as.POSIXct(lineOO$actual.start,
                                          "%Y-%m-%d %H:%M:%S", tz="")
  
  lineOO <- .add_line_name(lineOO)
  
  return(lineOO)  
}



############################################################################
# Save current report and compare with previous stored one.
#   keep - how many reports to keep
#
#
.saveAndCompare <- function(out, keep=100)
{
  dirName <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/"
  fName   <- paste(dirName, "outage_interfaces.RData", sep="")
 
  time <- format(Sys.time(), "%Y-%m-%d %H:00:00")
  hour <- as.numeric(format(Sys.time(), "%H"))
  EE <- list(out)
  names(EE)[1] <- time
  
  if (!file.exists(fName)){
    # first time user
    TT <- EE
    save(TT, file=fName)

    return("")
  }
   
  load(fName)                # load list TT
  if (length(TT) > keep){
    TT <- TT[(length(TT)-keep+2):length(TT)]  # keep only the last ones
  }
  prev <- TT[length(TT)]     # last saved one

  # check additions
  allTT <- merge(cbind(tag="prev", prev[[1]]), EE[[1]], all=TRUE)
  if (any(is.na(allTT$tag))){
    allTT <- allTT[is.na(allTT$tag),]
    allTT$tag <- NULL
    allTT <- allTT[, c(1:3, 3+order(colnames(allTT)[4:ncol(allTT)]))]
    allTT <- allTT[,-4]  # remove the prev day
    ADDED <- c("Added/Modified:\n", capture.output(print(allTT, row.names=F)))
  } else {
    ADDED <- c("Added/Modified:\n", "NONE")
  }

  # check deletions
  allTT <- merge(prev[[1]], cbind(tag="last", EE[[1]]), all=TRUE)
  if (any(is.na(allTT$tag))){
    allTT <- allTT[is.na(allTT$tag),]
    allTT$tag <- NULL
    allTT <- allTT[, c(1:3, 3+order(colnames(allTT)[4:ncol(allTT)]))]
    allTT <- allTT[,-ncol(allTT)]  # remove the last day
    REMOVED <- c("Removed:\n", capture.output(print(allTT, row.names=F)))
  } else {
    REMOVED <- c("Removed:\n", "NONE")
  }

  timestamp <- paste("Comparing ", names(prev), " with ",
                     names(EE), ":", sep="")
  ADDEDREMOVED <- c(timestamp, ADDED, "\n\n", REMOVED)
  
  # append current report and save  
  TT <- c(TT, EE)   
  save(TT, file=fName)
  rLog("Saved file", fName)

  return(ADDEDREMOVED)
}


  
############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(SecDb)


returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_monitor_interfaces.R")
rLog("Start at ", as.character(Sys.time()))

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.transmission.R")

lineOO <- .readLatestOutageFile()

if (class(lineOO) == "data.frame" && (nrow(lineOO) > 0)){
  outEx <- .get_outages_on_interfaces(lineOO, frequency="hour")

  outEx <- .add_interface_slack( outEx ) 
  
  # next 7 days ...
  today <- as.POSIXct(paste(Sys.Date(), "00:00:00"))
  endTime <- today + (7*24-1)*3600

  out <- .outages_byHour_toText(outEx, today, endTime, style="range")

  outDiff <- .saveAndCompare(out)

  out <- capture.output(print(out, row.names=FALSE))
  out <- c(out, "\n\n", outDiff)
} else {
  out <- "Cannot find an outage file for today or file is empty!"
}

out <- c(out,
  "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/nepool_monitor_interfaces",
  "Contact: Adrian Dragulescu\n")


to <- paste(c("andrew.hlasko@constellation.com", "necash@constellation.com"), sep="", collapse=",")
#to <- "adrian.dragulescu@constellation.com"
from <- "OVERNIGHT_PM_REPORTS@constellation.com"
sendEmail(from, to, "NEPOOL transmission outages for interface lines", out)


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}






