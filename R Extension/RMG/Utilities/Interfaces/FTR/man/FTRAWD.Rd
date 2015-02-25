\name{FTRAWD}
\docType{data}
\alias{FTRAWD}
\title{FTR awards data.}
\description{
  FTR awards data long and short term FTR auctions from 2005 to present.
  Shows customer.name, source, sink, class.type, award.price, auction.

  Run \code{NEPOOL$.makeAwardsArchive()} to create the data.
  First you may need to run \code{NEPOOL$.downloadFTRAwards}.
}
\usage{data(FTRAWD)}
\format{One path per row.}
\source{NEPOOL
  http://www.iso-ne.com/markets/othrmkts_data/ftr/auction_results/2011/jan/index.html
  the ftrstl file.}
\keyword{datasets}

