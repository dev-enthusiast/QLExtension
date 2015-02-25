###############################################################################
# LiveData.R
#
# Author: John Scillieri
# 
library(RODBC)
library(reshape)
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
LiveData = new.env(hash=TRUE)


################################################################################
# get the live position set
#
LiveData$allPositions <- function()
{
    cpsprodDeltas = LiveData$cpsprodPositions()
    
    rmsysDeltas = LiveData$rmsysPositions()
    
    # remove duplicate books
    rLog("Removing duplicate books...")
    duplicateBooks = intersect( unique(rmsysDeltas$book), 
            unique(cpsprodDeltas$book) )
    badBooks = which(rmsysDeltas$book %in% duplicateBooks)
    rmsysDeltas = rmsysDeltas[-badBooks, ]
    
    deltaPositions = rbind(cpsprodDeltas, rmsysDeltas)
    deltaPositions = unique(deltaPositions)
    names(deltaPositions) = c("BOOK", "COMMOD_CURVE", "CONTRACT_DATE", "DELTA" )
    
    return( deltaPositions )
}


################################################################################
# Does what it says. No vegas here either.
#
LiveData$cpsprodPositions<- function()
{
    rLog("Querying CCGFOP for most recent positions...")
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CCGFOP.dsn"
    conString <- paste( dsnPath, ";UID=cvar;PWD=c#var;", sep="")
    chan = odbcDriverConnect(conString)
    
    query = paste("select * from cvar.vw_rs_snapshot",
            "order by BOOK, COMMOD_CURVE, CONTRACT_DATE")
    firstTry = sqlQuery(chan, query)
    Sys.sleep(1)
    secondTry = sqlQuery(chan, query)
    
    while( !all(dim(secondTry)==dim(firstTry)) )
    {
        rLog("We caught CPSPROD in the middle of an update, trying again...")
        firstTry = sqlQuery(chan, query)
        Sys.sleep(1)
        secondTry = sqlQuery(chan, query)
    }
    
    odbcClose(chan)
    
    names(secondTry) = c("book", "curve.name", "contract.dt", "position")
    secondTry$book = toupper(secondTry$book)
    secondTry$curve.name = toupper(secondTry$curve.name)
    secondTry$contract.dt = as.Date(secondTry$contract.dt)
    
    return(secondTry)
}


################################################################################
# This gets all delta positions from the rmsys live table. We don't pull
# vegas because I don't have a good way to map vol types to the live position 
# table. 
#
LiveData$rmsysPositions <- function()
{
    
    # Connect to the database
    rLog("Querying RmSys for most recent positions...")
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn"
    conString = paste( dsnPath, ";UID=rmsys_read;PWD=rmsys_read;", sep="")
    chan = odbcDriverConnect(conString)
    
    # Query the database for the live data
    query <- paste("select d.secdb_book, pi.long_name, lp.contract_date, sum(lp.delta) ",
            "from live_positions lp, deals d, price_indexes pi ", 
            "where d.deal_id = lp.deal_id ", 
            "and lp.price_index = pi.index_num ",
            "and lp.delta <> 0 and lp.vega = 0 ",
            "group by d.secdb_book, pi.long_name, lp.contract_date", sep="")
    firstTry = sqlQuery(chan, query)
    Sys.sleep(1)
    secondTry = sqlQuery(chan, query)
    
    while( !all(dim(secondTry)==dim(firstTry)) )
    {
        rLog("We caught RmSys in the middle of an update, trying again...")
        firstTry = sqlQuery(chan, query)
        Sys.sleep(1)
        secondTry = sqlQuery(chan, query)
    }
    
    odbcClose(chan)
    
    # Clean up the data a little
    deltaPositions = secondTry
    names(deltaPositions) = c("book", "curve.name", "contract.dt", "value")
    deltaPositions$book = toupper(deltaPositions$book)
    deltaPositions$curve.name = toupper(deltaPositions$curve.name)
    deltaPositions$contract.dt = as.Date(deltaPositions$contract.dt)
    deltaPositions = deltaPositions[-grep("DOLLARS", deltaPositions$curve.name),]
    
    # Format the daily contracts into monthly terms so they can be aggregated below
    deltaPositions$contract.dt = as.Date(format(deltaPositions$contract.dt, "%Y-%m-01"))
    
    # Aggregate the deltas to get the final position
    deltaPositions = cast(deltaPositions, book+curve.name+contract.dt~., sum)
    names(deltaPositions) = c("book", "curve.name", "contract.dt", "position")
    
    # Remove any empty positions after the aggregation
    deltaPositions = subset(deltaPositions, deltaPositions$position!=0)
    
    return(deltaPositions)
}

