\name{addBucket}
\alias{addBucket}
\title{Get the bucket from a POSIXct time.}
\description{
  Get the bucket from a POSIXct time.  Function is vectorized.
}
\usage{
  addBucket(x, buckets=c("5X16", "2X16H", "7X8"), region="NEPOOL")
}
\arguments{
  \item{x}{A vector of HOURLY POSIXct datetimes.}
  \item{buckets}{The specific bucket names that you want to combine.}
  \item{region}{The canonical SecDb region name.}
}
\value{Returns a vector with the bucket name corresponding to the input
  vector.  Values will be NA if the time stamp does not match the
  requested buckets.} 
\seealso{\code{\link{filterBucket}}}
\examples{

  x <- seq(as.POSIXct("2013-01-01 01:00:00"), by="1 hour", length.out=50)
  res <- data.frame(index=x, bucket=addBucket(x))
  head(res, 9)
#                 index bucket
#1  2013-01-01 01:00:00    7X8
#2  2013-01-01 02:00:00    7X8
#3  2013-01-01 03:00:00    7X8
#4  2013-01-01 04:00:00    7X8
#5  2013-01-01 05:00:00    7X8
#6  2013-01-01 06:00:00    7X8
#7  2013-01-01 07:00:00    7X8
#8  2013-01-01 08:00:00  2X16H
#9  2013-01-01 09:00:00  2X16H
  

  data.frame(index=x, bucket=addBucket(x, buckets="PEAK"))  # OFFPEAK will be NA!

}
\note{}