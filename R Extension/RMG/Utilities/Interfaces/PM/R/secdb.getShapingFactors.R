##  Read Shaped Fwd curve or Shaping Factors from Secdb. Output in hour Beg format
##  Inputs needed
##  Region  - FERC region name. Default to NEPOOL.
##  loc - SecDb location name from. Default to hub
##  StartDate - Start Date of the Option. Default to nextMonth(Sys.Time)
##  EndDate -End Date of the option. Default to end of next Month
##  AsOfDate - valuation date. Default to Sys.Date()
##  Service Type - defaults to Energy

secdb.getSecDbShaping <- function(region="NEPOOL", loc="hub", StartDate=nextMonth(Sys.Date()), EndDate=nextMonth(nextMonth(Sys.Date()))-1, AsOfDate=as.Date(Sys.Date()), ServiceType ="Energy")
{
    libSec <- "_Lib Elec PPA Utils"
    fnSec <- "Elec Utils::FwdShapedCurve"

    output <- as.data.frame(secdb.invoke(libSec, fnSec, region, loc, ServiceType, as.Date(AsOfDate), as.Date(StartDate), as.Date(EndDate)))
    output$bucket <- addBucket(output$time)
    output <- rename(output, c(value="hrly_pwr"))
    # Change to hour beginning format
    output$time <- as.POSIXct(format(output$time-1, "%Y-%m-%d %H:00:00"))
    output$date <- as.Date(format(output$time, "%Y-%m-%d"))

    output$hour_beg <- as.double(format(output$time, "%H"))

    bucPrices <- ddply( output, .(date, bucket),summarize, buc_pwr=mean(hrly_pwr) )
    output <- merge(output, bucPrices)
    output <- output[order(output$time),]
    output$time <- NULL
    output <- output[, c("date", "hour_beg", "hrly_pwr", "buc_pwr")]
    rownames(output) <- 1:nrow(output)
    output$shaping_pct<- output$hrly_pwr/output$buc_pwr

    output
}


demo.secdb.getSecDbShaping <- function()
{
  require(CEGbase)
  require(SecDb)
  require(plyr)
  require(reshape2)

  region <- "NEPOOL"
  loc <- "hub"
  StartDate <- as.Date("2016-01-01")
  EndDate <- as.Date("2016-01-31")
  AsOfDate <- as.Date("2015-01-15")
  ServiceType <- "Energy"

  op <- secdb.getSecDbShaping(region, loc, StartDate, EndDate, AsOfDate, ServiceType)
}
