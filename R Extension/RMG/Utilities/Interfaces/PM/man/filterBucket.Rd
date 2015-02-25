\name{filterBucket}
\alias{filterBucket}
\title{Filter an houly zoo time series to observations in a given bucket.}
\description{
Given an hourly time series, filter it to observation in a given
bucket.
}
\usage{
  filterBucket(x, buckets=c("5X16", "2X16H", "7X8"), region="NEPOOL")
}
\arguments{
  \item{x}{A zoo time series with hourly frequency.}
  \item{buckets}{The specific bucket names that you want to combine.}
  \item{region}{The canonical SecDb region name.}
}
\value{Returns a list with one zoo time series for each bucket.} 
\seealso{\code{\link[FTR]{FTR.load.tsdb.symbols}}}
\examples{

# to aggregate a zoo object x by day and bucket, first filter
# then aggregate
#
#
# aux <- filterBucket(x, buckets=c("5X16", "2X16H", "7X8"))
# DD <- lapply(aux,            # the -1 is to deal with the HE convention
#   function(x){aggregate(x, as.Date(format(index(x)-1, "%Y-%m-%d")), mean, na.rm=TRUE)})
#
# To aggregate by month:
# MM <- lapply(aux,
#   function(x){aggregate(x, as.Date(format(index(x)-1, "%Y-%m-01")), mean, na.rm=TRUE)})
#
# To add the buckets as a column into each list element:
# aux <- mapply(function(x, y){
#   cbind(time=index(x), bucket=y, data.frame(x))
#   }, DD, names(DD), SIMPLIFY=FALSE)
# lapply(aux, head)
#    


}
\note{}