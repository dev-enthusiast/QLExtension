###############################################################################
# CacheVCentralData.R
#
# Author: e14600
#
memory.limit( 4095 )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/PositionEngineVCentral.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")

asOfDate = .getAsOfDate()
rLog("Caching VCentral data for date:", as.character( asOfDate ) )

# Get the low level Position data
positionEngine = PositionEngineVCentral

# Create a table of all positions by major commodity group
allPositions = positionEngine$getAllPositionTable( asOfDate )

rLog( "Done." )
