\name{termStartDate}
\alias{termStartDate}
\title{Get the start/end Date of a trading term.}
\description{Get the start/end Date of a trading term.}
\usage{
  termStartDate(term, Weather=FALSE)

  termEndDate(term, Weather=FALSE)
}
\arguments{
  \item{term}{A string with the term you want.  See notes for additional
      details.}
  \item{Weather}{A boolean, if you want to use the seasons for the
    weather trading.}
}
\value{
An R date.
}
\author{John Scillieri}
\note{Formats supported are of type:
  
  "F09" for month Jan-09,
  
  "Jan 08" for month Jan-08,
  
  "Q1 07" for first quarter of 2007,
  
  "Cal 08" for calendar 2008,
  
  "QU 08" for several months combos, in this case summer 08,
  
  "Bal 08" for balance of the year 2008,
  
  "Spr 09" for the spring season, Sum and Win seasons supported.
  They may not have the calendar meaning you expect, so please check. 

  "Next Day", "Bal Week", "2nd Week", "Bal Month" -- ICE style for
  power.  Added on 2010-12-20, may need more testing (AAD).
  
  PASS the year in format "\%y".  Otherwise unexpected results (BUGS) may occur!
}
\seealso{}
\examples{

term <- c("SPR 09", "NQ 09", "BAL 09", "CAL 10", "CAL 11")

# get the starting dates
as.Date(sapply(term, termStartDate),origin="1970-01-01")

# expand the months in the term
sD = as.Date(sapply(term, termStartDate),origin="1970-01-01")
eD = as.Date(sapply(term, termEndDate),origin="1970-01-01")
mapply(seq, sD, eD, by="1 month")
 

}
