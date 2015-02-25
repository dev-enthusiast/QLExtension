# A battery of tests for demand bids
# to run every morning at 8:00AM
#


##################################################################
##################################################################
options(width=400)  
require(CEGbase)
require(SecDb)
require(zoo)
require(reshape)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.check.demandbids.R")
   

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_check_demandbids.R")
rLog("Start at ", as.character(Sys.time()))

returnCode <- 0   # 0 means success

asOfDate <- Sys.Date() + 1
res <- try(.check_demand_bids( asOfDate, email=TRUE ))
if (class(res) == "try-error")
  returnCode <- 1                      # you failed

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



