###############################################################################
# Daily A' Summary Report for A. Good.
# 
# Author:	J. Scillieri
# Added:	main.R Initiated Feb 26, 2009
# 
library( RODBC )
source( "H:/user/R/RMG/Utilities/write.xls.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )


###############################################################################
main <- function()
{
    asOfDate = getAsOfDate()
    
    while( !dataAvailable( asOfDate ) ) 
    {
        rLog("Data not available... sleeping 5 minutes...")        
        Sys.sleep( 300 )
    }
    
    start = Sys.time()
    
    rLog("Pulling Sheet 1...")
    sheet1 = getDataByCounterpartyAgreement( asOfDate )
    
    rLog("Pulling Sheet 2...")
    sheet2 = getDataByBusinessUnit( asOfDate )
    
    rLog("Pulling Sheet 3...")
    sheet3 = getDataByTradeLevel( asOfDate )
    
    BASE_PATH = paste( "S:/All/Risk/Cash Project/Daily Collateral Analysis/",
            "DAILY APRIME_RPT/", sep="" )
    
    outputFolder = paste( BASE_PATH, format( asOfDate, "%Y%m" ), sep="" )
    dir.create( outputFolder, showWarnings = FALSE )
    
    outputFile = paste( outputFolder, "/", format( asOfDate, "%m%d%Y" ), 
            "_Daily.xlsx", sep="" ) 
    
    rLog("Copying template to new file...")
    TEMPLATE = paste( BASE_PATH, "TEMPLATE.xlsx", sep="" )
    file.copy( TEMPLATE, outputFile )
    
    rLog("Saving Sheet 1...")
    write.xls( sheet1, outputFile, sheet="By Counterparty" )
    
    rLog("Saving Sheet 2...")
    write.xls( sheet2, outputFile, sheet="By Business Unit" )
    
    rLog("Saving Sheet 3...")
    write.xls( sheet3, outputFile, sheet="Trade Level" )
    
    rLog("Done.")
    
    print( Sys.time() - start )
}


###############################################################################
dataAvailable <- function( asOfDate )
{
    query = paste( "select count(1) from aprime_base a ", 
            "where a.ASOFDATE = '", format( asOfDate, "%d%b%y"), "'", sep = "" )
    resultCount = runQuery( query )
    
    return( resultCount != 0 )
}


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate. 
# If that fails, use yesterday.
#
getAsOfDate <- function()
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


###############################################################################
# Summary by Counterparty - Agreement Level
#
getDataByCounterpartyAgreement<- function( asOfDate )
{
    query = paste( 
    "select 
      a.COUNTERPARTY
    , a.CREDIT_NETTINGAGREEMENT
    , a.COUNTERPARTY || ' - ' || a.CREDIT_NETTINGAGREEMENT CP_AGREEMENT
    
    , SUM(NVL(a.MTM, 0)) MTM
    , SUM(NVL(a.DNP, 0)) DNP
    , SUM(NVL(a.EXPOSURE, 0)) EXPOSURE
    
    , SUM(NVL(a.CASHPOST, 0)) CASH_POST
    , SUM(NVL(a.CASHHELD, 0)) CASH_HELD   
    , SUM(NVL(a.LCPOST, 0)) LC_POST
    , SUM(NVL(a.LCHELD, 0)) LC_HELD
    
    , SUM(NVL(a.cashpostreq, 0)) CASH_POST_PENDING
    , SUM(NVL(a.cashheldreq, 0)) CASH_HELD_PENDING
    , SUM(NVL(a.lcpostreq, 0)) LC_POST_PENDING
    , SUM(NVL(a.lcheldreq, 0)) LC_HELD_PENDING
    
    , SUM(NVL(a.Cashpost, 0) + NVL(a.cashpostreq, 0)) CASH_POST_NET
    , SUM(NVL(a.Cashheld, 0) + NVL(a.cashheldreq, 0)) CASH_HELD_NET
    , SUM(NVL(a.LCPOST, 0) + NVL(a.LCpostreq, 0)) LC_POST_NET
    , SUM(NVL(a.LCHELD, 0) + NVL(a.LCHELDreq, 0)) LC_HELD_NET    
    
    from aprime_base a
    where a.ASOFDATE = '", format( asOfDate, "%d%b%y"), "'
    AND a.COUNTERPARTY is not null
    group by a.COUNTERPARTY, a.CREDIT_NETTINGAGREEMENT", sep="" )
    
    data = runQuery( query )
    
    return( data )
}


###############################################################################
# Detail by Business Unit Counterparty
#
getDataByBusinessUnit <- function( asOfDate )
{
    query = paste( 
    "select 
    a.CER_GROUP BUSINESS_UNIT
    , a.COUNTERPARTY
    , a.CREDIT_NETTINGAGREEMENT AGREEMENT
    
    , NVL(SUM(a.MTM),0) MTM
    , NVL(SUM(a.DNP), 0) DNP
    , NVL(SUM(a.EXPOSURE), 0) EXPOSURE
    
    , SUM(NVL(a.CASHPOST, 0)) CASH_POST
    , SUM(NVL(a.CASHHELD, 0)) CASH_HELD   
    , SUM(NVL(a.LCPOST, 0)) LC_POST
    , SUM(NVL(a.LCHELD, 0)) LC_HELD
    
    , SUM(NVL(a.cashpostreq, 0)) CASH_POST_PENDING
    , SUM(NVL(a.cashheldreq, 0)) CASH_HELD_PENDING
    , SUM(NVL(a.lcpostreq, 0)) LC_POST_PENDING
    , SUM(NVL(a.lcheldreq, 0)) LC_HELD_PENDING
    
    , SUM(NVL(a.Cashpost, 0) + NVL(a.cashpostreq, 0)) CASH_POST_NET
    , SUM(NVL(a.Cashheld, 0) + NVL(a.cashheldreq, 0)) CASH_HELD_NET
    , SUM(NVL(a.LCPOST, 0) + NVL(a.LCpostreq, 0)) LC_POST_NET
    , SUM(NVL(a.LCHELD, 0) + NVL(a.LCHELDreq, 0)) LC_HELD_NET   
    
    from aprime_base a
    where a.ASOFDATE = '", format( asOfDate, "%d%b%y"), "'
    AND a.COUNTERPARTY is not null
    group by  a.CER_GROUP, a.COUNTERPARTY, a.CREDIT_NETTINGAGREEMENT", sep="" )  
    
    data = runQuery( query )
    
    return( data )
}


###############################################################################
# Detail by Trade Level
#
getDataByTradeLevel <- function( asOfDate )
{
    query = paste( 
    "select 
      a.COUNTERPARTY
    , a.CREDIT_NETTINGAGREEMENT AGREEMENT
    , a.CER_GROUP BUSINESS_UNIT
    , a.EXTERNALDEALID
    , a.BOOK_NAME BOOK
    
    , SUM(NVL(a.MTM, 0)) MTM
    , SUM(NVL(a.DNP, 0)) DNP
    , SUM(NVL(a.EXPOSURE, 0)) EXPOSURE
    
    , SUM(NVL(a.CASHPOST, 0)) CASH_POST
    , SUM(NVL(a.CASHHELD, 0)) CASH_HELD   
    , SUM(NVL(a.LCPOST, 0)) LC_POST
    , SUM(NVL(a.LCHELD, 0)) LC_HELD
      
    , SUM(NVL(a.cashpostreq, 0)) CASH_POST_PENDING
    , SUM(NVL(a.cashheldreq, 0)) CASH_HELD_PENDING
    , SUM(NVL(a.lcpostreq, 0)) LC_POST_PENDING
    , SUM(NVL(a.lcheldreq, 0)) LC_HELD_PENDING
    
    , SUM(NVL(a.Cashpost, 0) + NVL(a.cashpostreq, 0)) CASH_POST_NET
    , SUM(NVL(a.Cashheld, 0) + NVL(a.cashheldreq, 0)) CASH_HELD_NET
    , SUM(NVL(a.LCPOST, 0) + NVL(a.LCpostreq, 0)) LC_POST_NET
    , SUM(NVL(a.LCHELD, 0) + NVL(a.LCHELDreq, 0)) LC_HELD_NET      
        
    from aprime_base a
    where a.ASOFDATE = '", format( asOfDate, "%d%b%y"), "'
    AND a.COUNTERPARTY is not null
    group by a.EXTERNALDEALID, a.BOOK_NAME, a.COUNTERPARTY, 
	a.CREDIT_NETTINGAGREEMENT, a.CER_GROUP", sep="" )  
    
    data = runQuery( query )
    
    return( data )
}


###############################################################################
runQuery <- function( query )
{
    con = getConnection()
    
    data = sqlQuery( con, query )
    
    odbcClose( con )
    
    return( data )
}


###############################################################################
getConnection <- function()
{
    dsnFile = "RMGDBP.dsn"
    dsnPath = "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", dsnPath, dsnFile, 
            ";UID=RMGDBUSER;PWD=RMGDBPWD_PROD;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    return( con )
}

