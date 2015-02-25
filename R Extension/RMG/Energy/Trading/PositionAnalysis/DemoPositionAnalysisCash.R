rm(list = ls(all=TRUE))
require(RODBC)
require(lattice)
dyn.load("H:/user/R/RMG/Utilities/Database/LIM/Source/RMGxmimR.dll")
source("H:/user/R/RMG/Utilities/Database/LIM/read.LIM.R")
source("H:/user/R/RMG/Utilities/read.deltas.from.CPSPROD.R")
source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")
source("H:/user/R/RMG/TimeFunctions/bucket.ts.R")
source("H:/user/R/RMG/TimeFunctions/countBusinessDate.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/ReadDataCurveTableB.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/futurePriceTableUpdate.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/plotPositionsA.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/plotPositionsC.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/PositionAnalysisA.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/MonthlyAverage.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/RatioAnalysis.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/dataAUX.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/DOIa.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/CashFutDist.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/CashFutDistA.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/formatFuturePriceTable.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/PositionAnalysisBT.R")
source("H:/user/R/RMG/Energy/Trading/PositionAnalysis/PositionAnalysisCash.R")
source("H:/user/R/RMG/Finance/Heston/FitHeston.R")   
source("H:/user/R/RMG/Finance/Heston/qHeston.R")
options <- NULL
options$dirLimSymTable    <- "H:/user/R/RMG/Energy/Trading/PositionAnalysis/LimCommodCurveTable.xls"
options$asOfDate          <- as.Date("2007-07-03")    # data to read positions (can not be future/weekend)
options$endDate           <- options$asOfDate         # last data point to read historic data
options$startDate         <- as.Date("2002-01-01")    # first data point to read historic data
options$MOI               <- as.Date("2007-08-01")
options$DT <- 5
options$percentile <- 5 
options$showPlots = FALSE # show plots
options$RefineTimeInterval = FALSE 
options$flagValue = FALSE
options$nSim <- 1000
options$nFieldLimit <- 800
#options$pricing.dt.start  <- as.Date("2006-04-01")    # forward prices at the initial day in the past
#options$pricing.dt.end    <- options$asOfDate         # forward prices at the final day in the past
#options$contract.dt.start <- as.Date("2006-12-01")          # forward prices future initial date range
#options$contract.dt.end   <- as.Date("2007-02-01")    # forward prices future final date range
#options$DOI               <- as.Date("2006-12-01")    # here we are interested mostly in the month of interest
options$plot              <- 0       # plot some results if 1
options$ratioAnalysis     <- 1       # do ratio analysis if 1
options$plotPositions     <- 0
options$type              <- "raw"
options$units             <- "hour"
options$dir               <- "S:/All/Risk/Analysis/Portfolio.Analysis"
# block to change
options$AirportName       <- "JFK"
options$AirportFile       <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.", options$AirportName, ".Rdata", sep = "")
options$MARKET_SYMBOL     <- "PWY"
options$books             <- c("NY1ED", "NYCND", "NYHDG", "NYICAP", "NYICAPHS", "NYISOTCC", "NYSTRDL",
                               "NYTCC1", "NYTRADE", "NYVRT")
options$trader            <- "Joe_Kirkpatrick" 
# end block to change
options$dirRoot <- paste(options$dir, "/", options$trader, sep = "")
options$dir <- paste(options$dirRoot, "/asof_", format(options$asOfDate, "%d%b%Y"), sep = "")
dir.create(options$dir,  recursive = TRUE, showWarnings = FALSE)

PositionAnalysisCash(options)
#PositionAnalysisBT(options)

#options$MARKET_SYMBOL     <- "PWO"
#options$AirportName       <- "DFW"
#options$AirportFile       <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.", options$AirportName, ".Rdata", sep = "")
#options$trader            <- "Greg_Forero"        
#options$books             <- c("FORCASH", "FORERCT", "FORERCTS")
# PositionAnalysis(options)

