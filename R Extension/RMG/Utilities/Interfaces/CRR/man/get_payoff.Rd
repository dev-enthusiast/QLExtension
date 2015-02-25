\name{get_payoff}
\alias{get_payoff}
\title{Calculate the payoff on a path}
\description{Calculate the payoff on a path.}
\usage{
  get_payoff( paths,
    period=c("FROM_START", "CURRENT_MONTH", "NEXT_DAY"),
    asOfDate=Sys.Date() )
}
\arguments{
  \item{paths}{A list of \code{\link{Path}} objects or a single
    \code{Path}.}
  
  \item{period}{A string that specifies how to track the performance.
    The default choice is \code{FROM_START} calculates from the start of
    the auction.  Other valid choices are \code{CURRENT_MONTH} to
    calculate PnL only in the current month, or \code{NEXT_DAY} using
    the last available DA data.  If the path has completely settled,
    \code{CURRENT_MONTH} and \code{NEXT_DAY} will return \code{NA}.}

  \item{asOfDate}{An R Date.  Both  \code{CURRENT_MONTH} and
    \code{NEXT_DAY} are calculated relative to \code{asOfDate}.}
}

\value{
  A list of numerical vectors corresponding to each \code{period}
  requested.
}
\details{All paths need to be in the same region, but different auctions
  are allowed.}
\author{Adrian Dragulescu}
\note{}
\seealso{\code{\link{Path}}}
\examples{

  rLog("A NEPOOL path")
  load_ISO_environment(region="NEPOOL")
  auction <- parse_auctionName("M14", region="NEPOOL")

  path <- Path(source.ptid = 4000,
               sink.ptid   = 4008,
               bucket      = "PEAK",
               auction     = auction)
  
  res <- get_payoff( path )  
# > res
# $FROM_START
# [1] -426.64

# $CURRENT_MONTH
# [1] NA

# $NEXT_DAY
# [1] NA

  rLog("Three NEPOOL paths")
  paths <- list(Path(source.ptid = 4000,
                     sink.ptid   = 4008,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("F14", region="NEPOOL")),
                Path(source.ptid = 4000,
                     sink.ptid   = 4008,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("G14", region="NEPOOL")),
                Path(source.ptid = 4000,
                     sink.ptid   = 4006,
                     bucket      = "PEAK",
                     auction     = parse_auctionName("F14", region="NEPOOL")))
  res <- get_payoff( paths )
# > res
# $FROM_START
# [1]  -46.73 -510.50  138.07

# $CURRENT_MONTH
# [1] NA NA NA

# $NEXT_DAY
# [1] NA NA NA




}
