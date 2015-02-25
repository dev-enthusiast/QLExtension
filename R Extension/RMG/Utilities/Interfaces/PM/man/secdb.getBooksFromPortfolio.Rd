\name{secdb.getBooksFromPortfolio}
\alias{secdb.getBooksFromPortfolio}
\title{Get the books for a given portfolio}
\description{
  Get the books for a given portfolio (SecDb only for now.) 
}
\usage{
secdb.getBooksFromPortfolio(portfolio, from="SecDb")
}
\arguments{
  \item{portfolio}{a SecDb portfolio name}
  \item{from}{a string indicating the source of the data.  For now only
    SecDb, although I have code for all the company.}
}
\value{Returns a vector of book names.}
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

  books <- secdb.getBooksFromPortfolio("Nepool Load Portfolio")
  
}
