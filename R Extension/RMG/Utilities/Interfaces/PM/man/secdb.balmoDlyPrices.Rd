\name{secdb.balmoDlyPrices}
\alias{secdb.balmoDlyPrices}
\title{Read bal month prices from SecDb.}
\description{
  Read the bal month prices from SecDb.
}
\usage{
secdb.balmoDlyPrices( asOfDate, region="NEPOOL", location="Hub",
  buckets=NULL, forDate=NULL )
}
\arguments{
  \item{asOfDate}{an R Date}
  \item{commodity}{a character string giving the SecDb region name
    (e.g. NEPOOL, ERCOT, NYPP)}
  \item{location}{a character string with SecDb location name}
  \item{bucket}{a character string with SecDb bucket name.  If NULL,
    will return all buckets.}
  \item{forDate}{an R Date specifying the exact date you want.  If NULL 
    default it returns all days.}
}
\value{Returns a dataframe.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

  asOfDate <- as.Date("2012-07-24")
  region   <- "NEPOOL"
  location <- "CT"
  
  res <- secdb.balmoDlyPrices( asOfDate, region=region,
    location=location)

#         Date  2x16H   5x16    7x8
#1  2012-07-25  0.000 40.000 28.500
#2  2012-07-26  0.000 50.000 28.500
#3  2012-07-27  0.000 40.000 28.500
#4  2012-07-28 40.000  0.000 28.500


  
}
