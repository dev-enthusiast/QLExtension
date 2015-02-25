\name{secdb.getDiscountFactor}
\alias{secdb.getDiscountFactor}
\title{Get the discount factor for a set of dates as of a given date.}
\description{
Pull the discount factor (LIBOR) for a given asOfDate, for a set of
dates you are interested in, for a given currency.  
}
\usage{
fx <- secdb.getDiscountFactor(asOfDate, dates, currency="USD")
}
\arguments{
  \item{asOfDate}{an R Date}
  \item{dates}{a vector of R Dates}
  \item{currency}{the currency you need, e.g. "USD", "EUR", etc.}
}
\value{Returns a dataframe with the DF values. }
\seealso{
}
\examples{
library(SecDb)

# get the discount factor for the USD 
asOfDate  <- as.Date("2008-04-01")
dates     <- seq(as.Date("2008-04-01"), by="month", length.out=12)
secdb.getDiscountFactor( asOfDate, dates, currency="EUR" )

}
\details{
}
