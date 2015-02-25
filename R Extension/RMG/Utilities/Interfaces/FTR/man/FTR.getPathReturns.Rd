\name{FTR.getPathReturns}
\alias{FTR.getPathReturns}
\title{Calculate the hourly returns for a set of FTR paths.}
\description{
Calculate the hourly returns for a set of FTR paths.
}
\usage{
FTR.getPathReturns(paths, start.dt, end.dt=Sys.time(),
  ISO.env=ISO.env, melt=FALSE, hourly=FALSE)
}
\arguments{
  \item{paths}{The usual paths data frame.  It should contain a column "mw"
  for the quantity awarded.  If this column does not exist, it assumes
  mw=1.  Also paths should contain a column "CP" for the hourly clearing
  price for this path.  If it does not exists, it assumes CP=0.}
  \item{start.dt}{A starting date for the historical returns.}
  \item{end.dt}{An ending date.  Defaults to now.}
  \item{ISO.env}{An ISO environment.}
  \item{melt}{A boolean.  If the results should be returned melted or
    not. }
  \item{hourly}{A boolean.  If TRUE, the code will go to tsdb and return
  hourly path returns.  Not advised to use on more than 100 paths at a
  time. For many paths (> 300), it's indicated you use hourly=FALSE.}
}
\value{
If melt=TRUE, a data frame with historical returns, at hourly or daily
frequency.  If melt=FALSE, a list with the same info.  If hourly=TRUE,
you can calculate the cumulative payoff by simply adding the
observations by path.  If hourly=FALSE, you will have to average the
observations and multiply with the number of hours (more work if you are
intramonth).
}
\author{Adrian Dragulescu}
\seealso{ \code{\link{FTR.get.hSP.for.paths}}, \code{\link{FTR.get.hSP.for.paths}} }
\examples{
# Load an ISO.env, say NEPOOL which contains NEPOOL$hSP
#
FTR.load.ISO.env("NEPOOL")

Paths = data.frame(path.no=1:2, source.ptid = c(326, 4000),
  sink.ptid = c(4001, 4002), class.type=c("ONPEAK","OFFPEAK"),
  CP=c(0.2, 1.2), mw=c(50, 100))

start.dt <- as.Date("2008-04-01")

PP  <- FTR.getPathReturns(Paths, start.dt, ISO.env=NEPOOL, hourly=TRUE)
head(PP, 10)
#                          1    2
# 2008-04-01 00:00:00     NA -107
# 2008-04-01 01:00:00     NA -112
# 2008-04-01 02:00:00     NA -117
# 2008-04-01 03:00:00     NA -119
# 2008-04-01 04:00:00     NA -117
# 2008-04-01 05:00:00     NA -100
# 2008-04-01 06:00:00     NA -110
# 2008-04-01 07:00:00  -27.0   NA
# 2008-04-01 08:00:00  -33.5   NA
# 2008-04-01 09:00:00 -156.5   NA

PP  <- FTR.getPathReturns(Paths, start.dt, ISO.env=NEPOOL, melt=TRUE)

PP  <- FTR.getPathReturns(Paths, start.dt, ISO.env=NEPOOL)
head(PP)
#                    1         2
# 2008-04-01 -2.270625 -1.127500
# 2008-04-02 -2.567500 -1.200000
# 2008-04-03 -5.155625 -1.200000
# 2008-04-04 -0.894375 -1.152500
# 2008-04-05        NA -1.207083
# 2008-04-06        NA -1.196957

PP  <- FTR.getPathReturns(Paths, start.dt, ISO.env=NEPOOL, melt=TRUE)




}
