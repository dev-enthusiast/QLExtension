###############################################################################
# CalculateDifference.R
#
# Author: e14600
#
library(odfWeave)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Utilities/OOTools.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")


################################################################################
cache = new.env(hash=TRUE)


###############################################################################
.getTotalFromColumn <- function( column="POWER.2013", filter, data )
{
    result = sum( data[eval(parse(text=filter)), column], na.rm=TRUE )
    
    return( result )
}


###############################################################################
.readRowColumnCSV <- function( rowName, columnName, file, ... )
{
    data = read.csv( file, ... )
    
    result = data[ rowName, columnName ]
    return( result )
}


###############################################################################
.commodityPositionsForTenor <- function( commodityGroupList, tenorList, 
        margined = FALSE, inputFile )
{
    
    if( is.null( cache$positions ) )
    {
        positions = PositionEngineVCentral$positionsByTenor( .getAsOfDate() )
        positions$ETI = as.character( positions$ETI )
        
        raftExposureData = RaftInterface$getExposureData( .getAsOfDate() )
        # Remove duplicate CP, ETI pairs. We grouped by it in the query, 
        # these should be unique
        duplicates = which(duplicated(raftExposureData[,c("COUNTERPARTY","ETI")]))
        raftExposureData = raftExposureData[ -duplicates, ]
        raftExposureData$ETI = as.character( raftExposureData$ETI )
        raftExposureData$EXPOSURE = NULL
        raftExposureData$MTM = NULL
        head(raftExposureData)
        
        exposureAndPositions = merge( raftExposureData, positions, all=TRUE)
        exposureAndPositions$TENOR = as.Date( exposureAndPositions$TENOR )
        rm( raftExposureData )
        rm( positions )
        
        cache$positions = exposureAndPositions
        rm(exposureAndPositions)
        gc()
    }
    
    
    applicablePositions = cache[[inputFile]][[as.character(margined)]]
    if( is.null( applicablePositions ) )
    {
        
        data = read.csv( inputFile )
        
        # If we're interested in MARGINABLE
        if( margined == FALSE )
        {
            filter = which( data$MARGINED == TRUE )
        }
        else
        {
            filter = which( data$MARGINED==TRUE & data$CASHFLOW_CHANGE !=0 )
        }
        
        # Read margined/marginable netting agreements
        nettingAgreementList = unique( data[filter, c("COUNTERPARTY", "NETTING_AGREEMENT")] )
        applicablePositions = merge(cache$positions, nettingAgreementList, all.y=TRUE)
        cache[[inputFile]][[as.character(margined)]] = applicablePositions
    }
    
    positionSubset = subset( applicablePositions, 
            as.Date(TENOR) %in% as.Date(tenorList) & 
                    as.character(COMMODITY_GRP) %in% as.character(commodityGroupList) )
    finalValue = sum( positionSubset$DELTA, na.rm=TRUE )
    
    return( finalValue )
}


################################################################################
sendStatusEmail <- function( outputFile, asOfDate )
{
    valueLine = "The Collateral Delta-Gamma Report is attached below."
    
    subject = paste( "Collateral Delta-Gamma", format( asOfDate, "%d%b%y" ) )
    
    toList = getEmailDistro("CollateralStress")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, valueLine, attachments=outputFile )
}


################################################################################
main <- function()
{
    options( stringsAsFactors=FALSE )
    
    asOfDate = .getAsOfDate()
    rLog( "Running with asOfDate:", as.character( asOfDate ) )
    
    zipCmds = c("S:/All/Risk/Software/zip.exe -r $$file$$ .",
            "unzip -o $$file$$")
    control = odfWeaveControl( zipCmd = zipCmds )
    
    BASE_PATH = "S:/All/Risk/Collateral Scenarios"
    reportDir = file.path( BASE_PATH, "Reports", as.character( asOfDate ) )
    setwd( reportDir )
    
    TEMPLATE_FILE = "H:/user/R/RMG/Projects/CollateralAdHoc/Collateral Delta Summary - Template.ods" 
    originalODSFile = "S:/All/Risk/Software/R/prod/Projects/CollateralAdHoc/originalODSFile.ods"
    odfWeave( TEMPLATE_FILE, originalODSFile, control=control )
    
    outputFileName = paste( reportDir, "/Collateral DGC - ", format( asOfDate, "%d%b%y" ), 
            ".ods", sep="" )
    OOTools$fixODSFile( originalODSFile, outputFileName, control=control )
    
    sendStatusEmail( outputFileName, asOfDate )
    
    rLog("Done.")
}


################################################################################
main()

