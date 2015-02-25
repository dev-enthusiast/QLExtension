\name{secdb.getCurrentMarks}
\alias{secdb.getCurrentMarks}
\title{Read current commod curve prices from SecDb.}
\description{
  Read the current SecDb prices for a given curve name and contract
  dates. If the arguments are vectors, all possible combinations are returned.
}
\usage{
  secdb.getCurrentMarks( curveName, contractDate, expand=FALSE )
}
\arguments{
  \item{curveName}{an R character vector of valid curve names,
  (e.g. "Commod PWX 5x16 Physical").}
  \item{contractDate}{an vector R Dates, each element being the
  beginning of the month for the contract requested.}
  \item{expand}{A boolean indicating if an expand.grid should be run
  on the first two arguments or not.}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# you may need to change the dates below ... 
curveName <- "Commod PWX 5x16 Physical"
contractDate <- seq(as.Date("2010-01-01"), by="1 month", length.out=12)

data <- secdb.getCurrentMarks( curveName, contractDate )
head(data)
 
  
}
