###############################################################################
# Run the overnight position job 
#
# Last modified by John Scillieri on 12-14-2007
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.vcv.vcentral.r")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


###############################################################################
overnightPositionJob.main <- function()
{    
    asOfDate   <- overnightUtils.getAsOfDate()
    options = overnightUtils.loadOvernightParameters(asOfDate)
    
    PROCMON_DEPENDENCY = "vCentral/Prod/VCV_aggregate"
    
    while( Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE) != "Succeeded" )
    {
        rLog(PROCMON_DEPENDENCY, "Status:", 
                Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE))
        rLog("Sleeping for 15 mins...")
        Sys.sleep(900)   
    }
     
    QQ.all <- get.positions.from.vcv.vcentral(options)
    
    filename <- paste(options$save$datastore.dir, "Positions/vcv.positions.",
            options$asOfDate, ".RData", sep="")
    rLog("Saving", nrow(QQ.all), "results to file:", filename)
    save(QQ.all, file=filename)
    
    invisible(QQ.all) 
}

overnightPositionJob.main()

