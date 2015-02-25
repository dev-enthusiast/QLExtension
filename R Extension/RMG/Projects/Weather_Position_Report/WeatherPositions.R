#functions to organize and aggregate data needed for the Daily Weather 
#Position report
#BIG ASSUMPTIONS: ALL WEATHER CURVES HAVE "WTH" AND EITHER "HDD" OR "CDD" IN 
#THEIR NAMES
#
# Author: Sean McWilliams
###############################################################################

library(RODBC)
library(xtable)
library(lattice)
library(reshape)
library(SecDb)
source("H:/user/R/RMG/Projects/PV/Weather.R")
source("H:/user/R/RMG/Projects/PV/Term.R")
##############################################################################################
#File namespace

WeatherPositions = new.env(hash = TRUE)


##############################################################################################
#function to get the unique weather books (books mapped to COMMOD_CURVES with "WTH" in name) 
#of weather books 

WeatherPositions$getUniqueBooks <- function(date){
	allCurves <- LiveData$allPositions()
	weatherCurves <- allCurves[grep("WTH", allCurves$COMMOD_CURVE),]
	uniqueBooks <- unlist(levels(factor(weatherCurves$BOOK)))
	return(uniqueBooks)
}

##############################################################################################
#read in each books' positions 
#returns a vector of data frames; each data frame is for a different weather book

WeatherPositions$readAllPosFromSecDb <- function(uniqueBooks, date){
	
	weatherPositions <- lapply(uniqueBooks, Weather$readPositionsFromSecDb, date)
	names(weatherPositions) <- uniqueBooks
	weatherPositions <- weatherPositions[!is.null(weatherPositions)]
	return(weatherPositions)	
}

##############################################################################################
#function to reorganize data and generate report
#

WeatherPositions$generateReport <- function(asOfDate){
	
	setwd("S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports")
	reportDay <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
	prevDay <- as.Date(secdb.dateRuleApply(asOfDate, "-2b"))
	
	#get a vector of the unique weather book names for previous and report day
	uniqueDayOfBooks <- WeatherPositions$getUniqueBooks(reportDay)
	uniquePrevBooks <- WeatherPositions$getUniqueBooks(prevDay)
	
	#get positions for each of PM and TR for the report day
	DataDayOf <- WeatherPositions$.splitAgg(uniqueDayOfBooks, reportDay)
	dayOfPMPositions <- DataDayOf[[1]]
	dayOfTrPositions <- DataDayOf[[3]]
	dayOfPMBookPos <- DataDayOf[[5]]
	dayOfTrBookPos <- DataDayOf[[6]]
	#get positions for each of PM and TR for the day before the report
	DataPrev <- WeatherPositions$.splitAgg(uniquePrevBooks, prevDay)
	prevPMPositions <- DataPrev[[1]]
	prevTrPositions <- DataPrev[[3]]
	prevPMBookPos <- DataPrev[[5]]
	prevTrBookPos <- DataPrev[[6]]
	#split into CDD and HDD
	dayOfPMPositionsCDD <- dayOfPMPositions[grep("CDD", dayOfPMPositions$INDEX_TYPE),]
	dayOfTrPositionsCDD <- dayOfTrPositions[grep("CDD", dayOfTrPositions$INDEX_TYPE),]
	prevPMPositionsCDD <- prevPMPositions[grep("CDD", prevPMPositions$INDEX_TYPE),]
	prevTrPositionsCDD <- prevTrPositions[grep("CDD", prevTrPositions$INDEX_TYPE),]
	
	dayOfPMPositionsHDD <- dayOfPMPositions[grep("HDD", dayOfPMPositions$INDEX_TYPE),]
	dayOfTrPositionsHDD <- dayOfTrPositions[grep("HDD", dayOfTrPositions$INDEX_TYPE),]
	prevPMPositionsHDD <- prevPMPositions[grep("HDD", prevPMPositions$INDEX_TYPE),]
	prevTrPositionsHDD <- prevTrPositions[grep("HDD", prevTrPositions$INDEX_TYPE),]
	
	dayOfPMBookPosCDD <- .CHSplit(dayOfPMBookPos)[[1]]
	dayOfTrBookPosCDD <- .CHSplit(dayOfTrBookPos)[[1]]
	prevPMBookPosCDD <- .CHSplit(prevPMBookPos)[[1]]
	prevTrBookPosCDD <- .CHSplit(prevTrBookPos)[[1]]
	
	dayOfPMBookPosHDD <- .CHSplit(dayOfPMBookPos)[[2]]
	dayOfTrBookPosHDD <- .CHSplit(dayOfTrBookPos)[[2]]
	prevPMBookPosHDD <- .CHSplit(prevPMBookPos)[[2]]
	prevTrBookPosHDD <- .CHSplit(prevTrBookPos)[[2]]
	
	
	#get locations for each of PM and TR
	uniquePMLocations <- levels(factor(append(DataDayOf[[2]], DataPrev[[2]])))
	uniqueTrLocations <- levels(factor(append(DataDayOf[[4]], DataPrev[[4]])))
	
	#CDD PM
	CDDPM <- WeatherPositions$.aggregateData(dayOfPMPositionsCDD, prevPMPositionsCDD, uniquePMLocations, reportDay, prevDay)
	CDDPMBooks <- .bookAgg(dayOfPMBookPosCDD, prevPMBookPosCDD, reportDay, prevDay)
	#CDD Trading
	CDDTr <- WeatherPositions$.aggregateData(dayOfTrPositionsCDD, prevTrPositionsCDD, uniqueTrLocations, reportDay, prevDay)
	CDDTrBooks <- .bookAgg(dayOfTrBookPosCDD, prevTrBookPosCDD, reportDay, prevDay)
	#HDD PM
	HDDPM <- WeatherPositions$.aggregateData(dayOfPMPositionsHDD, prevPMPositionsHDD, uniquePMLocations, reportDay, prevDay)
	HDDPMBooks <- .bookAgg(dayOfPMBookPosHDD, prevPMBookPosHDD, reportDay, prevDay)
	#HDD Trading
	HDDTr <- WeatherPositions$.aggregateData(dayOfTrPositionsHDD, prevTrPositionsHDD, uniqueTrLocations, reportDay, prevDay)
	HDDTrBooks <- .bookAgg(dayOfTrBookPosHDD, prevTrBookPosHDD, reportDay, prevDay)
	
	#PM
	PMData <- WeatherPositions$appendCDDHDD(CDDPM, HDDPM)
	PMBookData <- .bookAppendCDDHDD(CDDPMBooks, HDDPMBooks)
	PM <- WeatherPositions$.makeTable(PMData, names(PMData))
	PMBooks <- .finalAgg(PMBookData)
	#Trading
	TrData <- WeatherPositions$appendCDDHDD(CDDTr, HDDTr)
	TrBookData <- .bookAppendCDDHDD(CDDTrBooks, HDDTrBooks)
	Trading <- WeatherPositions$.makeTable(TrData, names(TrData))
	TradingBooks <- .finalAgg(TrBookData)
	
	PMFilename <- paste("S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/PM", reportDay, ".csv", sep="")
	PMBooksFilename <- paste("S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/PMBooks", reportDay, ".csv", sep="")
	TradingBooksFilename <- paste("S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/TradingBooks", reportDay, ".csv", sep="")
	TradingFilename <- paste("S:/All/Risk/Reports/TradingControlReports/DailyWeatherPositionReports/Trading", reportDay, ".csv", sep="")
	
	
	write.csv(PM, PMFilename)
	write.csv(PMBooks, PMBooksFilename)
	write.csv(Trading, TradingFilename)
	write.csv(TradingBooks, TradingBooksFilename)
	
	return(list(Trading=Trading, PM=PM, TradingByBook=TradingBooks, PMByBook=PMBooks))
}

##################################################################################################
#internal function to facilitate aggregation of data 
#prevPositions is a dataframe containing information about positions
#vecLocations is a vector containing unique locations traded within specific portfolios

WeatherPositions$.aggregateData <- function(dayOfPositions, prevPositions, vecLocations, reportDay, prevDay){
	#list of dataframes; each dataframe has the data for the final report for one location
	locationsList <- list()
	
	#loop through the unique locations
	for(loc in vecLocations){
		#a local data frame containg only positions at location loc
		bool <- TRUE
		if(length(grep(loc, dayOfPositions$LOCATION_1)) == 0 && length(grep(loc, prevPositions$LOCATION_1)) > 0){
			locPrevDFrame <- prevPositions[grep(loc, prevPositions$LOCATION_1),]
			locDayOfDFrame <- locPrevDFrame
			locDayOfDFrame$DELTA <- 0
			locDayOfDFrame$VEGA <- 0
		}
		else if(length(grep(loc, dayOfPositions$LOCATION_1)) > 0 && length(grep(loc, prevPositions$LOCATION_1)) == 0){
			locDayOfDFrame <- prevPositions[grep(loc, dayOfPositions$LOCATION_1),]
			locPrevDFrame <- locPrevDFrame
			locPrevDFrame$DELTA <- 0
			locPrevDFrame$VEGA <- 0
		}
		else if(length(grep(loc, dayOfPositions$LOCATION_1)) > 0 && length(grep(loc, prevPositions$LOCATION_1)) > 0){
			locDayOfDFrame <- dayOfPositions[grep(loc, dayOfPositions$LOCATION_1),]
			locPrevDFrame <- prevPositions[grep(loc, prevPositions$LOCATION_1),]
		}
		else{
			bool <- FALSE
		}
		
		if(bool == TRUE){
			#loop through the start and end dates of the positions at location loc and add strip to each position
			locDayOfDFrame <- WeatherPositions$appendStrip(locDayOfDFrame)
			locPrevDFrame <- WeatherPositions$appendStrip(locPrevDFrame)	
		
		
			#getunique strips that existed both the day before the report and the day after
			uniqueStrips <- levels(factor(append(levels(factor(locDayOfDFrame$Strip)), levels(factor(locPrevDFrame$Strip)))))
			
			#get delta, vega, deltachange, vegachange for each strip for location loc
			DayOfData <- WeatherPositions$deltaVegaSum(uniqueStrips, locDayOfDFrame)
			PrevData <- WeatherPositions$deltaVegaSum(uniqueStrips, locPrevDFrame)
			
			dayOfDelta <- DayOfData[[1]]
			prevDelta <- PrevData[[1]]
			dayOfVega <- DayOfData[[2]]
			prevVega <- PrevData[[2]]
	
			deltaChange <- dayOfDelta - prevDelta
			vegaChange <- dayOfVega - prevVega
			
		
			
			#append prices and price changes to dframe
			MarkData <- WeatherPositions$.appendMarks(uniqueStrips, loc, dayOfPositions$INDEX_TYPE[[1]], reportDay, prevDay)
			dayOfMarks <- MarkData[[1]]
			markChange <- MarkData[[2]]
			
			LocationDFrame <- data.frame("Tenor"=uniqueStrips, "Delta"=dayOfDelta, "DeltaChange"=deltaChange, 
					"Vega"=dayOfVega, "VegaChange"=vegaChange, "Mark"=dayOfMarks, "MarkChange"=markChange, "IndexType"=dayOfPositions$INDEX_TYPE[[1]])
			loc2 <- paste("(", loc, ")", sep = "")
			loc1 <- paste(Weather$decodeAirport(loc), loc2, sep=" ")  
			locationsList[[loc1]] <- LocationDFrame
		}
	}
	return(locationsList)
}

#####################################################################################
#internal function splits a set of books into PM and Trading 
#gets passed a set of books with weather positions and a date 
#returns a list whose first element is a dataframe of aggregated positions 
#for the books passsed to the function, and whose second element is a set of unique locations
#traded in the books passed to the function

WeatherPositions$.splitAgg <- function(books, reportDay){
	
	if(length(books) > 0){
		
		#determine which books are PM
        isPMList = sapply(books, BookOwnership$isPM)
        isPMList = lapply(isPMList, function(x){if(is.null(x)) return(FALSE) else return(x)})
		isPMVector <- unlist(isPMList)
		
		#get all positions from secdb
		allPos <- WeatherPositions$readAllPosFromSecDb(books, reportDay)

		#create a data frame with all PM Positions
		PMWeatherBooks <- allPos[isPMVector]
		PMWeatherPositions <- PMWeatherBooks[[1]]
		for(df in PMWeatherBooks[-1]){
			PMWeatherPositions <- rbind.data.frame(PMWeatherPositions, df)
		}
		
		#create a dataframe containing the positions for Trading books
		TrWeatherBooks <- allPos[!isPMVector] 
		TrWeatherPositions <- TrWeatherBooks[[1]]
		for(df in TrWeatherBooks[-1]){
			TrWeatherPositions <- rbind.data.frame(TrWeatherPositions, df)
		}
		
		#create list of unique PM locations  
		uniquePMLocations <- levels(factor(PMWeatherPositions$LOCATION_1))
		
		#create list of unique Trading locations
		uniqueTrLocations <- levels(factor(TrWeatherPositions$LOCATION_1))
		
		return(list(PMWeatherPositions, uniquePMLocations, TrWeatherPositions, uniqueTrLocations, PMWeatherBooks, TrWeatherBooks))		
		
	}
	else{
		return(NULL)
	}
}

###############################################################################################
#function that takes a data frame and appends a strip to each of its rows 
WeatherPositions$appendStrip <- function(dframe){
	i <- 1
	dframe$Strip <- c(NA)
	while(i <= length(dframe$START_DT)){
		dframe$Strip[i] <- Term$fromDatePair(as.Date(dframe$START_DT[i]), as.Date(dframe$END_DT[i]))
		i <- i + 1
	}
	return(dframe)
}
###############################################################################################
#internal function to append CDD and HDD positions together at each location
WeatherPositions$appendCDDHDD <- function(CDD, HDD){
	masterList <- list()
	if(length(CDD) >= length(HDD)){
		for(loc in names(CDD)){
			if(length(HDD[[loc]]) != 0){
				masterList[[loc]] <- rbind.data.frame(CDD[[loc]], HDD[[loc]])
			}
			else{
				masterList[[loc]] <- CDD[[loc]]
			}
		}
	}
	else if(length(HDD) > length(CDD)){
		for(loc in names(HDD)){
			if(length(CDD[[loc]]) != 0){
				masterList[[loc]] <- rbind.data.frame(CDD[[loc]], HDD[[loc]])
			}
			else{
				masterList[[loc]] <- HDD[[loc]]
			}
		}
	}
	return(masterList)
}
###############################################################################################
#function to sum delta and vega for positions in strips for a certain dframe
WeatherPositions$deltaVegaSum <- function(uniqueStrips, dFrame){
	delta <- c()
	vega <- c()
	for(str in uniqueStrips){
		if(length(rows <- grep(str, dFrame$Strip)) == 0){
			delta <- append(delta, 0)
			vega <- append(vega, 0)
		}
		else{
			locDFrame <- dFrame[grep(str, dFrame$Strip),]
			delta <- append(delta, sum(locDFrame$DELTA))
			vega <- append(vega, sum(locDFrame$VEGA))
		}
	}
	return(list(delta, vega))
}
###############################################################################################
#internal function to append price changes to the report-take in dframe and 
#can then lappply it over the lists of dframes of delta aggregates
WeatherPositions$.appendMarks <- function(uniqueStrips, location, unit, reportDay, prevDay){
	dayOfMarks <- c()
	prevMarks <- c()

	for(str in uniqueStrips){
		
		startStrip <- Term$startDate(str, TRUE)
		endStrip <- Term$endDate(str, TRUE)
		stripLength <-  round( as.numeric( difftime(endStrip, startStrip, units="days") ) / 30 )
		
		dayOfMarks <- append(dayOfMarks, Weather$getWeatherCurveTsDbMark(startStrip, stripLength, 
						location, unit, reportDay))
		prevMarks <- append(prevMarks, Weather$getWeatherCurveTsDbMark(startStrip, stripLength, 
						location, unit, prevDay))
	}
	return(list(dayOfMarks, dayOfMarks - prevMarks))
}

#################################################################################################
#internal function to subtract strips
WeatherPositions$.subtractStrips <- function(bigStrip, littleStrip){
	bigMonths <- as.integer(substring(bigStrip, 1, 2)) * 12 + as.integer(substring(bigStrip, 3, 4))
	littleMonths <- as.integer(substring(littleStrip, 1, 2)) * 12 + as.integer(substring(littleStrip, 3, 4))
	return(bigMonths - littleMonths)	
}
###################################################################################################
#function to output latex code for the latex file that will be made into a pdf 
#reportData is the output of generateReport function
WeatherPositions$outputLatex <- function(reportData, owner){
		print(xtable(reportData[[owner]],digits=0,caption=paste(owner,"Weather Positions (Deltas: dollars per DD; Vegas: dollars per vol unit; Marks: DD)"),align=c("l","l","l","r","r","r","r","r","r","l")),
					floating=TRUE,table.placement="h!", 
					latex.environments="flushleft",caption.placement="top",include.rownames=FALSE)
}

WeatherPositions$outputLatexBook <- function(reportData, owner){
	print(xtable(reportData[[owner]],digits=0,caption=paste(owner,"Weather Positions (Deltas: dollars per DD; Vegas: dollars per vol unit; Marks: DD)"),align=c("l","l","l","l","r","r","r","r","r","r","l")),
			floating=TRUE,table.placement="h!", 
			latex.environments="flushleft",caption.placement="top",include.rownames=FALSE)
}
###################################################################################################
#function to make single tables for PM and Trading 
WeatherPositions$.makeTable <- function(reportData, locations){
	
	finalTable <- data.frame()
	for(loc in locations){
		reportData[[loc]]$Location <- loc
		finalTable <- rbind.data.frame(finalTable, reportData[[loc]])
	}
	finalTable <- finalTable[,c("Location", "Tenor","Delta", "DeltaChange", "Vega", "VegaChange", "Mark", 
			"MarkChange", "IndexType")]
	return(finalTable)
}

#####################################################################################################
#function to make 3D map of deltas and vegas
#locations are the locations with positions
#deltas and vegas is the aggregate data for each location
#code marked with lmdvr is from http://lmdvr.r-forge.r-project.org/figures/figures.html
#the code from Lattice by Deepayan Sarkar
WeatherPositions$plotUSMap <- function(tradingData, pmData){
	state.map <- map("state", plot=FALSE, fill=FALSE)
	#lmdvr-lattice
	panel.3dmap <- function(..., rot.mat, distance, xlim, ylim, zlim, xlim.scaled, ylim.scaled, zlim.scaled){
		scaled.val <- function(x, original, scaled){
			scaled[1] + (x - original[1])*diff(scaled)/diff(original)
		}
		m <- ltransform3dto3d(rbind(scaled.val(state.map$x, xlim, xlim.scaled), scaled.val(state.map$y, 
								ylim, ylim.scaled), zlim.scaled[1]), rot.mat, distance)
		panel.lines(m[1,], m[2,], col = "grey76")
	}
	tradingData$Owner <- c("Trading")
	pmData$Owner <- c("PM")
	finalData <- rbind.data.frame(WeatherPositions$.dataForPlot(tradingData), WeatherPositions$.dataForPlot(pmData))	
	
	if(length(finalData$delta[finalData$delta > 0]) > 0 && length(finalData$delta[finalData$delta < 0]) > 0 ){
		#lmdvr-lattice		
		print(cloud(delta ~ long + lat | owner, finalData, panel.3d.cloud = function(...){
					panel.3dmap(...)
					panel.3dscatter(..., col.line = c("red", "green"))
				}, type = "h", scales = list(z = list(draw=TRUE, arrows=FALSE), x = list(draw=FALSE), y = list(draw=FALSE)), zoom=0.9, xlim = state.map$range[1:2], ylim = state.map$range[3:4],
				xlab = NULL, ylab = NULL, zlab = NULL, aspect = c(diff(state.map$range[3:4])/diff(state.map$range[1:2]), 
				0.3), panel.aspect = 0.6, lwd = 2, screen = list(z = -30, x = -60), par.settings = list(axis.line = 
				list(col = "black"), box.3d = list(col = "transparent", alpha = 0)),groups = as.factor(negpos), auto.key=list(columns = 2, 
					title = "Deltas", col = c("red", "green"), points = FALSE), layout = c(2,1), position = c(0,0,1,1)))

	}
	else if(length(finalData$delta[finalData$delta < 0]) > 0 || length(finalData$delta[finalData$delta > 0]) > 0){
		#lmdvr-lattice
		print(cloud(delta ~ long + lat | owner, finalData, panel.3d.cloud = function(...){
							panel.3dmap(...)
							panel.3dscatter(..., col.line = c("red", "green"))
						}, type = "h", scales = list(z = list(draw=TRUE, arrows=FALSE), x = list(draw=FALSE), y = list(draw=FALSE)), zoom=0.9, xlim = state.map$range[1:2], ylim = state.map$range[3:4],
						xlab = NULL, ylab = NULL, zlab = NULL, aspect = c(diff(state.map$range[3:4])/diff(state.map$range[1:2]), 
								0.3), panel.aspect = 0.6, lwd = 2, screen = list(z = -30, x = -60), par.settings = list(axis.line = 
										list(col = "black"), box.3d = list(col = "transparent", alpha = 0)),groups = as.factor(negpos), auto.key=list(columns = 1, 
								title = "Deltas", col = c("red", "green"), points=FALSE), layout = c(2,1), position = c(0,0,1,1)))
	}
	
	
}
########################################################################################
#internal funtion to get airport code
WeatherPositions$.getAirportCode <- function(string){
	return(substring(string, nchar(string) - 3, nchar(string) - 1))
}

#######################################################################################
#function to reorganize data for plotting
WeatherPositions$.dataForPlot <- function(positionData){
	#JS
	AIRPORTS = c(
			"ATL", "BOS", "BWI", "CVG", "DFW", "DSM", "DTW", "IAH", "LAS", 
			"LGA", "MCI", "MSP", "ORD", "PDX", "PHL", "SAC", "SLC", "STK", "TUS" )
	AIRPORT_LAT = c( 33.63672, 42.3629722, 39.1753611, 39.0488367, 32.8968281, 
			41.5339722, 42.2124444, 29.9844336, 36.2, 40.77725, 
			39.2976053, 44.8819567, 41.9786033, 45.7, 39.8719444, 
			38.5125278, 40.7883878, 40.6153136, 32.1160833 )
	AIRPORT_LONG = c( -84.4280661, -71.0064167, -76.6683333, -84.0, 
			-97.0379958, -93.6630833, -83.3533889, -95.3414422, -114.9, 
			-73.8726111, -94.7139056, -93.2217656, -87.9048414, -123.4, 
			-75.2411389, -121.4934722, -111.9777731, -103.2648454, -110.9410278 )
	airportTable = data.frame( code=AIRPORTS, lat=AIRPORT_LAT, long=AIRPORT_LONG )
	#SPM
	positionData$Location <- unlist(lapply(positionData$Location, WeatherPositions$.getAirportCode))
	owner <- positionData$Owner[[1]]
	positionData <- positionData[,c(1,3)]
	names(positionData) <- c("code", "value")
	castedData <- cast(positionData, code~., sum)
	castedData$owner <- c(owner)
	finalData <- merge(castedData, airportTable)
	names(finalData) <- c("code","delta","owner","lat", "long")
	
	finalData$negpos <- c("Positive")
	finalData$negpos[finalData$delta < 0] <- "Negative" 
	finalData$delta <- unlist(lapply(finalData$delta, abs))
	return(finalData)
}

##########################################################################################
#performs split by CDD/HDD at book level
.CHSplit <- function(bookPos){
	bookPosCDD <- list()
	bookPosHDD <- list()
	for(n in names(bookPos)){
		bookPosCDD[[n]] <- bookPos[[n]][grep("CDD", bookPos[[n]]$INDEX_TYPE), ]
		bookPosHDD[[n]] <- bookPos[[n]][grep("HDD",bookPos[[n]]$INDEX_TYPE), ]
	}
	return(list(bookPosCDD, bookPosHDD))
}

#########################################################################################################
#Alternative aggregation function to make a table broken out by book
.bookAgg <- function(dayOfBookPos, prevBookPos, reportDay, prevDay){
	
	dayOfBooks <- names(dayOfBookPos)
	prevBooks <- names(prevBookPos)
	positions <- list()
	for(n in dayOfBooks){
		
		if(sum(n == prevBooks) > 0){
			positions[[n]] <- WeatherPositions$.aggregateData(dayOfBookPos[[n]], prevBookPos[[n]], append(levels(factor(
										dayOfBookPos[[n]]$LOCATION_1)), levels(factor(prevBookPos[[n]]$LOCATION_1))), reportDay, prevDay)
			prevBooks <- prevBooks[prevBooks!=n]
		}
		else{
			temp <- dayOfBookPos[[n]]
			temp$DELTA <- 0
			temp$VEGA <- 0
			positions[[n]] <- WeatherPositions$.aggregateData(dayOfBookPos[[n]], temp, append(levels(factor(
													dayOfBookPos[[n]]$LOCATION_1)), levels(factor(temp$LOCATION_1))), reportDay, prevDay)
		}
	}
	if(length(prevBooks) > 0){
		for(n in prevBooks){
			temp <- prevBookPos[[n]]
			temp$DELTA <- 0
			temp$VEGA <- 0
			positions[[n]] <- WeatherPositions$.aggregateData(temp, prevBookPos[[n]], append(levels(factor(
										prevBookPos[[n]]$LOCATION_1)), levels(factor(temp$LOCATION_1))), reportDay, prevDay)
		}
	}
	
	return(positions)
	
}
#########################################################################################################
#extra function for appended togteher CDD and HDD positions at the book level
.bookAppendCDDHDD <- function(CDDPos, HDDPos){
	CDDBooks <- names(CDDPos)
	HDDBooks <- names(HDDPos)
	positions <- list()
	if(length(CDDBooks) >= length(HDDBooks)){
		for(b in names(HDDPos)){
			positions[[b]] <- WeatherPositions$appendCDDHDD(CDDPos[[b]], HDDPos[[b]])
			CDDBooks <- CDDBooks[CDDBooks != b]
		}
		positions <- append(positions, CDDPos[CDDBooks])
		
	}
	else{
		for(b in books){
			positions[[b]] <- WeatherPositions$appendCDDHDD(CDDPos[[b]], HDDPos[[b]])
			HDDBooks <- HDDBooks[HDDBooks != b]
		}
		positions <- append(positions, HDDPos[HDDBooks])
	}
	return(positions)
}

#########################################################################################################
#function to make final table if broken down at book level
.finalAgg <- function(positions){
	for(n in names(positions)){
		positions[[n]] <- WeatherPositions$.makeTable(positions[[n]], names(positions[[n]]))
		positions[[n]] <- cbind(n, positions[[n]])
		names(positions[[n]]) <- c("Book", names(positions[[n]][-1]))
	}
	finalPos <- positions[[1]]
	for(b in positions[-1]){
		finalPos <- rbind(finalPos, b)
	}
	return(finalPos)
}
