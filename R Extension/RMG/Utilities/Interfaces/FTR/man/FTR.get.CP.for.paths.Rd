\name{FTR.get.CP.for.paths}
\alias{FTR.get.CP.for.paths}
\title{Get the clearing prices for a set of paths.}
\description{Get the clearing prices for a set of paths.}
\usage{
FTR.get.CP.for.paths(Paths, ISO.env, add.SP=FALSE, panelName=FALSE)
}
\arguments{
  \item{Paths}{A data.frame with at least c("class.type", "source.ptid",
    "sink.ptid") column names.  It is recommended to have a "path.no"
    column, in case we drop a few paths.  If \code{Paths} contains a
    column with auction name, it will return only the clearing prices
    for that auction.}
  \item{ISO.env}{An ISO environment that you get with
    \code{FTR.load.ISO.env}.}
  \item{add.SP}{If you want to add the settle prices to the results.
    This will work only for monthly auctions.}
  \item{panelName}{If TRUE, a column "panelName" will be added to the
    output data.frame for later use with \code{FTR.plot.CPSP}}.
}
\value{
A data frame with the auction name and the clearing price in $/MWh. 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{

# Load an ISO.env, say NEPOOL which contains NEPOOL$CP
#
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, class.type=c("OFFPEAK", "ONPEAK"),
  source.ptid = c(326, 4000), sink.ptid   = c(4001, 4002))

res <- FTR.get.CP.for.paths( Paths, NEPOOL )
head(res)
#  path.no auction         CP
#1       1     F06 -1.8495833
#2       1  F06-1Y -1.8563118
#3       1     F07 -1.0883673
#4       1  F07-1Y -0.7631517
#5       1     F08 -0.4006888
#6       1  F08-1Y -0.6120286

}