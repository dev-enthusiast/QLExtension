###############################################################################
# overnightVegaJob.R
# 
# Pull the vega positions from vcentral for inclusion in the VaR calc later.
#
# Author: John Scillieri
# 
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/getVegaPositions.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


###############################################################################
overnightVegaJob.main <- function()
{
    asOfDate   <- overnightUtils.getAsOfDate()
    options = overnightUtils.loadOvernightParameters(asOfDate, FALSE)
    
    PROCMON_DEPENDENCY = "vCentral/Prod/VCV_aggregate"
    
    while( Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE) != "Succeeded" )
    {
        rLog(PROCMON_DEPENDENCY, "Status:", 
                Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE))
        rLog("Sleeping for 15 mins...")
        Sys.sleep(900)   
    }
    
    vegaPositions = getVegaPositions(asOfDate)
    
    # Replace the blended with daily for now.
    blendedVolRows = which(vegaPositions$vol.type=="B")
    vegaPositions[blendedVolRows,"vol.type"]="D"
    
    fileName = paste( options$save$datastore.dir, "Positions/vega.positions.",
            asOfDate, ".RData", sep="")
    
    rLog("Saving vegaPositions to:", fileName)
    save(vegaPositions, file=fileName)
    
    rLog("Done.")
}


##############################################################################
overnightVegaJob.main()

