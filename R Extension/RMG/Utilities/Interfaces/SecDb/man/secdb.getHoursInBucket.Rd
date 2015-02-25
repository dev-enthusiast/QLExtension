\name{secdb.getHoursInBucket}
\alias{secdb.getHoursInBucket}
\title{Returning a data frame of only the hours in a given bucket}
\description{
Returns a data frame of times enumerating the hours in a given bucket
}
\usage{
secdb.getHoursInBucket( region, bucket, startDate, endDate )
}
\arguments{
  \item{region}{the region that the specified bucket should apply}
  \item{bucket}{the name of desired bucket, e.g. "5x16", "7x8", etc.}
  \item{startDate}{the lower bound of the returned time range}
  \item{endDate}{the upper bound of the returned time range}
}
\value{Returns a data frame of times in the bucket requested.}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Retrieve only the hours in the bucket
timeList = secdb.getHoursInBucket( "nepool", "7x8", Sys.Date()-1, Sys.Date() )

timeList
##                   time
## 1  2007-11-15 01:00:00
## 2  2007-11-15 02:00:00
## 3  2007-11-15 03:00:00
## 4  2007-11-15 04:00:00
## 5  2007-11-15 05:00:00
## 6  2007-11-15 06:00:00
## 7  2007-11-15 07:00:00
## 24 2007-11-16 00:00:00
## 25 2007-11-16 01:00:00
## 26 2007-11-16 02:00:00
## 27 2007-11-16 03:00:00
## 28 2007-11-16 04:00:00
## 29 2007-11-16 05:00:00
## 30 2007-11-16 06:00:00
## 31 2007-11-16 07:00:00
## 48 2007-11-17 00:00:00

}
\details{The rownames for the times return reflect the hour in the total range
requested. If you don't require this data, simply set rownames(data)=NULL. 
}
