\name{combineBuckets}
\alias{combineBuckets}
\title{Combine several buckets into one.}
\description{
Combine several buckets (for example the offpeak weekend and offpeak
overnight buckets) into one (offpeak) value.  Split the offpeak mark
into the two buckets 2X16H and 7X8 according to a set of weights.
  
Using the weight="none", you just do simple addition which is good for
positions.  If weight="hour", the two offpeak buckets will be weighted
by their hours.  This is desirable for prices.  After the two buckets
are combined, the new bucket is renamed "OFFPEAK", and appended to the
remaining data.frame.

The function can combine more than two buckets, for example to calculate
the "FLAT" price given the three buckets "2X16H", "7X8", "5X16". 

}
\usage{
combineBuckets(X, weight="none", buckets=c("2X16H", "7X8"),
  multiplier=rep(1, length(buckets)), name="OFFPEAK")
}
\arguments{
  \item{X}{A data.frame with at least columns c("contract.dt", "bucket",
    "value").  If weight="hour", an additional column called "hours" is
    required (but the code will try to get this info automatically.)  X
    can contain more buckets than the ones requested to combine, the
    unused buckets will be returned.}
  \item{weight}{Either "none" or "hour".  See description.}
  \item{buckets}{The specific bucket names that you want to combine.}
  \item{multiplier}{How to combine the buckets.  By default it performs
    addition between the buckets.  To substract a bucket, use c(1,-1).}
  \item{name}{The name of the buckets that will get combined.}
}
\value{Returns a dataframe with the new values bucketed.} 
\seealso{\code{\link{splitBucket}}}
\examples{

# some fabricated price data ... 
X = data.frame(
  contract.dt=as.Date(c("2009-01-01", "2009-01-01")),
  bucket=c("2X16H", "7X8"),
  hours=c(160, 248),
  value=c(55, 47))

combineBuckets(X, weight="hour", buckets=c("2X16H", "7X8"), name="OFFPEAK")
#  contract.dt    value  bucket
#1  2009-01-01 50.13725 OFFPEAK


# Example of use with negative weights
X = data.frame(
  contract.dt=as.Date(c("2009-01-01", "2009-01-01")),
  bucket=c("OFFPEAK", "7X8"),
  hours=c(408, 248),
  value=c(50.137, 47))

combineBuckets(X, weight="hour", buckets=c("OFFPEAK", "7X8"),
  multiplier=c(1,-1), name="2X16H")
#  contract.dt    value bucket
#1  2009-01-01 54.99935  2X16H



}
\note{}