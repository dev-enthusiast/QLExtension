\name{FTR.simulateAwards}
\alias{FTR.simulateAwards}
\title{Simulate the process of awarding bids.} 
\description{Simulate the process of awarding bids by using a
  probability of getting the award by path.}
\usage{
FTR.simulateAwards( bids, probs, noSims=10 )
}
\arguments{
  \item{bids}{A data.frame with bid information.  See
    \code{\link{FTR.makeBids}}.}
  \item{probs}{A matrix with dim=(noBids, noPaths) showing the
    probability of being awarded a bid.  The element probs[i,j]
    is the probability of being awarded bid "i-1" for path "j".
    probs[1,j] is the probability of not being awarded path "j". }
  \item{noSims}{Number of simulations.}
}
\value{
An awards data.frame with at least columns c("path.no", "bid.no",
"bid.price", "mw", "sim").  The column "sim" has the simulation number.

The probs matrix allows arbitrary settings for the award probability so
different scenarios can be tested easily.  For example, what if CT 
clears higher than usual, etc. 
}
\author{Adrian Dragulescu}
%\seealso{\code{\link{FTR.simulate.SP}} and \code{\link{FTR.checkBidLimits}}}
\examples{

bids <- FTR.makeBids( paths, bid.limits, sSP, options)

probs <- matrix(c(0.40,0.35,0.2, 0.05,rep(0,6)), ncol=nrow(paths),
  nrow=10, dimnames=list(c("out",1:9), paths$path.no))

sAwds <- FTR.simulateAwards(bids, probs)

# > head(awds)
#    path.no bid.no   mw    bid.price sim
# 1   133850      1 10.0 -0.002837358   1
# 7   365396      1 10.0 -0.070476190   1
# 14  372047      1  5.0 -0.052618608   1
# 28  485112      1  5.0 -0.444603495   1
# 42  500217      1  4.0  0.007426307   1
# 48  500815      1  2.5 -0.025147849   1


}