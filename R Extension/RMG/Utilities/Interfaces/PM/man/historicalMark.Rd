\name{historicalMark}
\alias{historicalMark}
\title{Calculate historical (settled) nodal marks.}
\description{
  A convenience function to return different components of a nodal
  price by month.  You can remove some specific days from history when
  calculating the monthly averages.
  
  Data is pulled from ntplot.  Only NEPOOL and NYPP is implemented.
  Only NEPOOL has \code{skipDates} flexibility. 
}
\usage{
historicalMark(ptid, buckets=c("5X16", "2X16H", "7X8"), region="NEPOOL",
  MAP=NULL, component=c("loss\%", "congestion", "hubMark", "nodeMark"),
  skipDates=NULL, start.dt=as.POSIXct("2003-01-01"), end.dt=Sys.time())
}
\arguments{
  \item{ptid}{A numeric ptid.}
  \item{buckets}{ A vector of buckets. Can be non-overlapping.}
  \item{region}{ A string with region's name.}
  \item{MAP}{ The zonal map.  For example \code{\link{FTR.load.ISO.env}}
    will load the map, among others.  Not needed if region is NEPOOL.}
  \item{component}{ A vector of strings with different components you
    want.  Only "loss\%", "loss\$", "congestion", "hubMark", "nodeMark" are
    supported now.}
  \item{skipDates}{A vector of R Dates to be removed from the history.
    You can cherry pick days.}
  \item{start.dt}{A POSIXct to specify starting of historical period.}
  \item{end.dt}{A POSIXct to specify end of historical period.}  
}

\value{
  Returns a data.frame.  See the examples.
}

\examples{

# calculate Maine loss factors excluding the first day of the month 
# (kinda silly) in 2003 and 2004
res <- historicalMark(4001, component="loss\%",
  skipDates=seq(as.Date("2003-01-01"), as.Date("2006-12-31"), by="1 month"),
  bucket="7X8")

head(res)
#  bucket      month       value component
#1    7X8 2002-12-01 -0.03590127     loss%
#2    7X8 2003-01-01 -0.03597328     loss%
#3    7X8 2003-02-01 -0.03597816     loss%
#4    7X8 2003-03-01 -0.05237064     loss%
#5    7X8 2003-04-01 -0.08343778     loss%
#6    7X8 2003-05-01 -0.07458624     loss%
  
}