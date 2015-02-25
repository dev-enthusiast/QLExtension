##  Read Monthly/Term Heat Rate Option Premium from SecDB
##  Inputs needed
##  Region  - FERC region name
##  loc - SecDb location name from Calc
##  gasCommod - name of the Secdb gas commod e.g NG ALGCG
##  gasLoc - name of the secdb gas location e.g Gas-Dly Mean
##  bucket - SecDb bucket name
##  freq - 'Daily' for Daily option, 'Monthly' for Monthly Option
##  OptType - Put/Call
##  StartDate - Start Date of the Option
##  NumMth - Number of months in the option
##  Strike - HR Option Strike Price
##  HR - Heat rate of the option. use "ATM" to use a ATM HR option
##  ExpRule - Expiration Date Rule. Note NEPOOL we use "-1b"
##  AsOfDate - valuation date
##  Aggregate - get term option price if TRUE, Monthly option price if FALSE

secdb.HR.OptPremium <- function(region="NEPOOL", loc="hub", gasCommod="NG ALGCG", gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call", StartDate=nextMonth(Sys.Date()), NumMth=1, Strike=0, HR="ATM", ExpRule="-1b", AsOfDate=as.Date(Sys.Date()), Aggregate = FALSE)
{
    AsOfDate <- secdb.dateRuleApply(AsOfDate, "-0b", calendar = "CPS-BAL")

    libSec <- "_Lib elec matlab Functions"
    fnSec <- "Matlab::ElecBasketExp"

    pwrCommod <- paste("PWR ", region, " Physical", sep="");
    EndDate <- as.Date(secdb.dateRuleApply(StartDate,paste("+J+",NumMth-1,"ce",sep="") ))

    ## Fetch market power, gas and heat rate
    pwrCrv <- secdb.getElecPrices( as.Date(AsOfDate), as.Date(StartDate), as.Date(EndDate), paste("Commod PWR ", region, " Physical", sep=""), loc, bucket, "Energy", FALSE)
    pwrCrv <- pwrCrv[,c("month", "value")]
    names(pwrCrv) <- c("month", "power")

    gasCrv <- secdb.getElecPrices( as.Date(AsOfDate), as.Date(StartDate), as.Date(EndDate), "Commod PWR NG Physical", paste(gsub("NG ", "", gasCommod),"/",gasLoc,sep=""), bucket, "Energy", FALSE)
    gasCrv <- gasCrv[,c("month", "value")]
    names(gasCrv) <- c("month", "gas")

    HRCrv <- merge(gasCrv, pwrCrv, by="month")
    if (HR=="ATM"){
        HRCrv$HR <- HRCrv$power/HRCrv$gas
    } else {
        HRCrv$HR <- HR
    }
    ret <- HRCrv

    ## Now ask SecDb for the HR option prices
    if (!Aggregate){
        seqMths <- seq(StartDate, EndDate, by="1 month") ##Fetch HR option prices By Month
        for (i in seq_along(seqMths)){
            thisMthB <- seqMths[i]
            thisMthE <- nextMonth(seqMths[i])-1
            thisHR <- HRCrv[HRCrv$month==as.Date(thisMthB),"HR"]
            output <- as.data.frame(secdb.invoke(libSec, fnSec, as.Date(AsOfDate), pwrCommod, loc, gasCommod, gasLoc, as.Date(thisMthB), as.Date(thisMthE), Strike, thisHR, freq, bucket, OptType, ExpRule))
            ret[ret$month==as.Date(thisMthB),c("HROpt")] <-as.double(output)
        }
    } else {
        ##Get HR Option prices for the term
           mthHrs <- ntplot.bucketHours( bucket, region=region, startDate=StartDate, endDate=as.Date(secdb.dateRuleApply( StartDate, paste("+J+",NumMth,"c",sep=""))))
           mthHrs <- rename(mthHrs, c("value"="hours", "date"="month"))
           x <- merge(HRCrv, mthHrs, by="month")[, c("month", "hours", "HR", "power", "gas" )]
           ret <- as.data.frame(lapply(x[,-c(1,2)], weighted.mean, w=x$hours)) ##Get Weighted average for all commodities
           ret$month <- "Term"
           ret$HR <- ret$power/ret$gas
           output <- as.data.frame(secdb.invoke(libSec, fnSec, as.Date(AsOfDate), pwrCommod, loc, gasCommod, gasLoc, as.Date(StartDate), as.Date(EndDate), Strike, ret$HR, freq, bucket, OptType, ExpRule))
           ret$HROpt <- as.double(output)
    }
    ret
}


demo.secdb.HR.OptPremium <- function()
{
  require(CEGbase)
  require(SecDb)
  require(plyr)

  op <- secdb.HR.OptPremium(region="NEPOOL", loc="hub", gasCommod="NG ALGCG", gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call", StartDate=nextMonth(Sys.Date()), NumMth=12, Strike=0, HR="ATM", ExpRule="-1b", AsOfDate=as.Date(Sys.Date()), Aggregate = FALSE)
}

