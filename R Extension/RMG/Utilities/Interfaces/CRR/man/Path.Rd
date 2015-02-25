\name{Path}
\alias{Path}
\title{Construct an FTR/TCC/CRR path}
\description{Construct an FTR/TCC/CRR path.}
\usage{
Path(source.ptid,
     sink.ptid,
     bucket, 
     auction,
     mw=1,
     contract="OBLIGATION", \dots)
}
\arguments{
  \item{source.ptid}{An integer representing the node ID of the source.}
  \item{sink.ptid}{An integer representing the node ID of the sink.}
  \item{bucket}{An String "PEAK", "OFFPEAK", or "FLAT" depending on the
    region.  A warning is issued if you try to construct a NYPP TCC path
    with a different bucket than FLAT, etc.}
  \item{auction}{A \code{\link{CRR_Auction}} object.}
  \item{mw}{A number representing the MW quantity awarded.}
  \item{contract}{A String either "OBLIGATION" or "OPTION".}
  \item{\dots}{Other path characteristics, for example a strategy name,
    buddy ptid, etc.}
}
\value{
  A Path object (internally a list).
}
\details{
  If \code{bucket} is missing and the auction region is NYPP, the bucket
  is set to "FLAT".

  Occasionally the ISO retires one node and the CRR needs to be
  reassigned to another node.  You can achieve this by passing in the
  \dots argument the fields \code{source.ptid.buddy},
  \code{source.ptid.buddy.startDt} and similarly for the sink location.
  This will allow you to control how to deal with retired/reassigned nodes. 
  
}
\author{Adrian Dragulescu}
\note{}
%\seealso{\code{\link{FTR.countBindingConstraints}}}
\examples{

  # a NEPOOL path
  FTR.load.ISO.env(region="NEPOOL")
  auction <- parse_auctionName("U13", region="NEPOOL")

  path <- Path(source.ptid = 4000,
               sink.ptid   = 4007,
               bucket      = "PEAK",
               auction     = auction,
               strategy    = "spec")

  # construct a path with a buddy node 
  path <- Path(source.ptid = 1691,
               sink.ptid   = 4006,
               bucket      = "PEAK",
               auction     = parse_auctionName("F12-1Y", region="NEPOOL"),
               source.ptid.buddy = 40327,
               source.ptid.buddy.startDt = as.POSIXct("2012-06-01 01:00:00"))



  # a few NYPP paths
  FTR.load.ISO.env(region="NYPP")
  auction <- parse_auctionName("K13-6M-R5", region="NYPP")
  paths <- list(Path(source.ptid = 61752,
                     sink.ptid   = 61757,
                     bucket      = "FLAT",
                     auction     = auction),
                Path(source.ptid = 61752,
                     sink.ptid   = 61761,
                     bucket      = "FLAT",
                     auction     = auction),
                Path(source.ptid = 61757,
                     sink.ptid   = 61761,
                     bucket      = "FLAT",
                     auction     = auction))

  # there is a nice as.data.frame method 
  do.call(rbind, lapply(paths, as.data.frame))


}
