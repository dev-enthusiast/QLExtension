\name{ntplot.estStrip}
\alias{ntplot.estStrip}
\title{Calculate historical values of a strip of contracts}
\description{
  See the eponymous function in NTPLOT.  The R version allows for a vector
  input for the symbol. 
}
\usage{
ntplot.estStrip(symbol, startDate, endDate)
}
\arguments{
  \item{symbol}{A tsdb symbol name, or a vector of tsdb symbol names.}
  \item{startDate}{An R Date variable for the start contract date.}
  \item{endDate}{An R Date variable for the end contract date}
}
\value{Returns a dataframe. The columns date and value are the
  historical date and the price requested.} 
\seealso{\code{\link{ntplot.bucketizeByMonth}}}
\examples{

# you want to see how Cal 11 has behaved historically
startDate  <- as.Date("2011-01-01")
endDate    <- as.Date("2011-12-01")
symbol     <- c("prc_pwx_5x16_phys", "prc_ng_exchange")

res <- ntplot.estStrip(symbol, startDate, endDate)

head(res)



}
