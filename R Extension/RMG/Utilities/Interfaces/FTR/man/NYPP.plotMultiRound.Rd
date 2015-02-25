\name{NYPP.plotMultiRound}
\alias{NYPP.plotMultiRound}
\title{Compare the clearing prices of different rounds of a NYPP auction.}
\description{
  Compare graphically the clearing prices of different rounds of a NYPP
  auction.
}
\usage{
NYPP.plotMultiRound( paths )
}
\arguments{
  \item{paths}{A paths data.frame with at least the columns \code{c("path.no",
      "class.type", "source.ptid", "sink.ptid")}.}
  \item{auctions=NULL}{A vector of auction names.}
  \item{pattern=NULL}{A pattern to use for subsetting previous auction
    results.}
  \item{...}{Other parameters that will be passed to the \code{dotplot}.}
}
\details{Get the historical clearing prices for a set of paths and do a
  dotplot.  The "source.name" and "sink.name" are required for the
  shingle name.}
\value{
  A plot.
}
\references{}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{
# create some paths
paths <- data.frame(path.no=1:(nrow(aux)-1), class.type="FLAT",
  source.ptid=aux$ptid[1], sink.ptid=aux$ptid[2:nrow(aux)])
paths <- paths[1:10,]

# add the path names
paths <- merge(paths, FTR.PathNamesToPTID(paths, NYPP$MAP, to="name"))

pattern <- "X08-"     # select the annual 08 winter auction 
xlab   <- "Clearing prices over West zone, $"
layout <- c(5,2)
main   <- paste("Auction", gsub("-", "", pattern))
NYPP.plotMultiRound(paths, pattern=pattern, layout=layout,
  xlab=xlab, main=main)

}

