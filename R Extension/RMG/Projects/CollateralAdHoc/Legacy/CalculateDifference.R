###############################################################################
# CalculateDifference.R
#
# Author: e14600
#
memory.limit( 4095 )
require( lattice )
require( reshape )
require( RODBC )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")


################################################################################
# Constants
APRIME_DATA = "S:/Risk/Temporary/CollateralAllocation/20081104/SourceData/AllPos_AGMTHScale_preibt_04NOV08.csv"
MEGA_REPORT = "S:/Risk/Temporary/CollateralAllocation/20081104/Exposure by Agreement Mega Rpt COB 2008-11-04 Prod.csv"
CP_ALIAS_MAP = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/alias_map.csv"


################################################################################
# Data Cache - stores data so you can rerun the main function without
# repulling from Raft or recalculating position sets.
#
cache = new.env(hash=TRUE)


################################################################################
convertExcelNumber <- function( x )
{
    cleanData = gsub( "(", "-", x, fixed=TRUE)
    cleanData = gsub( ")", "", cleanData, fixed=TRUE)
    cleanData = gsub( ",", "", cleanData, fixed=TRUE)
    cleanData = gsub( "$", "", cleanData, fixed=TRUE)
    cleanData = as.numeric( cleanData )
    
    return( cleanData )
}


################################################################################
getAPrimeData <- function()
{
    if( !is.null( cache$data ) )
    {
        return( cache$data )
    }
    
    results = read.csv( APRIME_DATA )
    names( results ) = toupper( names( results ) )
    results = results[ , sort( names( results ) ) ]
    
    results$EXPOSURE =  convertExcelNumber( results$EXPOSURE )
    results$MTM =  convertExcelNumber( results$MTM )
    results$CREDIT_CASHHELD = convertExcelNumber( results$CREDIT_CASHHELD )
    results$CREDIT_LCHELD = convertExcelNumber( results$CREDIT_LCHELD )
    results$CREDIT_CASHPOST = convertExcelNumber( results$CREDIT_CASHPOST )
    results$CREDIT_LCPOST = convertExcelNumber( results$CREDIT_LCPOST )
    
    cache$data = results
    
    return( cache$data )
}


################################################################################
getExcludedBookList <- function()
{
    # Get a list of PM books to exclude from the position set
    pmQuery = "select * from vcv.pm_excluded_books"
    
    con = odbcConnect("VCTRLP", "vcentral_read", "vcentral_read")
    pmExcludedBooks = sqlQuery(con, pmQuery)$PROFIT_CENTER_1
    odbcClose(con)
    
    return( pmExcludedBooks )
}


################################################################################
calculateMoveByFactor <- function( data, 
        powerFactor = 1, gasFactor = .1, coalFactor = 1 ) 
{
    adjustedMTM = data$MTM
    
    positions = data$POSITION
    
    powerCurves = which( data$COMMODITY_GRP == "POWER" )
    adjustedMTM[ powerCurves ] = adjustedMTM[ powerCurves ] +
            ( positions[ powerCurves ] * powerFactor) 
    
    ngCurves = which( data$COMMODITY_GRP == "GAS" )
    adjustedMTM[ ngCurves ] = adjustedMTM[ ngCurves ] +
            ( positions[ ngCurves ] * gasFactor ) 
    
    coalCurves = which( data$COMMODITY_GRP == "INTERNATIONAL COAL" | 
                    data$COMMODITY_GRP == "DOMESTIC COAL" )
    adjustedMTM[ coalCurves ] = adjustedMTM[ coalCurves ] +  
            ( positions[ coalCurves ] * coalFactor ) 
    
    return( adjustedMTM )
}


################################################################################
createPositionTable <- function( newData, 
        rowsToInclude = seq_len(nrow(newData)) )
{
    ID_COLUMNS = c('COUNTERPARTY','CREDIT_NETTINGAGREEMENT', 'COMMODITY_GRP')
    
    colsToKeep = c( ID_COLUMNS, 'POSITION' )
    positionTable = newData[ rowsToInclude, colsToKeep ]
    
    positionTable = melt( positionTable, id = ID_COLUMNS )
    
    positionTable = cast( positionTable, 
            COUNTERPARTY + CREDIT_NETTINGAGREEMENT ~ COMMODITY_GRP,
            sum, na.rm = TRUE, fill=0 )
    
    positionTable = positionTable[, c('COUNTERPARTY', 'CREDIT_NETTINGAGREEMENT',
                    'POWER', 'GAS', 'INTERNATIONAL COAL', 'DOMESTIC COAL')]
    
    return( positionTable )
}


################################################################################
getMegaData <- function()
{
    results = read.csv( MEGA_REPORT )
    names( results ) = toupper( names( results ) )
    results = results[ , sort( names( results ) ) ]
    
    return( results )
}


################################################################################
createCollateralTable <- function( megaData )
{
    collateralTable = megaData[ , c('AGREEMENT.ACTIVE.','ARAP','CASH.HELD',
                    'CASH.POSTED','CEG.THRESHOLD.AMT.USED..USD.',
                    'COUNTERPARTY','CPTY.ACTIVE.',
                    'CPTY.THRESHOLD.AMT.USED..USD.','CSA','LC.HELD','LC.POSTED',
                    'MTM','MTMPLUSACCRUALPLUSARAP','NETTING.AGREEMENT',
                    'SCREDITOFFICER','SHORTNAME','SPARTYTYPE') ]
    
    names( collateralTable ) =  c('AGREEMENT_ACTIVE','ARAP','CREDIT_CASHHELD',
            'CREDIT_CASHPOST','CEG_THRESHOLD','LONG_NAME','CPTY_ACTIVE',
            'COUNTERPARTY_THRESHOLD','CSA','CREDIT_LCHELD','CREDIT_LCPOSTED',
            'MTM','EXPOSURE','CREDIT_NETTINGAGREEMENT',
            'CREDIT_OFFICER','COUNTERPARTY','CP_TYPE')
    
    collateralTable = collateralTable[ , sort(names(collateralTable))]
    
    return( collateralTable )
}


################################################################################
calculateCollateral <- function( EXPOSURE=NA, LC_POST=NA, LC_HELD=NA, 
        CASH_POST=NA, CASH_HELD=NA, CEG_THRESHOLD=NA, COUNTERPARTY_THRESHOLD=NA )
{   
    cashFlow = rep( NA, length( EXPOSURE ) )
    
    EXPOSURE[ which(is.na(EXPOSURE)) ] = 0 
    LC_POST[ which(is.na(LC_POST)) ] = 0 
    LC_HELD[ which(is.na(LC_HELD)) ] = 0 
    CASH_POST[ which(is.na(CASH_POST)) ] = 0 
    CASH_HELD[ which(is.na(CASH_HELD)) ] = 0 
    CEG_THRESHOLD[ which(is.na(CEG_THRESHOLD)) ] = 0 
    COUNTERPARTY_THRESHOLD[ which(is.na(COUNTERPARTY_THRESHOLD)) ] = 0 
    
    # Case 1 - ITM, No Cash Held by Us
    itmNoCashHeld = which( ( is.na(CASH_HELD) | CASH_HELD == 0 ) & EXPOSURE >= 0 ) 
    cashFlow[ itmNoCashHeld ] = 0
    
    # Case 2 - ITM, Cash Held By Us
    itmCashHeld = which( !is.na(CASH_HELD) & CASH_HELD != 0 & EXPOSURE >= 0 ) 
    if( length( itmCashHeld ) )
    {
        itmCashFlow = rowSums( cbind( 
                        EXPOSURE[itmCashHeld], 
                        COUNTERPARTY_THRESHOLD[itmCashHeld] * -1 ), 
                na.rm=TRUE )
        finalCashFlow = pmax( itmCashFlow, 0 )
        
        cashFlow[ itmCashHeld ] = finalCashFlow
        
        giveBackCash = which( ( !is.na(CASH_HELD) ) & ( CASH_HELD != 0 ) & 
                        ( EXPOSURE >= 0 ) & 
                        ( (EXPOSURE-COUNTERPARTY_THRESHOLD) <= 0 ) ) 
        
        if( length( giveBackCash ) )
        {
            cashFlow[giveBackCash] = cashFlow[giveBackCash] - 
                                CASH_HELD[giveBackCash]
        }
        
    }
    
    
    # Case 3 - OTM
    otmRows = which( EXPOSURE < 0 ) 
    if( length( otmRows ) )
    {
        otmCashFlow = rowSums( cbind( 
                        EXPOSURE[otmRows], 
                        CEG_THRESHOLD[otmRows] ), 
                na.rm=TRUE )
        
        finalCashFlow = pmin( otmCashFlow, 0 )
        
        # If we're holding their cash too when we're OTM, we need to give it back
        finalCashFlow = rowSums( cbind( finalCashFlow, CASH_HELD[otmRows]*-1),
                na.rm=TRUE )
        
        cashFlow[ otmRows ] = finalCashFlow
    }
    
    
    return( cashFlow )
}


################################################################################
# NEW VERSION
# 
calculateCollateral.new <- function( EXPOSURE=NA, LC_POST=NA, LC_HELD=NA, 
        CASH_POST=NA, CASH_HELD=NA, CEG_THRESHOLD=NA, COUNTERPARTY_THRESHOLD=NA )
{   
    cashFlow = rep( NA, length( EXPOSURE ) )
    
    collateralData = data.frame( cbind( 
                    EXPOSURE, LC_POST, LC_HELD, CASH_POST, CASH_HELD, 
                    CEG_THRESHOLD, COUNTERPARTY_THRESHOLD ) )
    names(collateralData) = c("EXPOSURE", "LC_POST", "LC_HELD", "CASH_POST", 
            "CASH_HELD", "CEG_THRESHOLD", "COUNTERPARTY_THRESHOLD")
    
    collateralData$EXPOSURE[ which(is.na(collateralData$EXPOSURE)) ] = 0 
    collateralData$LC_POST[ which(is.na(collateralData$LC_POST)) ] = 0 
    collateralData$LC_HELD[ which(is.na(collateralData$LC_HELD)) ] = 0 
    collateralData$CASH_POST[ which(is.na(collateralData$CASH_POST)) ] = 0 
    collateralData$CASH_HELD[ which(is.na(collateralData$CASH_HELD)) ] = 0 
    collateralData$CEG_THRESHOLD[ which(is.na(collateralData$CEG_THRESHOLD)) ] = 0 
    collateralData$COUNTERPARTY_THRESHOLD[ which(is.na(collateralData$COUNTERPARTY_THRESHOLD)) ] = 0 
    
    # Case 1 - ITM
    itmRows = which( EXPOSURE >= 0 ) 
    if( length( itmRows ) )
    {
        weHold = rowSums( na.rm=TRUE, cbind(
                        collateralData$EXPOSURE[itmRows],
                        collateralData$LC_HELD[itmRows] * -1,
                        collateralData$CASH_HELD[itmRows] * -1, 
                        collateralData$COUNTERPARTY_THRESHOLD[itmRows] * -1) )
        
        lcTheoretical = pmax( weHold, 0 ) + collateralData$LC_HELD[itmRows]
        
        overThreshold = rowSums( na.rm=TRUE, cbind( 
                        collateralData$EXPOSURE[itmRows],
                        lcTheoretical * -1, 
                        collateralData$COUNTERPARTY_THRESHOLD[itmRows] * -1  ) )
        
        cashBalance = pmax( 0, overThreshold )
        
        cashChange = rowSums( na.rm=TRUE, cbind( 
                        cashBalance, 
                        collateralData$CASH_HELD[itmRows] * -1, 
                        collateralData$CASH_POST[itmRows] ) )
        
        cashFlow[ itmRows ] = cashChange
    }
    
    # Case 2 - OTM
    otmRows = which( EXPOSURE < 0 ) 
    if( length( otmRows ) )
    {
#        theyHold = rowSums( na.rm=TRUE, cbind( 
#                abs( collateralData$EXPOSURE[otmRows] ),
#                collateralData$LC_POST[otmRows] * -1,
#                collateralData$CASH_POST[otmRows] * -1, 
#                collateralData$CEG_THRESHOLD[otmRows] * -1 ) )
#        
#        lcTheoretical = pmax( theyHold, 0 ) + collateralData$LC_POST[otmRows]
        
        overThreshold = rowSums( na.rm=TRUE, cbind( 
                        abs( collateralData$EXPOSURE[otmRows] ),
#                lcTheoretical * -1, 
                        collateralData$CEG_THRESHOLD[otmRows] * -1 ) )
        
        liquidBalance = pmax( 0, overThreshold )
        
        liquidChange = rowSums( na.rm=TRUE, cbind( 
                        collateralData$LC_POST[otmRows], 
                        collateralData$CASH_POST[otmRows], 
                        liquidBalance * -1,
                        collateralData$CASH_HELD[otmRows] * -1 ) )
        
        cashFlow[ otmRows ] = liquidChange
    }
    
    
    return( cashFlow )
}


.testCalculateCollateralITM <- function()
{
    library(RUnit)
    
    #ITM No Cash Present, 0 Cash Held Case
    collateral = calculateCollateral( EXPOSURE=5, 
            CASH_HELD = 0, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 0, collateral )
    
    #ITM No Cash Present, NA Cash Held Case
    collateral = calculateCollateral( EXPOSURE=5, 
            CASH_HELD = NA, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 0, collateral )
    
    #'ITM' No Cash Present, 0 Exposure Case
    collateral = calculateCollateral( EXPOSURE=0, 
            CASH_HELD = 0, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 0, collateral )
    
    #'ITM' Cash Present, 0 Exposure Case
    collateral = calculateCollateral( EXPOSURE=0, 
            CASH_HELD = 1, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -1, collateral )
    
    #'ITM' Cash Present, 1 Exposure Case
    collateral = calculateCollateral( EXPOSURE=1, 
            CASH_HELD = 1, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -1, collateral )
    
    #'ITM' Cash Present, 2 Exposure Case
    collateral = calculateCollateral( EXPOSURE=2, 
            CASH_HELD = 1, LC_HELD = 1, COUNTERPARTY_THRESHOLD = 1,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 1, collateral )
    
}

.testCalculateCollateralOTM <- function()
{
    library(RUnit)
    
    #OTM, 0 Cpty Cash Held Case, 0 Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 0, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -5, collateral )
    
    #OTM, 0 Cpty Cash Held Case, NA Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 0, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = NA, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -5, collateral )
    
    #OTM, 0 Cpty Cash Held Case, 1 Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 0, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 1, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -4, collateral )
    
    #OTM, 0 Cpty Cash Held Case, Threshold = Exp
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 0, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 5, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 0, collateral )
    
    #OTM, 0 Cpty Cash Held Case, Threshold > Exp
    collateral = calculateCollateral( EXPOSURE=-1, 
            CASH_HELD = 0, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 5, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = 0, collateral )
    
    #OTM, 1 Cpty Cash Held Case, 0 Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 1, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -6, collateral )
    
    #OTM, 1 Cpty Cash Held Case, NA Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 1, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = NA, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -6, collateral )
    
    #OTM, 1 Cpty Cash Held Case, 1 Threshold
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 1, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 1, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -5, collateral )
    
    #OTM, 1 Cpty Cash Held Case, Threshold = Exp
    collateral = calculateCollateral( EXPOSURE=-5, 
            CASH_HELD = 1, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 5, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -1, collateral )
    
    #OTM, 1 Cpty Cash Held Case, Threshold > Exp
    collateral = calculateCollateral( EXPOSURE=-1, 
            CASH_HELD = 1, LC_HELD = 0, COUNTERPARTY_THRESHOLD = 0,
            CEG_THRESHOLD = 5, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -1, collateral )
    
    collateral = calculateCollateral( EXPOSURE=-1, 
            CASH_HELD = 26, LC_HELD = 32, COUNTERPARTY_THRESHOLD = 10,
            CEG_THRESHOLD = 0, LC_POST = 0, CASH_POST = 0 )
    checkEquals( target = -27, collateral )
    
}

.testCalculateCollateralITM()

.testCalculateCollateralOTM()


################################################################################
## Plan of attack:
##         Read in the A' data set
##         Recalculate the MTM number
##         Recalculate the Exposure Number
##         Aggregate A' by shortname and agreement		
##         Read in the Mega Report
##         Create a collateral table by counterparty & agreement
##         Merge A' and the mega report collateral data
##         Calculate the 'original' collateral number
##         Calculate the new collateral number
##         Write out a difference in MTM, Exposure and Collateral.
##         Write out a mega report with all these columns
##             Mega report needs columns for 'coal', 'ng', 'power' position
##
main <- function( powerFactor, gasFactor, coalFactor, outputFile )
{
    #Read in the A' data set and setup the new data to recalculate with
    newData = getAPrimeData()
    colsToKeep = c(
            'BOOK_NAME', 'COUNTERPARTY',
            'CREDIT_CSA','CREDIT_NETTINGAGREEMENT', 'CREDIT_CSA_DE_AA',
            'EXPOSURE','MTM', 'POSITION', 'COMMODITY_GRP', 
            'COMMOD_CURVE', 'CONTRACTMONTH', 'DNP', 'IBT_FLAG')
    newData = newData[, colsToKeep ]
    
    # remove flagged IBTs
    newData = newData[which(newData$IBT_FLAG!="Yes"),]
    newData$IBT_FLAG = NULL
    
    # Get the list of aliased counterparties and apply it to the data set
    aliasMap = read.csv( CP_ALIAS_MAP )
    aliasMap$name = toupper( aliasMap$name )
    aliasMap$alias = toupper( aliasMap$alias )
    newData$COUNTERPARTY = toupper( newData$COUNTERPARTY )
    newData = merge( newData, aliasMap, by.x = "COUNTERPARTY", by.y="name", all.x=TRUE)
    newData$COUNTERPARTY[which(!is.na(newData$alias))] = newData$alias[which(!is.na(newData$alias))]
    newData$alias = NULL
    
    # Get a list of books to exclude from the position set
    # and remove both sides where the excluded books are present
    excludedBooks = getExcludedBookList()
    newData = newData[-which(newData$BOOK_NAME %in% excludedBooks),]
    
    # Read in the Mega Report
    megaData = getMegaData()
    
    # Create a collateral table by counterparty & agreement
    collateralTable = createCollateralTable( megaData )
    
    
    # Clean up the missing entries
    newData$MTM[which(is.na(newData$MTM))] = 0
    newData$POSITION[which(is.na(newData$POSITION))] = 0
    newData$DNP[which(is.na(newData$DNP))] = 0
    newData$EXPOSURE[which(is.na(newData$EXPOSURE))] = 0
    newData$CONTRACTMONTH = as.Date( newData$CONTRACTMONTH, "%d%b%y")
    
    # Adjust the MTM number using the desired method
    newData$ADJUSTED_MTM = calculateMoveByFactor( newData, 
            powerFactor, gasFactor, coalFactor ) 
    
    
    # Recalculate the Exposure Number
    newData$ADJUSTED_EXPOSURE = newData$ADJUSTED_MTM + newData$DNP 
    
    
    # Aggregate A' by shortname and agreement
    exposureTable = newData[ , c('COUNTERPARTY','CREDIT_NETTINGAGREEMENT',
                    'EXPOSURE','MTM','ADJUSTED_EXPOSURE','ADJUSTED_MTM') ]
    exposureTable = melt( exposureTable, id=c('COUNTERPARTY','CREDIT_NETTINGAGREEMENT') )
    exposureTable = cast( exposureTable, COUNTERPARTY + CREDIT_NETTINGAGREEMENT ~ variable,
            sum, na.rm = TRUE )
    
    
    # Build a position data set to merge in later
    positionTable = createPositionTable( newData )
    
    # Build a position data set out through 2013 to merge in later
    positionsThrough2013 = which(newData$CONTRACTMONTH < as.Date("2014-01-01"))
    positionTable2013 = createPositionTable( newData, 
            rowsToInclude = positionsThrough2013 )
    
    
    # Merge exposure and position data
    finalTable = merge( exposureTable, positionTable, 
            by = c("COUNTERPARTY", "CREDIT_NETTINGAGREEMENT"), 
            all=TRUE )
    finalTable = finalTable[, sort(names(finalTable)) ]
    
    
    # And add in the 2013 data
    finalTable = merge( finalTable, positionTable2013, 
            by = c("COUNTERPARTY", "CREDIT_NETTINGAGREEMENT"), 
            all=TRUE, suffixes= c("", " THROUGH 2013") )
    finalTable = finalTable[, sort(names(finalTable)) ]
    
    
    # now merge in the collateral table from the mega report
    finalTable = merge( finalTable, collateralTable, 
            by = c("COUNTERPARTY", "CREDIT_NETTINGAGREEMENT"),
            suffixes = c(".APRIME", ".MEGARPT"), all = TRUE )
    finalTable = finalTable[, sort(names(finalTable)) ]
    
    
    # Calculate the 'original' collateral number
    finalTable$COLLATERAL = calculateCollateral( 
            finalTable$EXPOSURE.APRIME, 
            finalTable$CREDIT_LCPOSTED, finalTable$CREDIT_LCHELD, 
            finalTable$CREDIT_CASHPOST, finalTable$CREDIT_CASHHELD, 
            finalTable$CEG_THRESHOLD, finalTable$COUNTERPARTY_THRESHOLD )
    
    # Calculate the new collateral number
    finalTable$ADJUSTED_COLLATERAL = calculateCollateral( 
            finalTable$ADJUSTED_EXPOSURE, 
            finalTable$CREDIT_LCPOSTED, finalTable$CREDIT_LCHELD, 
            finalTable$CREDIT_CASHPOST, finalTable$CREDIT_CASHHELD, 
            finalTable$CEG_THRESHOLD, finalTable$COUNTERPARTY_THRESHOLD )
    
    
    # Write out a difference in MTM, Exposure and Collateral.
    finalTable$MTM_CHANGE = finalTable$ADJUSTED_MTM - finalTable$MTM.APRIME
    finalTable$EXPOSURE_CHANGE = finalTable$ADJUSTED_EXPOSURE - finalTable$EXPOSURE.APRIME
    finalTable$COLLATERAL_CHANGE = finalTable$ADJUSTED_COLLATERAL - finalTable$COLLATERAL
    
    # Set the MARGINED flag
    finalTable$MARGINED = finalTable$CSA != "" & !is.na(finalTable$CSA)
    
    
    counterpartiesToNeverMargin = c(
            'AEC','APMD','BGEMD','CCT','CILCOIL','CIPSIL','CL&P MILL','COMEDIL',
            'DPLDE','DPLMD','PENN PWR','PEPCOMD','PPL (PA)','PSE&G') 
    dontMargin = which(toupper( finalTable$COUNTERPARTY ) %in% counterpartiesToNeverMargin)
    finalTable$MARGINED[dontMargin] = FALSE
    
    agreementsToNeverMargin = c(
            'AGREEMENT 2098','AGREEMENT 4086','AGREEMENT 2214','AGREEMENT 1405',
            'AGREEMENT 2199','AGREEMENT 4251','AGREEMENT 4113' )
    dontMargin = which( toupper( finalTable$CREDIT_NETTINGAGREEMENT ) %in% agreementsToNeverMargin)
    finalTable$MARGINED[dontMargin] = FALSE
    
    finalTable$MARGINED[which( toupper( finalTable$COUNTERPARTY ) == "NYMEX" )] = TRUE
    
    
    # Write out a 'mega report' with all these columns
    mtmChange = sum ( finalTable$MTM_CHANGE, na.rm=TRUE )
    exposureChange = sum ( finalTable$EXPOSURE_CHANGE, na.rm=TRUE)
    marginedChange = sum ( finalTable$COLLATERAL_CHANGE[which(finalTable$MARGINED)], na.rm=TRUE)
    rLog( "MTM Change:", dollar( mtmChange ) ) 
    rLog( "Exposure Change:", dollar( exposureChange ) ) 
    rLog( "Margined Collateral Change:", dollar( marginedChange ) ) 
    
    
    finalTable = finalTable[, c(
                    'LONG_NAME','COUNTERPARTY','CP_TYPE','CPTY_ACTIVE',
                    'CREDIT_OFFICER',
                    'CSA','CREDIT_NETTINGAGREEMENT', 'AGREEMENT_ACTIVE',
                    'POWER', 'GAS', 'INTERNATIONAL COAL', 'DOMESTIC COAL',
                    'POWER THROUGH 2013', 'GAS THROUGH 2013', 
                    'INTERNATIONAL COAL THROUGH 2013', 'DOMESTIC COAL THROUGH 2013',
                    'CREDIT_CASHHELD','CREDIT_LCHELD',
                    'CREDIT_CASHPOST','CREDIT_LCPOSTED',
                    'CEG_THRESHOLD','COUNTERPARTY_THRESHOLD', 
                    'EXPOSURE.APRIME','EXPOSURE.MEGARPT',
                    'ADJUSTED_EXPOSURE','EXPOSURE_CHANGE',
                    'MTM.APRIME','MTM.MEGARPT','ADJUSTED_MTM','MTM_CHANGE',
                    'COLLATERAL','ADJUSTED_COLLATERAL','COLLATERAL_CHANGE', 
                    'MARGINED')]
    
    write.csv( finalTable, row.names=FALSE, file = outputFile )
    rLog( "Wrote output to file:", outputFile )
    
    invisible( NULL )
}


################################################################################
# Actually run the main function
#
main( powerFactor = 10, gasFactor = 2, coalFactor = 20, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/TenTwoTwentyNewUp.csv" )

main( powerFactor = -10, gasFactor = -2, coalFactor = -20, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/TenTwoTwentyNewDown.csv" )

main( powerFactor = 3.1, gasFactor = .46, coalFactor = 15, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/FourSigmaUp.csv" )

main( powerFactor = -3.1, gasFactor = -.46, coalFactor = -15, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/FourSigmaDown.csv" )

main( powerFactor = 1.55, gasFactor = .23, coalFactor = 7.5, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/TwoSigmaUp.csv" )

main( powerFactor = -1.55, gasFactor = -.23, coalFactor = -7.5, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/TwoSigmaDown.csv" )

main( powerFactor = 1.27, gasFactor = .19, coalFactor = 6.17, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/NinetyPercentUp.csv" )

main( powerFactor = -1.27, gasFactor = -.19, coalFactor = -6.17, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/NinetyPercentDown.csv" )

main( powerFactor = .78, gasFactor = .12, coalFactor = 3.75, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/OneSigmaUp.csv" )

main( powerFactor = -.78, gasFactor = -.12, coalFactor = -3.75, 
        outputFile = "C:/Documents and Settings/e14600/Desktop/CollateralAdHoc/OneSigmaDown.csv" )


