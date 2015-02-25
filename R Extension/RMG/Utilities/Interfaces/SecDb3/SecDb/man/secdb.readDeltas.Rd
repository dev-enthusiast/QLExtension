\name{secdb.readDeltas}
\alias{secdb.readDeltas}
\title{Returning a list deltas for a portfolio/book by commod curve}
\description{
Returns a list of commod curves and their respective deltas for a given portfolio or book.
}
\usage{
secdb.readDeltas( asOfDate, portfolioBook )
}
\arguments{
  \item{asOfDate}{the date to return deltas for}
  \item{portfolioBook}{a portfolio or book to query on}
}
\value{Returns a list of commod curves and their respective deltas.}
\examples{
library(SecDb)

secdb.readDeltas("2008-01-09", "JAMOPT")
## $`Commod PWJ 2x16H WEST Physical`
## NULL data frame with 0 rows
## 
## $`Commod PWJ 5x16 WEST PHYSICAL`
## date    value
## 1 2008-01-01 155.1528
## 2 2008-02-01 311.0908
## 
## $`Commod PWJ 7x8 WEST Physical`
## NULL data frame with 0 rows
## 
## $`Commod PWX 2x16 Physical`
## NULL data frame with 0 rows
## 
## $`Commod PWX 5x16 PHYSICAL`
## date      value
## 1  2008-01-01 -35931.550
## 2  2008-02-01 -23624.517
## 3  2008-03-01  -1278.380
## 4  2008-04-01  21323.687
## 5  2008-05-01  24568.335
## 6  2008-06-01  -1391.076
## 7  2008-07-01 -94918.271
## 8  2008-08-01 -87841.354
## 9  2008-09-01  -1681.916
## 10 2008-10-01  -8186.026
## 11 2008-11-01 -15654.128
## 12 2008-12-01 -36228.514
## 
## $`Commod PWX 7x8 Physical`
## NULL data frame with 0 rows

}

