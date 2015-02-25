\name{secdb.isHoliday}
\alias{secdb.isHoliday}
\title{Find out if a given date is a holiday or not.}
\description{
Use a SecDb calendar to see if a given date is a holiday.  The input can
be a vector of dates.  Note: If the default argument skipWeekends =
FALSE is used, the function returns FALSE for business days.  If
skipWeekends = TRUE, only the holidays that do not fall on weekends are
returned.  In this sense the (SecDb) function is a bit inconsistent.
}
\usage{
secdb.isHoliday( dates, calendar="CPS-BAL", skipWeekends=FALSE )
}
\arguments{
  \item{dates}{R Date vector}
  \item{calendar}{the supported calendar in SecDb}
  \item{skipWeekends}{if weekends should be counted or not}
}
\value{Returns a vector of booleans (TRUE/FALSE) values}
\seealso{
  The SecDb object \code{Holiday CPS-BAL}, value type \code{Holidays}
  for a quick list of the holidays. 
}
\examples{
library(SecDb)

dates <- seq(as.Date("2006-12-15"), as.Date("2007-01-15"), by="day")
(dates[!secdb.isHoliday(dates)])  # these are the business days

# get all the holidays that don't fall on weekends
(dates[secdb.isHoliday(dates, skipWeekends=TRUE)]) 

# get all the holidays
hols <- secdb.convertTime(unlist(secdb.getValueType("Holiday CPS-BAL", "Holidays")))
dates %in% as.Date(hols)

}
\details{The function is a simple wrapper on other slang code.}
