\name{get_clearing_price}
\alias{get_clearing_price}
\title{Get the clearing price for a list of paths}
\description{Get the clearing price for a list of paths}
\usage{
  get_clearing_price( paths )
}
\arguments{
  \item{path}{A list of \code{\link{Path}} objects or a single
    \code{Path}.}
}
\value{
  Returns a vector of numerical values. 
}
\details{Internally it loads \code{CRR:::.load_CP_archive} and filters it.}
\author{Adrian Dragulescu}
\note{}
%\seealso{\code{\link{CRR_auction}}}
\examples{

  rLog("Three NYPP paths")
  load_ISO_environment(region="NYPP")
  auction <- parse_auctionName("K13-6M-R6", region="NYPP")
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
  cp <- get_clearing_price( paths )
  # [1] 0.970000 4.612056 3.642056



}
