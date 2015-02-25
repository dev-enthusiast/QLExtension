\name{FTR.AuctionTermLevels}
\alias{FTR.AuctionTermLevels}
\title{Construct a factor for the action strings ordered in a certain way.}
\description{
Construct a factor for the action strings ordered in a certain way.
}
\usage{
FTR.AuctionTermLevels(auction)
}
\arguments{
  \item{auction}{A vector of strings as returned by FTR.AuctionTerm }
}
\value{
Returns an ordered factor (good for Lattice plotting).
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.AuctionTerm}}}
\examples{
  start.dts = as.Date(c("2008-01-01", "2008-01-01"))
  end.dts   = as.Date(c("2008-01-01", "2008-12-01"))

  auctions  = FTR.AuctionTerm(start.dts, end.dts)
  FTR.AuctionTermLevels(auctions)
 
}

\keyword{}
\keyword{}% __ONLY ONE__ keyword per line
