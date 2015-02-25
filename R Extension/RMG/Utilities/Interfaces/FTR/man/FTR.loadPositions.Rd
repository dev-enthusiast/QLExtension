\name{FTR.loadPositions}
\alias{FTR.loadPositions}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Load positions from different data sources.}
\description{
Load positions from different data sources.
}
\usage{
FTR.loadPositions(from="PRISM", books=NULL, asOfDate=NULL,
                              region=NULL, incdec=FALSE)
}
\arguments{
  \item{from}{A string to indicate the source of the positions. Can be
    either "PRISM" (recommended), or "SecDb".  See Notes below.}
  \item{books}{A list of books you want the positions.}
  \item{asOfDate}{An R Date to get positions as of that date.}
  \item{region}{A NERC region.  Get positions only for this region}
  \item{incdec}{A boolean if you want the inc/decs to be returned.}
}
\value{
  Returns a data frame. 
}
\author{Adrian Dragulescu}
\note{If from="SecDb", I use a script AAD: _lib SecDB to R,
  read_deltas_leaf, which is a bit rudimentary.  It is recommended you
  use from="PRISM", which is the new Black Bird FOIT infrastructure.

  As of now, the code will return only positions that have MCC in the Delivery
  Point, and combine long and short legs, and the offpeak curves to create a path. 
}
\seealso{}
\examples{

# make sure yesterday is a valid business day
res <- FTR.loadPositions(books="NESTAT", asOfDate=Sys.Date()-1)
res
#      eti region   book counterparty   start.dt     end.dt buy.sell class.type   mw sink.ptid source.ptid
#1 PZP2ZWW NEPOOL NESTAT       NEPOOL 2008-05-01 2008-05-31      BUY    OFFPEAK 25.0       345        1255
#2 PZP31EU NEPOOL NESTAT       NEPOOL 2008-05-01 2008-05-31      BUY    OFFPEAK 50.0      4128        4165
#3 PZP31EW NEPOOL NESTAT       NEPOOL 2008-05-01 2008-05-31      BUY    OFFPEAK 47.6      4153         433
#4 PZP31F0 NEPOOL NESTAT       NEPOOL 2008-05-01 2008-05-31      BUY    OFFPEAK 25.0      4194        4169



res <- FTR.loadPositions(from="SecDB", books=c("NYTCCTMP"))

head(res)
#       book bucket delivery.point   end.date                            leaf
# 1 NYTCCTMP   7x24    DAM MCC PJM 2009-02-28  EPHY  PWR DAMNYP 28Feb09 4XM 0
# 2 NYTCCTMP   7x24   DAM MCC HUDV 2009-02-28  EPHY  PWR DAMNYP 28Feb09 5ZY 0
# 3 NYTCCTMP   7x24   DAM MCC WEST 2009-02-28  EPHY  PWR DAMNYP 28Feb09 LKP 0
# 4 NYTCCTMP   Peak       DAM HUDV 2008-04-30 EPHY  PWR DAMNYP 30Apr08 3CYC 0
# 5 NYTCCTMP   7x24    DAM MCC PJM 2008-04-30  EPHY  PWR DAMNYP 30Apr08 67Z 0
# 6 NYTCCTMP   7x24   DAM MCC WEST 2008-04-30  EPHY  PWR DAMNYP 30Apr08 7TP 0
#            market.location quantity security.type start.date location
# 1 Commod PWR NYPP PHYSICAL   -49728 Elec Physical 2009-02-01  DAM PJM
# 2 Commod PWR NYPP PHYSICAL  -172704 Elec Physical 2009-02-01 DAM HUDV
# 3 Commod PWR NYPP PHYSICAL   222432 Elec Physical 2009-02-01 DAM WEST
# 4 Commod PWR NYPP PHYSICAL   -35200 Elec Physical 2008-04-01 DAM HUDV
# 5 Commod PWR NYPP PHYSICAL   289440 Elec Physical 2008-04-01  DAM PJM
# 6 Commod PWR NYPP PHYSICAL   -87840 Elec Physical 2008-04-01 DAM WEST



}
