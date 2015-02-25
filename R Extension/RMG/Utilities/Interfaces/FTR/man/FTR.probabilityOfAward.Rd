\name{FTR.probabilityOfAward}
\alias{FTR.probabilityOfAward}
\title{Estimate the expected awards given a set of bids.}
\description{Estimate the expected awards given a set of bids.}
\usage{
FTR.probabilityOfAward(bids, ISO.env)
}
\arguments{
  \item{bids}{A data.frame with at least c("path.no", "class.type",
    "source.ptid", "sink.ptid", "bid.no", "bid.price") column
    names.}
  \item{ISO.env}{An ISO environment that you get with
    \code{FTR.load.ISO.env}.}
}
\value{
A data frame with columns c("path.no", "bid.no", "probAward").

The probability of award is calculated by looking at the probability
distribution of cleared prices and see where the bids fall in the
empirical probability distribution.

}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{

FTR.load.ISO.env("NEPOOL")

bids <- data.frame(path.no=1:2, class.type=c("OFFPEAK", "ONPEAK"),
  source.ptid = c(326, 4000), sink.ptid   = c(4001, 4002),
  bid.no=c(1,1), mw=c(100,100), bid.price=c(-0.6, -0.35))

probAwds <- FTR.probabilityOfAward( bids, NEPOOL )

#  path.no bid.no probAward
#1       1      1 0.2696629
#2       2      1 0.3146067



}