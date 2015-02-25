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
library("RODBC", lib.loc="//ceg/cershare/All/Risk/Software/R/prod/cran-library/R-2.15.1")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/sendEmail.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")
library("bizdays")

################################################################################
# File Namespace
#
ExtractBrokerData = new.env(hash=TRUE)

################################################################################
ExtractBrokerData$main <- function()
{    
  
  options(stringsAsFactors=FALSE)
  rDates <- gsub("-","",as.character(bizseq(as.Date("2014-09-01"), as.Date("2014-09-15"))));
  
  for(i in 1:length(rDates))
  {
    ExtractBrokerData$PopulateForDate(rDates[i], 'gas','ice')
    ExtractBrokerData$PopulateForDate(rDates[i], 'power','ice')
  }  
  
  rDates <- gsub("-","",as.character(bizseq(as.Date("2014-09-16"), as.Date("2014-10-21"))));
  
  for(i in 1:length(rDates))
  {    
    ExtractBrokerData$PopulateForDate(rDates[i], 'power','ice')
  }  
  
  cat("Done.\n")   
  return( 0 )
}

ExtractBrokerData$PopulateForDate <- function(runDate, commodity, broker)
{
  
  
  rLog( "Running with broker:", broker, " commodity:", commodity, 
        " asOfDate:", as.character( ExtractBrokerData$.getAsOfDate(runDate) ) )
  
  dealdata = ExtractBrokerData$.readData( commodity, runDate )
  
  if( is.null( dealdata  ) )
  {
    q(status = -1);
  }
  
  finalDealData = ExtractBrokerData$.filterData( dealdata, commodity, broker, runDate )
  
  ExtractBrokerData$.writeToDatabase( finalDealData, 'RAW_LIQUIDITY_DEAL' )
  
  
  ## Update Activity Database (Bid/ask Spreads)
  
  activitydata = ExtractBrokerData$.readData_activity( commodity, runDate )
  
  finalActivityData = ExtractBrokerData$.filterData_activity( activitydata, commodity, broker, runDate )
  
  ExtractBrokerData$.writeToDatabase( finalActivityData, 'RAW_LIQUIDITY' )
}




################################################################################
ExtractBrokerData$.readData <- function( commodity, runDate )
{        
    asOfDate = ExtractBrokerData$.getAsOfDate(runDate)

    OUTPUT_DIR = paste( "//ceg/cershare/Risk/Houston Gas/", commodity, " EOD/", sep="" )

    if(asOfDate > "2014-09-04") {
    		file       = paste( OUTPUT_DIR, "EOD_", commodity, "enterprise_", 
                        format( asOfDate, "%Y_%m_%d" ), ".xlsx", sep=""  )
		    sheetName = "deals"
    } else {
    		file       = paste( OUTPUT_DIR, "EOD_", commodity, "enterprise_", 
                        format( asOfDate, "%Y_%m_%d" ), ".xls", sep=""  )
	    	sheetName = paste( commodity, "deals", sep="" )
    }

    columnNames = c( 'REGION', 'HUBS', 'PRODUCT', 'STRIP', 'BEGIN_DATE', 
                     'END_DATE', 'PRICE', 'TIME', 'QUANTITY', 
                     'TOTAL_VOLUME', 'ORDER_TYPE', 'REFERENCE_ID', 
                     'PARENT_ID', 'MARKET_ID', 'NGX_VOLUME', 
                     'IS_BLOCK_TRADE' )

    if( !file.exists( file ) )
    {
        file = ExtractBrokerData$.downloadFromICE( commodity, asOfDate )
    }
    

    connection.string =  paste("driver=Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb);dbq=",file)
    con = odbcDriverConnect(connection.string )
    
    if(con == -1 )    
    {        
        cat(paste(toString(odbcGetErrMsg(con)), "\n", sep =""))
        q( status = con )
    } 
    
    data      = sqlFetch( con, sheetName)
    
    if(length(odbcGetErrMsg(con)) != 0)
    {
      cat(toString(odbcGetErrMsg(con)), "\n", sep ="")
      q( status = -1 )
    }

    odbcCloseAll()

    
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
ExtractBrokerData$.getAsOfDate <- function(runDate)
{
    
    asOfDate = runDate;
    
    if( runDate != "" )
    {
        asOfDate = as.Date( runDate,format="%Y%m%d" )
        
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

    if(asOfDate > "2014-09-04")
    {
       INPUT_FILE  = paste( INPUT_DIR, "EOD_", commodity, "enterprise_", 
                         format( asOfDate, "%Y_%m_%d" ), ".xlsx", sep="" )
    } else {
	 INPUT_FILE  = paste( INPUT_DIR, "EOD_", commodity, "enterprise_", 
                         format( asOfDate, "%Y_%m_%d" ), ".xls", sep="" )
    }

	
    
    OUTPUT_DIR  = paste( "//ceg/cershare/Risk/Houston Gas/", commodity, " EOD/", sep="" )
    OUTPUT_FILE = paste( '"', OUTPUT_DIR, basename( INPUT_FILE ), '"', sep="" )
    command     = paste( "i:\\util\\wget --http-user=constellation --http-password=data ",
                         "--no-check-certificate ", "--output-document=",
                         gsub("/", "\\\\", OUTPUT_FILE), " ", INPUT_FILE, sep="" ) 
    
    errorCode = system( command )

    if( errorCode != 0 )
    {
	      file.remove( OUTPUT_FILE )    	 
        cat("File Not Found... Exiting.\n")
        q( status = errorCode )
    }    

    filePath = paste( OUTPUT_DIR, basename( INPUT_FILE ), sep="" )

  
    return( filePath )
}


################################################################################
ExtractBrokerData$.filterData <- function( data, commodity, broker, runDate )
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
    filtered$BEGIN_DATE     = as.Date( filtered$BEGIN_DATE)
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
    filtered$IMPORT_DATE    = ExtractBrokerData$.getAsOfDate(runDate)
    
    return( filtered )
}


####################################################################################################

ExtractBrokerData$.readData_activity <- function( commodity, runDate )
{    
    asOfDate = ExtractBrokerData$.getAsOfDate(runDate)
 
   

    OUTPUT_DIR = paste( "//ceg/cershare/Risk/Houston Gas/",commodity, " EOD/", sep="" )
    if(asOfDate > "2014-09-04") {
    		file       = paste( OUTPUT_DIR, "EOD_", commodity, "enterprise_", 
                        format( asOfDate, "%Y_%m_%d" ), ".xlsx", sep=""  )
		    sheetName = "activity"
    } else {
    		file       = paste( OUTPUT_DIR, "EOD_", commodity, "enterprise_", 
                        format( asOfDate, "%Y_%m_%d" ), ".xls", sep=""  )
	    	sheetName = paste( commodity, "activity", sep="" )
    }  


 
    columnNames = c( 'REGION', 'HUBS', 'PRODUCT',	'STRIP', 'BEGIN_DATE', 'END_DATE', 
                     'LAST_PRICE', 'TIME', 'HIGH_PRICE', 'LOW_PRICE', 
                     'LAST_BID_PRICE', 'LAST_BID_TIME', 'LAST_OFFER_PRICE', 'LAST_OFFER_TIME', 'BEST_BID',
                     'BEST_OFFER', 'BEST_BID2', 'BEST_OFFER2', 'WGT_AVG', 'TOTAL_VOLUME', 
                     'MARKET_ID', 'NGX_WAVG', 'NGX_VOLUME')

   

    if( !file.exists( file ) )
    {
        file = ExtractBrokerData$.downloadFromICE( commodity, asOfDate )
    }
        
    connection.string =  paste("driver=Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb);dbq=",file)
    con = odbcDriverConnect(connection.string )
        
    
    if(con == -1 )    
    {        
      cat(paste(toString(odbcGetErrMsg(con)), "\n", sep =""))
      q( status = con )
    } 
    
    data      = sqlFetch( con, sheetName)
    
    if(length(odbcGetErrMsg(con)) != 0)
    {
      cat(toString(odbcGetErrMsg(con)), "\n", sep ="")
      q( status = -1 )
    }
    
    odbcCloseAll()
 
    
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


#################################################################################################

ExtractBrokerData$.filterData_activity <- function( data, commodity, broker, runDate )
{    
    
    filtered = subset(data,subset=data$TIME!="" & data$TOTAL_VOLUME!="" )
    rownames( filtered ) = NULL
    
    columnNames = c( 'REGION', 'HUBS', 'PRODUCT', 'STRIP', 'BEGIN_DATE', 'END_DATE', 
                     'LAST_PRICE', 'TIME', 'HIGH_PRICE', 'LOW_PRICE', 
                     'LAST_BID_PRICE', 'LAST_BID_TIME', 'LAST_OFFER_PRICE', 'LAST_OFFER_TIME', 'BEST_BID',
                     'BEST_OFFER', 'BEST_BID2', 'BEST_OFFER2', 'WGT_AVG', 'TOTAL_VOLUME', 
                     'MARKET_ID', 'NGX_WAVG', 'NGX_VOLUME' )
 
    missingColumns = setdiff( columnNames , colnames(filtered) )
    if( length( missingColumns ) > 0 )
    {
        filtered[,missingColumns] = NA
    }
    
    filtered$REGION            = as.character( filtered$REGION )
    filtered$HUBS              = as.character( filtered$HUBS )
    filtered$PRODUCT           = as.character( filtered$PRODUCT )
    filtered$STRIP 	       = as.character( filtered$STRIP )
    filtered$BEGIN_DATE        =  as.Date( filtered$BEGIN_DATE, format="%m/%d/%y" )
    filtered$END_DATE          =  as.Date( filtered$END_DATE, format="%m/%d/%y" )
    filtered$LAST_PRICE        = as.numeric( filtered$LAST_PRICE )
    filtered$TIME              = as.character( filtered$TIME )
    filtered$HIGH_PRICE        = as.numeric( filtered$HIGH_PRICE )
    filtered$LOW_PRICE         = as.numeric( filtered$LOW_PRICE )
    filtered$LAST_BID_PRICE    = as.character( filtered$LAST_BID_PRICE )
    filtered$LAST_BID_TIME     = as.Date( filtered$LAST_BID_TIME , format="%m/%d/%y") 
    filtered$LAST_OFFER_PRICE  = as.character( filtered$LAST_OFFER_PRICE )
    filtered$LAST_OFFER_TIME   = as.Date( filtered$LAST_OFFER_TIME, format="%m/%d/%y" )
    filtered$BEST_BID          = as.numeric( filtered$BEST_BID )
    filtered$BEST_OFFER        = as.numeric( filtered$BEST_OFFER )
    filtered$BEST_BID2         = as.numeric( filtered$BEST_BID2 )
    filtered$BEST_OFFER2       = as.numeric( filtered$BEST_OFFER2 )
    filtered$WGT_AVG           = as.numeric( filtered$WGT_AVG )
    filtered$TOTAL_VOLUME      = as.numeric( filtered$TOTAL_VOLUME )
    filtered$MARKET_ID         = as.numeric( filtered$MARKET_ID )
    

    filtered$BROKER         = toupper( broker )
    filtered$COMMODITY      = toupper( commodity )
    filtered$IMPORT_DATE    = ExtractBrokerData$.getAsOfDate(runDate)
	  filtered$NGX_WAVG          = as.numeric( filtered$NGX_WAVG )
    filtered$NGX_VOLUME        = as.numeric( filtered$NGX_VOLUME )

    
    return( filtered )
}


################################################################################
ExtractBrokerData$.writeToDatabase <- function( data, tblname )
{
  
    dsnFile           = "RMGDBP_11g.dsn"
    dsnPath           = "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"

    
    connection.string =  paste( "FileDSN=", dsnPath, dsnFile, 
                                ";UID=RMGDBUSER;PWD=n0t3bo0k;", sep="")  


    con = odbcDriverConnect(connection.string)  
    
    rLog("Writing", nrow(data), "rows to the database...")
    sqlSave( con, data, tablename = tblname, append = TRUE,
             rownames = FALSE, colnames = FALSE,
             verbose = FALSE, test = FALSE, fast=TRUE )
	 
    cat("Database Write Complete: ",tblname, "\n")
    
    odbcCloseAll()
        
}

################################################################################

returnCode = ExtractBrokerData$main()
q( status = returnCode )