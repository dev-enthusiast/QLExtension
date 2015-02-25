\name{makePMview}
\alias{makePMview}
\title{Mark a node relative to Hub using PM tools.  For NEPOOL only.}
\description{
  Mark a node relative to Hub using PM tools.  The congestion component
  for the PEAK, 2X16H, 7X8 buckets is estimated using the method from
  the FTR package.  The loss component is calculated by extending
  forward periodically the last 12 months of realized losses.

  The return value can then be used with the function
  \code{constructNodeMark} to calculate the full price of the ptid.
}
\usage{
makePMview(ptid)
}
\arguments{
  \item{ptid}{ A ptid in NEPOOL.}
}
\value{Returns a dataframe with forecasted values for the congestion and
  losses (in percent) for the buckets "5X16", "OFFPEAK", "2X16H", "7X8".} 
\seealso{\code{\link{constructNodeMark}}}
\examples{

# to get the current PM view on NH
pmView <- makePMview(4002)
head(pmView)
#        month bucket    value  component source
# 1 2009-07-01   5X16 -0.30920 congestion     PM
# 2 2009-08-01   5X16 -0.57980 congestion     PM
# 3 2009-09-01   5X16 -0.66315 congestion     PM
# 4 2009-10-01   5X16 -0.52615 congestion     PM
# 5 2009-11-01   5X16 -0.74295 congestion     PM
# 6 2009-12-01   5X16 -0.65435 congestion     PM
# >

# construct the forward mark
asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-1b"))
startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
endDate   <- as.Date("2010-12-01")   # mark up to here   
pmMark <- constructNodeMark(pmView, asOfDate, startDate, endDate)
head(pmMark)

#         month bucket component       value source
# 1  2009-07-01  2X16H     loss  -0.01369873 pmView
# 3  2009-07-01   5X16     loss  -0.01339212 pmView
# 6  2009-07-01    7X8     loss  -0.01371556 pmView
# 8  2009-08-01  2X16H     loss  -0.02004273 pmView
# 9  2009-08-01   5X16     loss  -0.02027410 pmView
# 12 2009-08-01    7X8     loss  -0.02019972 pmView
# >


}
