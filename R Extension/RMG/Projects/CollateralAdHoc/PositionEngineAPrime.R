###############################################################################
# PositionEngineAPrime.R
# 
# Author: e14600
#
library( RODBC )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )


################################################################################
# File Namespace
#
PositionEngineAPrime = new.env(hash=TRUE)


################################################################################
# Data cache - stores the A' data so you can rerun the main function without
# reloading the whole file. See the getAPrimeData function for details.
#
PositionEngineAPrime$cache = new.env(hash=TRUE)


################################################################################
# Returns a data frame of ETIs with MTM adjustments
# 
PositionEngineAPrime$calculateMTMAdjustments <- function( date, 
        powerFactor = 1, gasFactor = .1, coalFactor = 1 ) 
{
    data = .getBasePositionSet( date )
    
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
PositionEngineAPrime$getAllPositionTable <- function( date )
{
    if( !is.null( PositionEngineAPrime$cache$allPositions ) )
    {
        return( PositionEngineAPrime$cache$allPositions )
    }
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            date, "/aprimeAllPositionTable.RData", sep="" )
    
    if( file.exists( inputFile ) )
    {
        allPositions = varLoad("allPositions", inputFile )
        PositionEngineAPrime$cache$allPositions = allPositions
        return( PositionEngineAPrime$cache$allPositions )
    }
    
    positionData = .getBasePositionSet( date )
    
    # Build a position data set to merge in later
    positionTable = .createPositionTable( positionData )
    
    # Build a position data set out through 2013 to merge in later
    positionsThrough2013 = which(as.Date(positionData$TENOR) < as.Date("2014-01-01"))
    positionTable2013 = .createPositionTable( positionData, 
            rowsToInclude = positionsThrough2013 )
    
    allPositions = merge( positionTable, positionTable2013, 
            by="ETI", suffixes = c("", ".2013"), all=TRUE )
    
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( allPositions, file=inputFile )
    PositionEngineAPrime$cache$allPositions = allPositions
    
    return( allPositions )
}


###############################################################################
.getBasePositionSet <- function( date )
{
    if( !is.null( PositionEngineAPrime$cache$positionData ) )
    {
        return( PositionEngineAPrime$cache$positionData )
    }
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            date, "/aprimePositionsByETI.RData", sep="" )
    
    if( file.exists( inputFile ) )
    {
        data = varLoad("data", inputFile )
        PositionEngineAPrime$cache$positionData = data
        
        return( PositionEngineAPrime$cache$positionData )
    }
    
    excludedBooks = .getExcludedBookList()
    
    query = paste(" SELECT 
                    externaldealid ETI, 
                    upper(commod_curve) CURVE,
                    contractmonth TENOR,
                    UPPER(commodity_grp) COMMODITY_GRP, 
                    SUM (pos_adj) DELTA
                    FROM aprime_base
                    WHERE asofdate = '", format( date, "%d%b%y" ), "' ",
            "and BOOK_NAME not in ('", paste(excludedBooks, collapse="','"), "') 
                    GROUP BY externaldealid, commod_curve, contractmonth,
                    commodity_grp", sep="")
    
    file.dsn <- "RMGDBD.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=rmgdbuser;PWD=rmgdbpwd;", sep="")  
    rLog("Connecting to RMGDBD...")
    con <- odbcDriverConnect(connection.string)  
    data = sqlQuery( con, query )
    odbcClose( con )
    
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( data, file=inputFile )
    PositionEngineAPrime$cache$positionData = data
    
    return( PositionEngineAPrime$cache$positionData )
}


################################################################################
.getExcludedBookList <- function()
{
    # Get a list of PM books to exclude from the position set
    pmQuery = "select * from vcv.pm_excluded_books"
    
    file.dsn <- "VCTRLP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    rLog("Connecting to VCentral...")
    con <- odbcDriverConnect(connection.string)  
    pmExcludedBooks = sqlQuery(con, pmQuery)$PROFIT_CENTER_1
    odbcClose(con)
    
    return( as.character( pmExcludedBooks ) ) 
}


################################################################################
.getPowerCurves <- function( positionData )
{
    powerCurves = which( positionData$COMMODITY_GRP == "POWER" )
    
    return( powerCurves )
}


################################################################################
.getGasCurves <- function( positionData )
{
    gasCurves = which( positionData$COMMODITY_GRP == "GAS" )
    return( gasCurves )
}


################################################################################
.getInternationalCoalCurves <- function( positionData )
{
    icCurves = which( positionData$COMMODITY_GRP == "INTERNATIONAL COAL" )
    return( icCurves )
}


################################################################################
.getDomesticCoalCurves <- function( positionData )
{
    dcCurves = which( positionData$COMMODITY_GRP == "DOMESTIC COAL" )
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
    powerCurves = .getPowerCurves( positionData )
    gasCurves = .getGasCurves( positionData )
    internationalCoalCurves = .getInternationalCoalCurves( positionData )
    domesticCoalCurves = .getDomesticCoalCurves( positionData )
    crudeCurves = .getCrudeCurves( positionData )
    positionData$COMMODITY_GRP = NA
    positionData$COMMODITY_GRP[powerCurves] = "POWER"
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
    
    positionTable = positionTable[, c('ETI', 'POWER', 'GAS', 
                    'INTERNATIONAL COAL', 'DOMESTIC COAL', 'CRUDE')]
    
    return( positionTable )
}





