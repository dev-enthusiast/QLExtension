#  Just a wrapper.  Will server the basis for the Rterm call.
#
#

source("H:/user/R/RMG/Energy/VaR/rmsys/Interactive/make.VaR.GUI.R")
source("H:/user/R/RMG/Energy/VaR/rmsys/Interactive/run.interactive.VaR.R")
make.VaR.GUI()

#=========================================================================
rm(list=ls())
options <- NULL
options$asOfDate <- as.Date("2007-01-18")
options$run$useLivePositions   <- "NO"
options$run$runIndividually    <- "NO"
options$run$extra.classFactors <- "gasseason"
options$run$is.overnight <- FALSE

RR <- matrix("", nrow=5, ncol=5, dimnames=list(1:5,
      c("weight", "portfolio", "book", "strategy", "trade")))
RR[1,"book"]  <- "housegas"
#RR[1, "strategy"] <- "STRUCT CATFISH"
#RR[2, "portfolio"] <- "storage"
#RR[3,"trade"] <- "OLS06M0"
#RR[4,"trade"] <- "NGS865Q"
#RR[3,"weight"] <- "-"
#RR[5,"portfolio"] <- "TRADING"
#RR[5,"book"] <- "EAST"

source("H:/user/R/RMG/Energy/VaR/rmsys/Interactive/run.interactive.VaR.R")
res <- run.interactive.VaR(RR, options)


odbcCloseAll()


###########################################################################

