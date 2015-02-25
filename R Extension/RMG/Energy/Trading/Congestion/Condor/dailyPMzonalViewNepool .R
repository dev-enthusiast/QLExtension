# Calculate daily my view on the zonal spreads
# No need to calculate it daily!  Run it weekly by hand, say on Sat
# takes a good 25 min to run!
#

###########################################################################
#
oneZone <- function(ptid, save=TRUE)
{
  deliv.pt <- names(ptid)
  
  # look at historical marks and fwd marks
  histNodeMark <- PM:::.getHistMark(ptid)
  histNodeMark$source <- "Historical"

  secDbNodeMark <- PM:::.getSecDbMark(deliv.pt, asOfDate, startDate, endDate)

  pmView <- makePMview(ptid)
  pmMark <- constructNodeMark(pmView, asOfDate, startDate, endDate)
  pmMark$source <- "pmView"

  # rbind for plotting
  allMarks <- rbind(histNodeMark, secDbNodeMark, pmMark)  
  plotMarks(allMarks, deliv.pt, main=paste("Hub to", deliv.pt), save=save)

  fileOut <- paste("allMarks_", deliv.pt, ".csv", sep="")
  write.csv(allMarks, file=fileOut, row.names=FALSE)
  cat("Output written to: ", getwd(), "/", fileOut, "\n", sep="")
  
}



###########################################################################
# MAIN
#
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(CEGbase); require(FTR); require(PM)
Sys.setenv(tz="")  
setwd("S:/All/Structured Risk/NEPOOL/Marks/AAD/")

# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

FTR.load.ISO.env("NEPOOL")

marksForDate <- Sys.Date()
asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-1b"))
startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
endDate   <- seq(startDate, by="11 months", length.out=2)[2] 

ptids  <- structure(c(4001:4008), names=c("MAINE", "NH", "VT", "CT",
  "RI", "SEMA", "WMA", "NEMA"))

for (p in 1:length(ptids)){
  rLog("Working on:", names(ptids)[p])
  try( oneZone(ptids[p], save=TRUE) )
}

out <- ""

rLog("Email results")
sendEmail("OVERNIGHT_PM_REPORTS",
#  getEmailDistro("daily_PnL_AAD"),
  "adrian.dragulescu@constellation.com",         
  paste("PM zonal view for ", as.character(asOfDate)), "Done!", out)

rLog(paste("Done at ", Sys.time()))

returnCode <- 0        # make it succeed.  I oon't need emails from IT!
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}
                    
                    




#########################################################################
#########################################################################





