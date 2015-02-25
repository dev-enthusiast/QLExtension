###############################################################################
# TickerMain.R
#
# Author: e14600
#
source("H:/user/R/RMG/Energy/Trading/Controls/Ticker/TickerUtils.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")


################################################################################
# File Namespace
#
TickerMain = new.env(hash=TRUE)


################################################################################
TickerMain$getDeltaPositions <- function()
{   
    deltaPositions = LiveData$cpsprodPositions()
    names(deltaPositions) = c("book", "curve.name", "contract.dt", "position")
    
    return( deltaPositions )
}


################################################################################
TickerMain$getVegaPositions <- function( asOfDate )
{   
    rLog("Loading current vega positions...")
    vegaDataFile = paste("//ceg/cershare/All/Risk/Data/VaR/prod/",
            "Positions/sas.vega.positions.", 
            as.character(asOfDate), ".RData", sep="")
    vegaPositions = varLoad( "vegaPositions", file=vegaDataFile )  
    vegaPositions$vega = vegaPositions$vega * 100
    
    names(vegaPositions) = c("book", "curve.name", "contract.dt", 
            "vol.type", "position")
    
    return( vegaPositions )
}


################################################################################
# Load the approprate vol/price dataframe and combine it with the position data.
# From there, calculate the PNL changes to be used in the var calculation later.
#
TickerMain$.getPNLChanges <- function( positionData, variableToLoad )
{
    rLog("Combining positions with", variableToLoad, "...")
    priceDataFile = paste(DATA_DIR, "/", variableToLoad, ".RData", sep="")
    allData = varLoad( variableToLoad, file=priceDataFile )
    mergedData = merge(positionData, allData) 
    
    rLog("Calculating P&L changes from merged data...")
    pnlChange = TickerUtils$getPNLChanges( mergedData, 
            mergedData$position )
    
    dataForVaR = cbind(mergedData$book, pnlChange)
    names(dataForVaR)[1] = "book"
    
    return(dataForVaR)
}


################################################################################
# Using the dataForVaR matrix, calculate var for every portfolio in the list.
# 
# The dataForVaR matrix must be P&L changes in dollars and have the associated
# book as a column. 
#
TickerMain$getVaRData <- function( dataForVaR, portfolioList, asOfDate)
{
    varValues = sapply( portfolioList, TickerUtils$varForPortfolio, 
            dataForVaR, asOfDate )
    
    varData = data.frame( portfolio = portfolioList, value = varValues )
    
    return(varData) 
}


################################################################################
# Take the four var dataframes and combine them into the approprate table to 
# be read by the ticker app.
#
TickerMain$generateFinalTable <- function( deltaVaR, vegaVaR, startingVaR, totalVaR )
{
    finalData = merge(deltaVaR, vegaVaR, by="portfolio")
    finalData = merge(finalData, totalVaR, by="portfolio")
    finalData = merge(finalData, startingVaR, by="portfolio")
    names(finalData) = c( "portfolio", "delta", "vega", "total", "start" )
    
    # do your percent change calulation
    finalData$percent = (finalData$total - finalData$start) / finalData$start
    finalData$percent = finalData$percent * 100
    finalData$percent[ which(is.nan(finalData$percent)) ] = 0
    finalData$percent = prettyNum( finalData$percent, digits=2, nsmall=2 )
    finalData$percent = paste( finalData$percent, "%", sep="")
    
    # Make it all look nice
    finalData$start = NULL
    finalData$delta = sapply( finalData$delta, dollar )
    finalData$vega = sapply( finalData$vega, dollar )
    finalData$total = sapply( finalData$total, dollar ) 
    
    names(finalData) = c( "Portfolio", "Delta VaR", "Vega VaR", 
            "Total VaR", "% Change" )
    
    return( finalData )
}


################################################################################
# This is the main function of the ticker code. It pulls the delta & vega 
# positions, creates the appropriate PNL change matrices, and calculates
# VaR for each one. It then writes to the file to be read by the ticker app. 
#
TickerMain$update <- function()
{  
    options(width=300)
    memory.limit(3 * 1024)
    
    asOfDate = TickerUtils$getAsOfDate() 
    
    deltaPositions = TickerMain$getDeltaPositions()
    deltaPNLChanges = TickerMain$.getPNLChanges( deltaPositions, "allPriceData" )
    
    vegaPNLChanges = varLoad( "vegaPNLChanges", VEGA_PNL_FILE, quiet=TRUE )
    if( is.null(vegaPNLChanges) )
    {
        vegaPositions = TickerMain$getVegaPositions( asOfDate )
        vegaPNLChanges = TickerMain$.getPNLChanges( vegaPositions, "allVolData" )
        
        rLog("Saving vegaPNLChanges to:", VEGA_PNL_FILE)
        save( vegaPNLChanges, file=VEGA_PNL_FILE )
    }
    
    # Ensure all the columns line up. We could have a situation where there
    # are more days in the price matrix vs the vol matrix, etc.
    colsToKeep = intersect( names(deltaPNLChanges), names(vegaPNLChanges) )
    deltaPNLChanges = deltaPNLChanges[ , colsToKeep ]
    vegaPNLChanges = vegaPNLChanges[ , colsToKeep ]
    
    rLog("Creating Total PnL Change Set...")
    totalPNLChanges = rbind(deltaPNLChanges, vegaPNLChanges)
    
    startingPNLChanges = varLoad( "startingPNLChanges", STARTING_PNL_FILE )
    if( is.null(startingPNLChanges) )
    {
        rLog("Saving Starting PnL Change Set")
        startingPNLChanges = totalPNLChanges
        save( startingPNLChanges, file=STARTING_PNL_FILE )
    } 
    
    rLog("Extracting all requested portfolios...")
    portfolioList = TickerUtils$getRequestedPortfolios()
    
    rLog("\nDelta VaRs...")
    deltaVaR = TickerMain$getVaRData( deltaPNLChanges, portfolioList, asOfDate )
    
    rLog("\nVega VaRs...")
    vegaVaR = TickerMain$getVaRData( vegaPNLChanges, portfolioList, asOfDate )
    
    rLog("\nStarting Total VaRs...")
    startingVaR = TickerMain$getVaRData( startingPNLChanges, portfolioList, asOfDate )
    
    rLog("\nCurrent Total VaRs...")
    totalVaR = TickerMain$getVaRData( totalPNLChanges, portfolioList, asOfDate )
    
    rLog("Merging Data Into Final Set...")
    finalData = TickerMain$generateFinalTable( deltaVaR, vegaVaR, startingVaR, totalVaR )
  
    rLog("Saving File...")
    outputFile = paste( DATA_DIR, "/", "VaRData.csv", sep="" )
    write.csv(finalData, file=outputFile, row.names=FALSE)
    
    rLog("Final Result Set:\n")
    print(finalData)
}


################################################################################
start = Sys.time()
TickerMain$update()
print(Sys.time()-start)
