\name{searchEvents}
\alias{searchEvents}
\title{Search ISO expanations for price separation events.}
\description{
  Grab the expanation the ISO provides in the weekly commentary and
  search it. 
}
\usage{
searchEvents(token, startDt=as.Date("2005-01-01"),
  endDt=Sys.Date(), event="Day-Ahead Price Separation", days=NULL, read=TRUE)
}
\arguments{
  \item{asOfDate}{a character string to search for, e.g. "CT" or "353".}
  \item{startDate}{an R Date variable for the beginning of the
    historical range}
  \item{endDate}{an R Date variable for the end of historical range}
  \item{event}{a character string.  Can be one of "Day-Ahead Price
    Separation", "Real-Time Price Separation", "Real-Time Price Corrections".}
  \item{days}{a vector of specific R dates that you want to
    investigate.  For example if you know something weird happened on 2008-06-10.}
  \item{read}{boolean, if you want the output to be text wrapped.}
}
\value{Returns a vector of characters.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{


searchEvents("353")



}
