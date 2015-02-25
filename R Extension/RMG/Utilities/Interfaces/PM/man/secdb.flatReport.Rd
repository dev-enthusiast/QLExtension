\name{secdb.flatReport}
\alias{secdb.flatReport}
\title{Run the flat report for a set of books}
\description{
  Run the flat report for a set of books and return the fields you
  want. 
}
\usage{
  secdb.flatReport(asOfDate=Sys.Date()-1, books=c("NPCLPNPS"),
    variables=c("Quantity", "Today Value", "Active Contracts",
    "Delivery Point", "Service Type", "Settlement Type"), filter=NULL, to.df=FALSE)
}
\arguments{
  \item{asOfDate}{an R Date}
  \item{books}{a vector of SecDb books}
  \item{variables}{ a vector with variables you want to extract.  Other
    useful variable is the bucket "Quantity Bucket Type". }
  \item{filter}{a function to filter the data.  It operates on each leaf
    separately.  See the example. }
  \item{to.df}{a boolean if you want to format the results as a
    \code{data.frame}.}
}
\value{Returns a dataframe or a list depending on the \code{to.df}
  argument.  NOTE that the filtering is done on the R side. }
%\seealso{\code{\link{tsdb.readCurve}}}
\examples{

  asOfDate <- as.Date("2012-08-30")
  books    <- "NPCLPNPS"
  # books <- secdb.getBooksFromPortfolio("Nepool Load Portfolio")

  # my custom filter function to return only the FwdRes service type
  filter <- function(x){ x$`Service Type` == "FwdRes" }
  
  res <- secdb.flatReport(asOfDate, books, filter=filter, to.df=TRUE)

  
}
