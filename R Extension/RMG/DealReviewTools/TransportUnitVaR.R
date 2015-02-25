################################################################################
#functions for Rpad web application to calculate transportation unit var
#
# Author: Sean McWilliams
#
################################################################################
#library(SecDb)
library(reshape)
source("H:/user/R/RMG/Utilities/Math/prodSum.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/daysInMonth.R")
################################################################################
#enviornment name
TransportUnitVaR = new.env(hash = TRUE)
#

################################################################################
#return a 71 day price history for a specific strip
#Curvename: TsDb curvename
#date is the most recent date from which to run the VaR report
#startStrip is the first contract date
#endStrip is the last contract date
#returns a dataframe with rows=contract dates, cols=pricing dates

TransportUnitVaR$getPriceData <- function( curveName, repDate=Sys.Date(), startStrip, endStrip)
{
	dateRange = as.character( seq( as.Date(repDate), as.Date(repDate) - 150, by="-1 day" ) )
	
	allData = NULL
	for( date in dateRange )
	{
		curveName <- toupper(curveName)
		inputFile = paste( "S:/All/Risk/Data/VaR/prod/Prices/sasPrices.", 
				date, ".RData", sep="" )
		#rLog( "Loading file:", inputFile )
		
		prices = varLoad( "hP", inputFile , TRUE)
		if( is.null( prices ) ) next
		
		prices <- prices[grep(curveName, prices$curve.name),]
		contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
		prices <- prices[match(contractDates, prices$contract.dt),]
		name <- names(prices)[3]
		prices <- data.frame(name=prices[,3])
		names(prices) <- name
		if(is.null(allData)){
			allData <- prices
		}
		else
		{
			allData = cbind( allData, prices )
		}
		if(length(allData) == 71) 
		{
			contractDates <- data.frame(Contract_Date=contractDates)
			allData <- cbind(contractDates,allData)
			break
		}
	}
	return(allData)
	
    
}

################################################################################
#function to get price changes for a 70 day history
#histPriceData <- is the 71 day price history
#returns a dataframe with rows=contract dates, cols=pricing dates, values=price changes

TransportUnitVaR$priceChange <- function(histPriceData)
{
    
    topData <- histPriceData[-2]
    botData <- histPriceData[-72]
    priceChanges <- topData - botData
    priceChanges$Contract_Date <- histPriceData$Contract_Date
    
    return(priceChanges)
}

################################################################################
#function to calculate the position for a month
#position is the mmBTU/day position
#fuel is the calculation based on US convention x/(1-%fuel)
#day is the first day of month to calculate for

.positionCalc <- function( day, position, fuel, delivery=TRUE, country)
{
	country <- toupper(country)
    class(day) = "Date"
    numDays <- daysInMonth(day)
    
    if(delivery == TRUE)
    {
        monthlyPos <- (position * numDays)
        return(monthlyPos)
    }
    else if(delivery == FALSE && country == "US")
    {     
        monthlyPos <- (-1 * position * numDays) / (1 - fuel)
        return(monthlyPos)
    }
	else if(delivery == FALSE && country == "CANADA")
	{
		monthlyPos <- (-1 * position * numDays) * (1 + fuel)
		return(monthlyPos)
	}
}

################################################################################
#function to calculate total UVaR for commod curve
#priceChangeData is the 70 day price change history dataframe for each contract month
TransportUnitVaR$UVaRCalc <- function( deliveryPriceChangeData, receiptPriceChangeData, position, fuel, country )
{
  
    #delivery
    deliveryPriceChangeData$positions <- unlist(lapply(deliveryPriceChangeData$Contract_Date, 
              .positionCalc, position, fuel, TRUE, country))
   
    
    #receipt
    receiptPriceChangeData$positions <- unlist(lapply(receiptPriceChangeData$Contract_Date, 
              .positionCalc, position, fuel, FALSE, country))
    
    #combine
    priceChangeData <- rbind.data.frame(deliveryPriceChangeData, receiptPriceChangeData)
    
    sums <- unlist(lapply(priceChangeData[,c(2:71)], Math$prodSum, priceChangeData$positions))
    UVaR <- sd(sums)*4
    UVaRPerDTH <- UVaR / sum(deliveryPriceChangeData$positions)
    return(data.frame(Unit_VaR=dollar(UVaR), Unit_VaR_per_DTH=dollar(UVaRPerDTH)))
}


##################################################################
#TEST CODE
#TransportUnitVaR$getPriceData("COMMOD NG NGPMIDCO PHYSICAL", "2008-05-14", "2008-10-01", "2011-04-01",TRUE)
#TransportUnitVaR$getPriceData("COMMOD NG NBPLHARP PHYSICAL", "2008-05-14", "2008-10-01", "2011-04-01",FALSE)
#load("C:/Program Files/IndigoPerl/apache/htdocs/Rpad/TUVARReceiptPriceData.RData")
#recCurvePriceData <- histPriceData

#load("C:/Program Files/IndigoPerl/apache/htdocs/Rpad/TUVARDeliveryPriceData.RData")
#delCurvePriceData <- histPriceData

#recPriceChanges <- TransportUnitVaR$priceChange(recCurvePriceData)

#delPriceChanges <- TransportUnitVaR$priceChange(delCurvePriceData)

#UVaRList <- TransportUnitVaR$UVaRCalc(delPriceChanges, recPriceChanges, 50000, .0425)

