\name{writeExternalDataFile}
\alias{writeExternalDataFile}
\title{Calculate the difference between a node's mark and the its
  corresponding zone mark.}
\description{

}
\usage{
writeExternalDataFile(X, marksForDate, maxMonth=as.Date("2017-12-01"),
  save=TRUE, filePostfix="_AAD", email=FALSE)
}
\arguments{
  \item{X}{A data.frame with at least columns c("ptid", "month",
    "bucket", "value") where column "value" represents the node mark for
    a given ptid, forward month, and bucket.}
  \item{marksForDate}{An R Date variable indicating the date of the marks.}
  \item{region}{A character string specifying the region.  Only NEPOOL
    is currently supported.}
  
}
\value{Returns a dataframe.  See the examples.} 
\seealso{\code{\link{writeExternalDataFile}}}
\examples{

# consider this input data.frame ... 
head(X)
#      ptid      month bucket    value
# 1273  547 2009-03-01  2X16H 50.26415
# 1274  547 2009-03-01   5X16 52.05676
# 1275  547 2009-03-01    7X8 38.47577
# 1276  547 2009-04-01  2X16H 48.51592
# 1277  547 2009-04-01   5X16 50.47622
# 1278  547 2009-04-01    7X8 36.66301

# make the call
Xz <- writeExternalDataFile(X, marksForDate)
head(Xz)

#   bucket      month zone.id ptid node.value         deliv.pt zone.value      value
# 1  2X16H 2009-03-01    4008  547   50.26415 WARD_HIL.RESN DA     50.474 -0.2098505
# 2  2X16H 2009-04-01    4008  547   48.51592 WARD_HIL.RESN DA     48.013  0.5029199
# 3  2X16H 2009-05-01    4008  547   47.34670 WARD_HIL.RESN DA     47.089  0.2577008
# 4  2X16H 2009-06-01    4008  547   49.03784 WARD_HIL.RESN DA     49.740 -0.7021581
# 5  2X16H 2009-07-01    4008  547   54.50414 WARD_HIL.RESN DA     55.491 -0.9868618
# 6  2X16H 2009-08-01    4008  547   54.10969 WARD_HIL.RESN DA     54.803 -0.6933097
  
}
\note{}