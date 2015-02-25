################################################################################
# Aggregate the curves from a simulation job that got split
# It will also copy the identical curves to their parents. 
#
cat("Running AggregateCurves.R...\n")

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
AggregateCurves = new.env(hash=TRUE)


################################################################################
# Read the curve list path environment variable to extract the curve group that
# this job will simulate. Once the curve list is loaded, pass it to the sim.all
# code written by Hao Wu.
#
AggregateCurves$main <- function()
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
 
    # get the curve pedigree
    allcurves <- get.curve.pedigree(curveNames, FP.options)

    rLog("Running aggregate.packets...")
    aggregate.packets(allcurves, FP.options)
  
    aux <- data.frame(na.omit(allcurves[,c("SecDbCurve", "Identical")]))
    if (nrow(aux)>0){
      rLog("Copy the identicals...")
      copy.Identicals(aux, FP.options)
    }  
    
  } else
  {
    rError("No curves found in this data set!")
  }
  
  rLog("Simulation Complete.")
  
}


################################ MAIN EXECUTION ################################
.start = Sys.time()

AggregateCurves$main()

Sys.time() - .start

