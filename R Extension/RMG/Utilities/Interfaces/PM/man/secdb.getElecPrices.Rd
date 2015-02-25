\name{secdb.getElecPrices}
\alias{secdb.getElecPrices}
\title{Read elec prices from SecDb.}
\description{
  Read the elec prices for a given location, bucket, service type.  If 
  the arguments are vectors, all possible combinations are returned.
}
\usage{
secdb.getElecPrices( asOfDate, startDate, endDate, commodity, location,
                     bucket, serviceType="Energy", useFutDates=FALSE )
}
\arguments{
  \item{asOfDate}{an R Date}
  \item{startDate}{an R Date variable for the beginning of the
    historical range}
  \item{endDate}{an R Date variable for the end of historical range}
  \item{commodity}{a character string giving the SecDb commod name}
  \item{location}{a character string with SecDb location name}
  \item{bucket}{a character string with SecDb bucket name}
  \item{serviceType}{a character string with SecDb service type}
  \item{useFutDates}{if you want the dates to be aligned on Contract month}  
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# you may need to change the dates below ... 
asOfDate    <- as.Date("2008-12-19")
startDate   <- as.Date("2009-06-01")
endDate     <- as.Date("2010-12-01")
commodity   <- "COMMOD PWR NYPP PHYSICAL"
location    <- "DAM MCC HUDV"
bucket      <- "FLAT"
serviceType <- "Energy"
useFutDates <- FALSE
  
# get the elec congestion marks
curveData = secdb.getElecPrices( asOfDate, startDate, endDate,
  commodity, location, bucket, serviceType, useFutDates)
head(curveData)

#     asOfDate     location bucket serviceType      month    value service
# 1 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-06-01 365.5476  Energy
# 2 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-07-01 459.9302  Energy
# 3 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-08-01 456.8993  Energy
# 4 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-09-01 396.2891  Energy
# 5 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-10-01 412.5698  Energy
# 6 2008-12-19 DAM MCC HUDV   FLAT      Energy 2009-11-01 440.8498  Energy
  
}
