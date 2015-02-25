\name{NYPP.prohibitGroups}
\alias{NYPP.prohibitGroups}
\title{Remove paths that NYISO prohibits.}
\description{
  NYPP publishes a pdf with groups of nodes that cannot have paths
  between the nodes belonging to the same group.
}
\usage{
NYPP.prohibitGroups( paths )
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{paths}{A data.frame with at least the columns \code{c("path.no",
      "source.name", "sink.name")}.}
}
\details{The function reads the file
  "S:/All/Risk/Data/FTRs/NYPP/ISO/DB/RData/nodeGroupConstraints.RData"
  with the grouping info.  This file gets greated by the function
  \code{FTR:::.read.NYPP.prohibited.pdf}.
}
\value{
  A data.frame with only the paths that are allowed.
}
\references{}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

}

