# Get the commod prices and save it locally for local consumption
# Use the good 'ol function from the R VaR project that has now failed.
#
#
# Written by AAD on 25-Jan-2010

#################################################################
# Takes about 2 min
#
get.one.day.prices <- function(asOfDate, dirOut)
{
  rLog("Connect to CPSPROD")
  con.string <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
       ";UID=stratdatread;PWD=stratdatread;", sep="")  
  con <- odbcDriverConnect(con.string)
 
  cat(paste("Get all prices for", asOfDate, "... "))
  query <- paste("SELECT commod_curve, start_date, end_date, price ",
    "from foitsox.sdb_prices WHERE PRICING_DATE=TO_DATE('",
    as.character(asOfDate), "','YYYY-MM-DD') order by commod_curve, end_date", sep="")
  hP <- sqlQuery(con, query)  # all the prices 
  names(hP) <- c("curve.name", "start.dt", "end.dt", as.character(asOfDate))
  odbcCloseAll()
  
  hP$start.dt <- as.Date(hP$start.dt); hP$end.dt <- as.Date(hP$end.dt)
  hP$contract.dt <- as.Date(format(hP$start.dt, "%Y-%m-01")) 
  eom <- as.Date(format(hP$contract.dt + 31, "%Y-%m-01"))-1  # end of the month 
  hP  <- hP[(hP$end.dt == eom),]  # keep only the balance of the month prices
  hP$start.dt <- hP$end.dt <- NULL
  hP$curve.name <- toupper(as.character(hP$curve.name)) 
  hP <- hP[!duplicated(hP),]                   # there are some weird duplicates
  
  filename <- paste(dirOut,"eod_commod_prices_", asOfDate, ".RData", sep="")
  if (nrow(hP)>0){save(hP, file=filename)}
  rLog("Saved prices to file", filename)
  cat("Done!\n")
}

#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)
require(methods)   # not sure why this does not load!
require(CEGbase); require(CEGterm); require(SecDb); require(RODBC)
Sys.setenv(tz="")
require(reshape)
#source("H:/user/R/RMG/Utilities/Procmon.R")


# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E47187;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

cArgs <- commandArgs()
if (length(cArgs)>6){
  asOfDate <- as.Date(cArgs[7])  # pass asOfDate ... 
} else {
  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
}

rLog(paste("Running report for", as.character(asOfDate)))
rLog("\n\n")


# check if the job has finished   -- NOT WORKING NOW
## PROCMON_DEPENDENCY = "power/SdbOraExport"
## while( Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE) != "Succeeded" )
## {
##   rLog(PROCMON_DEPENDENCY, "Status:", 
##     Procmon$getStatus(PROCMON_DEPENDENCY, asOfDate, exact=TRUE))
##   rLog("Sleeping for 15 mins...")
##   Sys.sleep(900)   
## }

dirOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Prices/"
get.one.day.prices(asOfDate, dirOut)

rLog(paste("Done at ", Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

