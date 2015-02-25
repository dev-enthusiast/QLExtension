\name{secdb.getLoadObject}
\alias{secdb.getLoadObject}
\title{Read a load object from SecDb.}
\description{
  Read a load object from SecDb.
}
\usage{
secdb.getLoadObject( loadName, loadClass, asOfDate=Sys.Date())
}
\arguments{
  \item{loadName}{a character string giving the load name}
  \item{loadClass}{a character string with load class}
  \item{asOfDate}{an R Date}
}
\value{Returns a dataframe. The columns date and value are the contract
  date and the price requested.} 
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

# As of 7/15/2011, you need the link below (and if you don't like it
# talk to Grant Roch.) before you call the function!  
secdb.evaluateSlang('link("_lib ntplot addin");') 
loadName  <- "RiProG10"
loadClass <- "Large DS"
asOfDate  <- Sys.Date()

res <- secdb.getLoadObject(loadName, loadClass, asOfDate)
head(res)
#                time     value
#1 2009-06-01 01:00:00 0.3465005
#2 2009-06-01 02:00:00 0.3434976
#3 2009-06-01 03:00:00 0.3408771
#4 2009-06-01 04:00:00 0.3392648
#5 2009-06-01 05:00:00 0.3429186
#6 2009-06-01 06:00:00 0.3527857


}