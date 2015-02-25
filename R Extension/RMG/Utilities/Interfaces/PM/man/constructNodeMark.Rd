\name{constructNodeMark}
\alias{constructNodeMark}
\alias{plotMarks}
\title{Construct the mark for a node from a view on congestion and losses.
  Plot marks.}
\description{
This function was developed to work with NEPOOL and construct the   
forward marks given a "view" on congestion and losses.    
}
\usage{
constructNodeMark(X, asOfDate=Sys.Date(), startDate,
  endDate=as.Date("2017-12-01"))

plotMarks(allMarks, nodeName, buckets=c("5X16", "2X16H", "7X8"),
  save=FALSE, fwd=12, ...)

}
\arguments{
  \item{X}{A data.frame with at least columns c("month", "bucket",
    "component", "value").  The \code{component} column can be one of
    "congestion", "loss%", "hubMark", "nodeMark", "zoneMark", or other.
    This is useful to keep track and aggregate the different price
    components.  If \code{X} does not have \code{component=="hubMark"}
    the code grabs the forward marks for the Hub, and appends them to the
    original input data.frame \code{X}.  If already there, it uses the
    values available.  This speeds up the calculation.

    The input data.frame \code{X} can be for the next 12 months only.
    For all missing months the code will extend the mark periodically in
    the future all the way to \code{endDate}. 
  }
  \item{startDate}{Never mark the cash month.  Always start with
    prompt month.  According to Kieran Coe SecDb does not like it if you mark
    the cash month, but I have no direct proof of that.}
  \item{endDate}{Somewhere safe in the future.}
  \item{fwd}{How many forward months to show in the plot.  Set to zero
    if you only want historical marks.}
}
\value{Returns a dataframe with all the components.  For later use, you
  may want to add the source of your mark.  The \code{source} column allows
  you to keep track of of your data source, say "Historical" or
  "WeatherNorm", etc.

The function \code{plotMarks} will produce three different files in the current
directory if \code{save=TRUE} showing the historical and forward
congestion, loss component and LMP for the three buckets. 
} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

marksForDate <- Sys.Date()
startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
endDate   <- as.Date("2017-12-01")   # mark up to here   

# construct your X from somewhere (WeatherNorm may be one option)
# > head(X, 6)
#        month       value bucket      source  component
# 1 2009-02-01 -1.53838010   5X16 WeatherNorm congestion
# 2 2009-02-01 -0.77648361  2X16H WeatherNorm congestion
# 3 2009-02-01  0.01881509    7X8 WeatherNorm congestion
# 4 2009-02-01 -0.04505083   5X16 WeatherNorm      loss\%
# 5 2009-02-01 -0.04751549  2X16H WeatherNorm      loss\%
# 6 2009-02-01 -0.04717133    7X8 WeatherNorm      loss\%
# >

modelNodeMark <- constructNodeMark(ptid, X, marksForDate, startDate, endDate)
modelNodeMark$source <- "WeatherNorm"  # better to keep track later 



}
