################################################################################
# Create the market price files
#
# Last modified by John Scillieri on 14-Mar-2007
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
overnightPriceMarketJob.main <- function()
{
    asOfDate <- overnightUtils.getAsOfDate()
    options  <- overnightUtils.loadOvernightParameters(asOfDate)

    make.mkt.price.files(options)
}

overnightPriceMarketJob.main()

