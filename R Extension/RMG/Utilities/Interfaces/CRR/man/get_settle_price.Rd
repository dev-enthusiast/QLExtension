\name{get_settle_price}
\alias{get_settle_price}
\alias{get_settle_price_hourly}
\title{Calculate the settle price for a list of Paths}
\description{Calculate the settle price for a list of Paths from the
  same region but not necessarily the same auction.}
\usage{
  get_settle_price( paths )

  get_settle_price_hourly( paths )
}
\arguments{
  \item{paths}{A list of \code{\link{Path}} objects or a single
    \code{Path}.}  
}
\value{
  
  Function \code{get_settle_price} returns a list with two elements.
  First element is contains the settle prices, second element is the
  hours for the path.  The calculation reads the hourly DA congestion
  prices from tsdb for the given bucket and takes the difference between
  the sink and source prices.

  Function \code{get_settle_price_hourly} returns a list of hourly zoo
  objects with the hourly settle price
}
\details{All paths need to be in the same region.}
\author{Adrian Dragulescu}
\note{}
\seealso{\code{\link{Path}}}
\examples{

  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("U13", region="NEPOOL")
  path <- Path(source.ptid = 4000,
               sink.ptid   = 4007,
               bucket      = "PEAK",
               auction     = auction)

  sph <- get_settle_price_hourly( path )


  rLog("Three NYPP paths")
  load_ISO_environment(region="NYPP")
  auction <- parse_auctionName("K13-6M", region="NYPP")
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
  sp <- get_settle_price( paths )



}
