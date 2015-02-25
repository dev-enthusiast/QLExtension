\name{CP}
\docType{data}
\alias{CP}
\title{FTR clearing price data.}
\description{
  FTR clearing price data from long and short term FTR auctions, 2005 to present.
  Shows source, sink, class.type, award.price, auction.

  Run \code{NEPOOL$.makeClearingPriceArchive()} to create the data.
  First you may need to run \code{NEPOOL$.downloadFTRAwards}.
}
\usage{data(FTRAWD)}
\format{One path per row.}
\source{NEPOOL
  http://www.iso-ne.com/markets/othrmkts_data/ftr/auction_results/2011/jan/index.html
  the ftrcp file.}
\keyword{datasets}

