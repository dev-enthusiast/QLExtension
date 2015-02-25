\name{FTR.constraintSensitivity}
\alias{FTR.constraintSensitivity}
\title{Simple statistics on the effect of binnding constraints.}
\description{Calculate simple statistics on the effect of binding
  constraints on a set of paths.  If you run out of memory, reduce the
  number of paths or use less constraints.}
\usage{
FTR.constraintSensitivity(paths, BC, ISO.env, start.dt=NULL)
}
\arguments{
  \item{paths}{The usual paths data.frame.  See \code{\link{FTR.get.hSP.for.paths}}}
  \item{BC}{A data.frame of binding constraints as returned by 
    \code{\link{FTR.getBindingConstraints}}.}
  \item{ISO.env}{The usual ISO.env these paths belong to.}
  \item{start.dt}{An R POSIXct datetime.  If \code{NULL} it takes the
    first time entry from the BC data.frame.}
}
\value{
  A data frame with info. 
}
\author{Adrian Dragulescu}
\note{This function calls \code{\link{FTR.getPathReturns}} to get the
  hourly settle price.}
\seealso{This function calls \code{\link{FTR.getBindingConstraints}}.}
\examples{

start.dt <- seq(Sys.time(), by="-28 months", length.out=2)[2]

constraint.names <- c("NE_E-W", "CT-IMP", "NHME", "SWCT", "LWSEMA")

paths <- data.frame(path.no=1:2, class.type=c("ONPEAK", "OFFPEAK"), 
                    source.ptid=4376, sink.ptid=4377, CP=0)

BC <- FTR.getBindingConstraints(region="NEPOOL", 
    start.dt=as.Date(start.dt), constraint.names=constraint.names)

CS <- FTR.constraintSensitivity(paths, BC, NEPOOL)

head(CS)

#   path.no constraint on.off    Min. X1st.Qu. Median     Mean X3rd.Qu.   Max. no.hours
# 1       1     CT-IMP      0 -120.50  -0.4800    0.0 -1.59900     0.00 141.00     7684
# 2       1     CT-IMP      1  -40.45  -0.0400    0.0 -1.02200     0.00 125.70     1845
# 3       1     LWSEMA      0 -106.20  -0.1200    0.0 -1.41900     0.00  10.80     5531
# 4       1     LWSEMA      1 -120.50  -0.8075    0.0 -1.58100     0.00 141.00     3998
# 5       1     NE_E-W      0 -120.50   0.0000    0.0  0.02059     0.00 141.00     7828
# 6       1     NE_E-W      1  -45.32 -11.7200   -6.6 -8.42500    -3.13   0.46     1701
# >



}
