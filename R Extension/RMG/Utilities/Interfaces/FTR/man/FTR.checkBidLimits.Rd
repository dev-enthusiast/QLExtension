\name{FTR.checkBidLimits}
\alias{FTR.checkBidLimits}
\title{Check the bid price limits given a set of simulated settle prices
  for the auction.}
\description{Check the bid price limits given a set of simulated settle prices
  for the auction.  If the max.bid price is higher than the median of
  the simulations, the bid range is moved at the 3/8 quantile of the
  simulated distribution.}
\usage{
FTR.checkBidLimits( sSP, bid.limits )
}
\arguments{
  \item{sSP}{The simulated settle prices for the auction as returned by
    \code{\link{FTR.simulate.SP}}}
  \item{bid.limits}{The bid limits data.frame as returned by \code{\link{FTR.makeBidLimits}}}
}
\value{
  Returns the bid.limits data.frame with the adjusted bids (if any found).
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.makeBidLimits}}}
\examples{



}