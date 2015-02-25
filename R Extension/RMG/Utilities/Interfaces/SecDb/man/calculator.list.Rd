\name{calculator.list}
\alias{calculator.list}
\title{Return all the calculators under a given group and trader.}
\description{
  Return all the calculators under a given group and trader.
}
\usage{
  calculator.list(group, trader)
}
\arguments{
  \item{group}{a character string giving the name of the SecDB
    calculator group}
  \item{trader}{a character string, the E number, or old SecDB user name}
}
\value{Returns a vector of strings with the calculator names.}
\seealso{
  \code{\link{calculator.get.deltas}}
}
\examples{

  # You can get all the calculators 
  cnames <- calculator.list("CPS Trading Group", "E13579")
  
}
\note{}