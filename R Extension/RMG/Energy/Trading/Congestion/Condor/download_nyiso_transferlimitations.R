# download the nyiso transfer limitations
#
#




#################################################################
#################################################################
options(width=500)  # make some room for the output
require(CEGbase)

rLog(paste("Start proces at", Sys.time()))
returnCode <- 99    # succeed = 0

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.outages.R")

x <- try(download_transfer_limitations())

returnCode <- 0           # succeed for now 

q( status = returnCode )




