###############################################################################
# SASVegaPositions.R
#
# This pulls all the SAS prices by day and saves them into the data dir.
# In the process, it creates a matrix of 71 days used to calculate the live var.
#
# Author: e14600


################################################################################
# External Libraries
#
library(RODBC)
library(reshape)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
# CONSTANTS
#
DATA_DIR = "//ceg/cershare/All/Risk/Data/VaR/prod/"


################################################################################
# File Namespace
#
SASVegaPositions = new.env(hash=TRUE)


################################################################################
SASVegaPositions$main <- function()
{
    asOfDate = secdb.dateRuleApply(Sys.Date(), "-1b")
    
    rLog("Beginning vega position pull...")
    vegaPositions = SASVegaPositions$.pull()
    
    outputFile = paste(DATA_DIR, "Positions/sas.vega.positions.", 
            as.character(asOfDate), ".RData", sep="")
    save( vegaPositions, file = outputFile )
    rLog( "Saved file:", outputFile )
    
    invisible(NULL)
}


################################################################################
SASVegaPositions$.pull <- function()
{
    connection = odbcConnect("SASPRD", believeNRows=FALSE)
    
    query = paste( "SELECT BOOK, LOCATION_NM, VALUATION_MONTH,",
            "VOL_FREQUENCY, VEGA",
            "FROM POSITION.POSITIONSAGG_ALL", 
            "WHERE VEGA <> 0")
    
    vegaPositions = sqlQuery(connection, query)
    odbcClose(connection)
    
    names(vegaPositions) = c("book", "curve.name", "contract.dt", 
            "vol.type", "value")
    vegaPositions$book = toupper( vegaPositions$book )
    vegaPositions$curve.name = toupper( vegaPositions$curve.name )
    vegaPositions$contract.dt = as.Date(vegaPositions$contract.dt) + 1
    vegaPositions$vol.type = toupper( vegaPositions$vol.type )
    
    vegaPositions = cast( vegaPositions, 
            book+curve.name+contract.dt+vol.type~., sum )
    names(vegaPositions) = c("book", "curve.name", "contract.dt", 
            "vol.type", "vega")
    
    emptyRows = which(vegaPositions$vega == 0)
    vegaPositions = vegaPositions[ -emptyRows, ]
    
    return( vegaPositions )
}


################################################################################
start = Sys.time()
SASVegaPositions$main()
print(Sys.time()-start)


