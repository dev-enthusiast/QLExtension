###############################################################################
# getVegaPositions.R
# 
# This code pulls & aggregates the vega positions from VCENTRAL.
#
# Author: John Scillieri
#
library(reshape)
source("H:/user/R/RMG/Utilities/RLogger.R")


###############################################################################
getVegaPositions <- function( asOfDate=Sys.Date()-1 )
{
    tradeData = .getVegaTradeData( asOfDate )
    
    cleanData = .formatTradeData( tradeData )
    
    rLog("Aggregating Data...")
    finalData = cast( cleanData, 
            book + curve.name + contract.dt + vol.type ~ ., sum)
    
    #rename the column and remove empty rows
    colnames(finalData)[5] = "vega"
    finalData = finalData[-which(finalData$vega==0),]
    rownames(finalData) = NULL
    
    rLog("Done.")
    return( finalData )
}


###############################################################################
# Pull the actual data from the database
#
.getVegaTradeData <- function( asOfDate )
{
    require(RODBC)
    file.dsn <- "VCTRLP.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=vcentral_read;PWD=vcentral_read;", sep="")  
    rLog("Connecting to VCentral...")
    con <- odbcDriverConnect(connection.string)  
    
    asOfDate <- as.Date( asOfDate )
    
    query <-  paste(
            "select th.profit_center_1, td.location_nm, td.valuation_month, ",
            "td.vol_frequency, td.vega ",
            "from vcentral.trade_detail td, vcentral.trade_header th ",
            "where th.trade_header_id=td.trade_header_id ", 
            "and td.valuation_date = '", format(asOfDate, "%d%b%y"), "'",
            "and td.vega != 0",
            sep = "")
    
    rLog("Querying Database...")
    vegas = sqlQuery(con, query) 
    rLog("Query Complete.")
    odbcClose(con)
    
    return(vegas)
}


###############################################################################
# make it a more R-friendly data frame
#
.formatTradeData <- function( tradeData )
{
    vegas = tradeData
    names(vegas) = c("book", "curve.name", "contract.dt", "vol.type", "value")
    
    vegas = vegas[ order( vegas$book, vegas$curve.name, 
                    vegas$contract.dt, vegas$vol.type), ]
    
    vegas$book = as.character(vegas$book)
    vegas$curve.name = toupper(as.character(vegas$curve.name))
    vegas$contract.dt = as.Date(vegas$contract.dt)
    vegas$vol.type = as.character(vegas$vol.type)
    
    # fix this stupid problem here
    ind <- grep("COMMOD PWR NEPOOL", vegas$curve.name)
    if( length(ind) > 0 )
    {
        vegas$curve.name[ind] <- "COMMOD PWX 5X16 BOS PHYSICAL"
    }
    
    return(vegas)
}


