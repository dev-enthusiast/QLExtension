################################################################################
# This code verifies that the price count in the database for today is within
# a certain percentage of the previous days counts
#

################################################################################
# External Libraries
#
source('H:/user/R/RMG/Utilities/Packages/load.packages.r')
load.packages( c("SecDb", "RODBC") )
source("//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/overnightUtils.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/sendemail.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/RLogger.R")
source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/easyOutliers.R")

################################################################################
# File Namespace
#
PriceSafetyCheck = new.env(hash=TRUE)




################################################################################
#
PriceSafetyCheck$main <- function()
{
  #how far back do we want to check CPSPROD
  DAY_RANGE_TO_QUERY = 7
  
  #How much leeway, as a percent to allow before a warning is triggered
  ROW_COUNT_LEEWAY = .1

  # Get the last DATE_RANGE_TO_QUERY business days
  endDate = as.Date(overnightUtils.getAsOfDate())
  startDate = as.Date( secdb.dateRuleApply( endDate, paste("-",
    DAY_RANGE_TO_QUERY, "b", sep="") ) )
  dateList = seq(startDate, endDate, by="day")
  dateList = lapply(as.list(as.character(dateList)), as.Date)
  isHoliday <- unlist( secdb.invoke("AAD: IsHoliday", "IsHoliday", dateList,
    "CPS-BAL", FALSE) )
  dateList = dateList[isHoliday==0]
  
  PriceSafetyCheck$checkRange( dateList, ROW_COUNT_LEEWAY )

  rLog("Done.")
}


################################################################################
PriceSafetyCheck$checkRange <- function( dateList, rowCountLeeway )
{
  #Connect to CPSPROD
  rLog("Connecting to CPSPROD...")
  con.string <- paste("FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn",
       ";UID=stratdatread;PWD=stratdatread;", sep="")
  CPSPROD <- odbcDriverConnect(con.string)
  rLog("Connected.")

  # Retrieve the row counts for each date in the dateList
  dateList = unlist(dateList)
  class(dateList) = "Date"
  rLog("Querying for previous days row counts...")
  query <- paste("SELECT count(*) from foitsox.sdb_prices ",
    "WHERE PRICING_DATE=TO_DATE('", as.character(dateList),
    "','YYYY-MM-DD')", sep="")
  rowCounts = unlist(lapply(query, sqlQuery, channel=CPSPROD))
  rLog("Queries Complete.")
  odbcClose(CPSPROD)

  todaysCount = rowCounts[length(rowCounts)]
  prevAvgCount = mean(rowCounts[1:length(rowCounts)-1])

  PriceSafetyCheck$.sendStatus( dateList[length(dateList)], todaysCount,
    prevAvgCount, rowCountLeeway )
}


################################################################################
# If today's count is not less than the rowCountLeeway, send the warning email
#
PriceSafetyCheck$.sendStatus <- function( asOfDate, todaysCount, prevAvgCount,
  rowCountLeeway )
{
  body = paste("As Of Date:", asOfDate)
  todayString = paste("Today's Count:", todaysCount)
  prevString = paste("Average of the Previous Counts:", prevAvgCount)
  body = paste(body, todayString, prevString,
    "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")

  if( ( abs(todaysCount-prevAvgCount) / prevAvgCount ) < rowCountLeeway )
  {
    subject = paste("SUCCESS: ", asOfDate, " Price Count Is Within ",
      rowCountLeeway, "% of Previous Price Counts", sep="")

    toList ="john.scillieri@constellation.com"

  } else
  {
    subject = paste("WARNING: ", asOfDate, " Price Count Is NOT Within ",
      rowCountLeeway, "% of Previous Price Counts", sep="")

    toList ="john.scillieri@constellation.com"
  }

  sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
    to=toList, subject, body)
  
  rLog(subject)
}


################################ MAIN EXECUTION ################################
res = try( PriceSafetyCheck$main() )
if (class(res)=="try-error"){
  cat("Failed Price Safety Check.\n")
}

