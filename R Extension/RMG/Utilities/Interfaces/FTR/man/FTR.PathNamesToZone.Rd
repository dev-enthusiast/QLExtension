\name{FTR.PathNamesToZone}
\alias{FTR.PathNamesToZone}
\title{Find the source zone and sink zone for a set of paths.}
\description{
  Given a set of paths in terms of source.ptid, sink.ptid, map the nodes
  to their corresponding zone, and return source.zone, sink.zone. 
}
\usage{
FTR.PathNamesToZone(Paths, MAP)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{Paths}{A data.frame with at least c("source.ptid", "sink.ptid")
    column names.  It's recommended to have a "path.no" column, in case
    we drop a few paths.  See note below.}
  \item{MAP}{Usually the data.frame ISO.env$MAP that you should have
    loaded before.  Contains the mapping between ptid and zone.}
}
\value{
  A data.frame with three columns c("path.no", "source.zone",
  "sink.zone")
}
\author{Adrian Dragulescu}
\note{In rare cases, the MAP data.frame may be missing elements.  Let me
  know so we can add them.}
\seealso{\code{\link{FTR.PathNamesToPTID}} }
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$MAP

Paths = data.frame(source.ptid = c(326,  6420),
                   sink.ptid   = c(4000, 4002))

FTR.PathNamesToZone(Paths, NEPOOL$MAP)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ }
\keyword{ }% __ONLY ONE__ keyword per line
