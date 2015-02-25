\name{CRR_Auction}
\alias{CRR_Auction}
\alias{parse_auctionName}
\alias{archived_auctions}
\title{Construct an FTR/TCC/CRR Auction}
\description{Construct an FTR/TCC/CRR Auction.  An auction is defined
  by three tokens: the start date, the duration, and the (optional) round.}
\usage{
  CRR_Auction(startDt, endDt, round=0, region="NEPOOL")

  parse_auctionName(auctionName, region="NEPOOL")

  archived_auctions(region="NEPOOL", startDt=NULL,
    fromDt=NULL, noMonths=NULL, round=NULL)
}
\arguments{
  \item{startDt}{An R Date, a beginning of the month date when the
    auction starts.}
  \item{endDt}{An R Date, an end of the month date.}
  \item{round}{A number for the round of the auction.}
  \item{region}{A String with the region name "NEPOOL", "NYPP", "MISO",
    "PJM".}
  \item{autionName}{A String which uniquely identifies the auction.  It
    is composed of three tokens separated by \code{-}.  First token is
    the start date of the auction, in format "MYY", e.g. "F14" for the
    auction starting on  1-Jan-2014.  Second token is the duration of
    the auction.  This can be expressed as "3M", "6M", "1Y", "2Y" for 3
    months, 6 months, 1 year, and 2 years respectively.  If the auction
    is only one month, this token can be omitted (if there is only one
    round).  The third token is the round of the auction, in format "R"+
    the round number, e.g. "R1" will be round one.}
  \item{fromDt}{An R Date, to filter all the auctions that start on or
    after this date.  Useful if you want to see all the auctions that
    happened from a given date.}
  \item{noMonths}{An integer, number of months in the auction.}
  \item{round}{An integer, the round number of the auction.}
}
\value{
  A CRR_Auction object (a list).  It contains \code{auctionName} a unique auction
  identifier.

  The function \code{parse_auctionName} is vectorized and returns a list 
  to \code{CRR_Auction} objects.

  The function \code{archived_auctions} returns a list of past
  \code{CRR_Auction} objects (taken from the archive of Cleared Prices.)

  The function \code{count_hours_auction} returns a data.frame with
  columns \code{c("bucket", "auction", "hours")}. 
 
}
\details{
  Usually you will use the \code{parse_auctionName} function to
  construct a \code{CRR_Auction} object because you know the
  \code{auctionName}.

  Function \code{archived_auctions} can be used to retrieve all the past 
  auctions that start on a given date, or have a certain duration, etc.
}
\author{Adrian Dragulescu}
\note{}
%\seealso{\code{\link{FTR.countBindingConstraints}}}
\examples{

  auctionNames <- c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
                    "M13-12M", "M13-6M-R1")
  auctions <- parse_auctionName( auctionNames, region="NEPOOL" )


  # there is a nice as.data.frame method for CRR_Auction
  do.call(rbind, lapply(auctions, as.data.frame))
  #  auctionName    startDt      endDt round noMonths region
  #1         X10 2010-11-01 2010-11-30     0        1 NEPOOL
  #2      X11-R2 2011-11-01 2011-11-30     2        1 NEPOOL
  #3      F10-1Y 2010-01-01 2010-12-31     0       12 NEPOOL
  #4   F13-1Y-R2 2013-01-01 2013-12-31     2       12 NEPOOL
  #5     M13-12M 2013-06-01 2014-05-31     0       12 NEPOOL
  #6   M13-6M-R1 2013-06-01 2013-11-30     0        6 NEPOOL


  auctions <- archived_auctions(region="NEPOOL", startDt=as.Date("2014-01-01"))
  sapply(auctions, "[[", "auctionName")
  # "F14-1Y-R1" "F14-1Y-R2" "F14"
  # one monthly and two rounds of annual auction

}
