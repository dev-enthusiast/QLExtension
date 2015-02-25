\name{secdb.getFXFwdCurve}
\alias{secdb.getFXFwdCurve}
\title{Get the forward FX rate between two currencies as of a given date.}
\description{
Pull the forward FX rate between two currencies as of a given date.  The
results may need interpolation.
}
\usage{
fx <- secdb.getFXFwdCurve(CurrencyFrom, CurrencyTo, asOfDate)
}
\arguments{
  \item{CurrencyFrom}{a string, eg. "USD"}
  \item{CurrencyTo}{a string, eg. "EUR"}
  \item{asOfDate}{an R date}
}
\value{Returns a dataframe with fx values. }
\seealso{
}
\examples{
library(SecDb)

# get the USD to EUR exchange rate
(fx <- secdb.getFXFwdCurve("USD", "EUR", as.Date("2007-12-06")))

}
\details{The function can be slow, and should not be used for
  multiple asOfDates.  If you want multiple asOfDates, we have another
  utility that extracts this data from the CPSPROD database.  Please
  contact somebody from RMG RA for details.  
}
