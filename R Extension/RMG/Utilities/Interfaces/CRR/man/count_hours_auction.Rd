\name{count_hours_auction}
\alias{count_hours_auction}
\title{Calculate the number of hours in an FTR/TCC/CRR auction}
\description{Calculate the number of hours in an FTR/TCC/CRR auction}
\usage{
  count_hours_auction(auctionName=c("F11", "F11-1Y"), region="NEPOOL",
    buckets=c("FLAT", "PEAK", "OFFPEAK"))
}
\arguments{
  \item{autionName}{A String which uniquely identifies the auction.  See
    \code{\link{CRR_Auction}}.} 
  \item{region}{A String with the region name "NEPOOL", "NYPP", "MISO",
    "PJM".}
  \item{buckets}{A vector of Strings with valid SecDb bucket names.}
}
\value{
  A data.frame with columns \code{c("bucket", "auction", "hours")}.
}
%\details{}
\author{Adrian Dragulescu}
\note{}
\seealso{\code{\link{CRR_auction}}}
\examples{

count_hours_auction("X10", region="NEPOOL")
#   bucket auction hours
#    FLAT     X10   721
#    PEAK     X10   336
# OFFPEAK     X10   385


}
