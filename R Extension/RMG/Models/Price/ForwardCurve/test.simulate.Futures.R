#
#
#




MarketData <- options <- NULL
MarketData$asOfDate <- as.Date("2006-06-09")
MarketData$contract.dt <- seq(as.Date("2007-01-01"), by="month", length.out=12)
MarketData$pricing.dt  <- seq(as.Date("2006-07-01"), by="month", length.out=5)

MarketData$forward.prices <- matrix(c(60,7), nrow=2, ncol=12,
   dimnames=list(c("Power", "Gas"), format(MarketData$contract.dt, "%b%y")))
MarketData$forward.vols <- matrix(c(0.6,0.4), nrow=2, ncol=12,
   dimnames=list(c("Power", "Gas"), format(MarketData$contract.dt, "%b%y")))


MarketData$Rho <- matrix(c(1, 0.8, 0.8, 1), nrow=2, ncol=2)

source("C:/R/Work/Energy/ForwardCurve/simulate.Futures.R")
res <- simulate.Futures(MarketData, options)

matplot(res[1,1,,], type="l")

