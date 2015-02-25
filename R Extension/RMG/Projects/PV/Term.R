###############################################################################
# Term.R
# 
# Utilities for converting to/from the {F-Z, Cal, Q, etc} nomenclature
#
# e.g. "F 08", "Cal 10", "Win 11"
#
# Author: John Scillieri
# 

################################################################################
# File Namespace
#
Term = new.env(hash=TRUE)


################################################################################
# Given a specified term, what's the start date?
#
Term$startDate <- function( term , Weather=FALSE )
{
	
	months <- c("F", "G", "H", "J", "K", "M", "N", "Q", "U", "V", "X", "Z")
    prefix = strsplit(as.character(term), " ")[[1]][1]
    year = strsplit(as.character(term), " ")[[1]][2]
    
    # If we have a pattern like 08/09
    if( length( grep("/", year) ) > 0 )
    {
        year = substring(year, 1,2)
    }
    
    prefix = toupper(prefix)
    
    monthTry = paste(prefix, "01", year, collapse = " ")
    date = as.Date(monthTry, format="%b %d %y")
    
    if( !is.na(date) )
    {
        startDate = as.Date(format( date, "%Y-%m-01" ))  
    }else if( prefix == "Q1" )
    {
        startDate = as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" ) 
    }else if( prefix == "Q2" )
    {
        startDate = as.Date( paste(year, "-04-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "Q3" )
    {
        startDate = as.Date( paste(year, "-07-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "Q4" )
    {
        startDate = as.Date( paste(year, "-10-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "CAL" )
    {
        startDate = as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "WIN" && Weather == TRUE)
	{
		startDate = as.Date( paste(year, "-11-01", sep=""), format="%y-%m-%d" )
	}else if( prefix == "WIN" )
    {
        startDate = as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "SPR" )
    {
        startDate = as.Date( paste(year, "-03-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "SUM" )
    {
        startDate = as.Date( paste(year, "-07-01", sep=""), format="%y-%m-%d" )
    }else if( prefix == "SUM" && Weather == TRUE)
	{
		startDate = as.Date( paste(year, "-05-01", sep=""), format="%y-%m-%d" )
	}else if( sum((month = substring(prefix,1,1)) == months) == 1)
    {
        startDate = as.Date( paste(year, .decodeStartDate(month), sep=""), format="%y-%m-%d")
    }else
    {
        startDate = NULL 
    }
    
    return( startDate)
}


################################################################################
# Given a specified term, what's the start date?
#
Term$startDate <- function( term , Weather=FALSE )
{
    
    months <- c("F", "G", "H", "J", "K", "M", "N", "Q", "U", "V", "X", "Z")
    prefix = strsplit(as.character(term), " ")[[1]][1]
    year = strsplit(as.character(term), " ")[[1]][2]
    
    # If we have a pattern like 08/09
    if( length( grep("/", year) ) > 0 )
    {
        year = substring(year, 1,2)
    }
    
    prefix = toupper(prefix)
    
    monthTry = paste(prefix, "01", year, collapse = " ")
    date = as.Date(monthTry, format="%b %d %y")
    
    rules = list()
    
    if( !is.na(date) )
    {
        startDate = as.Date(format( date, "%Y-%m-01" ))  
    }
    
    rules[["Q1"]] = function( year ) { as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" ) } 
    rules[["Q2"]] = function( year ) { as.Date( paste(year, "-04-01", sep=""), format="%y-%m-%d" ) }
    rules[["Q3"]] = function( year ) { as.Date( paste(year, "-07-01", sep=""), format="%y-%m-%d" ) }
    rules[["Q4"]] = function( year ) { as.Date( paste(year, "-10-01", sep=""), format="%y-%m-%d" ) }
    rules[["CAL"]] = function( year ) { as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" ) }
    rules[["WIN"]] = ifelse( Weather, as.Date( paste(year, "-01-01", sep=""), format="%y-%m-%d" ), 
            as.Date( paste(year, "-11-01", sep=""), format="%y-%m-%d" ) )
    rules[["SPR"]] = function( year ) { as.Date( paste(year, "-03-01", sep=""), format="%y-%m-%d" )}
    rules[["SUM"]] = ifelse( Weather,  as.Date( paste(year, "-07-01", sep=""), format="%y-%m-%d" ), 
            as.Date( paste(year, "-05-01", sep=""), format="%y-%m-%d" ) )

    startDate = rules[[prefix]](year)
    
    return( startDate )
}

################################################################################
# Given a specified term, what's the end date?
#
Term$endDate <- function( term , Weather=FALSE)
{
    library( SecDb )
    
	months <- c("F", "G", "H", "J", "K", "M", "N", "Q", "U", "V", "X", "Z")

    prefix = strsplit(as.character(term), " ")[[1]][1]
    year = strsplit(as.character(term), " ")[[1]][2]
    # If we have a pattern like 08/09
    if( length( grep("/", year) ) > 0 )
    {
        year = substring(year, 4,5)
    }
    
	numYear = as.integer(paste("20", year, sep=""))
    
    monthTry = paste(prefix, "01", year, collapse = " ")
    date = as.Date(monthTry, format="%b %d %y")
    
    prefix = toupper(prefix)
    
    if( !is.na(date) )
    {
        endDate = as.Date(secdb.dateRuleApply(date, "e"))
    }else if( prefix == "Q1" )
    {
        endDate = as.Date( paste(year, "-03-31", sep=""), format="%y-%m-%d" ) 
    }else if( prefix == "Q2" )
    {
        endDate = as.Date( paste(year, "-06-30", sep=""), format="%y-%m-%d" )
    }else if( prefix == "Q3" )
    {
        endDate = as.Date( paste(year, "-09-30", sep=""), format="%y-%m-%d" )
    }else if( prefix == "Q4" )
    {
        endDate = as.Date( paste(year, "-12-31", sep=""), format="%y-%m-%d" )
    }else if( prefix == "CAL" )
    {
        endDate = as.Date( paste(year, "-12-31", sep=""), format="%y-%m-%d" )
    }else if( prefix == "WIN" && Weather == TRUE)
    {
        endDate = as.Date( paste(year, "-03-31", sep=""), format="%y-%m-%d" )
    }else if( prefix == "WIN" )
    {
        beginOfFeb = as.Date( paste(year, "-02-01", sep=""), format="%y-%m-%d" )
        endDate = as.Date(secdb.dateRuleApply(beginOfFeb, "e"))
	}else if( prefix == "SPR" )
    {
        endDate = as.Date( paste(year, "-04-30", sep=""), format="%y-%m-%d" )
    }else if( prefix == "SUM" && Weather == TRUE)
    {
		endDate = as.Date( paste(year, "-09-30", sep=""), format="%y-%m-%d" )
	}else if( prefix == "SUM" )
	{    
		endDate = as.Date( paste(year, "-08-31", sep=""), format="%y-%m-%d" )
    }else if( sum((month =substring(prefix,2,2)) == months) == 1)
    {
        endDate = as.Date( paste(year, .decodeEndDate(month, numYear), sep=""), format="%y-%m-%d")
    }else
    {
        endDate = NULL 
    }
    
    return( endDate)
}


################################################################################
# Append a term label for a given set of trades
#
Term$appendToData <- function( data, format )
{
    data$term = NA
    for( rowIndex in seq_len(nrow(data)) )
    {
        rowData = data[rowIndex,]
        data$term[rowIndex] = Term$fromDatePair( rowData$SD, 
                rowData$ED, format )
    }
    
    return(data)
}


################################################################################
# Get the term label for a given start and end date
#
Term$fromDatePair <- function( startDate, endDate=NULL, format="%Y-%m-%d" )
{
    # If you don't get an endDate, assume they want a 1 month range
    if( is.null(endDate) )
    {
        endDate = startDate + 32
        endDate = as.Date(format(endDate, "%Y-%m-01"))
        endDate = endDate - 1 
    }
    
    # if you can't convert it to a date, you get a nice big fat error here
    startDate = as.Date(startDate, format=format)
    endDate = as.Date(endDate, format=format)
    
    # swap the dates if they're in the wrong order
    outOfOrder = which( startDate > endDate )
    temp = endDate[outOfOrder]
    endDate[outOfOrder] = startDate[outOfOrder]
    startDate[outOfOrder] = temp
    
    prefix = .getTermPrefix( startDate, endDate )
    
    suffix = .getTermSuffix( startDate, endDate )
    
    termList = paste(prefix, suffix)
    
    return(termList)
}


################################################################################
# Get the term label for a given start and end date
#
Term$reutersCode <- function( termString )
{
    termString = toupper( termString )
    
    monthYearPairs = strsplit(termString, " ")
    
    months = sapply( monthYearPairs, function(x){ .monthToReutersCode(x[1]) } )
    years = sapply( monthYearPairs, function(x){ x[2] } )
    
    codes = paste( months, years, sep="" )
    
    return( codes )
}


################################################################################
# Get a date from a given reuters code
#
Term$fromReutersCode <- function( codeString )
{
    codeString = as.character( codeString )
    
    # Replace extra spaces
    toSub = which( nchar(codeString) == 4 )
    codeString[toSub] = gsub(" ", "", codeString[toSub], fixed=TRUE ) 
 
    # Format single character codes
    toSub = which( nchar(codeString) == 1 )
    codeString[toSub] = paste( codeString[toSub], format(Sys.Date(), "%y"), sep="" )    

    # Stop if we have a code we can't format
    badCodes = which( nchar(codeString) != 3 )
    if( length(badCodes) > 0 )
    {
        stop("Unable to parse reuters code:", codeString[badCodes] )
    }
    
    monthCode = substr( codeString, 1, 1 )
    month = sapply( monthCode, .reutersCodeToMonth )
    dateString = paste( month, "01", substr(codeString, 2, 3 ), sep="-")
    date = as.Date( dateString, format="%b-%d-%y" )
    
    return( date )
}


################################################################################
# Run all the tests
#
Term$runTests <- function()
{
    .testStartDate()
    .testEndDate()
    .testGetTerm()
    .testFromReutersCode()
}


################################################################################
.monthToReutersCode <- function( monthString )
{
    codeValue = switch( monthString,
            "JAN" = "F",
            "FEB" = "G", 
            "MAR" = "H", 
            "APR" = "J",
            "MAY" = "K",
            "JUN" = "M", 
            "JUL" = "N", 
            "AUG" = "Q",
            "SEP" = "U",
            "OCT" = "V", 
            "NOV" = "X", 
            "DEC" = "Z",
            monthString )
    
    return( codeValue )
}


################################################################################
.reutersCodeToMonth <- function( monthString )
{
    codeValue = switch( monthString,
            "F" = "JAN",
            "G" = "FEB", 
            "H" = "MAR", 
            "J" = "APR",
            "K" = "MAY",
            "M" = "JUN", 
            "N" = "JUL", 
            "Q" = "AUG",
            "U" = "SEP",
            "V" = "OCT", 
            "X" = "NOV", 
            "Z" = "DEC",
            monthString )
    
    return( codeValue )
}


################################################################################
.getTermPrefix <- function( startDate, endDate )
{

    # set up the comparison variables
    startMonth = as.integer( format(startDate, "%m") )
    endMonth = as.integer( format(endDate, "%m") )
    
    # set up the list of returned terms
    termList = character( length(startDate) )
    
	#check for arbitrary strip
	anyMonthTerm = which( startMonth != endMonth )

	termList[anyMonthTerm] = paste(.encodeDate( startMonth[anyMonthTerm] ), 
            .encodeDate( endMonth[anyMonthTerm] ), sep="")
	
	#because 10-12 could be Q4
    calStrip = which( startMonth != 10 & endMonth == 12 )
    termList[calStrip] = format( startDate[calStrip], "Cal" )
    
    oneMonthTerm = which( startMonth == endMonth )
    termList[oneMonthTerm] = format( startDate[oneMonthTerm], "%b" )
    
	
    winterStrip = which( startMonth == 1 & endMonth == 2 )
    termList[winterStrip] = format( startDate[winterStrip], "Win" )
    
    winterStrip = which( startMonth == 11 & endMonth == 3 )
    termList[winterStrip] = format( startDate[winterStrip], "Win" )
    
    springStrip = which( startMonth == 3 & endMonth == 4 )
    termList[springStrip] = format( startDate[springStrip], "Spr" )
    
    summerStrip = which( startMonth == 7 & endMonth == 8 )
    termList[summerStrip] = format( startDate[summerStrip], "Sum" )
    
    ngSummerStrip = which( startMonth == 4 & endMonth == 10 )
    termList[ngSummerStrip] = format( startDate[ngSummerStrip], "Sum" )
    
    weatherSummerStrip = which( startMonth == 5 & endMonth == 9 )
    termList[weatherSummerStrip] = format( startDate[weatherSummerStrip], "Sum" )
	
	weatherSummerStrip = which( startMonth == 5 & endMonth == 10 )
	termList[weatherSummerStrip] = format( startDate[weatherSummerStrip], "Sum" )
    
    q1Strip = which( startMonth == 1 & endMonth == 3 )
    termList[q1Strip] = format( startDate[q1Strip], "Q1" )
    
    q2Strip = which( startMonth == 4 & endMonth == 6 )
    termList[q2Strip] = format( startDate[q2Strip], "Q2" )
    
    q3Strip = which( startMonth == 7 & endMonth == 9 )
    termList[q3Strip] = format( startDate[q3Strip], "Q3" )
    
    q4Strip = which( startMonth == 10 & endMonth == 12 )
    termList[q4Strip] = format( startDate[q4Strip], "Q4" )
    
    return( toupper(termList) )
}


################################################################################
.getTermSuffix <- function( startDate, endDate )
{
    termList = character(length(startDate))
    
    termList = lapply( seq_along(startDate), function(x){
                yearRange = seq(from=startDate[x], to=endDate[x], by="year")
                truncated = format(yearRange, "%y")
                return( paste(truncated, collapse="/") )
            } )
    
    termList = unlist( termList )
    
    return(termList)
}

##############################################################################################
#function to take date object (form year-month-date e.g. 2008-06-01)
#and return code for tenor month 
.encodeDate <- function(month)
{
    termList = month

    termList[ which( termList == 1) ] = "F"
    termList[ which( termList == 2) ] = "G"
    termList[ which( termList == 3) ] = "H"
    termList[ which( termList == 4) ] = "J"
    termList[ which( termList == 5) ] = "K"
    termList[ which( termList == 6) ] = "M"
    termList[ which( termList == 7) ] = "N"
    termList[ which( termList == 8) ] = "Q"
    termList[ which( termList == 9) ] = "U"
    termList[ which( termList == 10) ] = "V"
    termList[ which( termList == 11) ] = "X"
    termList[ which( termList == 12) ] = "Z"
    
    return( termList )
}


##############################################################################################
#function decode the start date from tenor code
.decodeStartDate <- function(month){
	if(month == "F"){
		return("-01-01")
	}
	else if(month == "G"){
		return("-02-01")
	}
	else if(month == "H"){
		return("-03-01")
	}
	else if(month == "J"){
		return("-04-01")
	}
	else if(month == "K"){
		return("-05-01")
	}
	else if(month == "M"){
		return("-06-01")
	}
	else if(month == "N"){
		return("-07-01")
	}
	else if(month == "Q"){
		return("-08-01")
	}
	else if(month == "U"){
		return("-09-01")
	}
	else if(month == "V"){
		return("-10-01")
	}
	else if(month == "X"){
		return("-11-01")
	}
	else if(month == "Z"){
		return("-12-01")
	}
}


##############################################################################################
#function to decode month from strip
.decodeMonth <- function(month){
	if(month == "F"){
		return("-01")
	}
	else if(month == "G"){
		return("-02")
	}
	else if(month == "H"){
		return("-03")
	}
	else if(month == "J"){
		return("-04")
	}
	else if(month == "K"){
		return("-05")
	}
	else if(month == "M"){
		return("-06")
	}
	else if(month == "N"){
		return("-07")
	}
	else if(month == "Q"){
		return("-08")
	}
	else if(month == "U"){
		return("-09")
	}
	else if(month == "V"){
		return("-10")
	}
	else if(month == "X"){
		return("-11")
	}
	else if(month == "Z"){
		return("-12")
	}
}


##############################################################################################
#function to decode the end date of a tenor
.decodeEndDate <- function(month, year){
	if(month == "F"){
		return("-01-31")
	}
	else if(month == "G"){
		if(((year %% 4 == 0) && (year %% 100 != 100)) || (year %% 400 == 0))
			return("-02-29")
		else{
			return("-02-28")
		}
	}
	else if(month == "H"){
		return("-03-31")
	}
	else if(month == "J"){
		return("-04-30")
	}
	else if(month == "K"){
		return("-05-31")
	}
	else if(month == "M"){
		return("-06-30")
	}
	else if(month == "N"){
		return("-07-31")
	}
	else if(month == "Q"){
		return("-08-31")
	}
	else if(month == "U"){
		return("-09-30")
	}
	else if(month == "V"){
		return("-10-31")
	}
	else if(month == "X"){
		return("-11-30")
	}
	else if(month == "Z"){
		return("-12-31")
	}
}
################################################################################
.testStartDate <- function()
{    
    library(RUnit)
    checkEquals(as.Date("2007-01-01"), Term$startDate("Q1 07"))
    checkEquals(as.Date("2008-04-01"), Term$startDate("Q2 08"))
    checkEquals(as.Date("2009-07-01"), Term$startDate("Q3 09"))
    checkEquals(as.Date("2010-10-01"), Term$startDate("Q4 10"))
    checkEquals(as.Date("2008-01-01"), Term$startDate("Cal 08"))
    checkEquals(as.Date("2008-01-01"), Term$startDate("Win 08"))
    checkEquals(as.Date("2008-03-01"), Term$startDate("Spr 08"))
    checkEquals(as.Date("2008-07-01"), Term$startDate("Sum 08"))
    checkEquals(as.Date("2008-01-01"), Term$startDate("Jan 08"))
    checkEquals(as.Date("2007-02-01"), Term$startDate("Feb 07"))
    checkEquals(as.Date("2008-03-01"), Term$startDate("Mar 08"))
    checkEquals(as.Date("2009-04-01"), Term$startDate("Apr 09"))
    checkEquals(as.Date("2010-05-01"), Term$startDate("May 10"))
    checkEquals(as.Date("2008-06-01"), Term$startDate("Jun 08"))
    checkEquals(as.Date("2008-07-01"), Term$startDate("Jul 08"))
    checkEquals(as.Date("2008-08-01"), Term$startDate("Aug 08"))
    checkEquals(as.Date("2008-09-01"), Term$startDate("Sep 08"))
    checkEquals(as.Date("2008-10-01"), Term$startDate("Oct 08"))
    checkEquals(as.Date("2008-11-01"), Term$startDate("Nov 08"))
    checkEquals(as.Date("2008-12-01"), Term$startDate("Dec 08"))
	checkEquals(as.Date("2008-02-01"), Term$startDate("GH 08"))
	checkEquals(as.Date("2008-05-01"), Term$startDate("KM 08"))
	checkEquals(as.Date("2008-08-01"), Term$startDate("QU 08"))
}



################################################################################
.testEndDate <- function()
{
    library(RUnit)
    checkEquals(as.Date("2007-03-31"), Term$endDate("Q1 07"))
    checkEquals(as.Date("2008-06-30"), Term$endDate("Q2 08"))
    checkEquals(as.Date("2009-09-30"), Term$endDate("Q3 09"))
    checkEquals(as.Date("2010-12-31"), Term$endDate("Q4 10"))
    checkEquals(as.Date("2008-12-31"), Term$endDate("Cal 08"))
    checkEquals(as.Date("2008-02-29"), Term$endDate("Win 08"))
    checkEquals(as.Date("2008-04-30"), Term$endDate("Spr 08"))
    checkEquals(as.Date("2008-08-31"), Term$endDate("Sum 08"))
    checkEquals(as.Date("2008-01-31"), Term$endDate("Jan 08"))
    checkEquals(as.Date("2007-02-28"), Term$endDate("Feb 07"))
    checkEquals(as.Date("2008-03-31"), Term$endDate("Mar 08"))
    checkEquals(as.Date("2009-04-30"), Term$endDate("Apr 09"))
    checkEquals(as.Date("2010-05-31"), Term$endDate("May 10"))
    checkEquals(as.Date("2008-06-30"), Term$endDate("Jun 08"))
    checkEquals(as.Date("2008-07-31"), Term$endDate("Jul 08"))
    checkEquals(as.Date("2008-08-31"), Term$endDate("Aug 08"))
    checkEquals(as.Date("2008-09-30"), Term$endDate("Sep 08"))
    checkEquals(as.Date("2008-10-31"), Term$endDate("Oct 08"))
    checkEquals(as.Date("2008-11-30"), Term$endDate("Nov 08"))
    checkEquals(as.Date("2008-12-31"), Term$endDate("Dec 08"))
	checkEquals(as.Date("2008-03-31"), Term$endDate("GH 08"))
	checkEquals(as.Date("2008-06-30"), Term$endDate("KM 08"))
	checkEquals(as.Date("2008-09-30"), Term$endDate("QU 08"))
}


################################################################################
.testGetTerm <- function()
{
    library(RUnit)
    checkEquals( Term$fromDatePair("2008-01-01", "2008-01-01"), "JAN 08" )
    checkEquals( Term$fromDatePair("2010-02-01", "2010-02-28"), "FEB 10" )
    checkEquals( Term$fromDatePair("2008-01-01", "2008-12-31"), "CAL 08" )
    checkEquals( Term$fromDatePair("2008-01-01", "2008-02-29"), "WIN 08" )
    checkEquals( Term$fromDatePair("2008-11-01", "2009-03-31"), "WIN 08/09" )
    checkEquals( Term$fromDatePair("2008-03-01", "2008-04-30"), "SPR 08" )
    checkEquals( Term$fromDatePair("2008-07-01", "2008-08-31"), "SUM 08" )
    checkEquals( Term$fromDatePair("2008-04-01", "2008-10-31"), "SUM 08" )
    checkEquals( Term$fromDatePair("2008-05-01", "2008-09-30"), "SUM 08" )    
    checkEquals( Term$fromDatePair("2008-01-01", "2008-03-31"), "Q1 08" )
    checkEquals( Term$fromDatePair("2008-04-01", "2008-06-30"), "Q2 08" )
    checkEquals( Term$fromDatePair("2008-07-01", "2008-09-30"), "Q3 08" )
    checkEquals( Term$fromDatePair("2008-10-01", "2008-12-31"), "Q4 08" )
    checkEquals( Term$fromDatePair("2008-01-01", "2009-01-01"), "JAN 08/09" )
    
    starts = c("2009-01-01", "2010-04-01")
    ends = c("2009-12-31", "2010-06-30")
    checkEquals( Term$fromDatePair(starts, ends), c("CAL 09", "Q2 10") )
}


################################################################################
.testFromReutersCode <- function()
{
    library(RUnit)
    checkEquals( Term$fromReutersCode("F08"), as.Date("2008-01-01") )
    checkEquals( Term$fromReutersCode("G10"), as.Date("2010-02-01") )
    checkEquals( Term$fromReutersCode("M 09"), as.Date("2009-06-01") )
    checkEquals( Term$fromReutersCode("Z 12"), as.Date("2012-12-01") )
    checkEquals( Term$fromReutersCode("Z"), as.Date(format(Sys.Date(), "%Y-12-01")) )
    checkEquals( Term$fromReutersCode("H"), as.Date(format(Sys.Date(), "%Y-03-01")) )
    
    checkEquals( Term$fromReutersCode(c("H", "M09", "G 10")), 
            c( as.Date(format(Sys.Date(), "%Y-03-01")),
                    as.Date("2009-06-01"), as.Date("2010-02-01")) )
}

