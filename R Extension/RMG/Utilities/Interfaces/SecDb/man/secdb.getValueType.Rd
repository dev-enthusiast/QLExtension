\name{secdb.getValueType}
\alias{secdb.getValueType}
\title{Returning a Value Type From SecDb}
\description{
Returns the named value type for a given security.
}
\usage{
secdb.getValueType( securityName, valueTypeName )
}
\arguments{
  \item{securityName}{a character string giving the name of the security to retrieve}
  \item{valueTypeName}{the name of the value type that will be converted and returned}
}
\value{Returns the SecDb value type in a compatible R format (list, vector, etc).}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get a given value type from a specified security
valueType = secdb.getValueType("Bal Close Attrition AcqCost",
  "Date Created")
print(secdb.convertTime(valueType))

##This returns an R list representing the SecDb Structure object
valueType2 = secdb.getValueType("Bal Close FrtMarks Config", "Contents")
print ("Got Contents of Bal Close FrtMarks Config")

}
\details{Although the appropriate value type conversions from SecDb objects 
will be made to return the most suitable R type, it is possible that a SecDb 
object is unsupported. If this is the case, please contact CCG RMG Analytics 
and we will look into adding the functionality. If a SecDb object is 
unsupported, the C++ conversion routine will return NULL back to the R code.
}