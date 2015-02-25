# Report PnL for Adrian Dragulescu
#
#
# Written by AAD on 15-Apr-2008

#################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(methods)   # not sure why this does not load!
require(CEGbase); require(RODBC); require(FTR)
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.daily.PnL.R")

books          <- c("NPFTRMT1")
reportName     <- "PM FTRs and TCCs PnL"
pnlFileArchive <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/PMcong_PnL_history.RData"

# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

FTR.load.ISO.env("NEPOOL")
NEPOOL$CP <- NEPOOL$.loadClearingPrice()
FTR.load.ISO.env("NYPP")

cArgs <- commandArgs()
if (length(cArgs)>6){
  asOfDate <- as.Date(cArgs[7])  # pass asOfDate ... 
} else {
  asOfDate <- Sys.Date()         # asOfDate <- as.Date("2009-01-12") #
}

rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")

## out <- main.PnL(asOfDate, books, reportName, pnlFileArchive,
##                 runFTRs=TRUE, runTCCs=TRUE, runINCDECs=TRUE)

## rLog("Email results")
## sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
##   "adrian.dragulescu@constellation.com",
##   paste("FTRs and TCCs PnL for PM as of", as.character(asOfDate)), out)

rLog(paste("Done at ", Sys.time()))

returnCode <- 0        # make it succeed.  I oon't need emails from IT!
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

