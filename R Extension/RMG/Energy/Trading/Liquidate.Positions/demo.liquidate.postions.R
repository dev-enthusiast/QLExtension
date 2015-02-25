

rm(list=ls())
LQ.options <- NULL
LQ.options$asOfDate <- as.Date("2008-01-15")
LQ.options$focus.month <- as.Date("2008-02-01")
LQ.options$use.hist.years  <- (2000:2007)

LQ.options$run$run.name <- "NYTRADE"   # put book, portfolio name, etc.

source("H:/user/R/RMG/Energy/Trading/Liquidate.Positions/liquidate.Positions.R")
QQ  <- LQ.Positions$main(LQ.options)



