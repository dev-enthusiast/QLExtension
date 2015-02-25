\name{FTR.PathNamesToPTID}
\alias{FTR.PathNamesToPTID}
\title{Map paths specified as pairs of ptids to other variables and vice-versa.}
\description{
  If a set of paths is given in terms of ptids, it maps the paths to
  zones, node names, areas, etc., any other variable that exists int the
  MAP data.frame (see below).
  names. 
}
\usage{
FTR.PathNamesToPTID(Paths, MAP, from="ptid", to)
}
\arguments{
  \item{Paths}{A data.frame with at least c("source.name", "sink.name") or
c("source.ptid", "sink.ptid") column names.  It is recommended to have
a "path.no" column, in case we drop a few paths.  See note 
below. }
  \item{MAP}{Usually the data.frame ISO.env$MAP that you should have
    loaded before.  Contains the mapping between ptid and name. }
  \item{from}{A string, the name of the variable to map from.  Defaults
    to "ptid".}
  \item{to}{A string, the name of the variable to map to.}
}
\value{
  A data.frame with three columns c("path.no", "source.name",
  "sink.name") if from="ptid" and to="name".

  NOTE: if to="ptid" only from="name" is allowed (for a one to one mapping).
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.ReportManager}}}
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$MAP
# or you can load it directly with 
#

Paths = data.frame(source.ptid = c(326, 4000),
                   sink.ptid   = c(4001, 4002))

FTR.PathNamesToPTID(Paths, NEPOOL$MAP, to="name")

#  path.no         source.name       sink.name
#1       1 UN.ALTRESCO115 ALTR        .Z.MAINE
#2       2     .H.INTERNAL_HUB .Z.NEWHAMPSHIRE


}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
