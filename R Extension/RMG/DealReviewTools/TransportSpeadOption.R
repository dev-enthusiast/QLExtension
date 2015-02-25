###########################################################################
#This file contains the back-end code for a Web application 
#for approximating the value of a spread option
#
#Author: Sean McWilliams
#
###########################################################################
#
library(reshape)
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/daysInMonth.R")
source("H:/user/R/RMG/Utilities/discountFactorUSD.R")
source("H:/user/R/RMG/Utilities/Math/prodSum.R")
#
##########################################################################
#
TransportSpreadOption = new.env(hash=TRUE)
#
###########################################################################
#bring in prices 
TransportSpreadOption$getPrices <- function( curveName, pricingDate=Sys.Date() - 1, startStrip, endStrip )
{
	curveName <- toupper(curveName)
	inFile <- paste("S:/All/Risk/Data/VaR/prod/Prices/sasPrices.", pricingDate, ".RData", sep="")
	prices <- varLoad("hP", inFile, TRUE)
	
	if(is.null(prices))
	{
		return(NULL)
	}else{
		
		prices <- prices[grep(curveName, prices$curve.name),]
		contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
		prices <- prices[match(contractDates, prices$contract.dt),]
		prices <- prices[[3]]	
	}
	return(prices)
}

###################################################################################
#bring in volatilities
TransportSpreadOption$getVols <- function(curveName, monthlyOrDaily, pricingDate=Sys.Date(), startStrip, endStrip)
{
	
	monthlyOrDaily <- toupper(monthlyOrDaily)
	curveName <- toupper(curveName)
	inFile <- paste("S:/All/Risk/Data/VaR/prod/Vols/sasVols.", pricingDate, ".RData", sep="")
	vols <- varLoad("hV", inFile, TRUE)
	
	if(is.null(vols))
	{
		return(NULL)
	}
	else{
		if(monthlyOrDaily == "MONTHLY")
		{
			
			vols <- vols[grep(curveName, vols$curve.name),]
			vols <- vols[vols$vol.type == "M",]
			contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
			vols <- vols[match(contractDates, vols$contract.dt),]
			vols <- vols[[4]]
		}
		else if(monthlyOrDaily == "DAILY")
		{
			vols <- vols[grep(curveName, vols$curve.name),]
			vols <- vols[vols$vol.type == "D",]
			contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
			vols <- vols[match(contractDates, vols$contract.dt),]
			vols <- vols[[4]]
		}
	}
	return(vols)
}

########################################################################################
#returns vector of values according to season (SUM or WINT) used for fuel charges 
#and Commodity Charges and Demand
.sumOrWint <- function(summer, winter, startStrip, endStrip)
{
	
	charges <- c()
	cd <- as.integer(format(seq.Date(as.Date(startStrip), as.Date(endStrip), by="month"), "%m"))
	charges[ 3 >= cd ] <- winter
	charges[cd >= 11 ] <- winter
	charges[is.na(charges)] <- summer
	return(charges)
}

#########################################################################################
#calculate adjusted receipt curve price with fuel charge incorporated 
.adjReceiptPrice <- function(recPrices, fuelCharge, country)
{
	country <- toupper(country)
	if(country == "US")
	{
		return(recPrices * (1 + fuelCharge))
	}
	else if (country == "CANADA")
	{
		return(recPrices / (1 - fuelCharge))
	}
	
}

#############################################################################################
#calculate time to maturity for strip contract
.timeToMaturity <- function(pricingDate = Sys.Date() - 1, startStrip, endStrip)
{
	contractDates <- seq.Date(as.Date(startStrip), as.Date(endStrip), by="month")
	matDates <- as.integer((contractDates - as.Date(pricingDate)))/365
	return(matDates)
}

###########################################################################################
#calculate risk free rte of return
.riskFreeRate <- function(discountFactor, timeToMaturity)
{
	
	return(-log(discountFactor) / timeToMaturity)
}

############################################################################################
#intermediate calculation functions
.intermediateV <- function(delVols, recVols, adjustedRecPrices, commodCharge, correlation)
{
	return(sqrt(delVols ^ 2 + (delVols * adjustedRecPrices / (adjustedRecPrices + commodCharge)) ^ 2 - 2 * correlation * 
							delVols * recVols * adjustedRecPrices / (adjustedRecPrices + commodCharge)))
}

.intermediateF <- function(adjustedRecPrices, commodCharge, delPrices)
{
	return(delPrices /(adjustedRecPrices + commodCharge))
}

.intermediateD1 <- function(FVals, VVals, timeToMat)
{
	return((log(FVals) + (VVals ^ 2 / 2) * timeToMat) / (VVals * sqrt(timeToMat)))
}

.intermediateD2 <- function(D1Vals, VVals, timeToMat)
{
	return(D1Vals - VVals * sqrt(timeToMat))
}

##############################################################################################
#calc call value
.callValue <- function(FVals, D1Vals, D2Vals, riskFreeRate, timeToMat, adjRecPrices, commodCharge)
{
	return((FVals * exp(-1 * riskFreeRate* timeToMat) * pnorm(D1Vals) - exp(-1 * riskFreeRate * timeToMat) * pnorm(D2Vals))*(adjRecPrices + commodCharge))
}	

##############################################################################################
#put value
.putValue <- function(FVals, D1Vals, D2Vals, riskFreeRate, timeToMat, adjRecPrices, commodCharge)
{
	return((exp(-1 * riskFreeRate * timeToMat) * pnorm(-1 * D2Vals) - FVals * exp(-1 * riskFreeRate * timeToMat) * pnorm(-1 * D1Vals)) * (adjRecPrices + commodCharge))
}

###############################################################################################
#intrinsic value
.intrinsicValue <- function(delPrices, adjRecPrices, commodCharge)
{
	return(max(c(0, delPrices - adjRecPrices - commodCharge)))
}

#################################################################################################
#Monthly Volume
.monthlyVolume <- function(numDays, deliveryVolume)
{
	return(numDays * deliveryVolume)
}

#################################################################################################
#NPVIntrinsic
.NPVIntrinsic <- function(intrinsicValue, monthlyVolume, discountFactor)
{
	return(intrinsicValue * monthlyVolume * discountFactor)
}

##################################################################################################
#TotalNPV 
.TotalNPV <- function(deliveredVolume, callValue, numDays)
{
	return(deliveredVolume * callValue * numDays)
}

################################################################################################
#NPVDemand
.NPVDemand <- function(dailyOrMonthly, deliveredVolume, demand, discountFactor, numDays)
{
	dailyOrMonthly <- toupper(dailyOrMonthly)
	
	if(dailyOrMonthly == "DAILY")
	{
		NPVDemand <- deliveredVolume * numDays * demand * discountFactor
	}
	else if(dailyOrMonthly == "MONTHLY")
	{
		NPVDemand <- deliveredVolume * demand * discountFactor
	}
	return(NPVDemand)
}

################################################################################################
#create data frame containing all of the necesary data 
TransportSpreadOption$dataTable <- function(pricingDate=Sys.Date()-1, startStrip, endStrip, recCurve, delCurve, 
									recVol, delVol, correlation, delVolume, country, sumFuel, winFuel, sumDemand, 
									winDemand, sumCommod, winCommod, dailyOrMonthly, monthlyOrDaily)
{
	
	data <- data.frame(Date=seq.Date(as.Date(startStrip), as.Date(endStrip), by="month") )
	data$Year <- format(data$Date, "%Y")
	data <- data[c(2,1)]
	data$Delivery_Volume <- delVolume
	data$Receipt_Forward_Price <- TransportSpreadOption$getPrices(recCurve, pricingDate, startStrip, endStrip)
	
	data$Delivery_Forward_Price <- TransportSpreadOption$getPrices(delCurve, pricingDate, startStrip, endStrip)
	
	data$Fuel_Charge <- .sumOrWint(sumFuel, winFuel, startStrip, endStrip)
	data$Fuel_Adjusted_Receipt_Price <- .adjReceiptPrice(data$Receipt_Forward_Price, data$Fuel_Charge, country)
	
	data$Commodity_Charge <- .sumOrWint(sumCommod, winCommod, startStrip, endStrip)
	data$Time_to_Maturity <- .timeToMaturity(pricingDate, startStrip, endStrip)
	discountFactor <- discountFactorUSD(pricingDate, startStrip, endStrip)
	data$Risk_Free_Rate <- .riskFreeRate(discountFactor, data$Time_to_Maturity)
	data$Delivery_Volatility <- TransportSpreadOption$getVols(delVol, monthlyOrDaily, pricingDate, startStrip, endStrip)
	data$Receipt_Volatility <- TransportSpreadOption$getVols(recVol, monthlyOrDaily, pricingDate, startStrip, endStrip)
	
	data$Correlation <- correlation
	
	data$v <- .intermediateV(data$Delivery_Volatility, data$Receipt_Volatility, data$Fuel_Adjusted_Receipt_Price, 
					data$Commodity_Charge, data$Correlation)
	data$F <- .intermediateF(data$Fuel_Adjusted_Receipt_Price, data$Commodity_Charge, data$Delivery_Forward_Price)
	data$d1 <- .intermediateD1(data$F, data$v, data$Time_to_Maturity)
	data$d2 <- .intermediateD2(data$d1, data$v, data$Time_to_Maturity)
	data$Call_Value <- .callValue(data$F, data$d1, data$d2, data$Risk_Free_Rate, data$Time_to_Maturity, 
					data$Fuel_Adjusted_Receipt_Price, data$Commodity_Charge)
	data$Put_Value <- .putValue(data$F, data$d1, data$d2, data$Risk_Free_Rate, data$Time_to_Maturity, 
			data$Fuel_Adjusted_Receipt_Price, data$Commodity_Charge)
	data$Intrinsic_Value <- .intrinsicValue(data$Delivery_Price, data$Fuel_Adjusted_Receipt_Prices, data$Commodity_Charge)
	data$Number_Days <- unlist(lapply(as.character(data$Date), daysInMonth))
	data$Monthly_Volume <- .monthlyVolume(data$Number_Days, data$Delivery_Volume)
	data$Discount_Factor_USD <- discountFactor
	data$NPV_Intrinsic <- .NPVIntrinsic(data$Intrinsic_Value, data$Monthly_Volume, data$Discount_Factor_USD)
	data$Total_NPV <- .TotalNPV(data$Delivery_Volume, data$Call_Value, data$Number_Days)
	data$Demand <- .sumOrWint(sumDemand, winDemand, startStrip, endStrip)
	data$NPV_Demand <- .NPVDemand(dailyOrMonthly, data$Delivery_Volume, data$Demand, data$Discount_Factor_USD, data$Number_Days)

	return(data)
}


TransportSpreadOption$yearlyData <- function(rawData, startStrip, endStrip)
{
	
	Years <- format(seq.Date(as.Date(startStrip), as.Date(endStrip), by="year"), "%Y")
	array <- array(0, c(6, length(Years)))
	data <- data.frame(Description=c("Total", "Intrinsic+Extrinsic", "Intrinsic", "Extrinsic", "Demand", "Volume_(nom)"), array)
	names(data) <- c("Description", Years)
	length <- length(names(data))
	
	data[2, c(2:length)] <- as.vector(unlist(lapply(names(data)[-1], function(x){return(sum(rawData$Total_NPV[rawData$Year==x]))})))
	data[3, c(2:length)] <- as.vector(unlist(lapply(names(data)[-1], function(x){return(sum(rawData$NPV_Intrinsic[rawData$Year==x]))})))
	data[5, c(2:length)] <- as.vector(unlist(lapply(names(data)[-1], function(x){return(sum(rawData$NPV_Demand[rawData$Year==x]))})))
	data[6, c(2:length)] <- as.vector(unlist(lapply(names(data)[-1], function(x){return(sum(rawData$Monthly_Volume[rawData$Year==x]))})))
	data[1, c(2:length)] <- data[2, c(2:length)] + data[5, c(2:length)]
	data[4, c(2:length)] <- data[2, c(2:length)] - data[3, c(2:length)]
	
	return(data)
}

TransportSpreadOption$totalData <- function(rawData)
{
	data <- list("Total" = 0, "Intrinsic+Extrinsic" = 0, "Intrinsic" = 0, "Extrinsic" = 0, "Demand" = 0, 
			"Volume_(nom)" = 0, "Volume_(pv)" = 0)
	
	data[["Intrinsic+Extrinsic"]] <- sum(rawData$Total_NPV)
	data[["Intrinsic"]] <- sum(rawData$NPV_Intrinsic)
	data[["Demand"]] <- sum(rawData$NPV_Demand)
	data[["Volume_(nom)"]] <- sum(rawData$Monthly_Volume)
	data[["Volume_(pv)"]] <- Math$prodSum(rawData$Monthly_Volume, rawData$Discount_Factor_USD)
	data[["Total"]] <- data[["Intrinsic+Extrinsic"]] + data[["Demand"]]
	data[["Extrinsic"]] <- data[["Intrinsic+Extrinsic"]] - data[["Intrinsic"]]
	return(data)
}


