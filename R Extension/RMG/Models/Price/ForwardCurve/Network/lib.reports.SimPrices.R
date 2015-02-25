# A library of battery tests on the simulated prices.
#
#
# Initial version by Adrian Dragulescu on 11-Jan-2008

################################################################################
library(filehash)


source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Utilities/Database/R/read.forward.price.sims.R")

################################################################################
# File Namespace
#
LibReports = new.env(hash=TRUE)

#########################################################################
# Calculate the standard deviation of simulated prices by curve,
# pricing.dt, contract.dt. 
#
LibReports$calculate.sd.SimPrices <- function(){

  rLog("Calculate standard deviation of simulated prices.")
  asOfDate <- as.Date(Sys.getenv("asOfDate"))
  rLog("As of date = ", as.character(asOfDate))

  FP.options <- ForwardPriceOptions$create(asOfDate)
  fhPath <- paste("R:/SimPrices/Network/", asOfDate, "/overnight/result",
                  sep="")
  sPP.env <- .getFileHashEnv(fhPath)

  uCurves <- ls(sPP.env)
  SD <- array(NA, dim=c(length(uCurves), length(FP.options$sim.days),
                    length(FP.options$contract.dts)))
  dimnames(SD) <- list(toupper(uCurves), as.character(FP.options$sim.days),
                       as.character(FP.options$contract.dts))
  for (ind.c in seq_along(uCurves)){
    rLog("Working on", uCurves[ind.c])
    aux <- sPP.env[[uCurves[ind.c]]]
    SD[ind.c,,] <- apply(aux, c(1,2), sd, na.rm=T)
  }
  
  
  filename <- paste("S:/All/Risk/Reports/Simulated.Prices/sd.", asOfDate,
                    ".RData", sep="")
  rLog("Saving results to :", filename)
  save(SD, file=filename)
  rLog("Done. Exiting.")
  
}


################################ MAIN EXECUTION ################################
.start = Sys.time()

#Sys.setenv(asOfDate="2008-01-10")
LibReports$calculate.sd.SimPrices()

Sys.time() - .start
