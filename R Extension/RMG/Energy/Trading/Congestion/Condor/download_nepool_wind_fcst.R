# Download and archie the nepool wind forecast
# Tsdb symbol is: nepool_wind_fcst 
#
# Written by Adrian Dragulescu on 2014-05-05



##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)

returnCode <- 99   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_nepool_wind_fcst.R")
rLog("Start at ", as.character(Sys.time()))

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.wind.R")


#day <- as.Date("2014-04-28")
day <- Sys.Date()
ind <- 0
while ((.download_wind_forecast( day ) == 1) && (ind < 10))  {
  day <- Sys.Date() - ind
  ind <- ind + 1
}

X <- .load_wind_forecast( day )
.update_tsdb_symbols(X)

returnCode <- 0
rLog("Done at ", as.character(Sys.time()))


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


