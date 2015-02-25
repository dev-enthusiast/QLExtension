\name{FTR.makePortfolioPaths}
\alias{FTR.makePortfolioPaths}
\title{Given a set of paths, create the bids data.frame, enforce some
  sanity constraints on paths.}
\description{Given a set of paths, create the bids data.frame, enforce some
  sanity constraints on paths.  No source/sink total
  quantity more than \code{options$max.mw.node}.  Max bid per path
  should have at least \code{options$min.mw.path} MW.  The risk/reward
  ratio is calculated with the highest bid.}
\usage{
FTR.makePortfolioPaths(paths, bid.limits, sSP, options,
  bidcurve.map=NULL, qmax.map=NULL)
}
\arguments{
  \item{paths}{The usual data.frame that defines the path.  It has at
    least c("path.no", "class.type", "source.ptid",
    "sink.ptid") column names.}
  \item{bid.limits}{A data.frame with at least c("path.no", "min.bid",
    "max.bid") columns.}
  \item{sSP}{A matrix with simulated settle prices for the auction
    term.  Dimensions c(noPaths, noSims).  Necessary to calculate the
    path payoffs, and the risk/reward ratio that determines the maximum
    quantity to bid on the path.}
  \item{options}{The usual options you get with
    \code{FTR.makeDefaultOptions}}
  \item{bidcurve.map}{A data.frame with two columns
    c("bid.price.percent", "bid.quant.percent") that specifies how the
    bid curve is constructed.  The sum of \code{bid.quant.percent}
    column should add up to 1.  See example below.}
  \item{qmax.map}{A data.frame with two columns c("ratio", "Qmax") to
    determine the maximum quantity per path given a risk/reward ratio.
    See example below.}
}
\value{
A data.frame with at least columns c("path.no", "bid.price", "mw"). 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.simulate.SP}} and \code{\link{FTR.checkBidLimits}}}
\examples{

# > qmax.map
#   ratio Qmax
# 1   2.0  200
# 2   1.0  100
# 3   0.5   50
# 4   0.0   25

# >  bidcurve.map
#   bid.price.percent bid.quant.percent
# 1              1.00             0.100
# 2              0.92             0.050
# 3              0.83             0.075
# 4              0.75             0.100
# 5              0.50             0.125
# 6              0.25             0.200
# 7              0.00             0.350
#
#options$max.mw.node <- 200
#options$min.mw.path <- 2.5

bids <- FTR.makePortfolioPaths( paths, bid.limits, sSP, options)
head(bids)

#   path.no Qmax class.type source.ptid sink.ptid   source.name           sink.name    bid.price   mw
# 4  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.002837358 10.0
# 6  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.051181798  5.0
# 2  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.105569293  7.5
# 3  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.153913733 10.0
# 1  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.304990108 12.5
# 7  133850  100     ONPEAK        4336       546 LD.LYNN    23 UN.RESCOS  115 RESC -0.456066482 20.0


}