################################################################################
# RaftInterface.R
# 
# Author: e14600
#
library( RODBC )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )


################################################################################
# File Namespace
#
RaftInterface = new.env(hash=TRUE)


################################################################################
# Data cache - stores the Raft data so you can rerun without
# reloading the whole file. See the getExposureData function for details.
#
RaftInterface$cache = new.env(hash=TRUE)


################################################################################
RaftInterface$getExposureData <- function( date )
{
    if( !is.null( RaftInterface$cache$exposureData ) )
    {
        return( RaftInterface$cache$exposureData )
    }
    
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            date, "/raftExposureByETI.RData", sep="" )
    if( file.exists( inputFile ) )
    {
        data = varLoad("data", inputFile )
        RaftInterface$cache$exposureData = data
        return( RaftInterface$cache$exposureData )
    }
    
    file.dsn <- "RaftEnergyCreditManual.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  
    
    con <- odbcDriverConnect(connection.string)  
    
    query = paste(
            "select ",
            "sCounterparty COUNTERPARTY, ",
            "sNettingAgreement NETTING_AGREEMENT, ",
            "sTicketLinkNumber ETI, ",
            "sum( dMTMBaseEquivalent + dDeliveredBaseEquivalent + dDeliveredInvoicedBaseEquivalent ) EXPOSURE, ",
            "sum( dMTMBaseEquivalent ) MTM ",
            "from EnergyCreditArchive.dbo.DealSnapshotHistory where ", 
            "DataAsOfDate='", format( date, "%d%b%y" ), "' ",
            "and sBook not in ('", 
            paste( .getRaftExcludedBookList( con ), collapse="','"), "') ",
            "group by sCounterparty, sNettingAgreement, sTicketLinkNumber", 
            sep="" ) 
    
    data = sqlQuery( con, query )
    odbcClose( con )
    
    data$ETI = as.character(data$ETI)
    data$ETI[which(data$COUNTERPARTY=="MACQUARIE2")] = 
            paste( data$ETI[which(data$COUNTERPARTY=="MACQUARIE2")], "-MAC", sep="" )
    
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( data, file=inputFile )
    RaftInterface$cache$exposureData = data
    
    return( RaftInterface$cache$exposureData )
}



################################################################################
RaftInterface$createCollateralTable <- function( asOfDate )
{
    megaData = .getMegaData( asOfDate )
    
    collateralTable = megaData[ , c('Agreement Active?','ARAP','Cash Held',
                    'Cash Posted','CEG Threshold Amt Used (USD)',
                    'Counterparty','Cpty Active?',
                    'Cpty Threshold Amt Used (USD)','CSA','LC Held','LC Posted',
                    'MTM','MTMPlusAccrualPlusARAP','Netting Agreement',
                    'sCreditOfficer','ShortName','sPartyType') ]
    
    names( collateralTable ) =  c('AGREEMENT_ACTIVE','ARAP','CREDIT_CASHHELD',
            'CREDIT_CASHPOST','CEG_THRESHOLD','LONG_NAME','CPTY_ACTIVE',
            'COUNTERPARTY_THRESHOLD','CSA','CREDIT_LCHELD','CREDIT_LCPOSTED',
            'MTM','EXPOSURE','NETTING_AGREEMENT',
            'CREDIT_OFFICER','COUNTERPARTY','CP_TYPE')
    
    collateralTable = collateralTable[ , sort(names(collateralTable))]
    
    collateralTable$EXPOSURE =  .convertExcelNumber( collateralTable$EXPOSURE )
    collateralTable$MTM = .convertExcelNumber( collateralTable$MTM )
    collateralTable$CREDIT_CASHHELD = .convertExcelNumber( collateralTable$CREDIT_CASHHELD )
    collateralTable$CREDIT_LCHELD = .convertExcelNumber( collateralTable$CREDIT_LCHELD )
    collateralTable$CREDIT_CASHPOST = .convertExcelNumber( collateralTable$CREDIT_CASHPOST )
    collateralTable$CREDIT_LCPOSTED = .convertExcelNumber( collateralTable$CREDIT_LCPOSTED )
    collateralTable$CEG_THRESHOLD =  .convertExcelNumber( collateralTable$CEG_THRESHOLD )
    collateralTable$COUNTERPARTY_THRESHOLD =  .convertExcelNumber( collateralTable$COUNTERPARTY_THRESHOLD )
    
    return( collateralTable )
}


###############################################################################
.getRaftExcludedBookList <- function( con )
{
    query = "select	* from EnergyCreditStaging.dbo.PNL_AccountingMethod 
	where UPPER(accounting_method) = UPPER('modeling')"
    
    excludedBooks = sqlQuery( con, query )$BOOK
    
    return( as.character( excludedBooks ) ) 
}


################################################################################
.convertExcelNumber <- function( x )
{
    cleanData = gsub( "(", "-", x, fixed=TRUE)
    cleanData = gsub( ")", "", cleanData, fixed=TRUE)
    cleanData = gsub( ",", "", cleanData, fixed=TRUE)
    cleanData = gsub( "$", "", cleanData, fixed=TRUE)
    cleanData = as.numeric( cleanData )
    
    return( cleanData )
}


################################################################################
.getMegaDataCSV <- function( inputFile )
{
    results = read.csv( inputFile, skip=1 )
    names( results ) = toupper( names( results ) )
    results = results[ , sort( names( results ) ) ]
    
    return( results )
}


################################################################################
.getMegaData <- function( asOfDate )
{
    file.dsn <- "RaftEnergyCreditManual.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  
    
    con <- odbcDriverConnect(connection.string)  
    
    query = paste(
            "select * from EnergyCreditArchive.dbo.ExposureByNA_Collateral_CSA_NC where ", 
            "DataAsOfDate='", format( asOfDate, "%d%b%y" ), "' ",
            sep="" ) 
    
    data = sqlQuery( con, query )
    odbcClose( con )
    
    return( data )
}


