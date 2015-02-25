##  Read Monthly/Term Option Premium from SecDB
##  Inputs needed
##  Region  - FERC region name
##  loc - SecDb location name from Calc
##  bucket - SecDb bucket name
##  freq - 'Day' for Daily option, 'Month' for Monthly Option
##  OptType - Put/Call
##  StartDate - Start Date of the Option
##  NumMth - Number of months in the option
##  Strike - Optiion Strike Price
##  ExpRule - Expiration Date Rule. Note NEPOOL we use "-1b"
##  AsOfDate - valuation date
##  Aggregate - get term option price if TRUE, Monthly option price if FALSE

secdb.getOptionPremium <- function(region="NEPOOL", loc="hub", bucket="5x16", freq="Day", OptType="Call", StartDate=nextMonth(Sys.Date()), NumMth=1, Strike=50, ExpRule="+0b", AsOfDate=as.Date(Sys.Date()), Aggregate = FALSE)
{
    libSec <- "_Lib matlab vol fns"
    fnSec <- "GetElecValue"
    inputs <- list(Asset=paste("Commod PWR ", region, " Physical",sep=""), Location=loc, Bucket=bucket, SecType=freq, OptType=OptType, StartDate=StartDate, NumMth=NumMth, Strike=Strike, ExpRule=ExpRule, AsOf=AsOfDate)
    output <- as.data.frame(secdb.invoke(libSec, fnSec, inputs))
    output$date <- as.Date(format(output$date, "%Y-%m-01"))
    if (!Aggregate)
        ret <- output else {
            ## Do weighted average over hours to get a term option price
           mthHrs <- ntplot.bucketHours( bucket, region=region, startDate = StartDate, endDate = as.Date(secdb.dateRuleApply( StartDate, paste("+J+",NumMth,"c",sep=""))))
           mthHrs <- rename(mthHrs, c("value"="hours"))
           x <- merge(output, mthHrs, by="date")[, c("date", "hours", "value" )]
           ret <- sum((x$hours*x$value))/sum(x$hours)
        }
    ret
}


demo.secdb.OptPremium <- function()
{
  require(CEGbase)
  require(SecDb)
  require(plyr)


  op <- secdb.getOptionPremium(region = "NEPOOL", loc = "Hub", StartDate = as.Date("2015-01-01"), NumMth = 2, Strike = 186, ExpRule = "-1b", AsOfDate = as.Date("2014-10-31"), Aggregate = TRUE)
}

