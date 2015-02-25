################################################################################
# This will run the simulations for all known curve groups one after the other.
# They are hard-coded out like this (as opposed to in a loop, apply, etc) 
# because dependencies exist that must be honored. 
#

################################################################################
# External Libraries
#

# This will error out because it's expecting the curveListPath environment
# variable to be set. We need to run it however to load the main function.
try(source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/SimulateCurves.R"))
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")

################################################################################
runSimulation <- function( dataFilePath )
{
  Sys.setenv( curveListPath = dataFilePath )
  SimulateCurves$main()
}


# Now that it's loaded, go ahead and run all the curve groups.
runSimulation(paste(PE$CONDOR_DIR, "RData/AllFuelReferenceCurves.RData", sep=""))

runSimulation(paste(PE$CONDOR_DIR, "RData/NGReferenceCurves.RData", sep=""))

runSimulation(paste(PE$CONDOR_DIR, "RData/ElecMarketReferenceCurves.RData", sep=""))

runSimulation(paste(PE$CONDOR_DIR, "RData/NGCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWCCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWECurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWICurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWJCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWKCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWLCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWMCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWNCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWOCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWPCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWQCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWSCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWXCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/PWYCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/CO2Curves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/COLCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/FCOCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/FO1Curves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/FRCCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/FRTCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/HOCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/IFOCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/LPGCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/NLNCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/NOXCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/SO2Curves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/UKNCurves.RData", sep=""))
 
runSimulation(paste(PE$CONDOR_DIR, "RData/WTICurves.RData", sep=""))
 