\name{secdb.convertTime}
\alias{secdb.convertTime}
\title{Convert a SecDb Time to an R Time}
\description{
Take a SecDb time object in seconds and convert it to an R time object.
}
\usage{
secdb.convertTime(timeValue)
}
\arguments{
  \item{timeValue}{a numeric returned from SecDb representing the time in 
  seconds since Jan 1, 1970.}
}
\value{Returns a R Time object equal to the returned SecDb object.}
\examples{
library(SecDb)

## The string used is a standard SecDb database path
secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get a given value type from a specified security
valueType = secdb.getValueType("Bal Close Attrition AcqCost",
  "Date Created")
print(secdb.convertTime(valueType))

}
