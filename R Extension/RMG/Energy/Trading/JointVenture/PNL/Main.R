###############################################################################
# Main.R
#
# Author: e14600
#
library( RODBC )
library( methods )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/sendEmail.R" )
source( "H:/user/R/RMG/Utilities/dollar.R" )
source( "H:/user/R/RMG/Energy/Trading/JointVenture/PNL/PNLMonitor.R" )
source( "H:/user/R/RMG/Energy/Trading/JointVenture/PNL/ReportGenerator.R" )


################################################################################
# File Namespace
#
Main = new.env(hash=TRUE)


################################################################################
# Cached Data
Main$.reportDataCache  = NULL


################################################################################
Main$run <- function( portfolio = Sys.getenv("portfolio") )
{
    if( portfolio == "" )
    {
        rLog("No portfolio specified. Exiting...")
        return( 1 )
    }
    
    asOfDate = .getAsOfDate()
    
    # Initialize SecDb for use on Procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    options(stringsAsFactors=FALSE)
    
    postedValue = PNLMonitor$postedValue( portfolio, asOfDate )
    
    if( is.null( postedValue ) )
    {
        rLog("Portfolio", portfolio, "hasn't posted PNL. Exiting...")
        return( 1 )
    }
    
    outputFile = ReportGenerator$forPortfolio( portfolio, asOfDate )
    
    CONFIG_FILE = "H:/user/R/RMG/Energy/Trading/JointVenture/PNL/ReportConfig.csv"
    configuration = read.csv( CONFIG_FILE )
    summary = .generateSummary( portfolio, postedValue, 
            outputFile, configuration )
    
    .sendStatusEmail( portfolio, postedValue, summary, outputFile )
    
    rLog("Done Generating PNL Report.")
    
    return( 0 )
}


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate. 
# If that fails, use today.
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
        asOfDate = Sys.Date()
    }
    
    return( asOfDate )
}


################################################################################
.generateSummary <- function( portfolio, postedValue, outputFile, configuration )
{
    Main$.reportDataCache = NULL
    
    portfolioConfig = configuration[
            which( toupper(configuration$portfolio) == toupper(portfolio) ), ]
    
    allSummaries = NULL
    for( highlightID in unique( portfolioConfig$id ) )
    {
        highlightData = subset( portfolioConfig, id==highlightID )
        
        pnlData = NULL
        descriptions = NULL
        for( highlightIndex in seq_len( nrow( highlightData ) ) )
        {
            rowSpec = highlightData$ROW_SPEC[highlightIndex]
            columnSpec = highlightData$COLUMN_SPEC[highlightIndex]
            
            data = .getReportData( outputFile, rowSpec, columnSpec )
            pnlData = rbind( pnlData, data )
            
            # determine, if there's a spread, what is the identifying characteristic
            descriptor = ifelse( (length( unique(highlightData$ROW_SPEC) ) > 1), 
                    rowSpec, columnSpec )
            
            dataDescription = .getDataDescription( descriptor, data )
            descriptions = c( descriptions, dataDescription ) 
        }
        
        individualDescriptions = paste( descriptions, collapse="; ")
        overallDescription = .getOverallDescription( highlightData, pnlData )
        
        summaryLine = paste( overallDescription, individualDescriptions, sep="; ")
        allSummaries = c( allSummaries, summaryLine )
    }
    
    return( allSummaries )
}


################################################################################
.getDataDescription <- function( descriptor, data )
{
    upDown = ifelse( data$change > 0, "up", "down" )
    
    position = round( data$position/1000000, 2 )
    change = dollar( data$change )
    price = dollar( data$price )
    
    description = paste( descriptor, " ", position, "MM marked ", upDown, " ",
            change, " at ", price, sep="" )
    
    return( description )
}


################################################################################
.getOverallDescription <- function( highlightData, pnlData )
{
    upDown = ifelse( sum(pnlData$pnl) > 0, "Up", "Down" )
    
    pnl = paste( dollar( sum(pnlData$pnl)/1000000 ), "MM", sep="")
    
    totalPosition = round( pnlData$position/1000000, 2 )
    
    type = ifelse( totalPosition > 0, "Length", "Shorts" )
    if( nrow( highlightData ) > 1 )
    {
        type = "Spread"
    }
    
    tenor = paste( unique( highlightData$COLUMN_SPEC ), collapse="/" )
    location = paste( unique( highlightData$ROW_SPEC ), collapse="/" ) 
    
    description = paste( "  * ", upDown, pnl, "from", tenor, location, type )
    
    return( description )
}


################################################################################
.getReportData <- function( outputFile, rowSpec, columnSpec )
{
    # If we haven't cached the PNL Data yet, do it now
    if( is.null( Main$.reportDataCache ) )
    {
        con = odbcConnectExcel( outputFile ) 
        Main$.reportDataCache = sqlFetch( con, "Day On Day PNL" )
        odbcClose( con )
    }
    
    # Extract the rows we want
    firstRow = which( Main$.reportDataCache$ROW_SPEC == rowSpec )
    if( length(firstRow) == 0 )
    {
        rWarn( "RowSpec:", rowSpec, "NOT FOUND!")
        dataBlock = data.frame( t( rep( 0, 4 ) ) )
        names(dataBlock) = Main$.reportDataCache$Type[1:4]
        return( dataBlock )
    }
    
    # Now pull out the specific column of data
    dataBlock = as.numeric( Main$.reportDataCache[ 
                    (firstRow:(length(TYPE_SPEC)-1+firstRow)), 
                    columnSpec ] )
    dataBlock = data.frame( t( dataBlock ) )
    
    if( length(dataBlock) == 0 )
    {
        rWarn( "ColumnSpec:", columnSpec, "NOT FOUND!")
        dataBlock = data.frame( t( rep( 0, 4 ) ) )
    }
    
    names(dataBlock) = Main$.reportDataCache$Type[1:4]
    
    return( dataBlock )
}


################################################################################
.sendStatusEmail <- function( portfolio, postedValue, summary, outputFile )
{
    time = paste( "Time:", Sys.time() )
    
    valueLine = paste( portfolio, "Posted PNL of", dollar( postedValue ), 
            "on the day. Below is a summary of its PnL." )
    
    summaryBody = paste( summary, collapse="\n" )
    valueLine = paste( valueLine, summaryBody, sep="\n\n" )
    
    footer = paste( "RMG - Risk Systems",
            "john.scillieri@constellation.com", "\n", sep="\n" )
    
    bodyOfEmail = paste( time, valueLine, footer, sep="\n\n")
    
    summaryOutputFile = paste( dirname( outputFile ), 
            "/", portfolio, " Summary.", as.character(.getAsOfDate()),
            ".txt", sep="" )
    rLog("Writing summary output to:", summaryOutputFile )
    sink( summaryOutputFile )
    cat( bodyOfEmail )
    sink()
    
    subject = paste( portfolio, "PNL Posted" )
    
    toList = getEmailDistro("PNL")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, bodyOfEmail, attachments=outputFile )
}


################################################################################
returnCode = Main$run()
q( status = returnCode )




