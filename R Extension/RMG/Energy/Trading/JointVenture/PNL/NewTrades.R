###############################################################################
# NewTrades.R
#
# Author: e14600
#
library( RODBC )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )


################################################################################
# File Namespace
#
NewTrades = new.env(hash=TRUE)


################################################################################
# Cached Values
NewTrades$.tradeData = NULL
NewTrades$.tradeDate = NULL


################################################################################
# Get the 'live' trade data
#
NewTrades$forDate <- function( asOfDate ) 
{
    asOfDate = as.Date( asOfDate )
    
    if( is.null( NewTrades$.tradeData ) || NewTrades$.tradeDate != asOfDate )
    {
        .saveNewTradeData( asOfDate )
        NewTrades$.tradeDate = asOfDate
    }
    
    return( NewTrades$.tradeData )
}


################################################################################
# Take the raw trade information, clean it up, and cache it in the environment
# 
.saveNewTradeData <- function( asOfDate = Sys.Date() )
{
    rLog("Beginning Trade Data Processing for:", as.character( asOfDate ), "...")
    
    rawTrades = .readRawTradeData( asOfDate )
    
    newTrades = .remapCurveNamesFromTradeData( rawTrades )
    
    newTrades$contract.dt = as.Date( format( newTrades$contract.dt, "%Y-%m-01" ) )
    newTrades$numMonths[ which( is.na( newTrades$numMonths ) ) ] = 1
    newTrades$value[ which( is.na( newTrades$value ) ) ] = 
            newTrades$originalQuantity[ which( is.na( newTrades$value ) ) ]
    
    singleMonthTrades = which( newTrades$numMonths == 1 )
    cleanTradeData = newTrades[ singleMonthTrades, ]
    
    # loop over the multi-month trades and expand them out
    unexpandedTrades = newTrades[ -singleMonthTrades, ]
    expandedTrades = .getExpandedTradeData( unexpandedTrades )
    
    # Join the two sets
    allTrades = rbind( cleanTradeData, expandedTrades )
    
    # Reverse the quantity if it's a sell
    allTrades$value = abs( allTrades$value )
    allTrades$value = ifelse( allTrades$buySell == "Sell", 
            -1 * allTrades$value, allTrades$value )
    
    NewTrades$.tradeData = allTrades[ , c("book", "curve.name", "contract.dt",
                    "value", "price" ) ]
    
    invisible( NewTrades$.tradeData )
}


################################################################################
# Connect to the database and read the new trades for the given asOfDate
#
.readRawTradeData <- function( asOfDate = Sys.Date() )
{
    query = paste( "select ",
            "a.PORTFOLIO1, b.CURVE_NAME, b.ASSET, b.START_DATE, a.NUMMONTHS, ",
            "a.TOTAL_QTY, b.QUANTITY, b.QUANTITYUOM, b.DELIVERY_LOCATION, ",
            "b.DELIVERYPOINT, b.SERVICETYPE, b.QTYBUCKETTYPE, ",
            "a.BUY_SELL, b.PRICE ",  
            "from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
            "a.AT_ID = b.AT_ID and ",
            "a.RECORD_TIME>TO_DATE('", asOfDate, " 00:00:00', 'YYYY-MM-DD HH24:MI:SS') and ",
            "a.RECORD_TIME<TO_DATE('", asOfDate, " 23:59:59', 'YYYY-MM-DD HH24:MI:SS') and ",
            "a.ENTRY_TYPE = 'NEW' and ",
            "b.CURVE_NAME is not null and b.CURVE_NAME != 'NA' and ",
            "b.QTYBUCKETTYPE is not null and ",
            "a.ACTIVE='Y' and b.PRICE != 0 ", sep="" )
    
    chan = odbcConnect("CPSPROD", "tradeauthread", "tradeauthread")
    rLog("Retrieving New Trade Data...")
    newTrades = sqlQuery(chan, query)
    odbcClose(chan)
    
    names( newTrades ) = c( 
            "book", "curve.name", "asset", "contract.dt", "numMonths", 
            "value", "originalQuantity", "unit", "region",
            "deliveryPoint", "serviceType", "bucket",
            "buySell", "price" )
    
    newTrades$book = toupper( newTrades$book )
    newTrades$curve.name = toupper( newTrades$curve.name )
    newTrades$asset = toupper( newTrades$asset )
    newTrades$unit = toupper( newTrades$unit )
    newTrades$region = toupper( newTrades$region )
    newTrades$deliveryPoint = toupper( newTrades$deliveryPoint )
    newTrades$serviceType = toupper( newTrades$serviceType )
    newTrades$bucket = toupper( newTrades$bucket )
    newTrades$buySell = as.character( newTrades$buySell )
    newTrades$contract.dt = as.Date( newTrades$contract.dt )
    
    return( newTrades )
}


################################################################################
# The power curves all need to me remapped, everything else can use the base
# curve name found in the trade data
# 
.remapCurveNamesFromTradeData <- function( tradeData )
{
    rLog("Remapping Curve Names From Trade Data...")
    
    # For curves without an asset, replace it with the curve name
    noAssetName = which( is.na( tradeData$asset ) )
    tradeData$asset[ noAssetName ] = tradeData$curve.name[ noAssetName ]
    
    # now that they all have asset names, when the two aren't equal,
    # it needs to be remapped
    validCurveName = which( tradeData$asset == tradeData$curve.name )
    
    # Generate a proper curve mapping
    invalidNames = tradeData[ -validCurveName, ]
    mapping = .getElecToCommodMap( invalidNames )
    
    # Apply it to the bad trade data
    withFixedNames = merge( invalidNames, mapping, suffixes = c( "", ".fixed" ), 
            by = c("region", "deliveryPoint", "serviceType", "bucket") )
    withFixedNames$curve.name = withFixedNames$curve.name.fixed
    withFixedNames$curve.name.fixed = NULL
    
    # Doesn't need a remap
    correctNames = tradeData[ validCurveName, ]
    
    # Join the two sets
    allTrades = rbind( correctNames, withFixedNames )
    
    return( allTrades )
}


################################################################################
# generate a mapping that can be used to merge with later on
# to get the correct curve name
# 
.getElecToCommodMap <- function( tradeData )
{
    rLog("Creating Elec To Commod Map...")
    
    locsToMap = unique( tradeData[ ,
                    c("region", "deliveryPoint", "serviceType", "bucket") ] )
    
    allMappings = NULL
    for( rowIndex in seq_len( nrow( locsToMap ) ) )
    {
        currentParams = locsToMap[ rowIndex, ]
        
        rTrace( rowIndex,  
                currentParams$region, currentParams$deliveryPoint, 
                currentParams$serviceType, currentParams$bucket, sep=", " )
        
        properCurveNames = secdb.invoke(
                "_Lib Vcentral delivery point", "VCN::EML2CML",
                currentParams$region, currentParams$deliveryPoint, 
                currentParams$serviceType, currentParams$bucket )
        
        # There's got to be a better way to do this...
        newMapping = NULL
        for( unusedCounter in seq_along( properCurveNames ) )
        {
            newMapping = rbind( newMapping, currentParams )
        }
        
        newMapping$curve.name = toupper( properCurveNames )
        
        allMappings = rbind( allMappings, newMapping )
        rTrace("")
    }
    
    return( allMappings )
}


################################################################################
# take the trades with multiple contract months and break them apart into
# their individual pieces
#
.getExpandedTradeData <- function( unexpandedTrades )
{
    rLog("Mapping Trades To Contract Months...")
    
    expandedTradeData = NULL
    
    for( rowIndex in seq_len( nrow( unexpandedTrades ) ) )
    {
        trade = unexpandedTrades[ rowIndex, ]
        
        # Create a new trade for each tenor of the trade
        for( x in seq_len( trade$numMonths ) ) 
        {
            rTrace( "Trade:", rowIndex, "Leg:", x, "of", trade$numMonths )
            
            newTrade = trade 
            newTrade$numMonths = 1
            
            # calculate the new contract month for this piece of the trade
            dateRule = paste( "+", x-1, "mJ", sep="" )
            startOfMonth = secdb.dateRuleApply( newTrade$contract.dt, dateRule ) 
            newTrade$contract.dt = as.Date( startOfMonth )
            
            # calculate the real quantity of this piece of the trade
            commodity = strsplit( newTrade$curve.name, " ")[[1]][2]
            if( substr( commodity, 1, 2 ) == "PW" )
            {
                # PWR VALUE = QUANTITY * BUCKET'S HOURS IN GIVEN MONTH
                multiplier = .getHoursForMonth( newTrade$curve.name, 
                        newTrade$contract.dt )
                
            } else if( commodity == "NG" )
            {
                # NG VALUE = QUANTITY * DAYS IN MONTH
                multiplier = .getDaysInMonth( newTrade$contract.dt )
                
            } else 
            {
                multiplier = 1
                rWarn( "Unknown Commodity Conversion:", commodity )    
            }
            
            newTrade$value = multiplier * newTrade$originalQuantity
            
            expandedTradeData = rbind( expandedTradeData, newTrade )
        }
    }
    
    return( expandedTradeData )
}


################################################################################
.getHoursForMonth <- function( curve, month )
{
    market = secdb.getValueType( curve, "Market Symbol" )
    bucket = secdb.getValueType( curve, "Quantity Bucket Type" )
    region = secdb.getValueType( curve, "Asset ID" )
    
    endDate = as.Date( secdb.dateRuleApply( month, "e" ) )
    
    hoursInBucket = nrow( secdb.getHoursInBucket( region, bucket, month, endDate ) )
    
    return( hoursInBucket )
}


################################################################################
.getDaysInMonth <- function( month )
{
    endDate = as.Date( secdb.dateRuleApply( month, "e" ) )
    
    difference = difftime( endDate, month, units = "days" )
    
    return( as.numeric( difference ) )
}




