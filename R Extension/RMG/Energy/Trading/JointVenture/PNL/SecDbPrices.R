###############################################################################
# SecDbPrices.R
#
# Author: e14600
#
library( SecDb )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Projects/PV/Term.R" )


################################################################################
# File Namespace
#
SecDbPrices = new.env(hash=TRUE)


################################################################################
# Only pull prices once...
.priceCache = new.env(hash=TRUE)


################################################################################
# Given a { curve name, contract date } pair, return the most 
# up-to-date or previous secdb price
#
SecDbPrices$forCurves <- function( data, asOfDate, current )
{
    priceLabel = ifelse( current, "price", "price.prev" )
    data[[priceLabel]] = NA
    
    for( curveIndex in seq_len( nrow( data ) ) )
    {
        curveName = data$curve.name[ curveIndex ]
        contractDate = data$contract.dt[ curveIndex ]
        price = SecDbPrices$get( curveName, contractDate, asOfDate, current )

        data[[priceLabel]][curveIndex] = price
    }
    
    rownames( data ) = NULL
    return( data )
}


################################################################################
# Given a { curve name, contract date } pair, return the most 
# up-to-date or previous secdb price
#
SecDbPrices$get <- function( curveName, contractDate, asOfDate, current=TRUE )
{ 
    asOfDate = as.Date( asOfDate )
    
    curveName = toupper( curveName )
    contractDate = as.character( contractDate )
    
    if( !exists( curveName, .priceCache ) )
    {
        .readPricesFromSecDb( curveName, asOfDate )
    }
    
    markToLoad = ifelse( current, "current", "previous" )
    
    # If the GetFutStrip function didn't have the price, 
    # we need to get it another way
    if( ( contractDate %in% names( .priceCache[[ curveName ]] ) ) == FALSE )
    {
        price = .getPriceViaForwardCurveMapping( curveName, contractDate, asOfDate )
        
        .priceCache[[ curveName ]][[ contractDate ]] = list()
        .priceCache[[ curveName ]][[ contractDate ]][[ "current" ]] = price
        .priceCache[[ curveName ]][[ contractDate ]][[ "previous" ]] = price
    }
    
    price = .priceCache[[ curveName ]][[ contractDate ]][[ markToLoad ]]
        
    return( price )
}


################################################################################
# Actually retrieve the price from secdb for a given curve name 
#
.readPricesFromSecDb <- function( curveName, asOfDate )
{   
    curveList = NULL
    curveList = lapply( curveName, function(x){ append(curveList, list("Loc"=x) ) } )
    
    lastMark = asOfDate
    yesterday = as.Date( secdb.dateRuleApply( lastMark, "-1b" ) ) 
    dateUntil = "Null"
    useClose = ifelse( asOfDate == Sys.Date(), TRUE, FALSE )
    cals = "CPS"
    
    curveData = secdb.invoke(
            "_lib elec dly fut changes", "DlyFutChanges::GetFutStrip", 
            curveList, lastMark, yesterday, dateUntil, useClose, cals )[[1]]
    
    rawTerms = names( unlist(curveData[["FutToday"]]) )
    termList = as.character( Term$fromReutersCode( rawTerms ) )
    priceList = as.numeric( unlist( curveData[["FutToday"]] ) )
    pricePrevList = as.numeric( unlist( curveData[["FutYesterday"]] ) ) 
    
    fxRate = .getFXRate( curveName )
    priceList = priceList * fxRate
    pricePrevList = pricePrevList * fxRate
    
    .priceCache[[curveName]] = list()
    for( index in seq_along( termList ) )
    {
        term = termList[ index ]
        .priceCache[[curveName]][[term]] = list()
        .priceCache[[curveName]][[term]][["current"]] = priceList[ index ]
        .priceCache[[curveName]][[term]][["previous"]] = pricePrevList[ index ]
    }
    
    invisible( NULL )
}


################################################################################
# Get the current FX rate to apply to a given price
#
.getFXRate <- function( curveName )
{ 
    denominated = secdb.getValueType( curveName, "Denominated" )
    dollarCross = paste( "USD/", as.character(denominated), sep="" ) 
    
    fxRate = secdb.getValueType( dollarCross, "Price" )
    
    return( fxRate )
}


################################################################################
# Given a { curve name, contract date } pair, return the most 
# up-to-date secdb price
#
.getPriceViaForwardCurveMapping <- function( curveName, contractDate, asOfDate )
{ 
    rLog("Getting SecDb Price Using Forward Curve Mappings...")

    if( asOfDate != Sys.Date() )
    rWarn("As Of Date functionality for Forward Curve Mapping NOT implemented!", 
            as.character( asOfDate ) )
    
    term = Term$reutersCode( format( as.Date( contractDate ), "%b %y" ) )
    
    secdbNames = gsub( "COMMOD ", "", curveName )
    splitNames = strsplit( secdbNames, " " )
    
    prefixStrings = sapply( splitNames, "[", 1 )
    
    suffixes = lapply( splitNames, "[", -1 )
    suffixStrings = sapply( suffixes, paste, collapse = " " )
    
    secdbName = paste( prefixStrings, term, " ", suffixStrings, sep = "" )
    
    price = secdb.getValueType( secdbName, "Forward Curve" )$value[1]
    
    fxRate = .getFXRate( curveName )
    finalPrice = price * fxRate
    
    return( finalPrice )
}


## Link("_Lib elec dly fut changes"); 
## 
## curve = Array(1);
## curve[0] = Structure();
## curve[0].Loc = "COMMOD PWJ 5X16 WEST PHYSICAL";
## debug;
## 
## today = @DlyFutChanges::GetFutStrip(curve, Date("02May08"), Date("01May08"), Date("31Jul08"), TRUE, "CPS");
## 
## yesterday = @DlyFutChanges::GetFutStrip(curve, Date("01May08"), Date("30Apr08"), Date("31Jul08"), TRUE, "CPS");
## 
## yesterdayNoClose = @DlyFutChanges::GetFutStrip(curve, Date("01May08"), Date("30Apr08"), Date("31Jul08"), FALSE, "CPS");
## 
## debug;




