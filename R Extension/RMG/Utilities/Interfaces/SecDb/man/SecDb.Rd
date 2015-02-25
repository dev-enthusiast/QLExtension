\name{SecDb}
\alias{SecDb-package}
\alias{SecDb}
\docType{package}
\title{
R to SecDb Library
}
\description{
An R to SecDb interface library. The SecDb library maps data types betweeen R and SecDb, providing support for securities, curves, vectors, arrays, matrices, etc. It also implements some TsDb functions and SecDb calculator functionality.
}
\details{
\tabular{ll}{
Package: \tab SecDb\cr
Version: \tab 1.9.7\cr
Date: \tab 2009-06-05\cr
Depends: \tab R (>= 2.0.0)\cr
SystemRequirements: \tab Windows\cr
License: \tab Private\cr
}

}
\author{
John Scillieri <john.scillieri@constellation.com>

Maintainer: Adrian Dragulescu <adrian.dragulescu@constellation.com>
}
\keyword{ package }
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

startDate = Sys.Date() - 50
endDate = Sys.Date() - 1

##Read some curve data from TsDb
curveData = tsdb.readCurve( curveName="PRC_NG_Exchange_0706", 
  startDate=startDate, endDate=endDate )
print(head(curveData))

##Read a time based curve from TsDb
curveData2 = tsdb.readCurve( "nepool_load_hist", startDate, endDate )
print(head(curveData2))

## NOTE:  to avoid the annoying warning messages related to time
   zone, set: Sys.setenv(tz="").  It will not affect any calculation. 

curveInfo =  tsdb.getSymbolInfo( "PRC_NG_Exchange_0706" ) 
print(curveInfo)

##Can also use Slang evaluator
curveInfo2 = secdb.evaluateSlang( 'TsdbSymbolInfo("PRC_NG_EXCHANGE_0706")' )
print(curveInfo2)

##Get a given value type from a specified security
valueType = secdb.getValueType("Bal Close Attrition AcqCost", 
  "Date Created")
print(secdb.convertTime(valueType))

valueType2 = secdb.getValueType("Bal Close FrtMarks Config", "Contents")
print(valueType2)

##Returns null for invalid security or value type
badValueType = secdb.getValueType("Incorrect Security", "Bad Value")


}
