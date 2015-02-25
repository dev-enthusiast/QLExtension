################################################################################
# Run the price calculations
#
# Last modified by John Scillieri on 01-May-2007
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
overnightPriceJob.main <- function()
{
    asOfDate <- overnightUtils.getAsOfDate()
    options  <- overnightUtils.loadOvernightParameters(asOfDate)
    
    PROCMON_DEPENDENCY = "power/SdbOraExport"
    
    while( Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE) != "Succeeded" )
    {
        rLog(PROCMON_DEPENDENCY, "Status:", 
                Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE))
        rLog("Sleeping for 15 mins...")
        Sys.sleep(900)   
    }
    
    rLog("Beginning Price Job.")
    get.one.day.prices(options$asOfDate, options)
    cPP <- get.corrected.prices(options)

    correct.hPrices.file(cPP, options)
}

overnightPriceJob.main()

