\name{ntplot.bucketizeByDay}
\alias{ntplot.bucketizeByDay}
\title{Extract daily average curve from a realtime symbol for
  specified bucket.}
\description{
Extract daily average curve from a realtime symbol for specified
bucket.  SamplingPeriod typically is "3600", but can be 900
(15-min) or 300 (5-min) for example.  See the eponymous function in
NTPLOT.  The R version allows for vector inputs. 
}
\usage{
ntplot.bucketizeByDay(symbol, bucket, startDate, endDate,
  region="NEPOOL", SamplingPeriod=3600, stat="Mean")
}
\arguments{
  \item{symbol}{A tsdb symbol name, or a vector of tsdb symbol names.}
  \item{bucket}{A valid bucket, or a vector of character buckets.}
  \item{startDate}{An R Date variable for the beginning of the
    historical range.  Optional.}
  \item{endDate}{An R Date variable for the end of historical range.
    Optional.}
  \item{region}{A character string giving the SecDb region (FreqDefs).}
  \item{SamplingPeriod}{A number giving the number of seconds.  Stick to
    3600.}
  \item{stat}{A character string specifying the statistic to calculate.
    You can choose between "mean", "sum", "min", "max".}
  
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
\seealso{\code{\link{ntplot.bucketizeByMonth}}}
\examples{

startDate  <- as.Date("2008-01-01")
endDate    <- as.Date("2008-12-01")
bucket     <- c("Offpeak", "Peak")
symbol     <- c("nepool_smd_da_4000_lmp", "nepool_smd_da_4002_lmp")

# bucketize by day
res <- ntplot.bucketizeByDay(symbol, bucket, startDate, endDate,
  region="NEPOOL", SamplingPeriod=3600, stat="Mean")

head(res)

#                      symbol  bucket        day     value
# 1946 nepool_smd_da_4000_lmp Offpeak 2008-01-01  58.68542
# 1947 nepool_smd_da_4000_lmp Offpeak 2008-01-02  75.52750
# 1948 nepool_smd_da_4000_lmp Offpeak 2008-01-03 115.66750
# 1949 nepool_smd_da_4000_lmp Offpeak 2008-01-04  98.05625
# 1950 nepool_smd_da_4000_lmp Offpeak 2008-01-05  70.99000
# 1951 nepool_smd_da_4000_lmp Offpeak 2008-01-06  59.59708


# You can also pass in expressions that will evaluate in ntplot
# for example: "nepool_smd_da_4002_lmp - nepool_smd_da_4000_lmp"
# if you want to calculate the daily spread by bucket. 

}
