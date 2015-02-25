


#########################################################
#########################################################
#

library(CEGbase)
library(RUnit)
library(SecDb)
library(plyr)
library(CRR)

### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/CRR_Auction.R")
### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/Path.R")
### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/get_settle_price.R")
### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/get_settle_price_hourly.R")
### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/get_clearing_price.R")
### source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/get_payoff.R")




source("H:/user/R/RMG/Utilities/Interfaces/CRR/test/test.CRR_Auction.R")
test.CRR_Auction()

source("H:/user/R/RMG/Utilities/Interfaces/CRR/test/test.get_settle_price.R")
test.get_settle_price()

source("H:/user/R/RMG/Utilities/Interfaces/CRR/test/test.get_clearing_price.R")
test.get_clearing_price()

source("H:/user/R/RMG/Utilities/Interfaces/CRR/test/test.get_payoff.R")
test.get_payoff()










## runTestFile(file.path(.path.package(package="FTR2"),
##                       "test/run_tests.R"))

