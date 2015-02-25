# Sets up the required options for the overnight run
#
# Last modified by John Scillieri on 01-May-2007
overnightSetupJob = new.env(hash=TRUE)

overnightSetupJob$main <- function()
{
  rm(list=ls())
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  
  options = new.env(hash=TRUE)
  options$asOfDate   <- as.Date(overnightUtils.getAsOfDate())
  options$keep.log   <- TRUE
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/set.parms.overnight.R")
  options <- set.parms.overnight(options)
  
  get.spot.fx.rates(options)

  get.curve.info(options)
  
}

overnightSetupJob$main()

