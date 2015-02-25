################################################################################
# An 'interactive' potential exposure calculator.
#
# It can be run by passing an asOfDate and either:
#    a generic position data frame with corresponding run name or 
#    a counterparty name
# Ex:
#    InteractivePECalc$byCounterparty("JPMORGAN", Sys.Date()-1)
#      or
#    InteractivePECalc$withPositionData( positionDataFrame, "run #1", 
#      as.Date("2007-07-31") )
#
# See the function comments below for more information
# 

################################################################################
# External Libraries
#
library(RODBC)
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.positions.R")
#source("H:/user/R/RMG/Energy/VaR/PE/Condor/CleanSimulationData.R")
#source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
#source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")


################################################################################
# File Namespace
#
InteractivePECalc = new.env(hash=TRUE)


################################################################################
# This is a generic version of the interactive calculator that takes a 
# set of positions and calculates the exposure for those positions.  
#
# The position data frame must have columns "curve.name", "contract.dt", 
# "position" and "FO" (current price). 
# 
# It operates by simulating prices for those positions only and then
# calculating potential exposure for that position set over time.
#
# Data is saved in the 'interactive' folders of the respective simulation
# and potential exposure parent folders.
# 
InteractivePECalc$withPositionData <- function( positionData, run, 
  asOfDate, ... )
{
    #Create the PE Options here to make sure they're in a higher scope.
    PE$loadPEOptions(asOfDate, isInteractive=TRUE)

##     # Where the simulation data is stored
##     PE$CURVE_DATA_DIR = paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
##       "overnight/result/", sep="")

    runName <- gsub(" ", "_", run$run.name)  # pdf plot doesn't like spaces
    
    # Run the PE Calculator in interactive mode and display the results
    PECalculator$run( asOfDate, positionData, runName,
      showPlot=TRUE, writeToFile=TRUE, isInteractive=TRUE )

    if (!is.null(run$extras) && run$extras$plots){
      loptions <- as.list(PE$options$fp)
      loptions$report.dir <- paste(PE$options$save$reports.dir, runName, "/", sep="")
      curvenames <- unique(positionData$curve.name)
      for (ind in 1:length(curvenames))
        plot.OneCurve(curvenames[ind], loptions, "pdf")
    }
    
}


################################################################################
# This code is intended to operate by reading the current positions for a
# given counterparty and running the generic PE calculation code above.
#
InteractivePECalc$byCounterparty <- function( asOfDate, run )
{
  setLogLevel(RLoggerLevels$LOG_DEBUG)

  rLog("Calculating Potential Exposure for:", run$counterparty)

  # Get the CP's position list
  positionData = PEUtils$getPositionsForCounterparty(run$counterparty)
  if( nrow(positionData) == 0 )
  {
    rError("Unable to extract position data for counterparty:", counterparty)
  } else
  {
    InteractivePECalc$withPositionData(positionData, runName=counterparty,
      asOfDate )
  }

  rLog("\nDone.\n")

  # Need to update clean code for {overnight,interactive}
}

################################################################################
# This code is intended to operate by reading the current positions for a
# given counterparty and running the generic PE calculation code above.
#
InteractivePECalc$makePositionsFromCalcs <- function( asOfDate, group, trader,
                                                     calc.names)
{
  QQ <- calculator.get.deltas(group, trader, calc.names)
  # get the asOfDate prices
  QQ <- subset(QQ, delta != 0)

  dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/Prices/"
  filename <- paste(dir, "hPrices.", asOfDate, ".RData", sep="")
  load(filename)
  QQ <- merge(QQ, hP, all.x=T)
  names(QQ)[3:4] <- c("position", "F0")

  return(QQ)
}

################################################################################
# This code is intended to operate by reading the current positions for a
# given portfolio.  Check the overnight.VaR.xls setup for more details.  
#   run$run.name <- BLACKDIAMD
#
InteractivePECalc$makePositionsFromPortfolios <- function( asOfDate, run )
{
  loptions <- NULL
  loptions$run <- run
  loptions$save$datastore.dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/"
  
  run.all <-  get.portfolio.book(loptions)
  loptions$run$specification <- subset(run.all,
    RUN_NAME == toupper(loptions$run$run.name))
  if (nrow(loptions$run$specification)==0){
    rLog("Could not find the run.name in the portfolio hierarchy.")
  }

  # from VaR/Base/aggregate.run.positions.R
  QQ <- .getDeltas(loptions$run, loptions)  
  QQ$vol.type <- NULL
  
  # add current day prices
  filename <- paste(loptions$save$datastore.dir, "Prices/hPrices.",
    loptions$asOfDate, ".RData", sep="")
  load(filename)
  QQ <- merge(QQ, hP, all.x=T)
  names(QQ)[4] <- "F0"           # asOfDate forward prices  
  
  return(QQ)
}


############################## SAMPLE EXECUTION ################################
#.start = Sys.time()
#counterparty = "TXU"
#asOfDate = Sys.Date()-3
#InteractivePECalc$byCounterparty(counterparty, asOfDate)
#Sys.time() - .start
#

################################################################################
##    OLD STUFF
################################################################################
##
##     # Set up the FP.options for interactive mode
##     rLog("Creating Forward Price Options")
##     FP.options = ForwardPriceOptions$create(asOfDate, isInteractive=TRUE)
##     FP.options$nsim <- 100
##     rLog("Options Created.")

##     # We need to clean the interactive directories and load the price data
##     # if it hasn't been done already
##     CleanSimulationData$main(isInteractive=TRUE)
    
##     #priceDataDir = FP.options$swapdata.dir
##     rLog("Running load.AllCurves...")
##     load.AllCurves(FP.options=FP.options)
##     rLog("Done.")

##     # Simulate the CP's positions
##     rLog("Running sim.all...")
##     curveNames = as.character( unique(positionData$curve.name) )
##     sim.all(curveNames, FP.options)
##     rLog("Simulation Complete.")

