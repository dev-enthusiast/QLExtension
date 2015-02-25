\name{calculator.get.deltas}
\alias{calculator.get.deltas}
\title{Return the deltas from a calculator.}
\description{
  Run a position report on a calculator and return the deltas as a data
  frame with columns curve.name, contract.dt, delta.
}
\usage{
  calculator.get.deltas(group, trader, calc.name)
}
\arguments{
  \item{group}{a character string giving the name of the SecDB
    calculator group}
  \item{trader}{a character string, the E number, or old SecDB user name}
  \item{calc.name}{a vector of strings with the names of calculators.}
}
\value{Returns a data frame with columns curve.name, contract.dt,
  delta.  For multiple calculators, an aggregation is perfomed before
  returning.}
\seealso{
  \code{\link{calculator.list}}
}
\examples{

  # You can get all the calculators 
  cnames <- calculator.list("CPS Trading Group", "E13579")
  calculator.get.deltas("CPS Trading Group", "E13579", cnames[1:2])

  # You can specify which one you want, note that the names are
  # different, no more "Calc" in front.
  cname <- "ELSO PJM WEST HEDGE JAN09-MAY09"
  calculator.get.deltas("CPS Trading Group", "E13579", cname)  
  
}
\note{}