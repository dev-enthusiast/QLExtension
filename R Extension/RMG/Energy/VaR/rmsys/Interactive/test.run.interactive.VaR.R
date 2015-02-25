#  Just a wrapper.  Will server the basis for the Rterm call.
#
#

source("H:/user/R/RMG/Energy/VaR/rmsys/Interactive/make.VaR.GUI.R")
source("H:/user/R/RMG/Energy/VaR/rmsys/Interactive/run.interactive.VaR.R")
make.VaR.GUI()

options <- NULL
options$asOfDate <- as.Date("2006-12-30")
options$useLivePositions   <- "YES"
options$runIndividually    <- "YES"
options$extra.classFactors <- "gasseason"

RR <- matrix("", nrow=1, ncol=5, dimnames=list(1:no.portfolios,
      c("weight", "portfolio", "book", "strategy", "trade")))
RR[1,"portfolio"] <- "West Gas Transport Portfolio"
