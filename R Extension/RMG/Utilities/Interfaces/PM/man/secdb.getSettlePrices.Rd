\name{secdb.getSettlePrices}
\alias{secdb.getSettlePrices}
\title{Read settlement prices from SecDb.}
\description{
  Get the settle prices for a given location from SecDb (not Tsdb). 
}
\usage{
  secdb.getSettlePrices(startDate, endDate, location,
    container="BAL Close PWR NEPOOL PM")
}
\arguments{
  \item{startDate}{an R Date variable for the beginning of the
    historical range}
  \item{endDate}{an R Date variable for the end of historical range}
  \item{location}{a character string with SecDb location name, can be a
	vector}
  \item{container}{a character string with the SecDb container name.}
}
\value{Returns a dataframe. The column value is the price requested.}
\details{Internally, the function uses the SecDb built-in 'Price Fixes
  By Time Dated Actual'.  The container name you pass in, should support
  that.}
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# after Mar-2010, you can do
startDate <- as.Date("2010-03-01")
endDate   <- as.Date("2010-03-31")
location  <- c("BUCKSPRT.4140 DA", "CL8")
container <-"BAL Close PWR NEPOOL PM"
  
# get the settle prices from secdb
curveData = secdb.getSettlePrices(startDate, endDate,
  location, container)
head(curveData)

#           location                time value
# 1 BUCKSPRT.4140 DA 2010-03-01 01:00:00 31.44
# 2 BUCKSPRT.4140 DA 2010-03-01 02:00:00 31.87
# 3 BUCKSPRT.4140 DA 2010-03-01 03:00:00 31.87
# 4 BUCKSPRT.4140 DA 2010-03-01 04:00:00 31.87
# 5 BUCKSPRT.4140 DA 2010-03-01 05:00:00 31.87
# 6 BUCKSPRT.4140 DA 2010-03-01 06:00:00 33.49
# >
  
}
