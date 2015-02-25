###############################################################################
# ExtractBrokerData.R
#
# IMPORTANT NOTE: The return codes in this file are kind of funny through 
# no fault of my own. For whatever reason, writing to the database with the 
# RODBC package causes the program to return a '3' error code on 
# successful write. That, in conjunction with procmon's inability to specify
# a single error code for success means that for all errors producted I have 
# to return 0 and for success, 3. It's a mess, I know, I'm sorry. 
#
# Risk Analytics
#
library( RODBC )
library( SecDb )
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
ExtractBrokerData = new.env(hash=TRUE)


################################################################################
ExtractBrokerData$main <- function()
{
    # Initialize SecDb for use on Procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    options(stringsAsFactors=FALSE)
    
    commodity = Sys.getenv( "commodity" )
    broker    = Sys.getenv( "broker" )
    rLog( "Running with broker:", broker, " commodity:", commodity, 
          " asOfDate:", as.character( ExtractBrokerData$.getAsOfDate() ) )
    
    data = ExtractBrokerData$.readData( commodity )
    
    if( is.null( data ) )
    {
        return( 0 )
    }
    
    finalData = ExtractBrokerData$.filterData( data, commodity, broker )
    
    ExtractBrokerData$.writeToDatabase( finalData )
    
    cat("Done.\n")   
    return( 3 )
}


################################################################################
ExtractBrokerData$.readData <- function( commodity )
{    
    asOfDate = ExtractBrokerData$.getAsOfDate()

    OUTPUT_DIR = paste( "S:/Risk/Houston Gas/", commodity, " EOD/", sep="" )
    file       = paste( OUTPUT_DIR, "EOD_", commodity, "enterprise_", 
                        format( asOfDate, "%Y_%m_%d" ), ".xls", sep=""  )

    columnNames = c( 'REGION', 'HUBS', 'PRODUCT', 'STRIP', 'BEGIN_DATE', 
                     'END_DATE', 'PRICE', 'TIME', 'QUANTITY', 
                     'TOTAL_VOLUME', 'ORDER_TYPE', 'REFERENCE_ID', 
                     'PARENT_ID', 'MARKET_ID', 'NGX_VOLUME', 
                     'IS_BLOCK_TRADE' )

    if( !file.exists( file ) )
    {
        file = ExtractBrokerData$.downloadFromICE( commodity, asOfDate )
    }
    
    con       = odbcConnectExcel( file )
    sheetName = paste( commodity, "deals", sep="" )
    data      = sqlFetch( con, sheetName )
    odbcClose( con )

    
    if( ncol(data) > length( columnNames ) )
    {
        namesToKeep = columnNames
        
    } else
    {
        namesToKeep = columnNames[ 1:ncol(data) ]  
    }
    names( data ) = namesToKeep
    data          = data[-c(1:4), namesToKeep ]
    
    return( data )
}


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate. 
# If that fails, use today.
#
ExtractBrokerData$.getAsOfDate <- function()
{
    runDate  = Sys.getenv("PROCMON_RUNDATE")
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
ExtractBrokerData$.downloadFromICE <- function( commodity, asOfDate )
{
    # for case sensitive ICE directory 
    Tmp    = strsplit(commodity,'')[[1]]
    Tmp[1] = toupper(Tmp[1])
    Tmp    = paste(Tmp,collapse='')

    INPUT_DIR   = paste( "https://downloads.theice.com/EOD_", Tmp, 
                         "_Enterprise/", sep="" )
    INPUT_FILE  = paste( INPUT_DIR, "EOD_", commodity, "enterprise_", 
                         format( asOfDate, "%Y_%m_%d" ), ".xls", sep="" )
    
    OUTPUT_DIR  = paste( "S:/Risk/Houston Gas/", commodity, " EOD/", sep="" )
    OUTPUT_FILE = paste( '"', OUTPUT_DIR, basename( INPUT_FILE ), '"', sep="" )
    command     = paste( "i:\\util\\wget --http-user=constellation --http-password=data ",
                         "--no-check-certificate ", "--output-document=",
                         gsub("/", "\\\\", OUTPUT_FILE), " ", INPUT_FILE, sep="" ) 
    
    errorCode = system( command )
    if( errorCode != 0 )
    {
        cat("File Not Found... Exiting.\n")
        file.remove( OUTPUT_FILE )
        q( status = errorCode )
    }
    
    filePath = paste( OUTPUT_DIR, basename( INPUT_FILE ), sep="" )
    
    return( filePath )
}


################################################################################
ExtractBrokerData$.filterData <- function( data, commodity, broker )
{
    filtered = data[which(data$TIME!=""),]
    rownames( filtered ) = NULL
    
    columnNames = c( 'REGION', 'HUBS', 'PRODUCT', 'STRIP', 'BEGIN_DATE', 
                     'END_DATE', 'PRICE', 'TIME', 'QUANTITY', 
                     'TOTAL_VOLUME', 'ORDER_TYPE', 'REFERENCE_ID', 
                     'PARENT_ID', 'MARKET_ID', 'NGX_VOLUME', 
                     'IS_BLOCK_TRADE' )

    missingColumns = setdiff( columnNames , colnames(filtered) )
    if( length( missingColumns ) > 0 )
    {
        filtered[,missingColumns] = NA
    }
    
    filtered$REGION         = as.character( filtered$REGION )
    filtered$HUBS           = as.character( filtered$HUBS )
    filtered$PRODUCT        = as.character( filtered$PRODUCT )
    filtered$STRIP          = as.character( filtered$STRIP )
    filtered$BEGIN_DATE     = as.Date( filtered$BEGIN_DATE )
    filtered$END_DATE       = as.Date( filtered$END_DATE )
    filtered$PRICE          = as.numeric( filtered$PRICE )
    filtered$TIME           = as.character( filtered$TIME )
    filtered$QUANTITY       = as.numeric( filtered$QUANTITY )
    filtered$TOTAL_VOLUME   = as.numeric( filtered$TOTAL_VOLUME )
    filtered$ORDER_TYPE     = as.character( filtered$ORDER_TYPE )
    filtered$REFERENCE_ID   = as.numeric( filtered$REFERENCE_ID )
    filtered$PARENT_ID      = as.numeric( filtered$PARENT_ID )
    filtered$MARKET_ID      = as.numeric( filtered$MARKET_ID )
    filtered$NGX_VOLUME     = as.numeric( filtered$NGX_VOLUME )
    filtered$IS_BLOCK_TRADE = as.character( filtered$IS_BLOCK_TRADE )
    filtered$BROKER         = toupper( broker )
    filtered$COMMODITY      = toupper( commodity )
    filtered$IMPORT_DATE    = ExtractBrokerData$.getAsOfDate()
    
    return( filtered )
}


################################################################################
ExtractBrokerData$.writeToDatabase <- function( data )
{
    dsnFile           = "RMGDBP.dsn"
    dsnPath           = "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste( "FileDSN=", dsnPath, dsnFile, 
                                ";UID=RMGDBUSER;PWD=n0t3bo0k;", sep="")  
    con               <- odbcDriverConnect(connection.string)  
    
    rLog("Writing", nrow(data), "rows to the database...")
    sqlSave( con, data, tablename = 'RAW_LIQUIDITY_DEAL', append = TRUE,
             rownames = FALSE, colnames = FALSE,
             verbose = FALSE, test = FALSE, fast=TRUE )
    
    cat("Database Write Complete.\n")
}


################################################################################
ExtractBrokerData$.writeToDatabaseDEV <- function( data )
{
    dsnFile           = "RMGDBD.dsn"
    dsnPath           = "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste( "FileDSN=", dsnPath, dsnFile, 
                                ";UID=RMGDBUSER;PWD=n0t3bo0k;", sep="")  
    con               <- odbcDriverConnect(connection.string)  
    
    rLog("Writing", nrow(data), "rows to the database...")
    sqlSave( con, data, tablename = 'RAW_LIQUIDITY_DEAL', append = TRUE,
             rownames = FALSE, colnames = FALSE,
             verbose = FALSE, test = FALSE, fast=TRUE )
    
    cat("Database Write Complete.\n")
}


################################################################################
returnCode = ExtractBrokerData$main()
q( status = returnCode )