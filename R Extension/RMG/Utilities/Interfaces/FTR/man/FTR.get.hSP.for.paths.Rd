\name{FTR.get.hSP.for.paths}
\alias{FTR.get.hSP.for.paths}
\title{Get daily historical settle prices for a set of paths.}
\description{Get daily historical settle prices for a set of paths.}
\usage{
FTR.get.hSP.for.paths(Paths, ISO.env, melt = FALSE, start.dt=NULL,
  end.dt=NULL, buddy=.get_buddy_node())
}
\arguments{
  \item{Paths}{A data.frame with at least c("source.ptid", "sink.ptid",
    "class.type")
    column names.  It is recommended to have a "path.no" column, in case
    we drop a few paths.}
  \item{ISO.env}{Your usual ISO.env that you get with \code{FTR.load.ISO.env}.}
  \item{melt}{A boolean. Set to TRUE if you want your data melted.}
  \item{start.dt}{An R Date for the beginning of the time series.}
  \item{end.dt}{An R Date for the end of the time series.}
  \item{buddy}{a named vector to indicate the buddy ptid.  This is
    useful when nodes get remapped.  The list is maintained in the
    archive "NEPOOL/CSV/buddy_ptid.csv" file}
}
\value{
If \code{melt=TRUE} a data frame with columns c("day", "path.no",
"value").  If \code{melt=FALSE} a zoo matrix with path.no as colnames.  See the
examples below for more info. 
}
\author{Adrian Dragulescu}
\note{}
\seealso{\code{\link{FTR.load.ISO.env}}}
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid   = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"))

res <- FTR.get.hSP.for.paths( Paths, NEPOOL, melt=F )
head(res)
#                    1         2
# 2003-11-01        NA 0.1921739
# 2003-11-02        NA 0.0112500
# 2003-11-03 -0.001875 0.0000000
# 2003-11-04  0.000000 0.0000000
# 2003-11-05  0.000000 0.0000000
# 2003-11-06 -0.000625 0.0012500



res <- FTR.get.hSP.for.paths( Paths, NEPOOL, melt=T )
head(res)
#          day path.no value
# 1 2006-01-01       2     0
# 2 2006-01-02       2     0
# 3 2006-01-03       2     0
# 4 2006-01-04       2     0
# 5 2006-01-05       2     0
# 6 2006-01-06       2     0


}