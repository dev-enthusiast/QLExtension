\name{FTR.enforcePathConstraints}
\alias{FTR.enforcePathConstraints}
\title{Enforce a set of constraints on a portfolio of paths.}
\description{Enforce a set of constraints on a portfolio of paths.
  Only these constraints are implemented now:
  - Control the number of MW at one node with \code{options$bids$max.mw.node},
  works for both sources and sinks. }
\usage{
FTR.enforcePathConstraints(paths, ISO.env, month=NULL, enh=3, causal=TRUE,
  sSP=NULL, cut.quantile=0.25)
}
\arguments{
  \item{paths}{The usual data.frame that defines the path.  It has at
    least c("path.no", "class.type", "source.ptid",
    "sink.ptid") column names.}
  \item{ISO.env}{The usual ISO.env.
    See \code{\link{FTR.load.ISO.env}}.}
  \item{month}{The month of the auction as an R Date.  Usually
    options$month.} 
  \item{enh}{How many times you want to weight more the similar months.}
  \item{causal}{If \code{causal==TRUE}, consider only the auctions
    before \code{month}.}
  \item{sSP}{Simulated settle prices.  As returned by
    \code{FTR.simulate.SP}.}
  \item{cut.quantile}{What quantile of the simulated settle price
    distribution is to be used as the max bid.}
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

res <- FTR.enforcePathConstraints(Paths, NEPOOL, month=as.Date("2008-06-01"), enh=3)
res[[2]]

#  path.no     CP.min  CP.enh.p5 CP.enh.p25 CP.enh.p50     SP.min SP.enh.p25 SP.nby.p25    min.bid   max.bid
#1       1 -7.0502976 -7.0502976 -4.8218342 -3.5018750 -9.2487216  -4.785093  -4.603363 -9.2487216 -4.785093
#2       2 -0.6349734 -0.4417015 -0.2531378 -0.1555875 -0.6383468  -0.272164  -0.278682 -0.6383468 -0.272164



}