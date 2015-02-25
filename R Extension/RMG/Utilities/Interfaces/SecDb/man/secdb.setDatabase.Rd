\name{secdb.setDatabase}
\alias{secdb.setDatabase}
\alias{setDatabase}
\title{Set the SecDb Database}
\description{
Sets the database that the SecDb library will connect to and read from.
}
\usage{
secdb.setDatabase("Bal Prod 1")
secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")
}
\arguments{
  \item{databaseName}{a character string giving the name of the database to
  connect to}
}
\examples{
library(SecDb)

## The string used is a standard SecDb database path
secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get a given value type from a specified security
valueType = secdb.getValueType("Bal Close Attrition AcqCost",
  "Date Created")
print(secdb.convertTime(valueType))

}
\note{This call \bold{does not} actually connect to the database, it simply
stores the string that will be used when an actual call to the library is made.

An underlying artifact of the C library used is that you may only connect to
one SecDb database per session.
}