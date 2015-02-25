\name{secdb.getShapingFactors}
\alias{secdb.getShapingFactors}
\title{Fetch SecDb shaping factors}
\description{
  Read the realised or current forward shaping factors from Secdb.
  Can be a historical date as well to compare with realised.
  Note that the data is in Hour Beginning format
}
\usage{
   secdb.getSecDbShaping(region="NEPOOL", loc="hub", StartDate=nextMonth(Sys.Date()), EndDate=nextMonth(nextMonth(Sys.Date()))-1, AsOfDate=as.Date(Sys.Date()), ServiceType ="Energy")
\arguments{
  \item{Region}{FERC region name, Default="NEPOOL"}
  \item{loc}{SecDb location name from Calc, Default="Hub"}
  \item{StartDate}{Start Date, Default=nextMonth(Sys.Date())}
  \item{EndDtae}{EndDate, Default=end of next month}
  \item{AsOfDate}{valuation date.}
  \item{ServiceType}{ Secdb service type. Defaults to Energy}
}
\value{Returns a dataframe with bucket price, hourly price and the
correspong Shaping factor.}
\author{Ankur Salunkhe}
\examples{

  require(CEGbase)
  require(SecDb)
  require(plyr)
  require(reshape2)

  region <- "NEPOOL"
  loc <- "hub"
  StartDate <- as.Date("2016-01-01")
  EndDate <- as.Date("2016-01-31")
  AsOfDate <- as.Date("2015-01-15")
  ServiceType <- "Energy"

  op <- secdb.getSecDbShaping(region, loc, StartDate, EndDate, AsOfDate, ServiceType)

##          month bucket                time hrly_pwr   asofdate location
## 1 2016-01-01    7X8 2016-01-01 00:00:00 78.39316 2015-01-15      hub
## 2 2016-01-01    7X8 2016-01-01 01:00:00 74.14992 2015-01-15      hub
##   servicetype buc_pwr shapingfac
## 1      Energy  78.119  1.0035095
## 2      Energy  78.119  0.9491919

}


