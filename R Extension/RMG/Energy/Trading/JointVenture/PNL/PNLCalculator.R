###############################################################################
# PNLCalculator.R
#
# Author: e14600
#
library(SecDb)
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/Database/SAS/SASInterface.R")
source("H:/user/R/RMG/Energy/Trading/JointVenture/PNL/SecDbPrices.R")
source("H:/user/R/RMG/Energy/Trading/JointVenture/PNL/NewTrades.R")


################################################################################
# File Namespace
#
PNLCalculator = new.env(hash=TRUE)


################################################################################
# Cached Values
PNLCalculator$.currentPositions = NULL


################################################################################
# Print a nice summary of the PQ 
#
PNLCalculator$pnlData <- function( portfolio, asOfDate ) 
{  
    pqPrevious = NULL
    pqNewTrade = NULL 
    
    previousPositions = .positionsForPortfolio( portfolio, asOfDate ) 
    if( nrow( previousPositions ) != 0 )
    {
        pqPrevious = .pqDataFromPositions( previousPositions, asOfDate ) 
    }
    
#    newTradePositions = .newTradePositions( portfolio, asOfDate )
#    if( nrow( newTradePositions ) != 0 )
#    {
#        pqNewTrade = .pqDataFromPositions( newTradePositions, asOfDate ) 
#    }
    
    invisible( list( pqPrevious=pqPrevious, pqNewTrade=pqNewTrade ) )
}


################################################################################
# Get the position data for a given portfolio
#
.positionsForPortfolio <- function( portfolio, asOfDate ) 
{
    asOfDate = as.Date( secdb.dateRuleApply( asOfDate, "-1b" ) )
    
    if( is.null( PNLCalculator$.currentPositions ) )
    {
        rLog("Retrieving Current Delta Positions for", 
                as.character( asOfDate ), "...")
        PNLCalculator$.currentPositions = SASInterface$getDeltas( asOfDate ) 
    }
    
    bookList = names( secdb.getValueType( portfolio, "Nodes" )$Book )
    portfolioPositions = subset( PNLCalculator$.currentPositions, book %in% bookList )
    portfolioPositions$book = NULL
    portfolioPositions$vol.type = NULL
    
    portfolioPositions = cast( portfolioPositions, 
            curve.name + contract.dt ~ ., sum )
    
    names( portfolioPositions ) = c( "curve.name", "contract.dt", "position" )

    return( portfolioPositions )
}


################################################################################
# Get the new trade data for a given portfolio
#
.newTradePositions <- function( portfolio, asOfDate ) 
{
    allTradeData = NewTrades$forDate( asOfDate )
    
    bookList = names( secdb.getValueType( portfolio, "Nodes" )$Book )
    portfolioPositions = subset( allTradeData, book %in% bookList )
    portfolioPositions$book = NULL
    
    names( portfolioPositions ) = c( "curve.name", "contract.dt", "position", "price.prev" )
    
    return( portfolioPositions )
}


################################################################################
# Get the current pnl data from a given set of positions
#
.pqDataFromPositions <- function( positions, asOfDate ) 
{
    uniquePositions = unique( positions[, c( "curve.name", "contract.dt" ) ] )
    
    rLog("Retrieving SecDb Prices for", as.character( asOfDate ), "...")
    newPrices = SecDbPrices$forCurves( uniquePositions, asOfDate, current=TRUE )
    
    if( "price.prev" %in% names( positions ) == FALSE )
    {
        previousPrices = SecDbPrices$forCurves( uniquePositions, asOfDate, current=FALSE )
        priceData = merge( newPrices, previousPrices )
    
    } else
    {
        priceData = newPrices
    }

    pqData = merge( positions, priceData )
    
    pqData$change = pqData$price - pqData$price.prev
    pqData$pnl = pqData$change * pqData$position
    
    pqData = .cleanupPQData( pqData )
    
    return( pqData )
}


################################################################################
.cleanupPQData <- function( pqData )
{   
    # Round & remove the zeros from the pnl data... not interesting
    pqData$pnl = round( pqData$pnl )
    pqData = pqData[ which( pqData$pnl != 0 ), ]
    
    pqData = pqData[ order( abs( pqData$pnl ), decreasing = TRUE ), ]
    row.names(pqData) = NULL
    
    pqData$position = round( pqData$position )
    pqData$price = round( pqData$price, 4 )
    
    # Reorganize the data.frame 
    pqData = pqData[, c( "curve.name", "contract.dt", 
                    "position", "price", "change", "pnl" ) ]
    
    return( pqData )
}


################################################################################
# Print a nice summary of the PQ 
#
PNLCalculator$printStatusReport <- function( portfolio, pqData, newTradeData=NULL ) 
{   
    rLog( "\n", portfolio, "-\n" )
    totalPNL = sum( pqData$pnl ) + sum( newTradeData$pnl )
    rLog( "Approximate Total Daily PnL:", dollar( totalPNL ) )
    rLog( "  From DeltaP * Q:", dollar( sum( pqData$pnl ) ) )    
    rLog( "  From new trades:", dollar( sum( newTradeData$pnl ) ) )
    
    rLog("\n")
    
    invisible( NULL )
}








