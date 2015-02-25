#-------------------------------------------------------------------------------
# This is the non-interactive PE Calculator script used by Condor.
# It takes no arguments but a counterparty environment variable and outputs
# its results to a pdf picture file.

#-------------------------------------------------------------------------------
# External Libraries
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")

#-------------------------------------------------------------------------------
# File Namespace
PECalculatorCondor <- new.env(hash=TRUE)

#-------------------------------------------------------------------------------
# This expects the counterparty environment variable to be set
PECalculatorCondor$run <- function()
{
    setLogLevel(RLoggerLevels$LOG_DEBUG)
    PE$loadPEOptions()
                                                      
    run.name <- as.character(Sys.getenv("runName"))
    rLog("Calculating Potential Exposure For:", run.name)

    run          <- PEUtils$setParmsRun( run.name ) 
    positionData <- PEUtils$getPositionsForRunName( PE$options$asOfDate, run )
  
    if( nrow(positionData) == 0 )
    {
        rError("Unable to extract position data for run name:", run.name)
    }else
    {
        # In Seconds
        # FPC = 18, TXU = 96, MORGAN = 516, AEC = 20, GREYSTONE=30, JPMORGAN = 192
        runName <- gsub(" ", ".", run.name)
        PECalculator$run( PE$options$asOfDate, positionData, runName, 
                          showPlot=FALSE, writeToFile=TRUE )
    }
    rLog("\nDone.\n")
}

#-------------------------------------------------------------------------------
# MAIN EXECUTION
.start <- Sys.time()
#to test, uncomment below
#Sys.setenv(counterparty="SARACEN")

res <- try( PECalculatorCondor$run() )

if (class(res)=="try-error")
{
    cat("Failed Counterparty Calculation.\n")
}

Sys.time() - .start