###############################################################################
# SASVaR.R
# 
# Author: e14600
#
library(reshape)
library(zoo)
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Database/SAS/VPort.R")
source("H:/user/R/RMG/Utilities/Database/SAS/SASInterface.R")


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
SASVaR$forPortfolio <- function( portfolio, asOfDate )
{
    books = VPort$booksForPortfolio( portfolio, asOfDate )
    if( length( books ) == 0 )
    {
        stop(portfolio, " is not a valid portfolio. ", 
                "Check the name and try again.")
    }
    
    var = SASVaR$forBooks( books, asOfDate )
    
    return(var)
}


################################################################################
SASVaR$forSecDbPortfolio <- function( portfolio, asOfDate )
{
    library(SecDb)
    
    books = names(secdb.getValueType(portfolio, "Nodes")$Book)
    if( is.null( books ) )
    {
        stop(portfolio, " is not a valid SecDb portfolio. ", 
                "Check the name and try again.")
    }
    
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
SASVaR$getIVaR <- function( books, asOfDate )
{
    positions = SASVaR$positionsForBooks( books, asOfDate )
    positions$book = NULL
    positions = cast( positions, riskFactor ~ ., sum )
    names(positions) = c("riskFactor", "value")
    
    missingPrices = which(positions$riskFactor == "NULL_PRICE")
    if( length(missingPrices) > 0 ) positions = positions[-missingPrices,]
    
    missingVols = which(positions$riskFactor == "NULL_VOL")
    if( length(missingVols) > 0 ) positions = positions[-missingVols,]
    
    rDebug("Calculating Delta P&L changes from merged data...")
    
    SASVaR$.loadPriceData()
    
    mergeColumns = "riskFactor"
    deltas = positions[grep("^PR_", positions$riskFactor), ]
    SASVaR$.identifyMissingCurves( SASVaR$.allPriceData, deltas, mergeColumns )
    
    deltas = merge(deltas, SASVaR$.allPriceData[, mergeColumns, drop=FALSE ], all.x = TRUE )
    deltaChanges = SASVaR$.getPriceChanges( deltas, SASVaR$.allPriceData, mergeColumns ) 
    
    
    rDebug("Calculating Vega P&L changes from merged data...")
    
    SASVaR$.loadVolData()
    
    vegas = positions[ -grep("^PR_", positions$riskFactor), ]
    SASVaR$.identifyMissingCurves( SASVaR$.allVolData, vegas, mergeColumns )
    
    vegas = merge(vegas, SASVaR$.allVolData[, mergeColumns, drop=FALSE ], all.x = TRUE)
    vegaChanges = SASVaR$.getPriceChanges( vegas, SASVaR$.allVolData, mergeColumns ) 
    
    allChanges = rbind( deltaChanges, vegaChanges )
    allPositions = rbind( deltas, vegas )
    ivarData = cbind( allPositions, allChanges )
    
    return( ivarData )
}


################################################################################
SASVaR$calculate <- function( positions )
{
    positionData = positions
    positionData$book = NULL
    positionData = cast( positionData, riskFactor ~ ., sum )
    names(positionData) = c("riskFactor", "value")
    
    deltaPnLChange = SASVaR$.getDeltaPnLChanges( positionData )
    vegaPnLChange = SASVaR$.getVegaPnLChanges( positionData )
    
    hasDeltaPositions = ( length( grep("^PR_", positionData$riskFactor) ) > 0 )
    hasVegaPositions = ( length( grep("^(M5|D5|B5)_", positionData$riskFactor) ) > 0 )
    if( hasVegaPositions && hasDeltaPositions )
    {
        # Ensure all the columns line up. We could have a situation where there
        # are more days in the price matrix vs the vol matrix, etc.
        colsToKeep = intersect( names(deltaPnLChange), names(vegaPnLChange) )
        
        if( length( colsToKeep ) != ncol( deltaPnLChange ) && 
                length( colsToKeep ) != ncol( vegaPnLChange ) )
        {
            rWarn( "Dropping columns from price or vol pnl matrix because we're missing a date..." )
        }
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
    marginalVaR  = SASVaR$.calculateMVaR( positions )
    
    deltas = positions[grep("^PR_", positions$riskFactor), ]
    componentVaR = merge( deltas, marginalVaR, all.x=TRUE, sort=FALSE )
    componentVaR$cvar = componentVaR$marginalVaR * componentVaR$value
    
    componentVaR = componentVaR[, c("riskFactor", "cvar")]
    
    return( componentVaR )
}


################################################################################
SASVaR$.calculateMVaR <- function( positions )
{
    SASVaR$.loadPriceData()
    
    deltas = positions[grep("^PR_", positions$riskFactor), ]
    deltas = deltas[ , c("riskFactor", "value"), drop=FALSE ]
    deltas = cast( deltas, riskFactor~., sum )
    names(deltas) = c("riskFactor", "value")
    
    # Only use deltas for which you'll have prices
    mergeColumns = "riskFactor"
    deltas = merge(deltas, SASVaR$.allPriceData[, mergeColumns, drop=FALSE ])
    
    priceChanges = SASVaR$.getPriceChanges( deltas, SASVaR$.allPriceData, 
            mergeColumns )
    
    pnlData = deltas$value * priceChanges
    
    dV <- colSums( pnlData, na.rm = TRUE )
    
    mVaR <- function(x){ cov(x, dV, use="pairwise.complete.obs") }
    
    marginalVaR  = 4 * apply(priceChanges, 1, mVaR) / sd(dV)
    
    marginalVaR = cbind( deltas[, "riskFactor", drop=FALSE], marginalVaR)
    
    return( marginalVaR )
}


################################################################################
SASVaR$positionsForBooks <- function( bookList, asOfDate = Sys.Date()-1 )
{
    deltas = SASVaR$.getDeltas( asOfDate )
    vegas = SASVaR$.getVegas( asOfDate )
    
    allPositions = rbind( deltas, vegas )
    
    if( is.null( bookList ) )
    {
        requestedPositions = allPositions
        
    } else
    {
        requestedPositions = subset( allPositions, allPositions$book %in% bookList)
    }
    
    return( requestedPositions )
}


################################################################################
SASVaR$.getDeltas <- function( asOfDate )
{
    if( !is.null( SASVaR$.deltas ) )
    {
        return( SASVaR$.deltas )
    }
    
    deltas = SASInterface$getDeltas( asOfDate )
    
    if( is.null( deltas ) )
    {
        return( NULL )
    }
    
    deltas = deltas[, c("book", "riskFactor", "value")]
    
    deltas = cast(deltas, book + riskFactor ~ ., sum) 
    names(deltas) = c("book", "riskFactor", "value" )
    
    deltas = subset(deltas, deltas$value!=0)    
    rownames( deltas ) = NULL

    SASVaR$.deltas = deltas
    
    return( deltas )
}


################################################################################
SASVaR$.getVegas <- function( asOfDate )
{
    if( !is.null(SASVaR$.vegas) )
    {
        return( SASVaR$.vegas )
    }
    
    vegas = SASInterface$getVegas( asOfDate )
    
    if( is.null( vegas ) )
    {
        return( NULL )
    }
    
    vegas = cast(vegas, book + riskFactor ~ ., sum) 
    names(vegas) = c("book", "riskFactor", "value" )
    
    vegas = subset(vegas, vegas$value != 0)
    
    rownames( vegas ) = NULL
    
    SASVaR$.vegas = vegas
    
    return( SASVaR$.vegas )
}


################################################################################
SASVaR$.loadPriceData <- function()
{
    if( is.null(SASVaR$.allPriceData) )
    {
        PRICE_DATA_FILE = "S:/All/Risk/Data/VaR/live/allPriceData.RData"
        allPriceData = varLoad( "allPriceData", file=PRICE_DATA_FILE )
        allPriceData$riskFactor = SASVaR$.createRiskFactor( 
                allPriceData$curve.name, allPriceData$contract.dt, NULL )
        allPriceData$curve.name = allPriceData$contract.dt = NULL
        SASVaR$.allPriceData = allPriceData 
    }
    
    invisible( SASVaR$.allPriceData )
}


################################################################################
SASVaR$.loadVolData <- function()
{
    if( is.null(SASVaR$.allVolData) )
    {
        VOL_DATA_FILE = "S:/All/Risk/Data/VaR/live/allVolData.RData"
        allVolData = varLoad( "allVolData", file=VOL_DATA_FILE )
        allVolData$riskFactor = SASVaR$.createRiskFactor( 
                allVolData$curve.name, allVolData$contract.dt, 
                allVolData$vol.type )
        allVolData$curve.name = NULL 
        allVolData$contract.dt = NULL
        allVolData$vol.type = NULL
        SASVaR$.allVolData = allVolData 
    }
    
    invisible( SASVaR$.allVolData )
}


################################################################################
SASVaR$.getDeltaPnLChanges <- function( positions )
{
    rDebug("Calculating Delta P&L changes from merged data...")
    
    deltas = positions[ grep("^PR_", positions$riskFactor), ]
    
    SASVaR$.loadPriceData()
    
    mergeColumns = "riskFactor"
    SASVaR$.identifyMissingCurves( SASVaR$.allPriceData, deltas, mergeColumns )
    
    deltas = merge(deltas, SASVaR$.allPriceData[, mergeColumns, drop=FALSE ])
    changeData = SASVaR$.getPriceChanges( deltas, SASVaR$.allPriceData, mergeColumns ) 
    
    deltaPnLChange = changeData * deltas$value
    
    return( deltaPnLChange )
}


################################################################################
SASVaR$.getVegaPnLChanges <- function( positions )
{
    rDebug("Calculating Vega P&L changes from merged data...")
    
    vegas = positions[ -grep("^PR_", positions$riskFactor), ]
    
    SASVaR$.loadVolData()
    
    mergeColumns = "riskFactor"
    SASVaR$.identifyMissingCurves( SASVaR$.allVolData, vegas, mergeColumns )
    
    vegas = merge(vegas, SASVaR$.allVolData[, mergeColumns, drop=FALSE ])
    changeData = SASVaR$.getPriceChanges( vegas, SASVaR$.allVolData, mergeColumns ) 
    
    vegaPnLChange = changeData * vegas$value
    
    return( vegaPnLChange )
}



################################################################################
SASVaR$.createRiskFactor<- function( curveName, contractDate, volType = NULL )
{
    
    monthly = which( volType == "M" )
    daily = which( volType == "D" )
    blended = which( volType == "B" )
    
    riskFactor = toupper( curveName )
    riskFactor[monthly] = gsub("COMMOD ", "M5_", riskFactor[monthly] )
    riskFactor[daily] = gsub("COMMOD ", "D5_", riskFactor[daily] )
    riskFactor[blended] = gsub("COMMOD ", "B5_", riskFactor[blended] )
    
    riskFactor = gsub("COMMOD ", "PR_", riskFactor )
    
    riskFactor = gsub( ".", "_", riskFactor, fixed=TRUE )
    riskFactor = gsub("( |-)", "_", riskFactor )
    riskFactor = paste( riskFactor, "_", 
            format( as.Date( contractDate ), "%y%m"), sep = "" )
    riskFactor = gsub("__", "_", riskFactor, fixed=TRUE )
    
    missing = which( curveName == "MISSING" )
    riskFactor[missing] = "NULL_PRICE"
    
    return( riskFactor )
}

.testCreateRiskFactor <- function()
{
    library(RUnit)
    
    checkEquals( SASVaR$.createRiskFactor("COMMOD NG EXCHANGE", 
                    "2008-12-01", NULL), "PR_NG_EXCHANGE_0812" )
    
    checkEquals( SASVaR$.createRiskFactor("MISSING", "2008-12-01", NULL),
            "NULL_PRICE" )
    
    checkEquals( SASVaR$.createRiskFactor("COMMOD FO1 .3HP NYH CFOB PLTM",
                    "2012-01-01", NULL), "PR_FO1_3HP_NYH_CFOB_PLTM_1201" )
    
    checkEquals( SASVaR$.createRiskFactor("COMMOD IFO 3.5 RTM INDEX",
                    "2008-11-01", NULL), "PR_IFO_3_5_RTM_INDEX_0811" )
    
    checkEquals( SASVaR$.createRiskFactor("COMMOD NG EXCHANGE", 
                    "2008-12-01", "M"), "M5_NG_EXCHANGE_0812" )
}


################################################################################
SASVaR$.identifyMissingCurves <- function( curveData, positionData, mergeColumns )
{
    curves = curveData[, mergeColumns, drop=FALSE ]
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

    WINDOW_SIZE = 71
    if( length(priceChangeColumns) > WINDOW_SIZE )
    {
        startIndex = length(priceChangeColumns) - WINDOW_SIZE + 1
        priceChangeColumns = priceChangeColumns[ startIndex:length(priceChangeColumns) ]
    }
    
    prices = as.matrix( positionsAndPrices[,priceChangeColumns, drop=FALSE] )
    
    prices = SASVaR$.fillMissingPrices( prices )
    
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


################################################################################
# Fill any NAs that might be in the data
# 
SASVaR$.fillMissingPrices <- function( priceData )
{
    prices = zoo( t( priceData ), as.Date( colnames(priceData) ) )    
    
    prices = na.approx( prices, na.rm=FALSE )
    prices = na.locf( prices, na.rm=FALSE )
    prices = na.locf( prices, na.rm=FALSE, fromLast=TRUE)
    
    prices = t( prices )
    rownames(prices) = NULL
    
    return( prices )
}

.testFillMissingPrices <- function()
{
    library(RUnit)
    library(xts)
    
    TIME_INDEX = seq(as.Date("2008-10-01"), as.Date("2008-10-07"), by="day")
    array_names = list(NULL,as.character(TIME_INDEX))
    
    testSet1 = t( xts( c(1,2,3,NA,5,6,7), TIME_INDEX ) ) 
    correctCase1 = array( c(1,2,3,4,5,6,7), c(1,7), dimnames=array_names )
    checkEquals( SASVaR$.fillMissingPrices(testSet1), correctCase1 )
    
    testSet2 = t( xts( c(NA,2,3,NA,5,6,7), TIME_INDEX ) ) 
    correctCase2 = array( c(2,2,3,4,5,6,7), c(1,7), dimnames=array_names )
    checkEquals( SASVaR$.fillMissingPrices(testSet2), correctCase2 )
    
    testSet3 = t( xts( c(1,2,3,NA,5,6,NA), TIME_INDEX ) ) 
    correctCase3 = array( c(1,2,3,4,5,6,6), c(1,7), dimnames=array_names )
    checkEquals( SASVaR$.fillMissingPrices(testSet3), correctCase3)
    
    testSet4 = t( xts( c(1,2,3,NA,5,NA,NA), TIME_INDEX ) ) 
    correctCase4 = array( c(1,2,3,4,5,5,5), c(1,7), dimnames=array_names )
    checkEquals( SASVaR$.fillMissingPrices(testSet4), correctCase4)
    
    testSet5 = t( xts( c(NA,NA,3,NA,5,6,7), TIME_INDEX ) ) 
    correctCase5 = array( c(3,3,3,4,5,6,7), c(1,7), dimnames=array_names )
    checkEquals( SASVaR$.fillMissingPrices(testSet5), correctCase5 )
    
}

