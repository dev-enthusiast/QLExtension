\name{secdb.getImpliedCongestionMarks}
\alias{secdb.getImpliedCongestionMarks}
\title{Calculate implied congestion marks from SecDb marks.}
\description{
  Read the elec prices for a given location, bucket, service type and
  substract the loss component to get the implied congestion.  The loss
  component is calculated from the last 12 months historical loss
  factors multiplied by the forward hub price. 
}
\usage{
secdb.getImpliedCongestionMarks(asOfDate=Sys.Date(),
  startDate=Sys.Date(), endDate=as.Date("2017-12-01"), region="NEPOOL",
  delivPt=c("CT", "SEMA", "RI", "Maine", "VT", "WMA", "NH", "Nema", "Hub"),
  bucket=c("PEAK", "OFFPEAK"), ptid=NA)
}
\arguments{
  \item{asOfDate}{an R Date}
  \item{startDate}{an R Date variable for the beginning of the
    historical range}
  \item{endDate}{an R Date variable for the end of historical range}
  \item{region}{a character string giving the SecDb region.  Only NEPOOL
  works now.}
  \item{delivPt}{a character vector with SecDb location names.}
  \item{bucket}{a character vector with SecDb bucket names.}
  \item{ptid}{a vector of ptids corresponding to the delivPt.  It needs
    names with delivPt.}
}
\value{Returns a dataframe.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# you may need to change the dates below ... 
asOfDate    <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
startDate   <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
endDate     <- as.Date("2012-12-01")

# get the implied congestion  
CC <- secdb.getImpliedCongestionMarks(asOfDate, startDate=startDate,
  endDate=endDate, delivPt=c("SEMA", "Hub"), ptid=structure(c(4006, 4000),
  names=c("SEMA", "Hub")))
head(CC)
#       month  bucket delivPt  basisMark   lossMark  congMark
#1 2010-01-01 OFFPEAK     HUB 53.2505660  0.0000000 0.0000000
#2 2010-01-01 OFFPEAK    SEMA  0.2494906 -0.1005189 0.3500095
#3 2010-01-01    PEAK     HUB 64.9500000  0.0000000 0.0000000
#4 2010-01-01    PEAK    SEMA  0.5000000 -0.1188539 0.6188539
#5 2010-02-01 OFFPEAK     HUB 53.2517273  0.0000000 0.0000000
#6 2010-02-01 OFFPEAK    SEMA  0.2494545 -0.1005211 0.3499757


  
}
