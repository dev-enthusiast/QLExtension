\name{FTR.makeBidLimits}
\alias{FTR.makeBidLimits}
\title{Create the max and min bid price for a set of paths.}
\description{Create the max and min bid price for a set of paths.  It
  compares the monthly settle prices and monthly cleared prices for the
  past ... auctions, and has a simple algo built in.   This is supposed
  to be a high throughput function.  All paths are supposed to by buys
  (bids).  If a simulated settle is provided
  \code{is.null(sSP)==FALSE}, the min(max) bid will be taken
  as the minimum(cut.quantile) of the simulated settle price distribution.}
\usage{
FTR.makeBidLimits(paths, sSP, cut.quantile=list(c(0,0.25)))
}
\arguments{
  \item{paths}{The usual data.frame that defines the path.  It has at
    least c("path.no", "class.type", "source.ptid",
    "sink.ptid") column names.}
  \item{sSP}{Simulated settle prices.  As returned by
    \code{FTR.simulate.SP}.}
  \item{cut.quantile}{What quantile of the simulated settle price
    distribution is to be used as the max bid.  A list with length equal
    to the number of paths.  The elements of the list are the min/max
    quantiles of the simulated prices.}
}
\value{
  A list with two data.frames.  The second one has what you want 
  with at least columns c("path.no", "min.bid", "max.bid"). 
}
\author{Adrian Dragulescu}
%\seealso{\code{\link{FTR.get.CP.for.paths}}}
\examples{

Paths <- data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid   = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"))

sSP <- FTR.simulate.SP( hSP, options, noSims=5000, noEns=50 )

res <- FTR.makeBidLimits( Paths, sSP, cut.quantile=list(0,0.1) )

#  path.no     CP.min  CP.enh.p5 CP.enh.p25 CP.enh.p50     SP.min SP.enh.p25 SP.nby.p25    min.bid   max.bid
#1       1 -7.0502976 -7.0502976 -4.8218342 -3.5018750 -9.2487216  -4.785093  -4.603363 -9.2487216 -4.785093
#2       2 -0.6349734 -0.4417015 -0.2531378 -0.1555875 -0.6383468  -0.272164  -0.278682 -0.6383468 -0.272164



}