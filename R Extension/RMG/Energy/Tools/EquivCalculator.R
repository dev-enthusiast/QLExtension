###############################################################################
## EquivCalculator.R
## 
## Author: e14600
##
## Given Curve/Tenor A (CT-A):
## 
## Gas Equiv Position A = lm( Price Changes(CT-A) , Price Changes(NYMEX T) ) * 
##         (Price( CT-A ) * Delta( CT-A )) / ( (Price(NYMEX T))*10 000 )
##


################################################################################
# Required library files
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/load.R") 


################################################################################
# File Namespace
EquivCalculator = new.env(hash=TRUE)


################################################################################
# Cached Data
EquivCalculator$.allPriceData = NULL


################################################################################
# Calculate the gas equivalent for a given power position
#
EquivCalculator$gasForPower <- function( curve.name, contract.dt, position )
{
    # Sanity check the inputs
    curve.name = as.character( curve.name )
    contract.dt = as.Date( contract.dt )
    position = as.numeric( position )
    
    # Load the 70d price data
    if( is.null( EquivCalculator$.allPriceData ) )
    {
        PRICE_DATA_FILE = "S:/All/Risk/Data/VaR/live/allPriceData.RData"
        EquivCalculator$.allPriceData = varLoad( "allPriceData", file=PRICE_DATA_FILE )
    }
    
    # Temp variable used for clarity 
    priceData = EquivCalculator$.allPriceData

    curvePrice = priceData[ which(priceData$curve.name == curve.name &
                    priceData$contract.dt == contract.dt ), ]
    nymexPrice = priceData[ which(priceData$curve.name == "COMMOD NG EXCHANGE" &
                    priceData$contract.dt == contract.dt ), ]
    
    # Drop the unneeded columns
    curvePrice$curve.name = NULL
    curvePrice$contract.dt = NULL
    nymexPrice$curve.name = NULL
    nymexPrice$contract.dt = NULL
    
    # Calculate the price changes
    curveChanges = diff( t( curvePrice ) ) 
    nymexChanges = diff( t( nymexPrice ) ) 
    
    regressionData = lm( curveChanges ~ nymexChanges )
    
    HR = curvePrice[ length(curvePrice) ] / nymexPrice[ length(nymexPrice) ]
    fullUpHedgeValue = position * HR / 10000 
    
    equivPosition = regressionData$coefficients[['nymexChanges']] * as.numeric(fullUpHedgeValue)
    
    return( as.numeric(equivPosition) )
}

curveForCurve <- function( curveOne, curveTwo, startDate, endDate )
{
    curveOneData = tsdb.readCurve( curveOne, startDate, endDate )
    curveTwoData = tsdb.readCurve( curveTwo, startDate, endDate )

    # Calculate the price changes
    curveOneChanges = diff( curveOneData$value ) 
    curveTwoChanges = diff( curveTwoData$value )  
    
    regressionData = lm( curveOneChanges ~ curveTwoChanges )
    HR = curvePrice[ length(curvePrice) ] / nymexPrice[ length(nymexPrice) ]

    return( regressionData$coefficients[2]  )
}


curveForCurve("PWJ_WEST_ANNUAL_STRIP_FLAT_2009", "NG_ANNUAL_STRIP_EXCHANGE_2009", start, end)
curveForCurve("PWJ_WEST_ANNUAL_STRIP_FLAT_2010", "NG_ANNUAL_STRIP_EXCHANGE_2010", start, end)
curveForCurve("PWJ_WEST_ANNUAL_STRIP_FLAT_2011", "NG_ANNUAL_STRIP_EXCHANGE_2011", start, end)
curveForCurve("PWJ_WEST_ANNUAL_STRIP_FLAT_2012", "NG_ANNUAL_STRIP_EXCHANGE_2012", start, end)
curveForCurve("PWJ_WEST_ANNUAL_STRIP_FLAT_2013", "NG_ANNUAL_STRIP_EXCHANGE_2013", start, end)



power = tsdb.
