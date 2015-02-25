# Automate the calculation of NEPOOL ancillaries actuals 
#
#


##################################################################
##################################################################

options(width=400)  # make some room for the output

require(CEGbase)
require(plyr)
require(reshape)
require(xlsx)
  
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.units.R")

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_ncpc_units_actuals.R")
rLog("Start at ", as.character(Sys.time()))

returnCode <- 0   # 0 means success

try(.make_monthly_report(month=prevMonth()))

try(.make_monthly_report(month=currentMonth()))

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



