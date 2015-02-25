###############################################################################
# Term.R
# 
# Utilities for converting to/from the {F-Z, Cal, Q, etc} nomenclature
#
# e.g. "F 08", "Cal 10", "Win 11"
#
# Author: John Scillieri
#         Adrian Dragulescu
# 

.nextdaystart <- function() Sys.Date()+1  # check if OK for Fri
.nextdayend   <- function() Sys.Date()+1  # does it matter, peak/offpeak? 
.balweekstart <- function() {
  aux <- Sys.Date() + 2
  #if (weekdays(aux)=="Saturday")
  return(aux)
}
.balweekend <- function(){
  aux <- as.POSIXlt(Sys.Date())
  if (aux$wday <= 5) {
    Sys.Date() + 5 -  aux$wday     # go the the next Fri
  } else {                         # it's Sat 
    Sys.Date() + 12 -  aux$wday                    # is this correct??
  }
}
.nextweekstart <- function(){
  aux <- as.POSIXlt(Sys.Date())
  if (aux$wday >= 2){
    Sys.Date() + 8 - aux$wday
  } else {                         # it's a Monday 
    Sys.Date() + 7 - aux$wday                       # is this correct??
  }
}
.nextweekend <- function() .nextweekstart()+4

.secondweekstart <- function() .nextweekstart()+7
.secondweekend   <- function() .nextweekstart()+11

.balmonthstart <- function() Sys.Date()+2
.balmonthend <- function()
  as.Date(seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="1 month",
          length.out=2)[2])-1


###############################################################################
# Given a specified term, what's the start date?
#
termStartDate <- function( term , Weather=FALSE )
{
    if (gsub(" ", "", tolower(term))=="nextday")
      return(.nextdaystart())
    if (gsub(" ", "", tolower(term))=="balweek")
      return(.balweekstart())
    if (gsub(" ", "", tolower(term))=="2ndweek")
      return(.secondweekstart())
    if (gsub(" ", "", tolower(term))=="balmonth")
      return(.balmonthstart())
  
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
    }else if( prefix == "BAL")
    {
        if (year == format(Sys.Date(), "%y"))
          startDate <- as.Date(format(Sys.Date(), "%Y-%m-01"))
        else
          startDate <- as.Date(paste(year, "-01-01", sep=""), format="%y-%m-%d")
    }else if(( sum((month=substring(prefix,1,1)) == months) == 1) & (nchar(prefix)<3))
    {  # for KM, QU and the likes ...
	startDate = as.Date( paste(year, .decodeStartDate(month), sep=""), format="%y-%m-%d")
    }else
    {
        startDate = NULL 
    }
    
    return( startDate)
}


################################################################################
# Given a specified term, what's the end date?
#
termEndDate <- function( term , Weather=FALSE)
{
    if (gsub(" ", "", tolower(term))=="nextday")
      return(.nextdayend())
    if (gsub(" ", "", tolower(term))=="balweek")
      return(.balweekend())
    if (gsub(" ", "", tolower(term))=="2ndweek")
      return(.secondweekend())
    if (gsub(" ", "", tolower(term))=="balmonth")
      return(.balmonthend())
    
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
    }else if( prefix == "BAL")
    {
        endDate = as.Date(paste(year, "-12-31", sep=""), format="%y-%m-%d")
    }else if( (sum((month =substring(prefix,2,2)) == months) == 1) & (nchar(prefix)<3))
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
# @param data is a data.frame
#
termAppendToData <- function( data, format )
{
    data$term = NA
    for( rowIndex in seq_len(nrow(data)) )
    {
        rowData = data[rowIndex,]
        data$term[rowIndex] = termFromDatePair( rowData$SD, 
                rowData$ED, format )
    }
    
    return(data)
}


################################################################################
# Get the term label for a given start and end date
#
termFromDatePair <- function( startDate, endDate=NULL, format="%Y-%m-%d" )
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
term.reutersCode <- function( termString )
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
termFromReutersCode <- function( codeString )
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
.monthToReutersCode <- function( monthString )
{
    codeValue = switch( toupper(monthString),
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
.reutersCodeToMonth <- function( monthLetter )
{
    codeValue = switch( toupper(monthLetter),
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
            monthLetter )
    
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

