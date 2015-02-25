# Example of how to use customize a calculator to book a 
# custom Elec CFD (monthly) trade.
#
# 

#####################################################################
#
read_data <- function(filename)
{
  filename <- "S:/All/Structured Risk/NEPOOL/Temporary/Book2.xlsx"
}


#####################################################################
#####################################################################
require(CEGbase)
require(SecDb)
require(reshape)
require(PM)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.book.trade.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))

# construct positions
qq <- data.frame(month=seq(as.Date("2012-01-01"), as.Date("2012-12-01"), by="1 month"))
qq$`CT DA_2X16H` <- rep(1, 12)
qq$`CT DA_5X16`  <- c(1, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 2)
qq$`CT DA_7X8`   <- rep(1, 12)


# get prices
startDate <- qq$month[1] 
endDate   <- qq$month[nrow(qq)]
PP <- .get_prices(asOfDate, qq)


# adjust prices for hedge costs... 
adjPP <- PP
## ind <- which(format(adjPP$month, "%Y")=="2012")
## adjPP[ind, 2] <- adjPP[ind, 2] - 0.40 


# customize the calc, you may need to change the delivery point!
# you'll get errors if things dont' work properly. 
fullCalcName <- "Calc ECFD NEPOOL 1x3          0"
secdb.customizeCalculator(fullCalcName, quantity=qq, fixPrice=adjPP)


