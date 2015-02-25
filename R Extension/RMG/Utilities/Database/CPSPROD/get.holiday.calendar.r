# This function returns a calendar, start date and end date based on specified asOfDate and
#      number of business days
#
# options$asOfDate <- as.Date("2007-03-15")
# options$number.of.days <-71
# options$calendar.type <- "CPS-BAL"
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"
# options$file.dsn <- "CPSPROD.dsn"
#
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.holiday.calendar.R")
# res <- get.holiday.calendar(options)

get.holiday.calendar <- function(options) {
   if (length(options$asOfDate)==0){options$asOfDate <- Sys.Date()-1}
   if (length(options$number.of.days)==0){options$number.of.days <- 71}
   if (length(options$calendar.type)==0){options$calendar.type <- "CPS-BAL"}
   if (length(options$file.dsn)==0){options$file.dsn <- "CPSPROD.dsn"}
   if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
   
   query <- paste("select calendar_date from foitsox.sdb_holiday_calendar ",
      "where pricing_date = to_date('", options$asOfDate, "', 'yyyy-mm-dd') ",
      "and calendar = '", options$calendar.type, "' ",
      "and holiday = 'N' ",
      "and weekend = 'N' ",
      "and calendar_date <= to_date('", options$asOfDate, "', 'yyyy-mm-dd')  ",
      "order by calendar_date desc ", sep='')
      
   connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn,
     ";UID=stratdatread;PWD=stratdatread;", sep="")
   con <- odbcDriverConnect(connection.string)
   
   tmp <- sqlQuery(con, query)
   odbcCloseAll()
   
   res <- NULL
   res$calendar <- as.Date(tmp$CALENDAR_DATE)
   res$start.date <- res$calendar[options$number.of.days]
   res$end.date <- res$calendar[1]
   res$calendar <- res$calendar[1:options$number.of.days]
   return(res)
}