\name{CEGterm-package}
\alias{CEGterm-package}
\docType{package}
\title{
A collection of functions for working with term quotes.
}
\description{
This package should contains basic functions for manipulating broker
quotes, and other date formatting. 
}
\details{
\tabular{ll}{
Package: \tab CEGterm\cr
Type: \tab Package\cr
Version: \tab 0.0.1\cr
Date: \tab 2009-02-26\cr
License: \tab CEG\cr
}

}
\author{
John Scillieri, Adrian Dragulescu.
}
\references{
}
\keyword{ package }
\examples{

# see all unit tests for the package:
runUnitTestsCEGterm()

# define a vector of terms
term <- c("SPR 09", "NQ 09", "BAL 09", "CAL 10", "CAL 11")

# pull the start and end date for these terms
aux <- data.frame(
  startDate = as.Date(sapply(term, termStartDate), origin="1970-01-01"),
  endDate   = as.Date(sapply(term, termEndDate), origin="1970-01-01"))

print(aux)
%         startDate    endDate
% SPR 09 2009-03-01 2009-04-30
% NQ 09  2009-07-01 2009-08-31
% BAL 09 2009-02-01 2009-12-31
% CAL 10 2010-01-01 2010-12-31
% CAL 11 2011-01-01 2011-12-31

# expand them to months.  
aux <- lapply(split(aux[,c("startDate", "endDate")], rownames(aux)),
  function(x){seq(x$startDate, x$endDate, by="1 month")})

# melt them for a data.frame
melt(aux)

}
