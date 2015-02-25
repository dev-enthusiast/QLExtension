\name{tsdbExists}
\alias{tsdbExists}
\title{Check if a tsdb symbols exists or not.}
\description{
  Check if a tsdb symbols exists or not.
}
\usage{
  tsdbExists(symbol)
}
\arguments{
  \item{symbol}{A character string representing the name of the symbol
    to check.}
}
\details{}
\value{Returns \code{TRUE} if the symbol exists and \code{FALSE} if it
  doesn't.} 
\seealso{\code{\link{tsdbCreate}}}
\examples{

  tsdbExists("nepool_load_hist")  # TRUE

  tsdbExists("goo goo gaa gaa")   # FALSE
}
%\note{}