\name{FTR.makeDefaultOptions}
\alias{FTR.makeDefaultOptions}
\title{Create the default options for an auction and region.}
\description{Create the default options for an auction and region.  This
is in a state of flux.  I may change the fields in the future.  Expect
changes. }
\usage{
FTR.makeDefaultOptions(auction, ISO.env)
}
\arguments{
  \item{auction}{An auction name, e.g. "F08", or "F08-1Y"}
  \item{region}{An ISO.env as returned by FTR.load.ISO.env.}
}
\value{
  An list with convenience constants related to this auction.}
\details{
  \enumerate{
  \item{auction}{the auction name.}
  \item{region}{the NERC region name.}
  \item{start.dt}{Start date for the auction period as an R Date.}
  \item{end.dt}{End date for the auction period as an R Date.}
  \item{month}{If monthly auction, the month as an R Date.}
  \item{nby.months}{The nearby months, M-1, M, M+1}
  \item{days}{Number of days by bucket.}
  \item{days.ind}{List with days in different buckets, e.g. which days
    are onpeak to be used by simulation later. }
  }
  etc.
}
\author{Adrian Dragulescu}
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
options <- FTR.makeDefaultOptions("M08", NEPOOL)
(options)


}