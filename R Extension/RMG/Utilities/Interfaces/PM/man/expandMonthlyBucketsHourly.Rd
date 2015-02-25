\name{expandMonthlyBucketsHourly}
\alias{expandMonthlyBucketsHourly}
\title{Given a set of values for a given month and buckets, create an
  hourly curve with values corresponding to each bucket mask.}
\description{
Given a set of values (prices or positions) for a given month and
various buckets, create an hourly curve with values corresponding to
each bucket mask.   
}
\usage{
  expandMonthlyBucketsHourly(month, x, region="NEPOOL", agg=FALSE,
    bucketMask=NULL)
}
\arguments{
  \item{month}{An R Date object representing the beginning of the month day.}
  \item{x}{A named numerical vector with valid bucket names as vector names.}
  \item{region}{The canonical SecDb region name.}
  \item{agg}{A boolean, if you want to aggregate all buckets into one.}
  \item{bucketMask}{A data frame with the bucket mask for the hours of
    the month.  If \code{NULL} will calculate it on the fly.}
}
\value{Returns a data.frame with one column for "time", and one for each
  bucket, or if \code{agg=TRUE}, only one column for "time", and one for
  aggregated values.

  If \code{x} is a unit vector, the result is actually the \code{bucketMask}.
}
\examples{

  require(SecDb)

  month  <- as.Date("2011-05-01")
  x <- structure(c(50,25,10), names=c("5X16", "2X16H", "7X8"))
  
  res <- expandMonthlyBucketsHourly(month, x, agg=FALSE)
  head(res)
#                   time 5X16 2X16H 7X8
# 25 2011-05-01 01:00:00    0     0  10
# 26 2011-05-01 02:00:00    0     0  10
# 27 2011-05-01 03:00:00    0     0  10
# 28 2011-05-01 04:00:00    0     0  10
# 29 2011-05-01 05:00:00    0     0  10
# 30 2011-05-01 06:00:00    0     0  10



}
\note{}