\name{FTR.incdec.PnL}
\alias{FTR.incdec.PnL}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Load positions from different data sources.}
\description{
Load positions from different data sources.
}
\usage{
FTR.incdec.PnL(QQ, ISO.env)
}
\arguments{
  \item{QQ}{A data.frame with inc/dec position info.  It must contain
    the following columns: c("source.schedule", "sink.schedule", "ptid",
    "bucket", "start.dt", "mw").  Ptid is required in order to map to
    the tsdb symbol.  No need for an end.dt because inc/decs are only
    one day.  Bucket is a valid SecDb bucket, e.g. "7X8".
    Source.schedule and sink.schedule are one of "DA" or "RT", depending
    if the trade is an inc or a dec.} 
  \item{region}{A NERC region.  Get positions only for this region}
}
\value{
  Returns a data frame. 
}
\author{Adrian Dragulescu}
% \note{If from="SecDb", I use a script AAD: _lib SecDB to R,
%   read_deltas_leaf, which is a bit rudimentary.  It is recommended you
%   use from="PRISM", which is the new Black Bird FOIT infrastructure.

%   As of now, the code will return only positions that have MCC in the Delivery
%   Point, and combine long and short legs, and the offpeak curves to create a path. 
% }
\seealso{}
\examples{

# Get the positions using FTR.loadPositions(..., incdec=TRUE)
#> QQ
#  source.schedule sink.schedule  ptid bucket   start.dt mw
#1              DA            RT 10683    7X8 2008-09-18 15
#2              RT            DA   388    7X8 2008-09-18 15
#3              DA            RT  4202    7X8 2008-09-18 15
#4              RT            DA  4235    7X8 2008-09-18 15
#5              DA            RT 10683    7X8 2008-09-19 15
#>
QQ <- FTR.incdec.PnL(QQ, "NEPOOL")
#> QQ
#  source.schedule sink.schedule  ptid bucket   start.dt mw     PnL
#1              DA            RT 10683    7X8 2008-09-18 15  334.05
#2              RT            DA   388    7X8 2008-09-18 15 1224.15
#3              DA            RT  4202    7X8 2008-09-18 15  462.45
#4              RT            DA  4235    7X8 2008-09-18 15 1227.00
#5              DA            RT 10683    7X8 2008-09-19 15 -819.75


}
