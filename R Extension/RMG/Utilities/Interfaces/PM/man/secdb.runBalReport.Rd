\name{secdb.runBalReport}
\alias{secdb.runBalReport}
\title{Run the SecDb Bal report for a set of dates.}
\description{
  Run the SecDb Bal report for a set of dates.
}
\usage{
  secdb.runBalReport( days, bookName )
}
\arguments{
  \item{days}{a vector of R Dates}
  \item{bookName}{a character string giving the SecDb book or portfolio name}
}
\value{Returns a dataframe with the same info as the Bal report from Tap.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

days <- seq(as.Date("2011-07-28"), as.Date("2011-07-31"), by="1 day")
bookName <- "CONGIBT"

out <- secdb.runBalReport(days, bookName)
#      Period Yesterday     Prev.Mark Time    Commod Locations FX.Spot    Rates Vols Corrs Data.Fixs Qty.Fcst SubTotals     Rolls New.Trades Expires      Mismatch      PNL    Growth    Today
#1 2011-07-28  29071608  0.000000e+00    0 -21143.26  215892.1       0 -803.656    0     0  35711.92        0  229657.1 -1.392025          0       0  3.026798e-08 229655.7  98.72217 29301362
#2 2011-07-29  29301362 -5.960464e-08    0 -27805.03  292333.8       0 -980.840    0     0  11083.38        0  274631.3 -6.962073          0       0 -3.632158e-08 274624.3 100.72291 29576087
 


}