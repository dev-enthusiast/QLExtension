\name{secdb.getBucketMask}
\alias{secdb.getBucketMask}
\title{Returning a mask of hours in a given bucket}
\description{
Returns a data frame of times & booleans enumerating the hours in a given bucket
}
\usage{
secdb.getBucketMask( region, bucket, startDate, endDate )
}
\arguments{
  \item{region}{the region that the specified bucket should apply}
  \item{bucket}{the name of desired bucket, e.g. "5x16", "7x8", etc.}
  \item{startDate}{the date that the bucket mask should start}
  \item{endDate}{the date that the bucket mask should stop}
}
\value{Returns a data frame of times and booleans.}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Retrieve the mask 
mask = secdb.getBucketMask( "nepool", "5x16", Sys.Date()-1, Sys.Date() )

##List the first day's worth of mask data
head(mask, n=24)

##                   time  5x16
## 1  2007-11-15 01:00:00 FALSE
## 2  2007-11-15 02:00:00 FALSE
## 3  2007-11-15 03:00:00 FALSE
## 4  2007-11-15 04:00:00 FALSE
## 5  2007-11-15 05:00:00 FALSE
## 6  2007-11-15 06:00:00 FALSE
## 7  2007-11-15 07:00:00 FALSE
## 8  2007-11-15 08:00:00  TRUE
## 9  2007-11-15 09:00:00  TRUE
## 10 2007-11-15 10:00:00  TRUE
## 11 2007-11-15 11:00:00  TRUE
## 12 2007-11-15 12:00:00  TRUE
## 13 2007-11-15 13:00:00  TRUE
## 14 2007-11-15 14:00:00  TRUE
## 15 2007-11-15 15:00:00  TRUE
## 16 2007-11-15 16:00:00  TRUE
## 17 2007-11-15 17:00:00  TRUE
## 18 2007-11-15 18:00:00  TRUE
## 19 2007-11-15 19:00:00  TRUE
## 20 2007-11-15 20:00:00  TRUE
## 21 2007-11-15 21:00:00  TRUE
## 22 2007-11-15 22:00:00  TRUE
## 23 2007-11-15 23:00:00  TRUE
## 24 2007-11-16 00:00:00 FALSE

}
\details{The column headings reflect the bucket requested. This is useful when
creating a mask for various buckets. All the resultant times could be 'cbind'ed
together creating the desired time mask (for 5x16, 2x16H, etc). 
}
