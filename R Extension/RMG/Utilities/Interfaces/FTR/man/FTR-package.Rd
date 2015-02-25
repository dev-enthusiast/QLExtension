\name{FTR-package}
\alias{FTR-package}
\alias{FTR}
\docType{package}
\title{
A collection of utilities for analyzing FTRs in several ISOs.
}
\details{
  \tabular{ll}{
Package: \tab FTR\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2008-04-16\cr
License: \tab CEG\cr
}
}
\author{
Adrian Dragulescu

Maintainer: Adrian Dragulescu <adrian.dragulescu@constellation.com>
}

\description{A collection of functions for analyzing FTRs in NEPOOL and
  NYISO.

  Treatment of negative congestion in NYISO.  When I create the NYPP$hP
  data.frame with the daily historical prices for all nodes, I reverse
  the sign of the tsdb hourly prices.  This way, the FTR payoff can still be
  calculated as SettlePriceSink - SettlePriceSource - (ClearedPriceSink
  - ClearedPriceSource).  The clearing prices that I keep in NYPP$CP are
  exactly the ones from the NYISO files, there is no need to change
  sign.  In function \code{FTR.getPathReturns} if \code{hourly=TRUE} and
  \code{ISO.env=NYPP}, I need to add a minus sign to the historical
  prices, as they come directly from tsdb.  No other special treatment 
  is needed.  The bid prices that get produced by \code{FTR.makeBids}
  are correct and ready to send to the NYISO.

  In SecDb, an ISO awarded path from A --> B, gets booked as B --> A,
  and a payment of (ClearedPriceSource - ClearedPriceSink) gets booked.
  Therefore, when I get the positions from SecDb/BlackBird with the 
  function \code{FTR.loadPositions}, I need to adjust for this, and will
  reverse the source and the sink names.  No other changes are necessary.   

}
\keyword{ package }

\examples{
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid   = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"))

hSP <- FTR.get.hSP.for.paths( Paths, NEPOOL, melt=F )

res <-FTR.pathApply(NULL, hSP, NEPOOL, quantile, probs=c(0,0.5), na.rm=T)
res

BC <- FTR.getBindingConstraints(region="NEPOOL", type="DA")
head(BC)

res <- FTR.get.CP.for.paths( Paths, NEPOOL )
head(res)


}
