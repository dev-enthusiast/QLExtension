\name{FTR.filter.hSP}
\alias{FTR.filter.hSP}
\title{Filter a set of timeseries and pick only data from months "similar" to a
  given month.}
\description{Filter a set of timeseries and pick only data from months
  "similar"  to a given month.  If the given month is M, similar months
  are defined as {M-1, M, M+1} from all previous years you have history.
}
\usage{
FTR.filter.hSP(hSP, month = as.Date("2008-05-01"))
}
\arguments{
  \item{hSP}{A set of historical settle prices, but can be anything as
    long as it's in the same format as returned by
    \code{FTR.get.hSP.for.paths}}. 
  \item{month}{An R Date. }
}
\value{
  Returns data in the same format as the input hSP. 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.get.hSP.for.paths}}}
\examples{
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid   = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"))

hSP <- FTR.get.hSP.for.paths( Paths, NEPOOL, melt=T )
res <- FTR.filter.hSP(hSP, month=as.Date("2008-06-01"))

head(res)
#            day path.no     value
# 121 2006-05-01       2 -0.001250
# 122 2006-05-02       2 -0.062500
# 123 2006-05-03       2  0.000000
# 124 2006-05-04       2  0.000000
# 125 2006-05-05       2 -0.018750
# 126 2006-05-06       2  7.269167

}