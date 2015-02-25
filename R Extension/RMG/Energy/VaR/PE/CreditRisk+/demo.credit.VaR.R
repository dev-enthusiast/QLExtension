# Example on how to run the Credit VaR
#
# 

rm(list=ls())
asOfDate <- ("2008-02-13")

source("H:/user/R/RMG/Energy/VaR/PE/CreditRisk+/calc.loss.distribution.R")

CVaR$main(asOfDate)


## sort(table(EX$cparty))

## plot(sort(EX$exposure))


