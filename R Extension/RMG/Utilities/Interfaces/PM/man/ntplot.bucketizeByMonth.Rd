\name{ntplot.bucketizeByMonth}
\alias{ntplot.bucketizeByMonth}
\title{Extract monthly average curve from a realtime symbol for
  specified bucket.}
\description{
Extract monthly average curve from a realtime symbol for specified
bucket.  SamplingPeriod typically is "3600", but can be "900"
(15-min) or "300" (5-min) for example.  See the eponymous function in
NTPLOT.  The R version allows for vector inputs. 
}
\usage{
ntplot.bucketizeByMonth(symbol, bucket, startDate, endDate,
  region="NEPOOL", stat="Mean")
}
\arguments{
  \item{symbol}{A tsdb symbol name, or a vector of tsdb symbol names.}
  \item{bucket}{A valid bucket, or a vector of character buckets.}
  \item{startDate}{An R Date variable for the beginning of the
    historical range.  Optional.}
  \item{endDate}{An R Date variable for the end of historical range.
    Optional.}
  \item{region}{A character string giving the SecDb region (FreqDefs).}
  \item{stat}{A string giving the statistic to calculate. One of Mean,
    Sum, Min, Max.}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
\seealso{\code{\link{ntplot.bucketizeByDay}}}
\examples{

startDate  <- as.Date("2008-01-01")
endDate    <- as.Date("2008-12-01")
bucket     <- c("Offpeak", "Peak")
symbol     <- c("nepool_smd_da_4000_lmp")

# get the monthly price by bucket
res <- ntplot.bucketizeByMonth(symbol, bucket, startDate, endDate,
  region="NEPOOL")

head(res)
#                    symbol  bucket      month    value
# 65 nepool_smd_da_4000_lmp Offpeak 2008-01-01 73.36962
# 66 nepool_smd_da_4000_lmp Offpeak 2008-02-01 66.55894
# 67 nepool_smd_da_4000_lmp Offpeak 2008-03-01 72.27516
# 68 nepool_smd_da_4000_lmp Offpeak 2008-04-01 79.82910
# 69 nepool_smd_da_4000_lmp Offpeak 2008-05-01 86.82799
# 70 nepool_smd_da_4000_lmp Offpeak 2008-06-01 93.85328

# get the gas burn at My7
res <- ntplot.bucketizeByMonth("BG_Mystic7_Gen_NG_MMBTu", "Flat",
  startDate, endDate, region="NEPOOL", stat="Sum")


}