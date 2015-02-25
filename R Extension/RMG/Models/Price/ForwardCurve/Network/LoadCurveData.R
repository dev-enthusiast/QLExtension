################################################################################
# Initial Curve Loading Job For The PE Simulation Condor Run
#
cat("Running LoadCurveData.R...\n")


################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages( c("SecDb", "MASS") ))
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")


################################################################################
# File Namespace
#
LoadCurveData = new.env(hash=TRUE)


################################################################################
LoadCurveData$main <- function()
{
  asOfDate = overnightUtils.getAsOfDate()

  ## make FP.options
  rLog("Creating Forward Price Options")
  FP.options = ForwardPriceOptions$create(asOfDate)
  rLog("Options Created.")
  
  rLog("Running load.AllCurves...")
  load.AllCurves(FP.options=FP.options)
  rLog("Done.")
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
LoadCurveData$main()
Sys.time() - .start

