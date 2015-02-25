###############################################################################
# CalculateDifference.R
#
# Author: e14600
#
memory.limit( 4095 )
require( reshape )
require( RODBC )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/cashFlowChange.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/RaftInterface.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/PositionEngineVCentral.R")


###############################################################################
# Nicer way to cast, assumes idColumns ~ dataFields for the cast function
# 
smartCast <- function( data, idColumns, dataFields, fun.aggregate, ... )
{
    columnsIndiciesToKeep = which( names(data) %in% dataFields )
    availableToFilter = names(data)[ columnsIndiciesToKeep ]
    
    # melt it down
    molten = melt( data, id.var = idColumns, measure.var = availableToFilter )
    
    # reshape it
    reshaped = cast( molten, ... ~ variable, fun.aggregate, ... )
    
    reshaped = data.frame( reshaped )
    
    return( reshaped )
}


################################################################################
setMarginedFlag <- function( finalTable )
{
    # Set the MARGINED flag
    MARGINED = finalTable$CSA != "" & !is.na(finalTable$CSA)
    
    MARGINED[which( toupper( finalTable$COUNTERPARTY ) == "NYMEX" )] = TRUE
    MARGINED[which( toupper( finalTable$COUNTERPARTY ) == "MACQUARIE2" )] = TRUE
    
    return( MARGINED )
}


################################################################################
writeTable <- function( finalTable, outputFile )
{
    # Write out a 'mega report' with all these columns
    exposureChange = sum ( finalTable$EXPOSURE_CHANGE, na.rm=TRUE)
    marginedChange = sum ( finalTable$CASHFLOW_CHANGE[which(finalTable$MARGINED)], na.rm=TRUE)
    rLog( "Exposure Change:", dollar( exposureChange ) ) 
    rLog( "Margined Collateral Change:", dollar( marginedChange ) ) 
    
    columnsToKeep = c(
            'LONG_NAME','COUNTERPARTY','CP_TYPE','CPTY_ACTIVE',
            'CREDIT_OFFICER',
            'CSA','NETTING_AGREEMENT', 'AGREEMENT_ACTIVE',
            "CRUDE", "DOMESTIC.COAL", "GAS", "INTERNATIONAL.COAL", "POWER.PEAK", "POWER.OFFPEAK", 
            "CRUDE.2009", "DOMESTIC.COAL.2009", "GAS.2009", "INTERNATIONAL.COAL.2009", "POWER.PEAK.2009", "POWER.OFFPEAK.2009", 
            "CRUDE.2010", "DOMESTIC.COAL.2010", "GAS.2010", "INTERNATIONAL.COAL.2010", "POWER.PEAK.2010", "POWER.OFFPEAK.2010", 
            "CRUDE.2011", "DOMESTIC.COAL.2011", "GAS.2011", "INTERNATIONAL.COAL.2011", "POWER.PEAK.2011", "POWER.OFFPEAK.2011", 
            "CRUDE.2012", "DOMESTIC.COAL.2012", "GAS.2012", "INTERNATIONAL.COAL.2012", "POWER.PEAK.2012", "POWER.OFFPEAK.2012", 
            "CRUDE.2013", "DOMESTIC.COAL.2013", "GAS.2013", "INTERNATIONAL.COAL.2013", "POWER.PEAK.2013", "POWER.OFFPEAK.2013",
            'CREDIT_CASHHELD','CREDIT_LCHELD', 'COUNTERPARTY_THRESHOLD',
            'CREDIT_CASHPOST','CREDIT_LCPOSTED', 'CEG_THRESHOLD', 
            'EXPOSURE.RAFT','EXPOSURE.MEGARPT',
            'ADJUSTED_EXPOSURE','EXPOSURE_CHANGE',
            'MTM.RAFT','MTM.MEGARPT','MTM_ADJUSTMENT',
            'CASHFLOW_CHANGE', 'MARGINED')

    columnsIndiciesToKeep = which( names(finalTable) %in% columnsToKeep )
    availableToFilter = names(finalTable)[ columnsIndiciesToKeep ]
    
    finalTable = finalTable[, availableToFilter ]
    
    res <- try( write.csv( finalTable, row.names=FALSE, file = outputFile ) )
    if (class(res)!="try-error")
    {
        rLog( "Wrote output to file:", outputFile )
    } 
    else 
    {
        rError( "Couldn't write to file:", outputFile, ".\nSkipping..." )
    }


    
    invisible( finalTable )
}


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate. 
# If that fails, use yesterday.
#
.getAsOfDate <- function()
{
    runDate = Sys.getenv("PROCMON_RUNDATE")
    asOfDate = Sys.getenv("asOfDate")
    
    if( runDate != "" )
    {
        asOfDate = as.Date( runDate, format="%Y%m%d" )
        
    } else if( asOfDate != "" )
    {
        asOfDate = as.Date( asOfDate )
        
    } else
    { 
        asOfDate = Sys.Date()-1
    }
    
    return( asOfDate )
}


################################################################################
runWithMTMAdjustments <- function( asOfDate, allPositions, mtmAdjustments, outputFile )
{

    ## RAFT DATA ###########################
    rLog("Reading Raft Data...")
    # Get the low level Raft exposure data
    raftExposureData = RaftInterface$getExposureData( asOfDate )
    
    # Remove duplicate CP, ETI pairs. We grouped by it in the query, 
    # these should be unique
    duplicates = which(duplicated(raftExposureData[,c("COUNTERPARTY","ETI")]))
    raftExposureData = raftExposureData[ -duplicates, ]
    
    # Create a collateral table by counterparty & agreement
    collateralTable = RaftInterface$createCollateralTable( asOfDate )
    
    
    
    ## Start the fun stuff #################
    rLog("Joining the data...")
    raftWithAdjustments = merge( raftExposureData, mtmAdjustments, 
            all.x = TRUE )
    raftWithAdjustments = merge( raftWithAdjustments, allPositions, 
            all.x = TRUE )
    
    
    # Calculate the adjusted exposure column
    rLog("Calculating adjusted exposure...")
    raftWithAdjustments$ADJUSTED_EXPOSURE = rowSums( 
            raftWithAdjustments[ , c("EXPOSURE", "MTM_ADJUSTMENT")], 
            na.rm = TRUE )

    # Now reshape the data to sum the numeric columns
    rLog("Reshaping by counterparty + netting agreement...")
    rm( raftExposureData )
    rm( allPositions )
    rm( mtmAdjustments )
    raftWithAdjustments$ETI = NULL
    gc()

    exposureData = smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("EXPOSURE", "MTM", "MTM_ADJUSTMENT", "ADJUSTED_EXPOSURE"), 
            sum, na.rm=TRUE)
    
    positionData =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE", "DOMESTIC COAL", "GAS", 
                    "INTERNATIONAL COAL", "POWER PEAK", "POWER OFFPEAK"), 
            sum, na.rm=TRUE)
    positionData2009 =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE.2009", "DOMESTIC COAL.2009", "GAS.2009", 
                    "INTERNATIONAL COAL.2009", "POWER PEAK.2009", "POWER OFFPEAK.2009"), 
            sum, na.rm=TRUE)
    positionData2010 =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE.2010", "DOMESTIC COAL.2010", "GAS.2010", 
                    "INTERNATIONAL COAL.2010", "POWER PEAK.2010", "POWER OFFPEAK.2010"),
            sum, na.rm=TRUE)
    positionData2011 =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE.2011", "DOMESTIC COAL.2011", "GAS.2011", 
                    "INTERNATIONAL COAL.2011", "POWER PEAK.2011", "POWER OFFPEAK.2011"),
            sum, na.rm=TRUE)
    positionData2012 =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE.2012", "DOMESTIC COAL.2012", "GAS.2012", 
                    "INTERNATIONAL COAL.2012", "POWER PEAK.2012", "POWER OFFPEAK.2012"),
            sum, na.rm=TRUE)
    positionData2013 =  smartCast( raftWithAdjustments, 
            c("COUNTERPARTY", "NETTING_AGREEMENT"), 
            c("CRUDE.2013", "DOMESTIC COAL.2013", "GAS.2013", 
                    "INTERNATIONAL COAL.2013", "POWER PEAK.2013", "POWER OFFPEAK.2013"),
            sum, na.rm=TRUE)
    
    rm( raftWithAdjustments )
    gc()
    
    positionDataAll = merge( positionData, positionData2009, all = TRUE )
    positionDataAll = merge( positionDataAll, positionData2010, all = TRUE )
    positionDataAll = merge( positionDataAll, positionData2011, all = TRUE )
    positionDataAll = merge( positionDataAll, positionData2012, all = TRUE )
    positionDataAll = merge( positionDataAll, positionData2013, all = TRUE )
    exposureAndPositions = merge( exposureData, positionDataAll, all = TRUE )
    
    # now merge in the collateral table from the mega report
    rLog("Creating the final table...")
    finalTable = merge( collateralTable, exposureAndPositions, 
            by = c("COUNTERPARTY", "NETTING_AGREEMENT"),
            suffixes = c(".MEGARPT", ".RAFT" ), all = TRUE )

    # Provide a convenient exposure change due to MTM adjustment
    finalTable$EXPOSURE_CHANGE = finalTable$ADJUSTED_EXPOSURE - 
            finalTable$EXPOSURE.RAFT
    
    # Calculate the cash flow change
    finalTable$CASHFLOW_CHANGE = cashFlowChange(
                finalTable$EXPOSURE.RAFT, finalTable$ADJUSTED_EXPOSURE, 
                finalTable$CREDIT_CASHPOST, finalTable$CREDIT_LCPOSTED, 
                finalTable$CEG_THRESHOLD, finalTable$CREDIT_CASHHELD, 
                finalTable$CREDIT_LCHELD, finalTable$COUNTERPARTY_THRESHOLD )
    
    # Determine which agreements are margined
    finalTable$MARGINED = setMarginedFlag( finalTable )
    
    rLog("Writing the table...")
    writeTable( finalTable, outputFile )
}


################################################################################
run <- function( asOfDate = .getAsOfDate(), 
        powerFactor=Sys.getenv("powerFactor"), 
        gasFactor=Sys.getenv("gasFactor"), 
        coalFactor=Sys.getenv("coalFactor"),
        filter = "", 
        outputFile=Sys.getenv("outputFile") )
{
    asOfDate = as.Date( asOfDate )
    
    
    ## POSITION DATA ########################
    rLog("Reading Position Data...")
    positionEngine = PositionEngineVCentral
    
    # Get the MTM adjustments at an ETI level
    mtmAdjustments = positionEngine$calculateMTMAdjustments( asOfDate, 
            powerFactor, gasFactor, coalFactor, filter )
    
    # Create a table of all positions by major commodity group
    allPositions = positionEngine$getAllPositionTable( asOfDate )
    
    runWithMTMAdjustments( asOfDate, allPositions, mtmAdjustments, outputFile )
}

