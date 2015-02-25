\name{FTR.AuctionTerm}
\alias{FTR.AuctionTerm}
\title{Return an abreviated string for auction term, e.g. F07 or F07-1Y,
or vice-versa.}
\description{
Given a vector of start dates and end dates, return a vector of
characters which indicates the auction term.  Also if an auction name is
passed in, it returns the start, end date of the auction. It's mostly a utility
function used for reporting. 
}
\usage{
FTR.AuctionTerm(start.dts, end.dts, auction=NULL, region = NULL)
}
\arguments{
  \item{start.dts}{is a vector of R Dates, beginning of the month }
  \item{end.dts}{is a vector of R Dates, beginning(or ending) of the month }
  \item{auction}{a vector of auction strings.}
  \item{region}{not implemented yet}
}
\details{
}
\value{
Returns a vector of characters ordered monthly auctions first, then 6
months ones, then 1 year auctions.  Six-month auctions are shown with
starting month and "-6M" appended, e.g. "K07-6M".  The one-year auctions
have the string "-1Y" appended to the starting month, e.g. "F08-1Y". 
}
\references{}
\author{Adrian Dragulescu}
\note{}
\seealso{ \code{\link{FTR.AuctionTermLevels}} }
\examples{

start.dts = as.Date(c("2008-01-01", "2008-01-01"))
end.dts   = as.Date(c("2008-01-01", "2008-12-01"))

FTR.AuctionTerm(start.dts, end.dts)
#[1] "F08"    "F08-1Y"


# make sure you name your input
FTR.AuctionTerm(auction=c("F07", "G08-6M", "H08-1Y"))

#    start.dt     end.dt
#1 2007-01-01 2007-01-31
#2 2008-02-01 2008-07-31
#3 2008-03-01 2009-02-28




}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ }
\keyword{ }% __ONLY ONE__ keyword per line
