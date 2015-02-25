# Report PnL for Adrian Dragulescu
#
#
# Written by AAD on 15-Apr-2008

#################################################################
options(width=800)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(methods)   # not sure why this does not load!  It's documented in ?Rscript
require(CEGbase)
require(RODBC)
require(FTR)
require(reshape)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.daily.PnL.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.FTR.auction.R")

books          <- c("NESTAT", "VIRTSTAT")
reportName     <- "NESTAT + VIRTSTAT PnL"
pnlFileArchive <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/AAD_PnL_history.RData"


rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

FTR.load.ISO.env("NEPOOL")     
NEPOOL$CP <- NEPOOL$.loadClearingPrice()
#FTR.load.ISO.env("NYPP")

asOfDate <- Sys.Date()+1         # asOfDate <- as.Date("2009-04-10") #

rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")

QQ <- FTR.loadPositions(from="XLSX",
                        books=c("NESTAT", "NPFTRMT1"),
                        asOfDate=asOfDate)
#QQ <- subset(QQ, eti=="PAPE3PI")

PnL <- daily.FTR.PnL(asOfDate, QQ, NEPOOL, hasFixPrice=TRUE)

daPnL <- subset(PnL, day==asOfDate)  # Day Ahead PnL
rownames(daPnL) <- NULL
daPnL$path.no   <- NULL
daPnL <- daPnL[order(daPnL$PnL),]

daPnL$day <- NULL
daPnL$PnL <- round(daPnL$PnL)
daPnL$CP  <- round(daPnL$CP, 2)

out <- c(
  paste("Total DA FTR PnL is:", dollar(sum(daPnL$PnL))),
  paste("Total DA FTR PnL for NESTAT   is:", dollar(sum(subset(daPnL, book=="NESTAT")$PnL))),
  paste("Total DA FTR PnL for NPFTRMT1 is:", dollar(sum(subset(daPnL, book=="NPFTRMT1")$PnL))),         
  "\n\n",
  capture.output(print(daPnL, row.names=FALSE)),
  "\n\nProcmon job: NESTAT_PnL\nContact: Adrian Dragulescu")

rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
#  getEmailDistro("daily_PnL_AAD"),
  "adrian.dragulescu@constellation.com",         
  paste("FTR DA PnL for ", as.character(asOfDate)), out)

rLog(paste("Done at ", Sys.time()))

returnCode <- 0        # make it succeed.  I oon't need emails from IT!
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}






## out <- main.PnL(asOfDate, books, reportName, pnlFileArchive, 
##                 runFTRs=TRUE, runTCCs=TRUE, runINCDECs=TRUE)


## cArgs <- commandArgs()
## ind <- max(grep("^--", cArgs))
## if (length(cArgs) > ind) {
##   asOfDate <- as.Date(cArgs[ind+1])  # pass asOfDate ... 
## }
