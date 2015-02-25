# Daily pull of CNE load signed
#
# Adrian Dragulescu, 2014-09-16




############################################################################
############################################################################
require(CEGbase)
require(reshape)
require(RODBC)
require(SecDb)


source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/previous_day_load_signed.R")

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/cne_previous_day_load_signed.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b", calendar="NYM"))

data <- .pull_data(asOfDate)
returnCode <- save_data( data )


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}




