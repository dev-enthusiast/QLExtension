\name{isotimeToPOSIXct}
\alias{isotimeToPOSIXct}
\title{Convert ISO style Date, Hour Ending info to a POSIXct datetime.}
\description{
  Convert ISO style Date and Hour Ending info to a POSIXct datetime.  All
  ISO reports come in this format.
}
\usage{
isotimeToPOSIXct(X, extraHourToken="02X")
}
\arguments{
  \item{X}{a data.frame with two character vector columns "Date" and "Hour.Ending".}
  \item{extraHourToken}{Character string to identify the extra hour
	during Fall DST.}
}
\value{Returns the data.frame with an extra column called datetime.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

X <- data.frame(Date=rep(c("2011-03-13"), 3),
                Hour.Ending=c("01", "03", "04"))
isotimeToPOSIXct(X)

#        Date Hour.Ending            datetime
#1 2011-03-13          01 2011-03-13 01:00:00
#2 2011-03-13          03 2011-03-13 03:00:00
#3 2011-03-13          04 2011-03-13 04:00:00

# fall back
X <- data.frame(Date=rep(c("2013-11-03"), 5),
                Hour.Ending=c("01", "02", "02X", "03", "04"))
y <- isotimeToPOSIXct(X)
as.numeric(y$datetime)    # separated by 3600, 02X is the extra hour  



}
