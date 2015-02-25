###############################################################################
# CacheRaftData.R
#
# Author: e14600
#
memory.limit( 4095 )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/RaftInterface.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")

asOfDate = .getAsOfDate()
rLog("Caching Raft data for date:", as.character( asOfDate ) )

# Get the low level Raft exposure data
rLog( "Pulling Exposure Data..." )
raftExposureData = RaftInterface$getExposureData( asOfDate )

rLog( "Done." )
