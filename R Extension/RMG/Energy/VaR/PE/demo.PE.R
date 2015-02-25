# Testing script for the PE calculation. 
#

rm(list=ls())
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")

setLogLevel(RLoggerLevels$LOG_DEBUG)

#Sys.setenv(runName = "overnight")
#Sys.setenv(runName = "test.JK")
#Sys.setenv(runName = "RMG Freight Dispensation Portfolio")
Sys.setenv(runName = "ADARO")
#Sys.setenv(runName = "TANGSHAN")
asOfDate <- as.Date("2008-03-12")

# you can overwrite nightly options here
PE$loadPEOptions(asOfDate)

run.name <- as.character(Sys.getenv("runName"))
run <- PEUtils$setParmsRun( run.name )

QQ <- PEUtils$getPositionsForRunName(asOfDate, run)
{
runName <- gsub(" ", ".", run.name)

#source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
PECalculator$run(PE$options$asOfDate, QQ, runName, 
    showPlot=TRUE, writeToFile=TRUE)
}

curve.names <- "COMMOD FCO INDOSBIT PHYSICAL"
curve.names <- unique(QQ$curve.name)
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
for (ind.c in seq_along(curve.names))
  plot.OneCurve(curve.names[ind.c], PE$options$fp, "pdf")




## QQ <- PEUtils$.makePositionsFromRunName(PE)

## # In Seconds
## # FPC = 18, TXU = 96, MORGAN = 516, AEC = 20, GREYSTONE=30, JPMORGAN = 192
## # counterparty <- "FPC"
## # testData <- PEUtils$getPositionsForCounterparty(counterparty)

## QQ.1 <- subset(QQ, curve.name=="COMMOD PWY 5X16 WEST PHYSICAL") 
## QQ.1 <- QQ.1[1:14, ]

## t1 <- Sys.time()
## #source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
## PECalculator$run(PE$options$asOfDate, QQ.1, PE$options$run.name, 
##   showPlot=TRUE, writeToFile=TRUE, isInteractive=FALSE)
## t2 <- Sys.time()
## (t2-t1)


#====================================================================
counterparty <- "test"
testData <- data.frame(curve.name  = "COMMOD NG EXCHANGE",
  contract.dt = seq(as.Date("2008-01-01"), by="month", length.out=12),
  position =1)

options <- NULL 
options$calc$hdays <- as.Date("2007-08-31")
source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
PP <- get.prices.from.R(testData[,1:2], options)
testData <- merge(testData, PP)
testData$curve.name <- as.character(testData$curve.name)
names(testData)[4] <- "F0"


#######################################################################
# EXAMPLE: backtest PE
#
#

start.dt <- as.Date("2007-01-01")
run.name <- "backtest"
QQ <- data.frame(curve.name  = "COMMOD NG EXCHANGE",
  contract.dt = seq(as.Date("2008-01-01"), by="month", length.out=12),
  position = 10000, stringsAsFactors=FALSE)
options <- NULL

source("H:/user/R/RMG/Energy/VaR/PE/backcast.PE.R")
backcast.PE(start.dt, run.name, QQ)




#====================================================================
reportsDir <- "R:/PotentialExposure/2007-10-02/overnight/RData/"
PECalculator$.aggregateResults("2007-10-02")


#====================================================================
options <- NULL; options$calc$hdays <- Sys.Date()
source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
PP <- get.prices.from.R(testData, options)
