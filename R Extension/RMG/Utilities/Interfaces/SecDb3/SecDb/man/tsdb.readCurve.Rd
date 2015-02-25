\name{tsdb.readCurve}
\alias{tsdb.readCurve}
\title{Read a curve from the TsDb}
\description{
Read both daily and hourly curves from TsDb
}
\usage{
tsdb.readCurve( curveName, startDate, endDate )
}
\arguments{
  \item{curveName}{a character string giving the name of the curve to read}
  \item{startDate}{an R Datetime variable of class POSIXlt or POSIXct
    that will be the lower bounds on the curve data.  If the startDate
    is of Date class, it will be converted to POSIXct assuming an
    EST5EDT timezone. }
  \item{curveName}{an R Datetime variable of class POSIXlt or POSIXct
    that will be the upper bounds on the curve data. If the endDate
    is of Date class, it will be converted to POSIXct assuming an
    EST5EDT timezone with the time part 23:59:00.}
}
\value{Returns a list of date/value parings. The dates returned may actually
be R times or R dates based on the type of curve queried (time or date
based).}
\examples{
  secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")
  
  startDate = Sys.Date() - 50
  endDate = Sys.Date() - 1
  
  ##A daily curve (SecDb type: Curve)
  curveData = tsdb.readCurve( curveName="PRC_NG_Exchange_0706", startDate=startDate, endDate=endDate )
  print(head(curveData))
  
  ##An hourly curve (SecDb type: TCurve)
  curveData2 = tsdb.readCurve( "nepool_load_hist", startDate, endDate )
  print(head(curveData2))

  # An hourly curve
  day = as.Date("2008-03-19")
  res = tsdb.readCurve("NYPP_DAM_ZONE_LBM_61757", day, day)
  (res[1:2,2] == c(70.28, 70.36))   # TSDB has c(70.28, 70.36)

}
\note{Based on the type of curve returned, the curve values will be paired with
either an R date object or an R time object.  Make sure your code checks 
accordingly. 
}
