\name{FTR.findReversePaths}
\alias{FTR.findReversePaths}
\title{Find the reverse path number (if it exists) in a path data.frame.}
\description{
  Find the reverse path number (if it exists) in a path data.frame. 
}
\usage{
FTR.findReversePaths( paths )
}
\arguments{
  \item{paths}{Usual paths data.frame.  You need at least the columns
    \code{c("path.no", "source.ptid", "sink.ptid", "class.type")}}. 
}
\value{
  The paths data.frame back with an extra column \code{path.no.rev}. 
}
\author{Adrian Dragulescu}
%\seealso{\code{\link{FTR.get.hSP.for.paths}}}
\examples{

paths <- FTR.findReversePaths( paths )

}