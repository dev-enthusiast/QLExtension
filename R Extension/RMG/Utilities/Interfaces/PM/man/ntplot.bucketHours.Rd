\name{ntplot.bucketHours}
\alias{ntplot.bucketHours}
\title{Return the number of bucket hours per time period ( Day or Month ).}
\description{
  Return the number of bucket hours in a period.  stat=Mean will return
average number of hours in period relative to 7x24.  IncludeZeros will
(when Period = "Day" ) return zeros when appropriate, otherwise those
days will be skipped.  See the eponymous NTPLOT function. 

}
\usage{
ntplot.bucketHours(bucket, region="NEPOOL", period="Month", stat="Sum",
  includeZeros=TRUE, startDate, endDate)
}
\arguments{
  \item{bucket}{A valid bucket, or a vector of character buckets.}
  \item{startDate}{An R Date variable for the beginning of the
    historical range.  Optional.}
  \item{endDate}{An R Date variable for the end of historical range.
    Optional.}
  \item{region}{A character string giving the SecDb region (FreqDefs).}
  \item{period}{A character string specifying if bucketing should be
    done by "Month" or "Day".}
  \item{stat}{A character string either "Sum" or "Mean".}
  \item{includeZeros}{Include days when #hours is zero.}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

startDate   <- as.Date("2009-01-01")
endDate     <- as.Date("2010-12-01")
bucket      <- c("Offpeak", "Peak")

# get the number of hours by bucket
res <- ntplot.bucketHours(bucket, startDate, endDate,
  region="PJM", period="Month")

head(res)
#      bucket       date value
# 469 Offpeak 2009-01-01   408
# 470 Offpeak 2009-02-01   352
# 471 Offpeak 2009-03-01   391
# 472 Offpeak 2009-04-01   368
# 473 Offpeak 2009-05-01   424
# 474 Offpeak 2009-06-01   368

# or get them all
res <- ntplot.bucketHours(bucket, region="PJM", period="Month")

  
}
