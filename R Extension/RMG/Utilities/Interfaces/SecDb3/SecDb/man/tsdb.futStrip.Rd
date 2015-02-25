\name{tsdb.futStrip}
\alias{tsdb.futStrip}
\title{Read a series of contract months, for several curves from tsdb.}
\description{
  Given a historical range, a set of commod curve names, and a set of
  contract dates, return the tsdb price info as a flat 2D table.  The
  results can then be easily processed with the reshape package. 
}
\usage{
tsdb.futStrip( curveNames, histStart.dt, histEnd.dt, contract.dts )
}
\arguments{
  \item{curveNames}{a character string giving the names of the curves to read}
  \item{histStart.dt}{an R Date variable for the beginning of the
    historical range}
  \item{histEnd.dt}{an R Date variable for the end of historical range}
  \item{contract.dts}{an R Date vector for the contract months that you
    want}
}
\value{Returns a dataframe with columns: curve.name, contract.dt,
  pricing.dt, value.  It can be then easily reshaped using cast.}
\seealso{
  \code{\link{tsdb.readCurve}}
}
\examples{

  curveNames  <- c("COMMOD NG EXCHANGE", "COMMOD PWJ 5X16 WEST PHYSICAL")
  histStart.dt <- as.Date("2006-01-01")
  histEnd.dt   <- as.Date("2008-01-01") 
  contract.dts <- seq(as.Date("2007-01-01"), by="month", length.out=12)
  
  # get the strip 
  curveData = tsdb.futStrip( curveNames, histStart.dt, histEnd.dt, contract.dts )
  head(curveData)
  
}
\note{}