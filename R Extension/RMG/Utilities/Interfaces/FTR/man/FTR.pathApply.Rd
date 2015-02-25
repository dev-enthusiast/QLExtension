\name{FTR.pathApply}
\alias{FTR.pathApply}
\title{Apply a function to a set of paths.}
\description{
  Apply a function to a set of paths.  Similar in spirit to all the
  apply functions in R. 
}
\usage{
FTR.pathApply(Paths = NULL, hSP = NULL, ISO.env = NEPOOL, FUN, ...)
}
\arguments{
  \item{Paths}{A data.frame with at least c("source.ptid", "sink.ptid",
    "class.type")}
  \item{hSP}{As returned by \code{FTR.get.hSP.for.paths} with
    melt=FALSE, hourly=FALSE.  Use this for a large number of paths.}
  \item{ISO.env}{Your usual ISO.env that you get with \code{FTR.load.ISO.env}.}
  \item{FUN}{Any valid R function, e.g. mean.}
  \item{\dots}{Other arguments to be passed to \code{FUN}.}
}
\details{One of \code{Paths} or \code{hSP} can be \code{NULL}.  If
  \code{Paths} is \code{NULL}, then \code{hSP} needs to be specified.
  If \code{hSP} is \code{NULL} the code will pull the \code{hSP} first 
  from the \code{ISO.env$hP} and then apply the function.}
\value{
Returns a data.frame with the results of the function you applied. 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.load.ISO.env}}, \code{\link{FTR.get.hSP.for.paths}}}
\examples{

# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid   = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"))

hSP <- FTR.get.hSP.for.paths( Paths, NEPOOL, melt=F )

res <-FTR.pathApply(NULL, hSP, NEPOOL, quantile, probs=c(0,0.5), na.rm=T)
res
#  path.no       X0.      X50.
#1       2  -8.79250  0.000000
#2       1 -36.93125 -1.582813

# If you pass Paths
res <- FTR.pathApply(Paths, NULL, NEPOOL, mean, na.rm=T)
res
#  path.no        res
#1       2 -0.1773282
#2       1 -3.1927180



}
