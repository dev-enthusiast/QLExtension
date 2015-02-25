################################################################################
library(RODBC)
library(reshape)
library(SecDb)
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# Available commodities include {NG, CL, HO}
#
.getNewestData <- function(commod="NG")
{
    contracts = unlist( secdb.getValueType("Bal Close NG Futures Strip", "Contracts") )

    prices = as.numeric( unlist( secdb.getValueType("Bal Close NG Futures Strip", "Price Quotes") ) )
    
    return( data.frame(Term = contracts, ATM = prices) )
    
}


################################################################################
.sendEmail <- function( output )
{   
    time = paste("Time:", Sys.time())
    
    valueLine = paste( capture.output(output), collapse="\n" )

    bodyOfEmail = paste( time, valueLine, 
            "CEG Risk Management Group - Analytics\njohn.scillieri@constellation.com\n\n", sep="\n\n")
    
    subject = "RmSys/SecDb Vol Report"
    
    toList = getEmailDistro("RmSysVols.R")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, bodyOfEmail )
}


################################################################################
.priceStraddle <- function( Term, Strike, Vol )
{
    term = as.character( Term )
    strike = as.numeric( Strike )
    vol = as.numeric( Vol )
    
    price = secdb.invoke( "RWrapper", "RWrapper::priceNymexStraddle", 
            term, strike, vol )
    
    return( price )
}


###############################################################################
DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn"
conString = paste( dsnPath, ";UID=rmsys_read;PWD=rmsys_read;", sep="")
rLog("Connecting to the database...")
chan = odbcDriverConnect(conString)

rLog("Pulling vol point data for today...")
today = as.character(Sys.Date())
query = paste( "select * from rmsys.vol_points ",
        "where strike=1 ",
        "and effective_date=to_date('", today, "', 'YYYY-MM-DD') ", 
        "and INDEX_NUM=4", sep="" )
todaysData = sqlQuery(chan, query)

rLog("Pulling vol point data for yesterday...")
yesterday = as.character(as.Date(secdb.dateRuleApply( Sys.Date(), "-1b")))
query = paste( "select * from rmsys.vol_points ",
        "where strike=1 ",
        "and effective_date=to_date('", yesterday, "', 'YYYY-MM-DD') ", 
        "and INDEX_NUM=4", sep="" )
yesterdaysData = sqlQuery(chan, query)
odbcClose( chan )

rLog("Calculating changes...")
allData = rbind(todaysData, yesterdaysData)
allData = allData[, c("EFFECTIVE_DATE", "FROM_DATE", "VOLATILITY")]
names(allData) = c("pricing.dt", "contract.dt", "value")
allData = cast( allData, contract.dt ~ pricing.dt )
allData$Change = allData[,3] - allData[,2]
allData$Percent = round((allData$Change / allData[,2] * 100), 1)

rLog("Getting reuters codes for dates")
allData$contract.dt = as.Date(allData$contract.dt)
allData$Term = Term$reutersCode(Term$fromDatePair(allData$contract.dt))

allData = subset( allData, contract.dt < as.Date("2010-01-01") )

allData = allData[, c("Term", yesterday, today, "Change", "Percent")]

rLog("Pulling newest market data...")
marketData = .getNewestData()
finalData = merge(allData, marketData, sort=FALSE)

rLog("Pricing straddles...")
finalData$Straddle = apply(finalData, 1, 
        function(x){ .priceStraddle( x[["Term"]], x[["ATM"]], x[[today]] )})

rLog("Formatting data...")
output = finalData[, c("Term", yesterday, today, "Change", "Percent", "ATM", "Straddle")]
SIGNIFICANT = 4
output[[yesterday]] = prettyNum(output[[yesterday]], digits = SIGNIFICANT)
output[[today]] = prettyNum(output[[today]], digits = SIGNIFICANT)
output$Change = prettyNum(output$Change, digits = SIGNIFICANT)
output$Percent = prettyNum(output$Percent, digits = SIGNIFICANT)
output$ATM = prettyNum(output$ATM, digits = SIGNIFICANT)
output$Straddle = prettyNum(output$Straddle, digits = SIGNIFICANT)

.sendEmail(output)
rLog("\nDone.")



