# Check how CNE is using the nightly marks
#
#
#




####################################################################
# Calculate 1 day PnL on the CNE retail deals.
# For example deals priced on 11/1/2013 (Fri) are priced with
# 10/31/2013 (Thu) marks, but they only show up in SecDb on
# 11/5/2013 (Tue).  Price change is between 10/31 and 11/5. 
#
#
.calc_1day <- function(pricingDate, books, rule.PP1="-1b")
{
  pricedOnDay <- as.Date(secdb.dateRuleApply(pricingDate, rule.PP1))
  day1 <- as.Date(secdb.dateRuleApply(pricingDate, "+1b"))
  day2 <- as.Date(secdb.dateRuleApply(pricingDate, "+2b"))

  # Exposed to 
  PP1 <- .pull_marks(pricedOnDay, "HUB DA", startDate=nextMonth(pricingDate),
                     bucket=c("PEAK", "OFFPEAK"))
  PP2 <- .pull_marks(day2, "HUB DA", startDate=nextMonth(pricingDate),
                     bucket=c("PEAK", "OFFPEAK")) 

  # how much has the quantity changed between day1 and day2 when it shows
  # in the books
  QQ1 <- secdb.readDeltas2(day1, books, aggregate=TRUE, energyOnly=TRUE)
  QQ1$bucket <- classifyCurveName(QQ1$curve, by=c("peak/offpeak"))$pkoffpk
  qq1 <- cast(QQ1, bucket + month ~ ., sum, value="value")
  names(qq1)[3] <- "q1"
  qq1 <- subset(qq1, month >= nextMonth(pricingDate))
  
  QQ2 <- secdb.readDeltas2(day2, books, aggregate=TRUE, energyOnly=TRUE)
  QQ2$bucket <- classifyCurveName(QQ2$curve, by=c("peak/offpeak"))$pkoffpk
  qq2 <- cast(QQ2, bucket + month ~ ., sum, value="value")
  names(qq2)[3] <- "q2"
  qq2 <- subset(qq2, month >= nextMonth(pricingDate))

  dQ <- merge(qq1, qq2)
  dQ$value <- dQ$q2 - dQ$q1
  dQ <- dQ[, c("bucket", "month", "value")]

  dP <- merge(PP1, PP2, by=c("mkt", "location", "bucket", "month"))
  dP$value <- dP$value.y - dP$value.x
  
  pnl <- calc_pnl_pq( PP1, PP2, dQ, dQ, groups=c("year") )
  
  cbind(pricingDate=pricingDate,
        .price_diff_summary(dP),
        dQ  = round(sum(dQ$value)),
        PnL = round(sum(pnl$PnL$PnL)))
}


####################################################################
# Calculate the 12month, 24mth, 36mth peak price change
#
.price_diff_summary <- function(X)
{
  x <- subset(X, location == "HUB DA" & bucket == "PEAK")

  x <- x[order(x$month),]

  data.frame(dP.12mth = round(mean(x$value[1:12]),2),
             dP.24mth = round(mean(x$value[1:24]),2),
             dP.36mth = round(mean(x$value[1:36]),2))
}


####################################################################
####################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(PM)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/cne_price_gouging.R")

 
  books <- PM:::secdb.getBooksFromPortfolio("CNE NewEngland Load Portfolio")

  days <- seq(as.Date("2014-01-01"), as.Date("2014-04-27"), by="1 day")

  res <- ldply(days, function(day) {
    pricingDate <- as.Date(day, origin="1970-01-01")
    if (pricingDate != as.Date(secdb.dateRuleApply(pricingDate, "+0b"))) 
      return(NULL)   # it's not a business day!
    
    rLog("Working on day", format(pricingDate))
    res <- try(.calc_1day(pricingDate, books, rule.PP1="-1b"))  
    if (class(res) == "try-error") NULL else res
  })
  print(res, row.names=FALSE)

  sum(res$PnL)

  plot(res$dP.12mth, res$PnL)

  # save the data in a spreadsheet in
  # H:\user\R\RMG\Energy\Trading\Congestion\NEPOOL\CNE/Other/
  

  
}
