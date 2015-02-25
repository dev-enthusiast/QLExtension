\name{FTR.makePositionsForVaR}
\alias{FTR.makePositionsForVaR}
\title{Convert the paths positions to commod curves for VaR calculation.}
\description{
}
\usage{
FTR.makePositionsForVaR( filepath, ISO.env )
}
\arguments{
  \item{filepath}{Full path to the file with bids ready to submit to the
  ISO.}
  \item{ISO.env}{Your usual ISO.env that you get with \code{FTR.load.ISO.env}.}
}
\value{
 A position data frame with columns c("curve.name", "contract.dt",
 "vol.type", "position")
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.nodeToCommod}}}
\examples{

# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
FTR.load.ISO.env("NEPOOL")

filepath <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/2008-05/CCG Stat Bids May08.csv"
QQ <- FTR.makePositionsForVaR(filepath, NEPOOL)
head(QQ)
#                       curve.name contract.dt vol.type position
#1 COMMOD PWX 2X16H MAINE PHYSICAL  2008-05-01       NA        0
#2  COMMOD PWX 5X16 MAINE PHYSICAL  2008-05-01       NA        0
#3   COMMOD PWX 7X8 MAINE PHYSICAL  2008-05-01       NA        0

}