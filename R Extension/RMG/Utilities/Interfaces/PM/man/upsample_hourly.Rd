\name{upsample_hourly}
\alias{upsample_hourly}
\title{Upsample a timeseries to hourly frequency}
\description{
If you have a daily gas or oil curve and you want to expand it to hourly
frequency.  This is useful if you want to calculate an hourly heat rate
for example. 
}
\usage{
  upsample_hourly(X, rule=c("gas", "normal"))
}
\arguments{
   \item{X}{ A zoo time series, with one or more columns and a Date
     index}
   \item{rule}{ A string indicating a possible rule.  The "gas" rule
     assumes that prices for the day are from 10AM to next day's 10AM.
     Rule "normal" is for a midnight to midnight expansion. 
   }
}
\details{
  
}
\value{
  An hourly zoo timeseries. 
}
\references{ }
\author{Adrian Dragulescu}
\examples{
  require(CEGbase)
  require(zoo)
  require(reshape)
  require(SecDb)
  
  
  startDt  <- as.POSIXct("2013-07-01")
  endDt    <- as.POSIXct("2013-07-14")
  
  # get the gas, gen out
  GG <- FTR:::FTR.load.tsdb.symbols(c("gasdailymean_algcg",
    "gasdailymean_tetm3"), startDt, endDt)
  colnames(GG) <- c("algcg", "tetm3")
  GG
  
  HH <- upsample_hourly(GG, rule="gas")
  head(HH, 36)



}
\keyword{}
