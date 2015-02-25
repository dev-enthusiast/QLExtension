
################################################################################
# Test the AggregateCurves.R script.
#
# Open an RGUI on one of the R:\PotentialExposure\Condor\RData\ files.


require("SecDb")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")

asOfDate = as.Date("2008-02-18")

rLog("Creating Forward Price Options")
FP.options = ForwardPriceOptions$create(asOfDate)
rLog("Options Created.")

# 'data' was loaded above
cnames = as.character(data$curve.name)

# get the curve pedigree
allcurves <- get.curve.pedigree(cnames, FP.options)

source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
aggregate.packets(allcurves, FP.options)


################################################################################
# Test the SimulateCurves.R script.
#
# Open an RGUI on one of the R:\PotentialExposure\Condor\RData\ files.


require(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")

Sys.setenv(FP_NO_PACKETS="1")
Sys.setenv(FP_PACKET_NO="1")
asOfDate = as.Date("2008-02-14")

rLog("Creating Forward Price Options")
FP.options = ForwardPriceOptions$create(asOfDate)
rLog("Options Created.")
FP.options$nsim <- 10

# 'data' was loaded above
curvenames = as.character(data$curve.name)
3
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
sim.all(curvenames, FP.options)
 

################################################################################
# Export one price filehash curve to csv
#

curve.name <- "commod ng hscbea physical"

