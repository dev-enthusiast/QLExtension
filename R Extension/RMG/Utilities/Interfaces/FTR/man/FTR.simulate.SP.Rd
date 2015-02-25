\name{FTR.simulate.SP}
\alias{FTR.simulate.SP}
\title{Simulate the settle prices for a monthly auction, given a set of paths.}
\description{Simulate the settle prices for a monthly auction, given a
  set of paths.  The algorithm has 3 steps. 

  Step 1 creates ensembles of similar history for
  daily settle prices.  We use a maximum likelihood approach as in
  package \code{meboot}, but applied on quantiles.  This guarantees that
  the dependency betweeen different paths is preserved. 

  Step 2, we sample the simulated daily settle prices at
  chunks of days with length  with the number of days the auction has
  (e.g. 31 contiguous days for auction="F08").  This maintains the
  serial correlation between paths.  The sampling is done by giving more
  weight to recent history, and to similar months.  The default value is 
  noSamples=10, only 10 cuts for a monthly auction with one historical ensamble.  

  Step 3, calculate the monthly mean.}
\usage{
FTR.simulate.SP(hSP, options, noSims=5000, noSamples=10, noEns=100,
  reversePaths=NULL, check=TRUE)
}
\arguments{
  \item{hSP}{Daily historical settle prices by path.
    See \code{\link{FTR.get.hSP.for.paths}}.}
  \item{options}{The usual options you get with
    \code{FTR.makeDefaultOptions}}
  \item{noSims}{The number of simulations.}
  \item{noSamples}{The number of samples.  If you have 2 years of
    historical data, think how many monthly samples you want to
    extract from one ensamble.}
  \item{noEns}{The number of ensembles to generate at once.
    Don't modify this number unless
    you run out of memory because you have too many paths.  Then you
    need to reduce this number. }
  \item{reversePaths}{A data.frame with two columns c("path.no",
    "path.no.rev") that indicate the direct and reversed path given in
    term of path.no (should match the colnames of hSP).  For
    efficiency, the code will not simulate the reverse paths, will
    simply multiply the direct path distribution by -1.  If NULL, the
    code will simulate all paths.}
  \item{check}{Reports if you have NA's in the simulated prices.  You
    should have none.}
}
\value{
A matrix with dimensions c(noPaths, noSims). 
}
\author{Adrian Dragulescu}
\seealso{\code{\link{FTR.get.hSP.for.paths}}}
\examples{



}