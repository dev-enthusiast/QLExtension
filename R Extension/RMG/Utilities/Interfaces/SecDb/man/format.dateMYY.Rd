\name{format.dateMYY}
\alias{format.dateMYY}
\title{Format an R date to the SecDB MYY format and back.}
\description{
Convert an character vector of dates in the MYY format to an R vector of
dates.  And backwards. 
}
\usage{
format.dateMYY( dates, direction=1 )
}
\arguments{
  \item{dates}{R Date vector}
  \item{direction}{If direction=1, convert from MYY to as.Date.  If
    direction=-1 convert from as.Date tp mYY. }
}
\value{Returns a vector of Dates or characters depending on the value of
  direction}
\seealso{
}
\examples{
library(SecDb)

# from YMM to Date
format.dateMYY(c("F07", "Q09"))

# from Date to YMM
dates <- seq(as.Date("2006-12-15"), as.Date("2007-01-15"), by="day")
(format.dateMYY(dates, -1))

}
\details{}
