# Demo script of how to combine a given set of calculators 
# with a portfolio to get the incremental VaR. 
#
# First version, Adrian Dragulescu 15-Oct-2007


rm(list=ls())
require(SecDb)
source("H:/user/R/RMG/Energy/VaR/Tools/utils.combine.VaR.R")

run.name <- ".DealReview.John.UI"     # put something meaningful

asOfDate <- as.Date("2007-10-12")
base.portfolio <- "nepool accrual portfolio"
IVaR.base <- load.base.portfolio.IVaR(asOfDate, base.portfolio)

#------------------------------------------------------------
group  <- "CPS Trading Group"
trader <- "E13927"
all.calcs <- calculator.list(group, trader)

ind <- grep("UI", all.calcs)  # do some subsetting if necessary
calc.names <- all.calcs[ind]
QQ <- calculator.get.deltas(group, trader, calc.names)

source("H:/user/R/RMG/Energy/VaR/Tools/utils.combine.VaR.R")
aux <- make.IVaR.from.positions(QQ, asOfDate, run.name)
IVaR.calc <- aux[[1]]
options   <- aux[[2]]
run       <- aux[[3]]

#------------------------------------------------------------
# Combine the IVaR's and write the table and reports
#
IVaR <- NULL
IVaR$Changes  <- rbind(IVaR.base, IVaR.calc$Changes)
class.Factors <- make.class.Factors(IVaR$Changes[,1:4], run, options)

PnL <- calc.PnL.by.factor(IVaR, run, class.Factors, options)
report.VaR(PnL, run, options)

