# Deal with terms.  Should be an improvement over CEGTerm package.  
# A Term is a date interval. 
#
# Term                         # the constructor
# parse_term
#



#################################################################
# Define a Term.  A Term is a Month, MonthRange, Year, YearRange
# (not a Day or DayRange).  
# @param startDt a start Date
# @param endDt an end Date
# @param name the name of this term, make up something if NA
# @noMonths the number of months in this term
# @return a Term object
#
Term <- function(startDt, endDt, name=NA, noMonths=NA)
{
  if (is.na(noMonths)) {
    begm <- as.Date(startDt, "%Y-%m-01")
    endm <- as.Date(endDt, "%Y-%m-01")
    noMonths <- length(seq(begm, endm, by="1 month"))
  }

  if (is.na(name)) {
    # come up with a name
    if (noMonths == 1) {
      name <- format(startDt, "%b%y")
    } else if (noMonths == 12) {
      if (format(startDt, "%m") == "01") {
        name <- format(startDt, "Cal %y")
      } else {
        name <- paste(format(startDt, "%b%y"), format(endDt, "%b%y"), sep="-")
      }
    }
  }
  
  
  structure(list(name = name,
                 startDt = startDt,
                 endDt = endDt,
                 noMonths = noMonths),
            class="Term")
}


Term.parse <- function(x)
{
  parse_term(x)
}

#################################################################
# This function is not vectorized!
# @param x a string that should be converted to a Term 
# @return a term object
#
parse_term <- function(x)
{
  xL <- nchar(x)

  # if it's a month in the F13 format
  if (xL == 3) {
    first <- substring(x, 1, 1)
    ind <- match(first, c("F", "G", "H", "J", "K", "M", "N", "Q", 
          "U", "V", "X", "Z"))
    if ( is.na(ind) ) 
      stop("Wrong input ", x)
    yy <- as.numeric(substring(x, 2, 3))
    yyyy <- if (yy > 50) 1900+yy else 2000+yy
    startDt <- as.Date(paste(yyyy, ind, "01", sep="-"))
    
    return(Term(startDt, nextMonth(startDt)-1, x, 1))
  }

  # if it's a month in the Jan13 format
  if (xL == 5) {
    startDt <- as.Date(paste(1, x, sep=""), format="%d%b%y")
    
    return( Term(startDt, nextMonth(startDt)-1, x, 1) )
  }


  # a month range in this form "Jan13-Feb13"
  if (xL == 11) {
    startDt <- as.Date(paste(1, substring(x,1,5), sep=""), format="%d%b%y")
    endDt <- as.Date(paste(1, substring(x,7,11), sep=""), format="%d%b%y")

    return( Term(startDt, nextMonth(endDt)-1, x,
            length(seq(startDt, endDt, by="1 month"))) )
  }


  # a calendar
  if (grepl("^CAL", toupper(x))) {
    yy <- substring(x, xL-1, xL)
    startDt <- as.Date(paste("1Jan", yy, sep=""), format="%d%b%y")
    endDt <- as.Date(paste("31Dec", yy, sep=""), format="%d%b%y")
    
    return( Term(startDt, nextMonth(endDt)-1, x, 12) )
  }

  stop("Don't know how to parse ", x)
}





#################################################################
#################################################################
# 
.main <- function()
{
  require(CEGbase)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.term.R")
 
  xs <- c("X14",            # a month
          "Jan13",          # a month 
          "Jan13-Feb13",    # winter
          "Jul13-Aug13",    # summer     
          "CAL 2012",       # a year
          "Cal 11"          # a year, ICE style 
          )

  lapply(xs, parse_term)

  
  

}
