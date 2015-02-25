###############################################################################
# SASVaR.R
# 
# Author: e14600
#
library(reshape)
library(zoo)
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
SASVaR = new.env(hash=TRUE)


################################################################################
# Variables used to cache most commonly used data
# This will increase the memory footprint, but it should also really
# speed up the calculation process. 
#
SASVaR$.deltas = NULL
SASVaR$.vegas = NULL
SASVaR$.allPriceData = NULL
SASVaR$.allVolData = NULL


################################################################################
SASVaR$forSecDbPortfolio <- function( portfolio, asOfDate )
{
    library(SecDb)
    
    books = names(secdb.getValueType(portfolio, "Nodes")$Book)
    var = SASVaR$forBooks( books, asOfDate )
    
    return(var)
}


################################################################################
SASVaR$forBooks <- function( books, asOfDate )
{
    positions = SASVaR$positionsForBooks( books, asOfDate )
    var = SASVaR$calculate( positions )
    
    return(var)
}


################################################################################
SASVaR$calculate <- function( positions )
{
    positionData = positions
    positionData$book = NULL
    positionData = cast( positionData, curve.name + contract.dt + vol.type ~ ., sum )
    names(positionData) = c("curve.name", "contract.dt", "vol.type", "value")
    
    deltaPnLChange = SASVaR$.getDeltaPnLChanges( positionData )
    vegaPnLChange = SASVaR$.getVegaPnLChanges( positionData )
    
    hasDeltaPositions = ( length( which(is.na(positionData$vol.type)) ) > 0 )
    hasVegaPositions = ( length( which(!is.na(positionData$vol.type)) ) > 0 )
    if( hasVegaPositions && hasDeltaPositions )
    {
        # Ensure all the columns line up. We could have a situation where there
        # are more days in the price matrix vs the vol matrix, etc.
        colsToKeep = intersect( names(deltaPnLChange), names(vegaPnLChange) )
        deltaPnLChange = deltaPnLChange[ , colsToKeep ]
        vegaPnLChange = vegaPnLChange[ , colsToKeep ]
    }
    
    finalPNLChange = rbind( deltaPnLChange, vegaPnLChange )
    
    cleanData = na.omit(finalPNLChange)
    if( nrow(cleanData) != nrow(finalPNLChange) )
    {
        rWarn("NAs Present In PNL Change Matrix... Omitting...")
    }
    
    var = SASVaR$.varFromPNLChanges( cleanData )
    
    return( var )
}


################################################################################
SASVaR$calculateCVaR <- function( positions )
{
    marginalVaR  = SASVaR$calculateMVaR( positions )
    
    deltas = positions[which(is.na(positions$vol.type)), ]
    componentVaR = merge( deltas, marginalVaR, all.x=TRUE, sort=FALSE )
    componentVaR$cvar = componentVaR$marginalVaR * componentVaR$value
    
    componentVaR = componentVaR[, c("book", "curve.name", "contract.dt", "vol.type", "cvar")]
    
    return( componentVaR )
}


################################################################################
SASVaR$calculateMVaR <- function( positions )
{
    if( is.null(SASVaR$.allPriceData) )
    {
        PRICE_DATA_FILE = "S:/All/Risk/Data/VaR/live/allPriceData.RData"
        SASVaR$.allPriceData = varLoad( "allPriceData", file=PRICE_DATA_FILE )
    }
    
    deltas = positions[which(is.na(positions$vol.type)), ]
    deltas = deltas[ , c("curve.name", "contract.dt", "value") ]
    deltas = cast( deltas, curve.name+contract.dt~., sum )
    names(deltas) = c("curve.name", "contract.dt", "value")
    
    priceChanges = SASVaR$.getPriceChanges( deltas, SASVaR$.allPriceData, 
            c("curve.name", "contract.dt") )
    
    pnlData = deltas$value * priceChanges
    
    dV <- colSums(pnlData)
    
    mVaR <- function(x){ cov(x, dV) }
    
    marginalVaR  = 4 * apply(priceChanges, 1, mVaR) / sd(dV)
    
    marginalVaR = cbind( deltas[, c("curve.name", "contract.dt")], marginalVaR)
    
    return( marginalVaR )
}


################################################################################
SASVaR$positionsForBooks <- function( bookList, asOfDate = Sys.Date()-1 )
{
    deltas = SASVaR$.getDeltas( asOfDate )
    vegas = SASVaR$.getVegas( asOfDate )
    
    allPositions = rbind( deltas, vegas )
    
    requestedPositions = subset( allPositions, allPositions$book %in% bookList)
    
    return( requestedPositions )
}


################################################################################
SASVaR$.getDeltas <- function( asOfDate )
{
    if( !is.null(SASVaR$.deltas) )
    {
        return( SASVaR$.deltas )
    }
    
    fileName <- paste("S:/All/Risk/Data/VaR/prod/Positions/vcv.positions.",
            asOfDate, ".RData", sep="")                          
    
    deltas = varLoad("QQ.all", fileName)
    deltas = deltas[,c("BOOK", "CURVE_NAME", "CONTRACT_MONTH", "DELTA")]
    colnames(deltas) = c("BOOK", "CURVE_NAME", "CONTRACT_MONTH", "value")
    deltas$CONTRACT_MONTH = as.Date(format(deltas$CONTRACT_MONTH, "%Y-%m-01"))
    
    deltas = cast(deltas, BOOK + CURVE_NAME + CONTRACT_MONTH ~ ., sum) 
    colnames(deltas) = c("BOOK", "CURVE_NAME", "CONTRACT_MONTH", "value")
    
    deltas = subset(deltas, deltas$value!=0)
    names(deltas) = c("book", "curve.name", "contract.dt", "value")
    
    deltas$vol.type = NA
    rownames(deltas) = NULL
    deltas = deltas[, c("book", "curve.name", "contract.dt", "vol.type", "value")]
    
    SASVaR$.deltas = deltas
    
    return( SASVaR$.deltas )
}


################################################################################
SASVaR$.getVegas <- function( asOfDate )
{
    if( !is.null(SASVaR$.vegas) )
    {
        return( SASVaR$.vegas )
    }
    
    fileName <- paste("S:/All/Risk/Data/VaR/prod/Positions/sas.vega.positions.",
            asOfDate, ".RData", sep="")                          
    
    vegas = varLoad("vegaPositions", fileName)
    
    names(vegas) <- c("book", "curve.name", "contract.dt", "vol.type", "value")
    
    vegas$value = vegas$value * 100
    
    rownames( vegas ) = NULL
    
    SASVaR$.vegas = vegas
    
    return( SASVaR$.vegas )
}


################################################################################
SASVaR$.getDeltaPnLChanges <- function( positions )
{
    rDebug("Calculating Delta P&L changes from merged data...")
    
    deltas = positions[ which(is.na(positions$vol.type)), ]
    
    if( is.null(SASVaR$.allPriceData) )
    {
        PRICE_DATA_FILE = "S:/All/Risk/Data/VaR/live/allPriceData.RData"
        SASVaR$.allPriceData = varLoad( "allPriceData", file=PRICE_DATA_FILE )
    }
    
    mergeColumns = c("curve.name", "contract.dt")
    
    SASVaR$.identifyMissingCurves( SASVaR$.allPriceData, deltas, mergeColumns )
    
    changeData = SASVaR$.getPriceChanges( deltas, SASVaR$.allPriceData, mergeColumns ) 
    
    deltaPnLChange = changeData * deltas$value
    
    return( deltaPnLChange )
}


################################################################################
SASVaR$.getVegaPnLChanges <- function( positions )
{
    rDebug("Calculating Vega P&L changes from merged data...")
    
    vegas = positions[which(!is.na(positions$vol.type)), ]
    
    if( is.null(SASVaR$.allVolData) )
    {
        VOL_DATA_FILE = "S:/All/Risk/Data/VaR/live/allVolData.RData"
        SASVaR$.allVolData = varLoad( "allVolData", file=VOL_DATA_FILE )
    }
    
    mergeColumns = c("curve.name", "contract.dt", "vol.type")
    
    SASVaR$.identifyMissingCurves( SASVaR$.allVolData, vegas, mergeColumns )
    
    changeData = SASVaR$.getPriceChanges( vegas, SASVaR$.allVolData, mergeColumns ) 
    
    vegaPnLChange = changeData * vegas$value
    
    return( vegaPnLChange )
}


################################################################################
SASVaR$.identifyMissingCurves <- function( curveData, positionData, mergeColumns )
{
    curves = curveData[, mergeColumns]
    curves$curvePresent = TRUE
    curveMap = merge( positionData, curves, all.x=TRUE )
    
    missingCurves = curveMap[ which(is.na(curveMap$curvePresent)), ]
    
    if( nrow(missingCurves) > 0 )
    {
        rLog("Missing Prices For:")
        print(missingCurves)
    }
}


################################################################################
# Extract the prices, calculate a change matrix
# 
SASVaR$.getPriceChanges <- function( positionData, curveData, mergeColumns )
{
    positionsAndPrices = merge( positionData, curveData, by=mergeColumns, 
            sort=FALSE, na.fill=0 ) 
    
    priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
            names(positionsAndPrices), extended=TRUE, value=TRUE)
    if( length(priceChangeColumns) > 71 )
    {
        startIndex = length(priceChangeColumns) - 71 + 1
        priceChangeColumns = priceChangeColumns[ startIndex:length(priceChangeColumns) ]
    }
    
    prices = as.matrix( positionsAndPrices[,priceChangeColumns, drop=FALSE] )
    
    # Fill any NAs that might be in the data
    prices = zoo( t( prices ) )    
    prices = na.approx( prices, na.rm=FALSE )
    prices = na.locf( prices, na.rm=FALSE )
    prices = na.locf( prices, na.rm=FALSE, fromLast=TRUE)
    prices = t( prices )
    rownames(prices) = NULL
    
    pricesTZero = prices[ , 2:ncol(prices), drop=FALSE ]
    pricesTMinus1 = prices[ , 1:(ncol(prices)-1), drop=FALSE]
    
    changeData = data.frame( pricesTZero-pricesTMinus1, check.names=FALSE )
    
    return( changeData )
}


################################################################################
# basic var calculator, takes a n X m matrix of pnl changes
#
SASVaR$.varFromPNLChanges <- function( changeMatrix )
{
    vars = apply(changeMatrix, 2, sum)
    if( length(which(is.na(vars))) != 0 )
    {
        finalVaR = NA
        
    }else
    {
        finalVaR = sd(vars)*4
    }
    
    return(finalVaR)
}




