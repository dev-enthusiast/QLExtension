# Automate the calculation of NEPOOL ancillaries actuals 
#
#


##################################################################
##################################################################

options(width=400)  # make some room for the output
require(CEGbase)
require(SecDb)
require(zoo)
require(reshape)
require(PM)
require(xlsx)
  
source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")  
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.actuals.ancillaries.R")


rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_ancillaries_actuals.R")
rLog("Start at ", as.character(Sys.time()))

returnCode <- 0   # 0 means success

res <- try(.calculate_actuals( prevMonth() ))
if (class(res) != "try-error") 
  .report_actuals( res )

res <- try(.calculate_actuals(currentMonth() ))
if (class(res) != "try-error") 
  .report_actuals( res )
  

rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}



