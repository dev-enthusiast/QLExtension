# Functions to deal with CNE capacity reports, etc.
#
# get_booked_capacity_cerinp
#

#######################################################################
# 
get_booked_capacity_cerinp <- function(month, asOfDate=NULL)
{
  require(RODBC)
  startDt <- as.Date(format(month, "%Y-%m-01"))    # beginning of month
  endDt   <- seq(startDt, by="1 month", length.out=2)[2]-1
  if (is.null(asOfDate))
    asOfDate <- as.Date(secdb.dateRuleApply(endDt, "-0b"))
  query <- paste("select load_date, zone, sum(amount)/1000 from ",
    "CER.capacity_load_obligation where pricing_date=to_date('",
    format(asOfDate, "%Y-%m-%d"), "', 'YYYY-MM-DD') ", 
    "and control_area='ISONE' and hedge_type='Capacity' ",  
    "and load_date>=to_date('", format(startDt), "', 'YYYY-MM-DD') ",
    "and load_date<=to_date('", format(endDt), "', 'YYYY-MM-DD') ",
    "group by load_date, zone order by load_date, zone",         
    sep="")

  rLog("Pulling CNE capacity data from CERINT as of", format(startDt))
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/CERINP.dsn"),
    ";UID=cer_pmo_ro;PWD=cerpmoro;", sep="")  
  con <- odbcDriverConnect(con.string)
  data <- sqlQuery(con, query)
  #data   <- sqlFetch(con, "CER.capacity_load_obligation", max=4)
  odbcCloseAll()

  colnames(data) <- c("date", "zone", "capacity")
  rLog("Done.")
  data <- cbind(month=month, data)

  return(data)
}

