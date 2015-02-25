# Code used for calculating hedge cost for pricing
#
#
# Written by Chelsea Yeager on 16-Apr-2012

#------------------------------------------------------------
# MAIN
#

require(CEGbase)
options(width=150)      
require(RODBC)
require(reshape)
require(SecDb)
require(PM)
require(methods)



strHeader = "H:/user/R/RMG/Energy/PM/ERCOT/"
source(paste(strHeader,"rt/lib.hedge_cost.r",paste="",sep=""))
rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0



#------------------------------------------------------------
# Collect and upload data
#

    ## Run Hedge Cost File
      strDays <- 16
      Calculate.HedgeCost(strDays)       

    ## Log finish
      rLog(paste("Done at ", Sys.time()))


q(status=returnCode)
