\name{ntplot.rtrhq}
\alias{ntplot.rtrhq}
\title{Extract hourly quantities from secdb risk server.}
\description{
  Extract hourly quantities from secdb risk server.  For a detailed
  description see
  \url{http://wiki.constellation.com/confluence/display/CCG/RTRHQ}.
}
\usage{
ntplot.rtrhq(portfolio, filter="Fcst", datasource="Archive")
}
\arguments{
  \item{portfolio}{A vector of secdb portfolios or books.}
  \item{filter}{A vector of filters, see the Wiki.}
  \item{datasource}{A valid datasource, see the Wiki.}
}
\value{Returns a dataframe for all the combinations of portfolio and
  filter.}
\seealso{\code{\link{ntplot.bucketizeByDay}}}
\examples{

  portfolio  <- "nepool load portfolio"
  filter     <- c("Region=NEPOOL;Zone=CT", "Region=NEPOOL;Zone=NEMA")
  datasource <- "Archive"

  res <- ntplot.rtrhq(portfolio, filter)
  

  head(res)


}