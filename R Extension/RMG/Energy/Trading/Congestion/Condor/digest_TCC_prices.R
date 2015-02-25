# Add to the archive of NYISO$CP the latest TCC clearing prices that
# SecDb downloads them.  The procmon process is datafeed/NYPP/DnldTCC
# and runs every Mon morning at 6:00am.
#
# Written by AAD on 6-Aug-2009



#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase)
require(SecDb)
require(CRR)
require(reshape)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.TCC.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate <- Sys.Date()

rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")

load_ISO_environment("NYPP")
update_clearingprice_archive()


out <- paste("procmon job: RMG/R/Reports/Energy/Trading/Congestion/digest_TCC_prices",
             "contact: Adrian Dragulescu", sep="\n")
rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
  "adrian.dragulescu@constellation.com", 
  "NYPP$CP archive update updated", out)

rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}






