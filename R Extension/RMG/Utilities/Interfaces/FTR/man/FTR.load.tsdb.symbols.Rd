\name{FTR.load.tsdb.symbols}
\alias{FTR.load.tsdb.symbols}
\title{Load tsdb symbols between a start and end time.}
\description{
Load tsdb symbols between a start and end time.  It is a vectorized
version of tsdb.readCurve from the SecDb package.  It returns the
resulting price series as a zoo object.  You can create new variables,
by specifiying a list to manipulate the symbols. 
}
\usage{
FTR.load.tsdb.symbols(tsdb.symbols, start.dt, end.dt, newvars=NULL,
  aggHourly=rep(FALSE, length(tsdb.symbols)))
}
\arguments{
  \item{tsdb.symbols}{A vector of tsdb symbols.}
  \item{start.dt}{An R POSIXct datetime.}
  \item{end.dt}{An R POSIXct datetime.}
  \item{newvars}{An R list with each element corresponding to a new
    variable to be created.  Each element of the this list is another
    list with three elements, a variable name, a vector of symbols, and
    a custom function.  The first argument of the custom function is the
    time-series of symbols with names requested. }
  \item{aggHourly}{a vector of boolean.  To aggregate hourly an intra
	hour symbol set the corresponding elements of this vector to
	TRUE.  This works properly only for symbols in Hour Ending
	convention.} 
}
\details{If the timeseries is missing in tsdb, it is not returned.  If
  \code{newvars} is specified, new variables are created using their
  corresponding custom functions.}
\value{
  A zoo matrix with colnames the tsdb symbols.  It contains the merged
  data with all=TRUE, so missing data is represented as NA. 
}
\author{Adrian Dragulescu}
\seealso{}
\examples{
tsdb.symbols = c("nepool_smd_da_4000_congcomp",
                 "nepool_smd_da_4001_congcomp")

res = FTR.load.tsdb.symbols(tsdb.symbols, Sys.time()-24*3600, Sys.time())
head(res)
#                    nepool_smd_da_4000_congcomp nepool_smd_da_4001_congcomp
#2008-04-15 12:00:00                       -0.36                      -15.64
#2008-04-15 13:00:00                       -0.42                      -10.21
#2008-04-15 14:00:00                        0.87                      -14.33
#2008-04-15 15:00:00                        0.19                      -12.19
#2008-04-15 16:00:00                        0.32                      -12.42
#2008-04-15 17:00:00                       -0.33                      -10.32

# to calculate the me/hub congestion spread
newvars = list(
  list("me/hub.cong",
       c("nepool_smd_da_4001_congcomp", "nepool_smd_da_4000_congcomp"),
       function(x){x[,1]-x[,2]}),
)
# when passed to the function, x will be the zoo object with two columns
res = FTR.load.tsdb.symbols(tsdb.symbols, Sys.time()-24*3600,
  Sys.time(), newvars)
head(res)



}

