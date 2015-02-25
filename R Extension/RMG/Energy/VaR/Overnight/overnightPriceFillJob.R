################################################################################
# Fill the market price files
#
# Last modified by John Scillieri on 01-May-2007
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
overnightPriceFillJob.main <- function()
{
    asOfDate <- overnightUtils.getAsOfDate()
    options  <- overnightUtils.loadOvernightParameters(asOfDate)

    rLog("Beginning Price Filling Job.")

    fill.mkt.price.files(options)

}

overnightPriceFillJob.main()

