\name{tsdb.futCurve}
\alias{tsdb.futCurve}
\title{Read a curve from the TsDb}
\description{
Read both daily and hourly curves from TsDb
}
\usage{
tsdb.futCurve( curveName, asOfDate, startDate, endDate )
}
\arguments{
  \item{curveName}{a character string giving the name of the curve to read}
  \item{asOfDate}{an R Date variable that will serve as the pricing date}
  \item{startDate}{an R Date variable that will be the lower bounds on the curve data}
  \item{curveName}{an R Date variable that will be the upper bounds on the curve data}
}
\value{Returns a list of date/value parings.}
\examples{
  secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")
  
  startDate = Sys.Date() + 1
  endDate = Sys.Date() + 365
  asOfDate = Sys.Date() - 1
  
  ## The NYMEX future curve...
  curveData = tsdb.futCurve( curveName="PRC_NG_Exchange", asOfDate=asOfDate, 
    startDate=startDate, endDate=endDate )
  print(head(curveData))

}
