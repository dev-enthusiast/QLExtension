###############################################################################
# PositionEngineVCentral.R
# 
# Author: e14600
#
library( RODBC )
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
PositionEngineVCentral = new.env(hash=TRUE)


################################################################################
# Data cache - stores the VCentral data so you can rerun without
# reloading the whole file. See the getBasePositionSet function for details.
#
PositionEngineVCentral$cache = new.env(hash=TRUE)


################################################################################
# Returns a data frame of ETIs with MTM adjustments
# 
PositionEngineVCentral$calculateMTMAdjustments <- function( date, 
        powerFactor = 1, gasFactor = .1, coalFactor = 1, filter="" ) 
{
    data = .getBasePositionSet( date )
    
    if( filter != "" )
    {
        subset = which( eval( parse( text=filter ) ) )
        data = data[subset, , drop=FALSE]
    }

    data$MTM_ADJUSTMENT = 0
    
    powerCurves = .getPowerCurves( data )
    data$MTM_ADJUSTMENT[ powerCurves ] = data$MTM_ADJUSTMENT[ powerCurves ] +
            ( data$DELTA[ powerCurves ] * powerFactor ) 
    
    ngCurves =  .getGasCurves( data )
    data$MTM_ADJUSTMENT[ ngCurves ] = data$MTM_ADJUSTMENT[ ngCurves ] +
            ( data$DELTA[ ngCurves ] * gasFactor ) 
    
    coalCurves = c( .getInternationalCoalCurves( data ), 
            .getDomesticCoalCurves( data ) )
    data$MTM_ADJUSTMENT[ coalCurves ] = data$MTM_ADJUSTMENT[ coalCurves ] +
            ( data$DELTA[ coalCurves ] * coalFactor ) 
    
    finalData = aggregate( data$MTM_ADJUSTMENT, list(data$ETI ), 
            sum, na.rm=TRUE )
    names( finalData ) = c( "ETI", "MTM_ADJUSTMENT" )
    
    return( finalData )
}


################################################################################
PositionEngineVCentral$getAllPositionTable <- function( date )
{
    if( !is.null( PositionEngineVCentral$cache$allPositions ) )
    {
        return( PositionEngineVCentral$cache$allPositions )
    }
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            date, "/vcvAllPositionTable.RData", sep="" )
    
    if( file.exists( inputFile ) )
    {
        allPositions = varLoad("allPositions", inputFile )
        PositionEngineVCentral$cache$allPositions = allPositions
        return( PositionEngineVCentral$cache$allPositions )
    }
    
    positionData = .getBasePositionSet( date )
    
    # Build a position data set to merge in later
    positionTable = .createPositionTable( positionData )
    
    # Build a position data for each year to merge in later
    years = format( seq( as.Date("2009-01-01"), 
                    as.Date("2013-01-01"), by="year" ), "%Y" ) 
    allPositions = positionTable
    for( currentYear in years )
    {
        positionsForYear = which( positionData$YEAR == currentYear )
        positionTableForYear = .createPositionTable( positionData, 
                rowsToInclude = positionsForYear )
        
        allPositions = merge( allPositions, positionTableForYear, 
                by="ETI", suffixes = c("", paste(".", currentYear, sep="") ), all=TRUE )
    }
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( allPositions, file=inputFile )
    PositionEngineVCentral$cache$allPositions = allPositions
    
    return( allPositions )
}


################################################################################
.getBasePositionSet <- function( date )
{
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            date, "/vcvPositionsByETI.RData", sep="" )
    
    if( !is.null( PositionEngineVCentral$cache$positionData ) )
    {
        positions = PositionEngineVCentral$cache$positionData 
    }    
    else if( file.exists( inputFile ) )
    {
        positions = varLoad("positions", inputFile )
    }
    else
    {
        positions = .readPositionsFromDatabase( date )

        dir.create( dirname( inputFile ), showWarnings=FALSE )
        save( positions, file=inputFile )
    }
    
    # Set the cache value to the UNFILTERED positions
    PositionEngineVCentral$cache$positionData = positions
    
    bookList = .getIncludeOnlyBookList()
    if( !is.null( bookList ) )
    {
        positions = subset( positions, BOOK %in% bookList )
    }
    
    return( positions )
}


################################################################################
.readPositionsFromDatabase <- function( date )
{
    excludedBooks = .getExcludedBookList()
    
    query = paste(" SELECT 
                    externaldealid ETI, 
                    UPPER(profit_center_1) BOOK, 
                    UPPER(market_family) COMMODITY, 
                    upper(service_type) TYPE, 
                    upper(flow_type) BUCKET,
                    upper(location_nm) CURVE, 
                    valuation_month TENOR, 
                    qty_uom UNIT,
                    SUM (delta) DELTA
                    FROM vcv.app_positions_detail
                    WHERE valuation_date = '", format( date, "%d%b%y" ), "'
                    GROUP BY externaldealid, profit_center_1, market_family, 
                    service_type, flow_type, location_nm, valuation_month, qty_uom", 
            sep="")
    
    file.dsn <- "VCTRLP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    rLog("Connecting to VCentral...")
    con <- odbcDriverConnect(connection.string)  
    data = sqlQuery( con, query )
    odbcClose( con )
    
    # Remove excluded books as there are too many for the query
    data = data[-which(data$BOOK %in% excludedBooks),]
    
    TON_CF = 1/1.10231
    THERM_CF = 1/10
    
    data$DELTA[ which( data$UNIT=="TONS" ) ] = 
            data$DELTA[ which( data$UNIT=="TONS" ) ] * TON_CF 
    data$DELTA[ which( data$UNIT=="TON" ) ] = 
            data$DELTA[ which( data$UNIT=="TON") ] * TON_CF 
    data$DELTA[ which( data$UNIT=="THERM" ) ] = 
            data$DELTA[ which( data$UNIT=="THERM" ) ] * THERM_CF 
    
    data$UNIT = NULL
    data$YEAR = format( data$TENOR, "%Y" )
    
    return( data )
}


################################################################################
.getExcludedBookList <- function()
{
    query = "SELECT BOOK
            FROM bookmgr.rmg_bookmappings
            WHERE RISK_REPORTING_FLAG = 'N'
            AND upper(CER_GROUP_2) NOT IN ('DISC OPS DOWNSTREAM', 
            'DISC OPS UK', 'DISC OPS UK MIRROR')
            ORDER BY BOOK"
    
    file.dsn <- "GRIZZLYP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=bookmgr_read;PWD=bookmgr_read;", sep="")  
    rLog("Connecting to Grizzly...")
    con <- odbcDriverConnect(connection.string)  
    
    excludedBooks = sqlQuery( con, query )$BOOK
    odbcClose(con)
    
    return( as.character( excludedBooks ) ) 
}


################################################################################
# Included here to support runs for only a subset of books
#
.getIncludeOnlyBookList <- function()
{
    return( NULL )
}


################################################################################
.getPowerCurves <- function( positionData )
{
    powerCurves = which( positionData$COMMODITY == "POWER" &
                    positionData$TYPE == "ENERGY")
    
    return( powerCurves )
}


################################################################################
.getPeakPowerCurves <- function( positionData )
{
    powerCurves = which( positionData$COMMODITY == "POWER" &
                    positionData$TYPE == "ENERGY" &
                    positionData$BUCKET == "PEAK" )
    
    return( powerCurves )
}


################################################################################
.getOffPeakPowerCurves <- function( positionData )
{
    powerCurves = which( positionData$COMMODITY == "POWER" &
                    positionData$TYPE == "ENERGY" &
                    positionData$BUCKET == "OFFPEAK" )
    
    return( powerCurves )
}


################################################################################
.getGasCurves <- function( positionData )
{
    gasCurves = which( positionData$COMMODITY == "NATGAS" )
    return( gasCurves )
}


################################################################################
.getInternationalCoalCurves <- function( positionData )
{
    icCurves = grep( "COMMOD FCO", positionData$CURVE )
    return( icCurves )
}


################################################################################
.getDomesticCoalCurves <- function( positionData )
{
    dcCurves = grep( "COMMOD COL", positionData$CURVE )
    return( dcCurves )
}


################################################################################
.getCrudeCurves <- function( positionData )
{
    dcCurves = grep( "COMMOD WTI", positionData$CURVE )
    return( dcCurves )
}


################################################################################
.createPositionTable <- function( positionData, 
        rowsToInclude = seq_len(nrow(positionData)) )
{
    peakPowerCurves = .getPeakPowerCurves( positionData )
    offpeakPowerCurves = .getOffPeakPowerCurves( positionData )
    gasCurves = .getGasCurves( positionData )
    internationalCoalCurves = .getInternationalCoalCurves( positionData )
    domesticCoalCurves = .getDomesticCoalCurves( positionData )
    crudeCurves = .getCrudeCurves( positionData )
    positionData$COMMODITY_GRP = NA
    positionData$COMMODITY_GRP[peakPowerCurves] = "POWER PEAK"
    positionData$COMMODITY_GRP[offpeakPowerCurves] = "POWER OFFPEAK"
    positionData$COMMODITY_GRP[gasCurves] = "GAS"
    positionData$COMMODITY_GRP[internationalCoalCurves] = "INTERNATIONAL COAL"
    positionData$COMMODITY_GRP[domesticCoalCurves] = "DOMESTIC COAL"
    positionData$COMMODITY_GRP[crudeCurves] = "CRUDE"
    positionData$COMMODITY_GRP = as.factor( positionData$COMMODITY_GRP )
    
    colsToKeep = c( 'ETI', 'COMMODITY_GRP', 'DELTA' )
    positionTable = positionData[ rowsToInclude, colsToKeep ]
    
    positionTable = melt( positionTable, id = c("ETI", "COMMODITY_GRP") )
    
    positionTable = cast( positionTable, 
            ETI ~ COMMODITY_GRP,
            sum, na.rm = TRUE, fill=0 )
    
    # As we may not have all of the columns available, make sure you only 
    # subset the data frame on the included names
    namesToKeep = c('ETI', 'POWER PEAK', 'POWER OFFPEAK', 'GAS', 
            'INTERNATIONAL COAL', 'DOMESTIC COAL', 'CRUDE')
    columnsIndiciesToKeep = which( names(positionTable) %in% namesToKeep )
    availableToFilter = names(positionTable)[ columnsIndiciesToKeep ]
    
    positionTable = positionTable[, availableToFilter ]
    
    return( positionTable )
}


################################################################################
PositionEngineVCentral$positionsByTenor <- function( asOfDate,
        rowsToInclude = seq_len(nrow(positionData)) )
{
    positionData = .getBasePositionSet( asOfDate )
    
    peakPowerCurves = .getPeakPowerCurves( positionData )
    offpeakPowerCurves = .getOffPeakPowerCurves( positionData )
    gasCurves = .getGasCurves( positionData )
    internationalCoalCurves = .getInternationalCoalCurves( positionData )
    domesticCoalCurves = .getDomesticCoalCurves( positionData )
    crudeCurves = .getCrudeCurves( positionData )
    positionData$COMMODITY_GRP = NA
    positionData$COMMODITY_GRP[peakPowerCurves] = "POWER PEAK"
    positionData$COMMODITY_GRP[offpeakPowerCurves] = "POWER OFFPEAK"
    positionData$COMMODITY_GRP[gasCurves] = "GAS"
    positionData$COMMODITY_GRP[internationalCoalCurves] = "INTERNATIONAL COAL"
    positionData$COMMODITY_GRP[domesticCoalCurves] = "DOMESTIC COAL"
    positionData$COMMODITY_GRP[crudeCurves] = "CRUDE"
    positionData$COMMODITY_GRP = as.factor( positionData$COMMODITY_GRP )
    
    colsToKeep = c( 'ETI', 'COMMODITY_GRP', 'TENOR', 'DELTA' )
    positionTable = positionData[ rowsToInclude, colsToKeep ]
    
    positionTable = melt( positionTable, id = c("ETI", "COMMODITY_GRP", "TENOR") )
    
    positionTable = cast( positionTable, 
            ETI + COMMODITY_GRP + TENOR~.,
            sum, na.rm = TRUE, fill=0 )
    names(positionTable) = colsToKeep
    
    return( positionTable )
}

