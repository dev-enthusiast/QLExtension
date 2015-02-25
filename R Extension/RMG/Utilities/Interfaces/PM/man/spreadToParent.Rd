\name{spreadToParent}
\alias{spreadToParent}
\title{Calculate the difference between a node's mark and the its
  corresponding parent.}
\description{
Calculate the difference between a node's mark and the its corresponding
parent for the three buckets (5x16, 7x8, 2x16H) in NEPOOL.  The
function is useful for preparing data for submission to ExternalData.
The function calls \code{secdb.getElecPrices} internally to grab the
parent marks.

A parent is customarily defined in the sense of SecDb DeliveryPoint
parent, as you can find in the "Bal Close PWR NEPOOL PM" and "Bal Close
PWR NEPOOL MCC", for NEPOOL.  These relationships are available in the
PM package as a data set that can be loaded with \code{data(MAP_DP)}. 
}
\usage{
spreadToParent(X, asOfDate=Sys.Date(), maxMonth=as.Date("2017-12-01"),
  region="NEPOOL")
}
\arguments{
  \item{X}{A data.frame with at least columns c("delivPt", "month",
    "bucket", "value") where column "value" represents the node mark for
    a given ptid, forward month, and bucket.  An optional column
    "parentDelivPt" can be specified, if the spread to a parent
    different from the parent in SecDb is requested. }
  \item{asOfDate}{An R Date variable indicating the date of the marks.}
  \item{maxMonth}{An R Date somewhere safe in the future.}
  \item{region}{A character string specifying the region.  Only NEPOOL
    is currently supported.}
}
\value{Returns a data.frame.  See the examples.} 
\seealso{\code{\link{addSpreadToParent}} for the function that performs
  the oposite operation, and \code{\link{writeExternalDataFile}} on how
  to format for ExternalData.}
\examples{

# consider this input data.frame ... 
head(X)
#            delivPt      month bucket    value
# 1 BPT_RESC.BPTR DA 2009-03-01  2X16H 45.71182
# 2 BPT_RESC.BPTR DA 2009-03-01   5X16 48.59922
# 3 BPT_RESC.BPTR DA 2009-03-01    7X8 37.11362
# 4 BPT_RESC.BPTR DA 2009-04-01  2X16H 44.37191
# 5 BPT_RESC.BPTR DA 2009-04-01   5X16 47.90505
# 6 BPT_RESC.BPTR DA 2009-04-01    7X8 34.09497
# >

# make the call
spread <- spreadToParent(X, asOfDate=Sys.Date())
head(spread)
#        month bucket parentDelivPt          delivPt      value parentMark childMark
# 1 2009-03-01  2X16H            CT BPT_RESC.BPTR DA -0.7050694   46.41689  45.71182
# 2 2009-03-01   5X16            CT BPT_RESC.BPTR DA -4.2223682   52.82159  48.59922
# 3 2009-03-01    7X8            CT BPT_RESC.BPTR DA -0.8338162   37.94744  37.11362
# 4 2009-04-01  2X16H            CT BPT_RESC.BPTR DA -2.6810931   47.05300  44.37191
# 5 2009-04-01   5X16            CT BPT_RESC.BPTR DA -4.1699512   52.07500  47.90505
# 6 2009-04-01    7X8            CT BPT_RESC.BPTR DA  0.1309698   33.96400  34.09497
# >

  
}
