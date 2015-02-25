\name{FTR.PathNamesToArea}
\alias{FTR.PathNamesToArea}
\title{Find the source RSP area and sink RSP area for a set of paths.}
\description{
  *********************************************************************
  NOTE:  This function has been superseeded by FTR.PathNamesToPTID.
         It will be removed soon.
  *********************************************************************
  
  Given a set of paths in terms of source.ptid, sink.ptid, map the nodes
  to their corresponding area, and return source.area, sink.area.  The
  function can now be used for any other variable that exists in the
  \code{MAP} dataframe, e.g. use \code{what="zone"}, to map ptid's to
  zones. 
}
\usage{
FTR.PathNamesToArea(Paths, MAP, what="area")
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{Paths}{A data.frame with at least c("source.ptid", "sink.ptid")
    column names.  It's recommended to have a "path.no" column, in case
    we drop a few paths.  See note below.}
  \item{MAP}{Usually the data.frame \code{ISO.env$MAP} that you should have
    loaded before.  Contains the mapping between ptid and RSP area.}
  \item{what}{The name of a variable from data.frame \code{MAP}, see
    description. }
}
\value{
  A data.frame with three columns c("path.no", "source.area",
  "sink.area")
}
\author{Adrian Dragulescu}
\note{In rare cases, the MAP data.frame may be missing elements.  Let me
  know so we can add them.}
\seealso{\code{\link{FTR.PathNamesToPTID}} }
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$MAP

Paths = data.frame(source.ptid = c(326,  6420),
                   sink.ptid   = c(4000, 4002))

FTR.PathNamesToArea(Paths, NEPOOL$MAP)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ }
\keyword{ }% __ONLY ONE__ keyword per line
