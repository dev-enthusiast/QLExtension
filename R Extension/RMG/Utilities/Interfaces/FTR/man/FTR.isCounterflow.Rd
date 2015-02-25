\name{FTR.isCounterflow}
\alias{FTR.isCounterflow}
\title{Determine if a path is counterflow or not.}
\description{Determine if a path is counterflow or not based on the
  historical settle prices.  The function uses \code{FTR.basicStats} to
  construct a heuristic criterion by looking at the ratio of days
  up/days down, ratio of EWin/ELoss and ratio of
  (Q(75)-Q(50))/(Q(50)-Q(25)).

  The function also returns a measure of skewness of the historical
  settle price distribution in variable \code{r.cntflw}.  If the value
  of the variable is greater than 0, the path has a heavier right tail,
  if less than zero, the path has a heavier left tail.    
}
\usage{
FTR.isCounterflow( hSP, ISO.env)
}
\arguments{
  \item{hSP}{The daily historical settle prices as returned by \code{FTR.get.hSP.for.paths}.}
  \item{ISO.env}{The usual ISO environment you get with \code{FTR.load.ISO.env}.}
}
\value{
NONE.
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.get.hSP.for.paths}}}
\examples{
cflow <- FTR.isCounterflow(hSP, NEPOOL)

% > head(cflow)
%   path.no  r.cntflw is.counterflow
% 1       1 3.6051017          FALSE
% 2       2 5.4525781          FALSE
% 3       3 5.9519172          FALSE
% 4       4 7.5543251          FALSE
% 5       5 3.5625571          FALSE
% 6       6 0.6939313             NA
% >

}