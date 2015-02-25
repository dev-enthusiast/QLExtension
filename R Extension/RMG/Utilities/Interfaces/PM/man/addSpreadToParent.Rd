\name{addSpreadToParent}
\alias{addSpreadToParent}
\title{Add a spread value to a parent node mark.}
\description{
Add a value to a parent node mark for the three buckets (5x16, 7x8,
2x16H) in NEPOOL.  If the parentMark is missing, the function calls
\code{secdb.getElecPrices} internally to grab the parent marks.

The function is conceptually very simple, by doing a simple addition.
However, it is useful if you want to add to several nodes at the same time,
etc. 
}
\usage{
addSpreadToParent(sprdToParent, parentMark, asOfDate=Sys.Date(),
  matrix=TRUE)
}
\arguments{
  \item{sprdToParent}{ A data.frame with the spread to add.  It must have
    columns c("delivPt", "parentDelivPt", "month", "bucket", "value"), 
    where column "value" represents the additional value to add.}
  \item{parentMark}{ A data.frame with parent marks (on what you
    add the spread.)  It needs to have the columns: c("delivPt",
    "parentDelivPt", "bucket", "month", "parentMark").  If missing, the
    marks are  pulled from SecDb.}
  \item{asOfDate}{ An R Date variable indicating the date of the marks.}
  \item{matrix}{ A boolean, indicating if the return value should be
    reshaped for ExternalData.}
}
\value{Returns a data.frame.  See the examples.} 
\seealso{\code{\link{spreadToParent}} for the function that performs
  the oposite operation.}
\examples{

# consider this input data.frame ... 
head(X)


# make the call
spread <- addSpreadToParent(X, marksForDate)
head(spread)


  
}