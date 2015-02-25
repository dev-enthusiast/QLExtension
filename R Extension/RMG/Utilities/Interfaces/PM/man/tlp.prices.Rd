\name{tlp.prices}
\alias{tlp.prices}
\title{Return the tlp (mean) prices.}
\description{
  Return the tlp prices given an id.  Currently this function is a
  wrapper for ntplot:::tlpprices.  Should be implemented independently. 

}
\usage{
tlp.prices(id, bucket=c("5X16", "2X16H", "7X8"),
  region="NEPOOL", zone="HUB", scenario="Base", model="DA")
}
\arguments{
  \item{id}{An id (or a date in format "1Jun10"). }
  \item{bucket}{A valid bucket, or a vector of character buckets.}
  \item{region}{A character string giving the SecDb region (FreqDefs).}
  \item{zone}{A character string specifying the zone.}
  \item{scenario}{A character string.}
  \item{model}{A character string, "DA" or "RT".}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

id       <- "8448"
bucket   <- c("5X16", "2X16H", "7X8")  
region   <-"NEPOOL"
zone     <- "HUB"
scenario <- "LT_CORPORATE"          
model    <- "DA"

tlp <- tlp.prices(id, bucket, region=region, zone=zone,
  scenario=scenario, model=model)


  
}
