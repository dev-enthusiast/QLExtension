\name{FTR.bucketHours}
\alias{FTR.bucketHours}
\title{Return the number of hours by bucket by month for a NERC region.}
\description{
 Return the number of hours by bucket in a given by month for a NERC region.  
}
\usage{
FTR.bucketHours(region = "NEPOOL", start.dt=as.POSIXlt("2003-01-01 01:00:00"),
         end.dt=as.POSIXlt("2013-01-01 00:00:00"), buckets=NULL)
}
\arguments{
  \item{region}{Name of a NERC region name as defined in SecDB (see the
    COMMOD PWR XXXXX Physical objects.)}
  \item{start.dt}{A POSIXlt time.}
  \item{end.dt}{A POSIXlt time.}
  \item{buckets}{A vector of bucket names.  If NULL, will return
    c("flat", "peak", "offpeak", "2x16h", "7x8", "5x16")}
}
\details{The start and end datetimes need to be in hour ending format.}
\value{
  A data frame with columns c("contract.dt", "class.type", "hours").
  The "PEAK" bucket gets renamed to "ONPEAK" for NEPOOL's sake. 
}
\references{}
\author{Adrian Dragulescu}
\note{Requires SecDb package.}
\seealso{}
\examples{

  res <- FTR.bucketHours()
  head(res)
#  contract.dt class.type hours
#1  2003-01-01       FLAT   744
#2  2003-02-01       FLAT   672
#3  2003-03-01       FLAT   744
#4  2003-04-01       FLAT   719
#5  2003-05-01       FLAT   744
#6  2003-06-01       FLAT   720


}
