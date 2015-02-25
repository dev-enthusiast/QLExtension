###############################################################################
# Weather.R
# 
# Tools used to generate the weather PV report.
#
# Quote Types: 
#	PUT, STRDL, SWAP, CALL, COLLAR, STNGL, CSPRD, PSPRD
#
# Locations: 
#    AMS - Amsterdam, Netherlands
#    ATL - Atlanta, GA
#    BCN - Barcelona, Spain
#    BOS - Boston, MA
#    BWI - Baltimore, MD
#    CIA - Rome, Italy
#    CVG - Cincinnati, OH
#    DFW - Dallas, TX
#    DSM - Des Moines, IA
#    DTW - Detroit, MI
#    ESS - Essen, Germany
#    IAH - Houton, TX
#    LAS - Las Vegas, NV
#    LGA - New York, NY
#    LHR - London, England
#    MCI - Kansas City, MO
#    MSP - Minneapolis, MN
#    ORD - Chicago, IL
#    ORY - Paris, France
#    PDX - Portland, OR
#    PHL - Philadelphia, PA
#    SAC - Sacramento, CA
#    SLC - Salt Lake City, UT
#    STK - Sterling, CO
#    THF - Tempelhof, Germany
#    TUS - Tucson, AZ
#
# TsDb Quotes:
# 	PRC_WTH_<LOC>_<{HDD,CDD}>_IND
#	e.g. - tsdb.futCurve(
#		"PRC_WTH_BOS_HDD_IND", Sys.Date()-1, 
#		as.Date("2007-11-01"), as.Date("2008-03-31") )
#
# Author: John Scillieri Added to by: Sean McWilliams
# 
library(RODBC)
library(reshape)
library(maps)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")
source("H:/user/R/RMG/Utilities/Database/R/BookOwnership.R")


################################################################################
# File Namespace
#
Weather = new.env(hash=TRUE)


################################################################################
# Constants
Weather$DB_TABLE = "YJ.WEATHER_RPT" 
Weather$DB_NAME = "CCGFOP"
Weather$USER_NAME = "YJOPS"
Weather$PASSWORD = "YJOPS"

################################################################################
# Pull the active quote data from the yellow jacket DB 
# for the giving pricing date or pull the quotes from a 
# previous day's RData file
#
Weather$quoteData <- function( asOfDate=Sys.Date(), rDataFile=NULL )
{
	if( is.null(rDataFile) )
	{
		pricingDate = format(asOfDate, "%d%b%y")
		
		query = paste( "select * from YJ.WEATHER_RPT where PRICING_DT='", 
				pricingDate, "'", sep="")
		
		connection = .connectToDb()
		data = sqlQuery(connection, query)
		odbcClose(connection)
		
	} else
	{
		data = varLoad("data", rDataFile)
	}
	
	data = .filterQuoteData(data)
	
	return( data )
}


################################################################################
# Remove the junk we don't need for PV
#
.filterQuoteData <- function( data )
{
	data$LOCATION_1 = as.character(data$LOCATION_1)
	data$START_DT = as.Date(data$START_DT)
	data$END_DT = as.Date(data$END_DT)
	data$ASK_CONTACT = NULL
	data$BID_CONTACT = NULL
	
	data = data[ order(data$ID, data$TIME_STAMP), ]
	
	quoteData = subset( data, is.na(LOCATION_2)==TRUE & ACTIVE_FLAG==TRUE 
                   # & is.na(BID)==FALSE & is.na(ASK)==FALSE
    )
	quoteData$PRICING_DT = NULL
	quoteData$ID = NULL
	quoteData$LOCATION_2 = NULL
	quoteData$STRIKE_2 = NULL
	quoteData$TICK_2 = NULL
	quoteData$LIMIT_2 = NULL
	quoteData$TEN_YEAR = NULL
	quoteData$TIME_STAMP = NULL
	quoteData$ACTIVE_FLAG = NULL
	
	quoteData = quoteData[ do.call(order, quoteData), ]
	
	return(quoteData)
} 


################################################################################
# Pull the current price in SecDb
#
Weather$getSwapMark <- function( location, startDate, endDate, unit )
{
	startDate = as.Date(startDate)
	endDate = as.Date(endDate)
	
	dateRange = seq(startDate, endDate, by="month")
	
	curvename = paste("COMMOD WTH ", location, " ", unit, " INDEX", sep="")
	curveData = data.frame(secdb.getValueType(curvename, "Futures Strip"))
	curveData$date = as.Date(format(curveData$date, "%Y-%m-01"))
	
	mark = 0
	for( contractDate in dateRange )
	{
		class(contractDate) = "Date"
		contractMonth = as.Date( format(contractDate, "%Y-%m-01") )
		
		rTrace("\nContract Month:", as.character(contractMonth) )
		
		data = curveData[which(curveData$date==contractDate), ]
		if( nrow(data) == 0 )
		{
			data = NULL
			data$value = 0
		}
		
		rTrace("Contract Date:", as.character(contractDate), 
				"data$value =", data$value)
		mark = mark + data$value
	}
	
	return( mark )
}


################################################################################
Weather$getOptionMark <- function( asOfDate, location, startDate, endDate, 
		unit, type, strike, limit )
{
	if( limit==0 )
	{
		limit = -1
	}
	
	
	mark = secdb.invoke( "RWrapper", "RWrapper::priceWeatherOption", 
			asOfDate, location, startDate, endDate, unit, type, strike, limit ) 
	
	return( mark )
}


################################################################################
.connectToDb <- function() 
{
	conn = odbcConnect( Weather$DB_NAME, Weather$USER_NAME, Weather$PASSWORD )
	
	return( conn )
}


################################################################################
# read the position File position set
# File Generated From: "_ut get portfolio greeks" SecDb Script
#
Weather$readPositionFile <- function( positionFile , portfolioOrBook=NULL)
{   
	data = read.csv( positionFile )
	data = data[!is.na(data$Security.Name),]
	data = data[!is.na(data$Security.Tenor),]

	if(length(data$Security.Name) <= 0)
    {
        return(NULL)
    }

    splitStrings = strsplit(data$Security.Name, " ")
    dateStrings = unlist( lapply( splitStrings, function(x){return(x[5])} ) )
    data$"End.Month" = as.Date(dateStrings, format="%d%b%y")
    
    splitLocations = strsplit(data$Location, ";")
    data$Location = unlist( lapply( splitLocations, function(x){return(x[2])} ) )
    data$CurveType = unlist( lapply( splitLocations, function(x){return(x[1])} ) )
    
    data = data[which(data$CurveType == "WEATHER"), ]
    
    #get whether or not a position is in CDD or HDD
    
    allBooks = names( secdb.getValueType(portfolioOrBook, "Nodes")$Book )
    children = names( unlist( lapply( allBooks, secdb.getValueType,  "Children") ) )
    leaves = names(unlist(lapply(children, secdb.getValueType, "Leaves")))
    leaves = leaves[grep("^EWTH", leaves)]
    floatData = lapply(children, secdb.getValueType, "Index Map Instream Struct Float")
    indexTypes = unlist(lapply(floatData, "[", "Index Map Index Type"))
    toMerge = data.frame(Security.Name=leaves, IndexType=indexTypes)
    
    data = merge(data, toMerge )
    
    positionData = data[,c("Location", "Month", "Security.Tenor", "C.P", "Qty", 
                    "Strike", "Dollar.Price", "Underlying", "BDelta", "BVega", "IndexType")]
    names(positionData) = c("LOCATION_1", "START_DT", "DURATION", "POSITION_TYPE", 
            "POSITION_SIZE", "POSITION_STRIKE", "PRICE", "UNDERLYING", 
            "DELTA", "VEGA", "INDEX_TYPE")
    
    positionData$START_DT = as.Date(positionData$START_DT, format="%m/%d/%Y")	
    positionData$POSITION_TYPE = toupper( positionData$POSITION_TYPE )
    
    positionData$END_DT = positionData$START_DT
    for( rowIndex in seq_len(nrow(positionData)) )
    {
        dateRange = seq( positionData$START_DT[rowIndex], by="month", 
                length.out=positionData$DURATION[rowIndex] )
        endDate = dateRange[length(dateRange)]
        positionData$END_DT[rowIndex] = as.Date( secdb.dateRuleApply(endDate, "e") )
    }
    
    return(positionData)
    
}


################################################################################
# Get the mark for a given quote, either from TSDB(swap) or SecDb(option) 
#
.getMarkForQuote <- function( quote, asOfDate )
{
	location = as.character( quote$LOCATION_1 )
	startDate = as.Date( quote$START_DT )
	endDate = as.Date( quote$END_DT )
	unit = as.character( quote$UNIT ) 
	type = as.character( quote$TYPE )
	strike = as.numeric( quote$STRIKE_1 )    
	limit = as.numeric( quote$LIMIT_1 )
	
	rDebug( "\n", paste( capture.output( format(quote, justify="left") ), 
					collapse="\n" ) )
	
	if( type == "CALL" || type == "PUT" )
	{
		mark = Weather$getOptionMark( asOfDate, location, startDate, endDate, 
				unit, type, strike, limit )
		rDebug("Retrieved quote from secdb: ", mark) 
		
	} else if( type == "SWAP" )
	{
		mark = Weather$getSwapMark( location, startDate, endDate, unit )
		rDebug("Retrieved quote from tsdb: ", mark) 
		
	} else
	{
		rWarn("Invalid Quote Type:", type, ". Returning 0.")
		mark = 0
	}
	
	return( mark )
}


################################################################################
# test a set of quotes to see if they fall within the bid ask spread
#
Weather$testQuotes <- function( quoteData, positionData, asOfDate )
{
    uniquePositions = unique( positionData[,c("LOCATION_1", "START_DT", "END_DT")])
    
	quotes = merge( quoteData, uniquePositions )
	
	if( nrow(quotes) == 0 )
	{
		print(quotes)
		return(NULL)
	}
	
	startDate = as.Date(quotes$START_DT)
	endDate = as.Date(quotes$END_DT)
	quotes$TERM = Term$fromDatePair( startDate, endDate )
	
	quotes$MARK = NA
	quotes$MARK_OK = NA
	
	for( rowIndex in seq_len(nrow(quotes)) ){
		quote = quotes[rowIndex,]
		
		mark = .getMarkForQuote( quote, asOfDate )
		quotes$MARK[rowIndex] = round(mark, digits=1)
		
		bid = as.numeric(quote$BID)
		ask = as.numeric(quote$ASK)
        if( is.na( bid ) | is.na( ask ) )
        {
            markOK = "N/A"
        } else if( bid <= mark & ask >= mark )
		{
			markOK = "OK"
		} else if( mark < bid )
		{
			markOK = "LOW"
		} else if( mark > bid )
		{
			markOK = "HIGH"
		} else
		{
			markOK = "WTF?"
		}
		
		quotes$MARK_OK[rowIndex] = markOK
	}
	
	quotes$BID = round(quotes$BID)
	quotes$ASK = round(quotes$ASK)
	quotes$SPREAD = round(quotes$SPREAD)
	quotes = quotes[, c("STRUCTURE", "TERM", "UNIT", "LAST", 
					"BID_SIZE", "ASK_SIZE", "BID", "ASK", "SPREAD", "MARK", "MARK_OK")]   
	
	names(quotes) = c( "Structure", "Term", "Unit", "Last", 
			"Bid Size", "Ask Size", "Bid", "Ask", "Spread", "Mark", "Mark OK" )
	
	return(quotes)
}


################################################################################
# Calculate the coverage ratio from a set of positions, either by delta or
# position size
#
Weather$coverageRatio <- function( quotes, positions, columnToVerify="DELTA" )
{
	quoteTypes = quotes[, c("LOCATION_1", "START_DT", "END_DT")]
	quoteTypes = unique(quoteTypes)
	
	positionsVerified = merge(quoteTypes, positions)
	
	# Incase there are error values in the column
	verifiedColumn = as.numeric( positionsVerified[[columnToVerify]] )
	totalColumn = as.numeric( positions[[columnToVerify]] )
	
	sizePositionsVerified = sum( abs( na.omit(verifiedColumn) ) )
	totalPositionSize = sum( abs( na.omit(totalColumn) ) )
	
	coverageRatio = sizePositionsVerified / totalPositionSize  
	
	return(coverageRatio)  
}


################################################################################
Weather$getLatLong <- function( airportCode )
{
	address = paste("http://www.airnav.com/airport/", airportCode, sep="")
	pageData = readLines( address )
	
	latLongLine = pageData[grep("Lat/Long:", pageData, fixed=TRUE)]
	
	latLong = strsplit(latLongLine, "<BR>")[[1]][3]
	latLong = as.numeric( strsplit( latLong, "/" )[[1]] )
	
	return( latLong )
}


################################################################################
Weather$drawAirportMap <- function( width, height, positions )
{
	AIRPORTS = c(
			"ATL", "BOS", "BWI", "CVG", "DFW", "DSM", "DTW", "IAH", "LAS", 
			"LGA", "MCI", "MSP", "ORD", "PDX", "PHL", "SAC", "SLC", "STK", "TUS" )
	AIRPORT_LAT = c( 33.63672, 42.3629722, 39.1753611, 39.0488367, 32.8968281, 
			41.5339722, 42.2124444, 29.9844336, 36.0800556, 40.77725, 
			39.2976053, 44.8819567, 41.9786033, 45.5887222, 39.8719444, 
			38.5125278, 40.7883878, 40.6153136, 32.1160833 )
	AIRPORT_LONG = c( -84.4280661, -71.0064167, -76.6683333, -84.6678222, 
			-97.0379958, -93.6630833, -83.3533889, -95.3414422, -115.15225, 
			-73.8726111, -94.7139056, -93.2217656, -87.9048414, -122.5975, 
			-75.2411389, -121.4934722, -111.9777731, -103.2648454, -110.9410278 )
	
	airportTable = data.frame( code=AIRPORTS, lat=AIRPORT_LAT, long=AIRPORT_LONG )
	
	airportTable = merge(airportTable, positions, by.x="code", by.y="LOCATION_1")
	windows( width, height )
	map("usa", xlim=c(-130, -65), mar=c(-5, 0, -5, 0))
	text(airportTable$long, airportTable$lat, airportTable$code, col = "red")
	#text(airportTable$long, airportTable$lat-1.25, paste(airportTable$TERM, " @ ", airportTable$MARK) )
	#text(airportTable$long, airportTable$lat-2.5, paste("Change:", airportTable$MARK - airportTable$PREV_MARK, sep="") )
	
	invisible(NULL)
}


################################################################################
Weather$readPositionsFromSecDb <- function( portfolioOrBook, asOfDate=Sys.Date() )
{
	
	asOfDate = as.Date( asOfDate )
	
	SECDB_END_OF_TIME = as.Date("2952-12-31")
	
	outputFile = tempfile()
	
	secdb.invoke("_Lib Elec Get Portfolio Greeks", "Greeks Rpt::Get Position", 
			portfolioOrBook, asOfDate, asOfDate, 
			as.Date(secdb.dateRuleApply(asOfDate, "+e+1d")), 
			SECDB_END_OF_TIME, FALSE, outputFile )
	
	positionTable = Weather$readPositionFile( outputFile , portfolioOrBook)
	file.remove( outputFile )
	
	return( positionTable )
}


################################################################################
Weather$getPositionChangeData <- function( current, asOfDate, prevAsOfDate )
{
	locations = unique( current[, c("LOCATION_1", "START_DT", "END_DT")] )
	
	unit = Weather$getUnit( locations$START_DT[1] )
	curves = paste( "COMMOD WTH", unique(locations$LOCATION_1), unit, "INDEX" )
	
	dateRange = seq( min(locations$START_DT), max(locations$END_DT), by="month")
	curveData = tsdb.futStrip( curves, prevAsOfDate, asOfDate, dateRange )
	
	locations$MARK = NA
	locations$PREV_MARK = NA
	for( rowIndex in seq_len( nrow(locations) ) )
	{
		rowData = locations[rowIndex,]
		fullCurveName = paste( "COMMOD WTH", rowData$LOCATION_1, unit, "INDEX" )
		asOfData = subset( curveData, curveData$curve.name == fullCurveName &
						contract.dt >= rowData$START_DT & 
						contract.dt < rowData$END_DT &
						pricing.dt == asOfDate )
		prevData = subset( curveData, curveData$curve.name == fullCurveName &
						contract.dt >= rowData$START_DT & 
						contract.dt < rowData$END_DT &
						pricing.dt == prevAsOfDate )
		
		locations$MARK[rowIndex] = sum( asOfData$value )
		locations$PREV_MARK[rowIndex] = sum( prevData$value )
	}
	
	locations$TERM = Term$fromDatePair( locations$START_DT, locations$END_DT )
	locations = locations[, c("LOCATION_1", "TERM", "MARK", "PREV_MARK")]
	
	return( locations )
}


################################################################################
Weather$getUnit <- function( date )
{
	monthVal = as.numeric( format( as.Date(date), "%m" ) )
	if( monthVal %in% c(11,12,1,2,3) )
		return( "HDD" )
	else if( monthVal %in% c(5:9) )
		return( "CDD" )
	else
		stop("COULD BE EITHER HDD OR CDD FOR MONTH:", date)
}


prettyTable <- function( positions )
{
	filtered = positions
	rownames(filtered) = NULL
	filtered$TERM = Term$fromDatePair(filtered$START_DT, filtered$END_DT)
	filtered = filtered[ , c("TERM", "POSITION_TYPE", "POSITION_SIZE", 
					"POSITION_STRIKE", "UNDERLYING", "DELTA", "VEGA")]
	names(filtered) = c("Term", "Type", "Qty", "Strike", "Underlying", "Delta", "Vega")
	filtered = filtered[do.call(order, filtered),]
	
	return(filtered)
}

##############################################################################################
#function to decode airport codes
Weather$decodeAirport <- function(code){

	if(code == "AMS"){
		return("Amsterdam, Netherlands")
	}  
	if(code == "ATL"){
		return("Atlanta, GA")
	}  
	if(code == "BCN"){
		return("Barcelona, Spain")
	}  
	if(code == "BOS"){
		return("Boston, MA")
	}  
	if(code == "BWI"){
		return("Baltimore, MD")
	}  
	if(code == "CIA"){
		return("Rome, Italy")
	}  
	if(code == "CVG"){
		return("Cincinnati, OH")
	}  
	if(code == "DFW"){
		return("Dallas, TX")
	}  
	if(code == "DSM"){
		return("Des Moines, IA")
	}  
	if(code == "DTW"){
		return("Detroit, MI")
	}  
	if(code == "IAH"){
		return("Houston, TX")
	}
	if(code == "LAS"){
		return("Las Vegas, NV")
	}
	if(code == "LGA"){
		return("New York, NY")
	}
	if(code == "LHR"){
		return("London, England")
	}
	if(code == "MCI"){
		return("Kansas City, MO")
	}
	if(code == "MSP"){
		return("Minneapolis, MN")
	}
	if(code == "ORD"){
		return("Chicago, IL")
	}
	if(code == "ORY"){
		return("Paris, France")
	}
	if(code == "ESS"){
		return("Essen Germany")
	}
	if(code == "TUS"){
		return("Tucson, AZ")
	}
	if(code == "THF"){
		return("Tempelhof, Germany")
	}
	if(code == "STK"){
		return("Sterling, CO")
	}
	if(code == "SLC"){
		return("Salt Lake City, UT")
	}
	if(code == "SAC"){
		return("Sacramento, CA")
	}
	if(code == "PHL"){
		return("Philadelphia, PA")
	}
	if(code == "PDX"){
		return("Portland, OR")
	}
}

##############################################################################################
#function to get weather curve mark from TsDb
Weather$getWeatherCurveTsDbMark <- function(startStrip, stripLength, location, unit, date){
	
	strip <- format(as.Date(startStrip), "%y%m") 
	curveName <- paste("PRC_WTH_", location, "_", unit, "_IND_", sep="")
	i <- 0
	mark <- 0
	while(i <= stripLength){
		tMark <- tsdb.readCurve(paste(curveName, strip, sep=""), date, date)$value[[1]]
		if(length(tMark) == 0){
			mark <- mark + 0
		}
		else{
			mark <- mark + tMark
		} 
		i <- i + 1
		
		strip <- Weather$addOneToStrip(strip)
	}
	return(mark)
	
}
##############################################################################################
#takes a year month strip and adds one to it (e.g. makes "0809" into "0810")
Weather$addOneToStrip <- function(strip){
	month <- as.integer(substring(strip, 3, 4))
	if(month <= 8){
		month <- month + 1
		return(paste(substring(strip, 1, 2), "0", month, sep=""))
	}
	else if(month >= 9 && month <= 11){
		month <- month + 1
		return(paste(substring(strip, 1, 2), month, sep=""))
	}
	else if(month == 12){
		year <- as.integer(substring(strip, 1, 2)) 
		if(year <= 8){
			year <- year + 1
			return(paste("0", year, "01", sep=""))
		}
		else if(year >= 9){
			year = year + 1
			return(paste(year, "01", sep=""))
		}
	}
}
##############################################################################################
#testing code
#asOfDate = as.Date( secdb.dateRuleApply( Sys.Date(), "-1b" ) )
#prevAsOfDate = as.Date( secdb.dateRuleApply( Sys.Date(), "-2b" ) )

#currentPositions = Weather$readAllPosFromSecDb(asOfDate)
#previousPositions = Weather$readAllPosFromSecDb(prevAsOfDate)

#today = Weather$getPositionChangeData( currentPositions, asOfDate, prevAsOfDate)

#Weather$drawAirportMap( 11, 8.5, today )

#locationSplit = split(today, today$LOCATION_1)
#lapply(locationSplit, function(x){format(prettyTable(x), width=9)})





