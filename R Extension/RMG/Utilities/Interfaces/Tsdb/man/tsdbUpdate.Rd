\name{tsdbUpdate}
\alias{tsdbUpdate}
\title{Add/overwrite points to a tsdb symbols.}
\description{
  Add/overwrite points to a tsdb symbols.
}
\usage{
  tsdbUpdate(symbol, x)
}
\arguments{
  \item{symbol}{The name of an existing tsdb symbol.}
  \item{x}{A data.frame with at least two columns \code{time} and
	\code{value}.}
}
\details{
  Inserting a point for DST hours appears to work correctly.

  Current implementation is not the most efficient.  Please report
  issues. 
}
\value{Returns \code{TRUE} for a successful update.} 
\seealso{\code{\link{tsdbCreate}}}
\examples{

symbol <- "pm_nepool_second"
times  <- seq(as.POSIXct("2008-01-01 01:00:00"), by="1 hour", length.out=10)
values <- 200 + rnorm(length(times), 0, 10)
x <- data.frame(time=times, value=values)

tsdbUpdate(symbol, x)


}
%\note{}