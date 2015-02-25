\name{FTR.calc.SP.for.paths}
\alias{FTR.calc.SP.for.paths}
\title{Calculate the historical monthly settle price for a set of FTR paths.}
\description{
  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
FTR.calc.SP.for.paths(Paths, melt = FALSE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{Paths}{ ~~Describe \code{Paths} here~~ }
  \item{ISO.env}{ ~~Describe \code{Paths} here~~ }
  \item{melt}{ ~~Describe \code{melt} here~~ }
}
\details{
  ~~ If necessary, more details than the description above ~~
}
\value{
  ~Describe the value returned
  If it is a LIST, use
  \item{comp1 }{Description of 'comp1'}
  \item{comp2 }{Description of 'comp2'}
  ...
}
\references{}
\author{Adrian Dragulescu}
\note{}
\seealso{ \code{\link{FTR.get.hSP.for.paths}} }
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function( Paths, melt=FALSE )
{
  if (length(Paths$path.no)==0){Paths$path.no <- 1:nrow(Paths)}

  SP <- NULL
  batch.size <- 10000  
  batches <- trunc(nrow(Paths)/batch.size)+1
  for (b in seq_along(1:batches)){
    rLog(paste("Working on batch no.", b))
    ind.b <- ((b-1)*batch.size+1):min(b*batch.size, nrow(Paths))
    hSP <- FTR.get.hSP.for.paths(Paths[ind.b, ], NEPOOL)

    SP  <- rbind(SP, FTR.calc.SP.for.pathsBatch(Paths, hSP))
  }
##   if (melt){
##     SP <- rbind(data.frame(bucket="OFFPEAK", melt(as.matrix(SP$OFFPEAK))),
##                 data.frame(bucket="ONPEAK", melt(as.matrix(SP$ONPEAK))))
##     names(SP)[2:3] <- c("day", "path.no")
##     SP$day <- as.Date(SP$day)
##   }
  SP <- na.omit(SP)
  SP$path.no <- as.numeric(as.character(SP$path.no))
  SP$auction <- as.character(SP$auction)
  
  return(SP)
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
