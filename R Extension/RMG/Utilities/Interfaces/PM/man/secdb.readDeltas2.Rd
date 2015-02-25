\name{secdb.readDeltas2}
\alias{secdb.readDeltas2}
\title{Get deltas of a portfolio/book from SecDb}
\description{
  Get deltas of a portfolio/book from SecDb delta archives.

  This function deprecates the previous secdb.readDeltas from the SecDb
  package.  That function used to read from the dgc archives and they
  are no longer maintained the same way after exgen integration.
  secdb.readDeltas will return empty for a date far enough in the past.
}
\usage{
  secdb.readDeltas2( asOfDate, portfolioBook, discounted=TRUE,
    aggreagate=TRUE, energyOnly=FALSE )
}
\arguments{
  \item{asOfDate}{the date to return deltas for}
  \item{portfolioBook}{a vector of portfolios or books to query on.  Can
    be any combination of SecDb portfolios and books.}
  \item{discounted}{a boolean, if \code{TRUE} return discounted deltas.}
  \item{aggregate}{a boolean, if \code{TRUE} sum the positions by
    \code{portfolioBook}, otherwise return a list for each of the
    \code{portfolioBook}}
  \item{energyOnly}{a boolean, if \code{TRUE} try and remove all the
    non-energy curves, e.g. ISOFee, VLRCST, UCAP, etc.}
}
\value{Returns a data.frame with columnames c("curve", "month", "value")
  if \code{aggregate==TRUE}, or a list of data.frames with columnames
  c("portfolioBook", "curve", "month", "value").
}
\examples{
library(SecDb)

  asOfDate <- as.Date("2012-12-31")
  book <- "NEPPWRBS"
  res  <- secdb.readDeltas2(asOfDate, book)
  head(res)
#                     curve      month    value
#1 COMMOD PWX 2X16 PHYSICAL 2013-01-01 47475.48
#2 COMMOD PWX 2X16 PHYSICAL 2013-02-01 44265.31
#3 COMMOD PWX 2X16 PHYSICAL 2013-03-01 35171.44
#4 COMMOD PWX 2X16 PHYSICAL 2013-04-01 28130.94
#5 COMMOD PWX 2X16 PHYSICAL 2013-05-01 13810.53
#6 COMMOD PWX 2X16 PHYSICAL 2013-06-01 13744.89

# You can then classifyCurve ...

}

