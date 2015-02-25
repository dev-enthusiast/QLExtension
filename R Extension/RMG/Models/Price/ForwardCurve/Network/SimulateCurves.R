################################################################################
# Initial Curve Loading Job For The PE Simulation Condor Run
#
cat("Running SimulateCurves.R...\n")

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages( c("SecDb", "MASS") ))
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")


################################################################################
# File Namespace
#
SimulateCurves = new.env(hash=TRUE)


################################################################################
# Read the curve list path environment variable to extract the curve group that
# this job will simulate. Once the curve list is loaded, pass it to the sim.all
# code written by Hao Wu.
#
SimulateCurves$main <- function()
{

  curveListPath = Sys.getenv("curveListPath")
  rLog("Trying to load file:", curveListPath)
  #loads a 'data' variable
  load(curveListPath)
  rLog("File Loaded.")

  if( nrow(data) != 0 )
  {

    asOfDate = overnightUtils.getAsOfDate()
  
    ## make FP.options
    rLog("Creating Forward Price Options")
    FP.options = ForwardPriceOptions$create(asOfDate)
    rLog("Options Created.")
  
    # 'data' was loaded above
    curveNames = as.character(data$curve.name)
    
    rLog("Running sim.all...")
    sim.all(curveNames, FP.options)

  } else
  {
    rError("No curves found in this data set!")
  }
  
  rLog("Simulation Complete.")
  
}


################################ MAIN EXECUTION ################################
.start = Sys.time()

SimulateCurves$main()

Sys.time() - .start

