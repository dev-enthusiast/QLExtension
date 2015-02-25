###############################################################################
# SecDbPriceFeed.R
# 
# Author: e14600
#
library( SecDb )
source( "H:/user/R/RMG/Projects/PV/Term.R" )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )


################################################################################
# Compare yesterday's prices with the current prices in SecDb
#
main <- function( counterpartyList = NULL )
{ 
    # Properly connect to secdb from procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    asOfDate = as.character( as.Date( 
                    secdb.dateRuleApply( Sys.Date( ), "-1b" ) ) )
    
    rLog("Loading previous day's prices...")
    previousPrices = .getPreviousPrices( asOfDate )

    
    rLog("Pulling counterparty positions...")
    validCurves = .getCounterpartyCurves( counterpartyList, asOfDate )
    previousPrices = merge( previousPrices, validCurves )
    
    
    rLog("Pulling today's prices from SecDb...")
    securityNames = .getSecDbSecurityNames( previousPrices$curve.name, 
            previousPrices$contract.dt )
    previousPrices$secdbName = securityNames
    newPrices = .getSecDbPrices( securityNames )
    
    
    rLog("Calculating price differences...")
    final = merge( previousPrices, newPrices, all=TRUE )
    final$price[ which( is.na( final$price ) ) ] = 
            final[[ asOfDate ]][ which( is.na( final$price ) ) ]
    
    final$diff = round( final$price - final[[ asOfDate ]], 6 ) 
    final = final[ , c( "curve.name", "contract.dt", asOfDate, "price", "diff" ) ]
    names(final) = c( "curve.name", "contract.dt", "previous", "current", "diff" )
    
    # clean up the data; it can be INF if the contract has expired
    final = final[ which( is.finite( final$diff ) ), ]
    final = final[ which( final$diff != 0 ), ]
    final = final[ do.call( order, final ), ]
    
    rLog("Writing data file...")
    OUTPUT_FILE = paste( "S:/Risk/Trading Controls/Daily Reports/RTPrice/",
            "priceChanges.", as.character( Sys.Date() ), ".", 
            format( Sys.time(), "%H%M" ), ".csv", sep = "" )
    write.csv( final, file = OUTPUT_FILE, row.names = FALSE )
    
    rLog("Done.")
    invisible( NULL )
}


################################################################################
# Load the previous day's prices 
#
.getPreviousPrices <- function( asOfDate )
{ 
    inputPriceFile = paste( "S:/All/Risk/Data/VaR/prod/Prices/sasPrices.", 
            asOfDate, ".RData", sep = "" )
    
    previousPrices = varLoad( "hP", inputPriceFile )
    
    return( previousPrices )
}


################################################################################
# Given a { curve name, contract date } pair, return the secdb security 
# that will give you the most up to date price
#
.getSecDbSecurityNames <- function( curveNames, contractDates )
{ 
    terms = Term$reutersCode( format( contractDates, "%b %y" ) )
    
    secdbNames = gsub( "COMMOD ", "",  curveNames )
    splitNames = strsplit( secdbNames, " " )
    
    prefixStrings = sapply( splitNames, "[", 1 )
    
    suffixes = lapply( splitNames, "[", -1 )
    suffixStrings = sapply( suffixes, paste, collapse = " " )
    
    securityNames = paste( prefixStrings, terms, " ", suffixStrings, sep = "" )
    
    return( securityNames )
}


################################################################################
# Given a specified term, what's the start date?
#
.getSecDbPrices <- function( securities )
{ 
    prices = sapply( securities, function( x )
            { 
                rLog("Retrieving security:", x)
                curveData = secdb.getValueType( x, "Forward Curve" )
                return( as.numeric( curveData$value ) ) 
            } )
    
    # remove curves that consist of more than one price.
    # I think this is caused by an improper mapping to secdb securities above
    priceLength = sapply( prices, length )
    noExtraPrices = prices[ which( priceLength == 1 ) ]
    
    newPriceData = data.frame( secdbName = names( noExtraPrices ), 
            price = unlist( noExtraPrices ) )
    row.names( newPriceData ) = NULL
    
    
    rLog("Performing FX Adjustment...")
    fxTable = .getFXTable( newPriceData$secdbName )
    adjustedPrices = merge( newPriceData, fxTable )
    adjustedPrices$price = adjustedPrices$price * adjustedPrices$rate
    adjustedPrices$rate = NULL
    adjustedPrices$cross = NULL
    
    return( adjustedPrices )
}


################################################################################
# Given a specified term, what's the start date?
#
.getFXTable <- function( curveList )
{
    curveList = unique( as.character( curveList ) )
    denominated = sapply( curveList, secdb.getValueType, "Denominated" )
    dollarCross = paste( "USD/", as.character(denominated), sep="" )
    
    curveMap = data.frame( secdbName = curveList, cross = dollarCross )
    
    uniqueCrosses = unique( curveMap$cross )
    fxRates = sapply( uniqueCrosses, secdb.getValueType, "Forward Point" )
    rateTable = data.frame( cross=uniqueCrosses, rate=fxRates )
    
    fxTable = merge( curveMap, rateTable )
    
    return( fxTable )
}


################################################################################
.getCounterpartyCurves <- function( counterpartyList, asOfDate )
{
    inputPositionFile = paste( "S:/All/Risk/Data/VaR/prod/Positions/vcv.", 
            "positions.", asOfDate, ".RData", sep = "" )
    
    cpPositions = varLoad( "QQ.all", inputPositionFile )
    
    if( !is.null( counterpartyList ) )
    {
        cpPositions = subset( cpPositions, 
                toupper(COUNTERPARTY) %in% counterpartyList )
    }
    
    cpPositions = cpPositions[, c("CURVE_NAME", "CONTRACT_MONTH")]
    names( cpPositions ) = c("curve.name", "contract.dt")
    
    cpPositions$contract.dt = as.Date( format( cpPositions$contract.dt, "%Y-%m-01" ) )
    cpPositions = unique( cpPositions )
    
    return( cpPositions )
}


################################################################################
start = Sys.time()
rLog("\nRunning price pull for position subset...")
cpList = c("BOTCC", "CHICAGOCFE", "ECX", "NYMEX", "NYM EXCHANGE", "CME EXCHANGE")
main( cpList )
print( Sys.time() - start )

start = Sys.time()
rLog("\nRunning complete price pull...")
main()
print( Sys.time() - start )

