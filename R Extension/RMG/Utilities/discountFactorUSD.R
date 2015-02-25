################################################################################
#function that gets pieces of the USD discount factor curve from archived sas data
#
#Author: Sean McWilliams
#
################################################################################

source("H:/user/R/RMG/Utilities/load.R")

################################################################################
discountFactorUSD <- function(pricingDate = Sys.Date() - 1, startStrip, endStrip)
{
	
	startStrip <- as.Date(startStrip) + (daysInMonth(as.Date(startStrip)) - 1) + 25
	endStrip <- as.Date(endStrip) + (daysInMonth(as.Date(endStrip)) - 1) + 25
	contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
	
	inFile <- paste("S:/All/Risk/Data/VaR/prod/DiscountFactors/usd.", pricingDate, ".RData",sep="")
	discounts <- varLoad("discountCurveUSD", inFile, TRUE)
	
	discounts <- discounts[match(contractDates, discounts$date),]
	
	return(discounts$value)
	
}