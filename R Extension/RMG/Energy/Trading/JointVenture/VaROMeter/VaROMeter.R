###############################################################################
# VaROMeter.R
# 
# Author: e14600
#

###############################################################################
library(SecDb)
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")
source("H:/user/R/RMG/Energy/VaR/Tools/SASVaR.R")
source("H:/user/R/RMG/Utilities/Database/SAS/SASInterface.R")
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
VaROMeter = new.env(hash=TRUE)


################################################################################
# Cached Values
VaROMeter$.currentPositions = NULL
VaROMeter$.startingPositions = NULL
VaROMeter$.timeOfPositionPull = NULL
VaROMeter$.outputFile = NULL
VaROMeter$.tenorFile = NULL


################################################################################
VaROMeter$main <- function( portfolio, outputDir = NULL )
{   
    if( !is.null( outputDir ) )
    {
        outputFile = file.path( outputDir, "Summary.txt" )
        VaROMeter$.outputFile = file( outputFile, open = "w" )
        
        tenorFile = file.path( outputDir, "ChangesByTenor.txt" )
        VaROMeter$.tenorFile = file( tenorFile, open = "w" )
    }
    
    currentPositions = VaROMeter$.getPortfolioPositions( portfolio, current=TRUE )
    startingPositions = VaROMeter$.getPortfolioPositions( portfolio, current=FALSE )
    
    diffs = VaROMeter$.calculateDifferences( startingPositions, currentPositions )
    diffOutputFile = file.path( outputDir, "AllDiffs.csv" )
    write.csv( diffs, row.names=FALSE, file=diffOutputFile )
    
    # Section 1
    rLog("Calculating VaR Differences...")
    VaROMeter$.printVaRDifferences( startingPositions, currentPositions )
    
    # Section 2
    rLog("Calculating Differences By Commodity Bucket...")
    bucketOutput = VaROMeter$.diffsByBucket( diffs )
    VaROMeter$.printBucketDifferences( bucketOutput )

    # Section 3 
    rLog("Calculating Gas Buy/Sell Highlights...")
    VaROMeter$.printGasHighlights( currentPositions )
    
    # Section 4
    rLog("Calculating Differences By Tenor...")
    VaROMeter$.printDiffsByTenor( diffs )

    if( !is.null( outputDir ) )
    {
        close( VaROMeter$.outputFile )
        close( VaROMeter$.tenorFile )
    }
    
    invisible( NULL )
}


################################################################################
VaROMeter$.getPortfolioPositions <- function( portfolio, current=FALSE )
{
    asOfDate = as.Date( secdb.invoke("_Lib Date Functions", 
                    "DateFns::Last Recent Business Day"), format="%Y%m%d" )
    
    if( current )
    {
        if( is.null( VaROMeter$.currentPositions ) ) 
        {
            VaROMeter$.currentPositions = LiveData$cpsprodPositions()
            VaROMeter$.timeOfPositionPull = Sys.time()
        }
        
        positions = VaROMeter$.currentPositions
        
    } else
    {
        if( is.null( VaROMeter$.startingPositions ) ) 
        {
            deltas = SASInterface$getDeltas( asOfDate )
            VaROMeter$.startingPositions = deltas
        }
        
        positions = VaROMeter$.startingPositions
    }
    
    # Add in the vega positions
    vegas = SASVaR$.getVegas( asOfDate )

    positions = rbind( positions, vegas )
    
    books = names( secdb.getValueType(portfolio, "nodes")$Book )
    bookPositions = subset(positions, positions$book %in% books)
    
    bookPositions$book = NULL
    bookPositions = cast( bookPositions, 
            riskFactor + curve.name + contract.dt + vol.type ~ ., sum )
    names( bookPositions ) = c( "riskFactor", "curve.name", "contract.dt", 
            "vol.type", "value" )
    
    bookPositions = data.frame( bookPositions )
    
    return( bookPositions )
}


################################################################################
VaROMeter$.calculateDifferences <- function( startingPositions, currentPositions )
{
    diffs = merge( currentPositions, startingPositions, 
            by = c("curve.name", "contract.dt", "vol.type"), all=TRUE, fill=0 )
    
    diffs$value = diffs$value.x - diffs$value.y
    
    diffs = subset( diffs, diffs$value != 0 )
    rownames(diffs) = NULL
    
    bucketTable = VaROMeter$.getBucketTable( diffs$curve.name )
    diffs = merge( diffs, bucketTable )

    return( diffs )
}


################################################################################
VaROMeter$.getBucketTable <- function( curveList )
{
    uniqueCurves = sort( unique( curveList ) )
    
    bucketList = NULL
    for( curve in as.character( uniqueCurves ) )
    {
        bucket = secdb.getValueType( curve, "Region Name")
        if( is.null( bucket) ) bucket = NA
        
        bucketList = c( bucketList, bucket)
    }
    
    bucketData = data.frame( curve.name = uniqueCurves, bucket = toupper( bucketList ) )
   
    # Fix the peak/offpeak problems w/the curve name
    peakLabeled = grep( " PK ", bucketData$curve.name, fixed = TRUE )
    bucketData$bucket[peakLabeled] = "PEAK"
    offpeakLabeled = grep( " OP ", bucketData$curve.name, fixed = TRUE )
    bucketData$bucket[offpeakLabeled] = "OFFPEAK"
    
    # Fix the peak/offpeak problems with regions that are more descriptive
    # e.g. HUDV OFFPEAK, WEST PEAK, etc
    peakLabeled = grep( "( PEAK)", bucketData$bucket )
    bucketData$bucket[peakLabeled] = "PEAK"
    offpeakLabeled = grep( "( OFFPEAK)", bucketData$bucket )
    bucketData$bucket[offpeakLabeled] = "OFFPEAK"
    
    bucketData$bucket[ which(bucketData$curve.name=="COMMOD NG EXCHANGE") ] = "NG"
    
    return( bucketData )
}


################################################################################
VaROMeter$.printDiffsByTenor <- function( diffs )
{
    tenorOutput = diffs[, c("contract.dt", "bucket", "value")]
    tenorOutput = cast( tenorOutput, bucket + contract.dt ~ ., 
            function( x ){ round( sum( x ) ) } )
    
    names(tenorOutput) = c( "bucket", "contract.dt", "value" )
    tenorOutput = tenorOutput[ order(tenorOutput$bucket, tenorOutput$contract.dt,
                    abs( tenorOutput$value ), decreasing=FALSE), ]
    
    # Clean up the results
    tenorOutput = subset( tenorOutput, tenorOutput$value != 0 )
    tenorOutput = subset( tenorOutput, bucket %in% c( "PEAK", "OFFPEAK", "NG" ) )
    tenorOutput = tenorOutput[-which(tenorOutput$bucket=="NG" & abs(tenorOutput$value) < 100),]
    tenorOutput = tenorOutput[-which(tenorOutput$bucket=="PEAK" & abs(tenorOutput$value) < 1000),]
    tenorOutput = tenorOutput[-which(tenorOutput$bucket=="OFFPEAK" & abs(tenorOutput$value) < 1000),]
    row.names( tenorOutput ) = NULL
    
    if( !is.null( VaROMeter$.tenorFile ) )
    {
        sink( VaROMeter$.tenorFile )
    }
    
    cat( "Position Changes By Bucket & Tenor\nNG Moves > 100 MMBTU, PEAK/OFFPEAK > 1000 MWH\n\n" )
    print( tenorOutput )
    
    if( !is.null( VaROMeter$.tenorFile ) )
    {
        sink()
    }
    
    invisible( tenorOutput )
}


################################################################################
VaROMeter$.diffsByBucket <- function( diffs )
{
    bucketOutput = diffs[, c("bucket", "value")]
    bucketOutput = cast( bucketOutput, bucket ~ ., function(x){round(sum(x))} )
    
    return( bucketOutput )
}


################################################################################
VaROMeter$.printVaRDifferences <- function( startingPositions, currentPositions )
{
    startingVaR = SASVaR$calculate( startingPositions )
    currentVaR = SASVaR$calculate( currentPositions )
    
    varDifference = currentVaR - startingVaR 
    changeString = ifelse( startingVaR > currentVaR, "decreased ", "increased ")
    
    .vomLog( "Based on new trades as of ", 
            format( VaROMeter$.timeOfPositionPull, "%I:%M" ), 
            ", the desk Total VaR ", changeString,  dollarShortForm( varDifference ) , 
            " (from ", dollarShortForm( startingVaR ), " to ", dollarShortForm( currentVaR ),
            ").\n", sep="" )
    
    invisible( varDifference )
}


################################################################################
VaROMeter$.printBucketDifferences <- function( bucketOutput )
{
    rowsToKeep = which( bucketOutput$bucket == "NG" | 
                    bucketOutput$bucket == "OFFPEAK" | 
                    bucketOutput$bucket == "PEAK" )
    bucketOutput = bucketOutput[ rowsToKeep, ]
    
    for( rowIndex in seq_len( nrow( bucketOutput ) ) )
    {
        rowData = bucketOutput[ rowIndex, ]
        names(rowData) = c("bucket", "value")
        if( rowData$bucket == "NG" )
        {	
            rowData$value = round( rowData$value/10000 )
            unit = "Lots"
        } else
        {
            unit = "MWH"
        }
        
        if( rowData$value > 0 ) purchaseString = "Bought"
        if( rowData$value < 0 ) purchaseString = "Sold"
        if( rowData$value == 0 ) purchaseString = "Flat"
        
        .vomLog("*  ", purchaseString, 
                dollarShortForm( abs( rowData$value ), do.dollar=FALSE), 
                unit, rowData$bucket )
    }
    
    .vomLog( "" )
    
    invisible( bucketOutput )
}


################################################################################
VaROMeter$.printGasHighlights <- function( currentPositions )
{
    currentVaR = SASVaR$calculate( currentPositions )
    
    promptContract = secdb.getValueType("BAL Close NG Futures Strip", 
            "Contracts" )[[1]]
    startMonth = Term$fromReutersCode( promptContract )
    endOfMonth = as.Date( secdb.dateRuleApply( startMonth, "e" ) )
    VaROMeter$.printBuySellPair( currentPositions, currentVaR, 
            1, startMonth, endOfMonth )
    
    VaROMeter$.printBuySellPair( currentPositions, currentVaR, 
            1, "2009-01-01", "2009-12-31" )
    
    VaROMeter$.printBuySellPair( currentPositions, currentVaR, 
            1, "2010-01-01", "2010-12-31" )
    
    VaROMeter$.printBuySellPair( currentPositions, currentVaR, 
            1, "2011-01-01", "2011-12-31" )
    
    invisible( NULL )
}


################################################################################
VaROMeter$.printBuySellPair <- function( currentPositions, currentVaR, 
        countPerDay, startDate, endDate )
{
    buyPositions = VaROMeter$.gasPosition( countPerDay, startDate, endDate )
    buyVaR = SASVaR$calculate( rbind( currentPositions, buyPositions ) )
    
    sellPositions = VaROMeter$.gasPosition( -countPerDay, startDate, endDate )
    sellVaR = SASVaR$calculate( rbind( currentPositions, sellPositions ) )
    
    term = Term$fromDatePair( startDate, endDate )
    
    buyChange = ifelse( buyVaR > currentVaR, "increases ", "decreases ")
    buyDifference = dollarShortForm( buyVaR - currentVaR )
    .vomLog( "Buying ", countPerDay, "/d ", term, " NYMEX ", buyChange,
            "VaR by ", buyDifference, sep="" )
    
    sellChange = ifelse( sellVaR > currentVaR, "increases ", "decreases ")
    sellDifference = dollarShortForm( sellVaR - currentVaR )
    .vomLog( "Selling ", countPerDay, "/d ", term, " NYMEX ", sellChange,
            "VaR by ", sellDifference, sep="" )
    
    .vomLog ( "" )
    
    invisible( NULL )
}


################################################################################
VaROMeter$.gasPosition <- function( countPerDay, startDate, endDate )
{
    startDate = as.Date( startDate )
    endDate = as.Date( endDate )
    
    tenors = seq( startDate, endDate, "month" )
    
    allPositionData = NULL
    for( tenor in as.character( tenors ) )
    {
        tenor = as.Date( tenor )
        nextMonth = as.Date( secdb.dateRuleApply( tenor, "e+1d" ) )
        numberOfDays = difftime( tenor, nextMonth, "days")
        numberOfDays = abs( as.numeric( numberOfDays ) )
        
        position = countPerDay * numberOfDays * 10000
        
        rowData = data.frame( curve.name = "COMMOD NG EXCHANGE", 
                contract.dt = tenor, vol.type = NA_character_, 
                value = position )
        allPositionData = rbind( allPositionData, rowData )
    }
    
    return( allPositionData )
}


################################################################################
.vomLog <- function( ... )
{
    cat( ..., "\n", file = VaROMeter$.outputFile )
    
    invisible( NULL )
} 


