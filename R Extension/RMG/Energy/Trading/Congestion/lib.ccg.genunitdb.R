# Functions to look into gen unit database
#
#
#


#######################################################################
# return all the units monitored in the db.
# table is quite big +2 MM
#
.get_all_ccg_units <- function()
{

  query <- "select distinct unit_name from genunit.outage_his"
  
  rLog("Querying GenUnit db ...")
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ASTOPP.dsn"),
    ";UID=genunit_read;PWD=genunit_read;", sep="")  
  con <- odbcDriverConnect(con.string)
  data <- sqlQuery(con, query) 
  odbcCloseAll()

  data <- data[order(data$UNIT_NAME),]
  
  data
}


#######################################################################
# pull info only on the units you're interested
#
.get_planned_outages <- function(units, asOfDate=Sys.Date()-1)
{
  query <- paste("select * from genunit.outage_his where ",
    "REPORT_DATE = '", toupper(format(asOfDate, "%d-%b-%y")), "' and ",      
    "UNIT_NAME in ('", paste(units, sep="", collapse="', '"), "')",
    sep="")
  
  rLog("Querying GenUnit db ...")
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ASTOPP.dsn"),
    ";UID=genunit_read;PWD=genunit_read;", sep="")  
  con <- odbcDriverConnect(con.string)
  data <- sqlQuery(con, query) 
  odbcCloseAll()

  data <- data[order(data$OUTAGE_START_DATE),]
  
  data
}

#######################################################################
#######################################################################
#
.main <- function()
{

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.genunitdb.R")
  require(CEGbase)
  require(RODBC)
 
  all_units <- .get_all_ccg_units()

  units <- c(
    grep("^My", all_units, value=TRUE),        # all the Mystics
    grep("^Fore Ri", all_units, value=TRUE),   # all ForeRiver
    grep("^Kl", all_units, value=TRUE)         # no Kleen in there
    )

  asOfDate <- Sys.Date()-1
  out <- .get_planned_outages(units, asOfDate=asOfDate)

  fname <- paste("C:/Temp/genunit_outages_", format(asOfDate), ".csv", sep="")
  write.csv(out, file=fname, row.names=FALSE)
  
  
}

