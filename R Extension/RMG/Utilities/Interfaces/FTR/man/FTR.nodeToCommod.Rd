\name{FTR.nodeToCommod}
\alias{FTR.nodeToCommod}
\title{Map a list of node names to commod curves.}
\description{
  Map a list of node names to commod curves.
}
\usage{
FTR.nodeToCommod(nodeNames, ISO.env)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{nodeNames}{A vector of node names.}
  \item{ISO.env}{The usual ISO environment.}
}
\details{}
\value{
  A data.frame with column names c("curve.name", "location")
}
\references{}
\author{Adrian Dragulescu}
\note{Uses a slang scrip "_Lib Vcentral delivery point", "VCN::EML2CML"}
\seealso{}
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$MAP

nodeNames <- c("LD.RUMFD_IP115", "UN.ELLSWRTH34.5BARH")

res <- FTR.nodeToCommod(nodeNames, NEPOOL)

#                       curve.name      location
#1 COMMOD PWX 2X16H MAINE PHYSICAL RUMFD_IP.4138
#2  COMMOD PWX 5X16 MAINE PHYSICAL RUMFD_IP.4138
#3   COMMOD PWX 7X8 MAINE PHYSICAL RUMFD_IP.4138
#4 COMMOD PWX 2X16H MAINE PHYSICAL ELLSWRTH.BARH
#5  COMMOD PWX 5X16 MAINE PHYSICAL ELLSWRTH.BARH
#6   COMMOD PWX 7X8 MAINE PHYSICAL ELLSWRTH.BARH



}

