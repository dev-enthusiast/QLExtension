\name{splitBucket}
\alias{splitBucket}
\title{Split a forward price into two component buckets.}
\description{
  Split a forward price into two component buckets.  For example, given
  a FLAT mark (shaped), split it into PEAK and OFFPEAK marks.  This
  operation is not directly possible as there are two unknowns and only
  one equation.  To get the second equation, we assume a linear
  relationship between the variables to split.  The regression is
  performed in the function body, and has the formula \code{toSplit[1] ~ toSplit[2]}.
  
  The function can also be used to split the OFFPEAK mark into the 2X16
  and 7X8 buckets. 
  
}
\usage{
  splitBucket(X, toSplit=c("PEAK", "OFFPEAK"), fromBucket="FLAT",
    region="NEPOOL", verbose=FALSE)
}
\arguments{
  \item{X}{ A data.frame that will have the columns c("month",
    toSplit, fromBucket).  The rows for the \code{toSplit} variables
    should contain NA's in the contract months that you want to
    calculate. }
  \item{toSplit}{ A vector of characters with SecDb bucket names in the
    order that will be used by the linear regression.}
  \item{fromBucket}{ A character string with SecDb bucket name (the
    parent of the two buckets you split into).}
  \item{region}{ A region name.  Defaults to NEPOOL.  Only tested with
    NEPOOL.  It is used to calculated the hourly weights.}
  \item{verbose}{Boolean, if you want to see details on the quality of
    the regression.}  
}
\value{Returns a dataframe with the previous missing rows now filled.}
\seealso{\code{\link{combineBuckets}}}
\examples{

startDate  <- as.Date("2006-01-01")
endDate    <- as.Date("2009-06-01")-1
bucket     <- c("FLAT", "PEAK", "OFFPEAK", "2X16H", "7X8")
symbol     <- c("nepool_smd_da_4006_lmp")

# get the historical price by bucket
res <- ntplot.bucketizeByMonth(symbol, bucket, startDate, endDate,
  region="NEPOOL")

X <- cast(res, month ~ bucket, I, fill=NA)

# looks good! linear relationship
plot(X$OFFPEAK, X$PEAK, col="blue")

# create some forward data (fake stuff)
X <- merge(X, data.frame(month=as.Date(c("2010-01-01", "2010-02-01")),
                         FLAT=c(52.35, 51.07)), all=TRUE)
tail(X)
#         month     FLAT     PEAK  OFFPEAK    2X16H      7X8
# 38 2009-02-01 50.63603 56.04028 45.72307 48.98188 43.86089
# 39 2009-03-01 42.49398 47.22310 38.23657 40.79764 36.74348
# 40 2009-04-01 35.16031 38.42284 32.03962 37.33805 29.21379
# 41 2009-05-01 37.45554 41.46975 34.42594 39.26545 30.99145
# 42 2010-01-01 52.35000       NA       NA       NA       NA
# 43 2010-02-01 51.07000       NA       NA       NA       NA
# >

# now do the split
X <- splitBucket(X, toSplit=c("PEAK", "OFFPEAK"),
  fromBucket="FLAT", region="NEPOOL", verbose=TRUE)
tail(X)
#         month     FLAT     PEAK  OFFPEAK    2X16H      7X8
# 38 2009-02-01 50.63603 56.04028 45.72307 48.98188 43.86089
# 39 2009-03-01 42.49398 47.22310 38.23657 40.79764 36.74348
# 40 2009-04-01 35.16031 38.42284 32.03962 37.33805 29.21379
# 41 2009-05-01 37.45554 41.46975 34.42594 39.26545 30.99145
# 42 2010-01-01 52.35000 59.52969 46.93137       NA       NA
# 43 2010-02-01 51.07000 57.40083 45.31470       NA       NA
# >
  
}