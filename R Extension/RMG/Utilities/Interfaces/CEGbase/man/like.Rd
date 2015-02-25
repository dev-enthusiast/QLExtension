\name{like}
\alias{like}
\title{Convenience function for calling regexpr.}
\description{
  Intended for subsetting vectors.  The function is just a convenience
  wrapper of \code{regexpr}.  It is not implemented as the SQL
  \code{like} as regexpr is more powerful. 
}
\usage{
vector \%like\% pattern
}
\arguments{
   \item{vector}{Either a character vector or a factor.  A factor is faster.}
   \item{pattern}{Passed on to \code{\link{regexpr}}.}
}
\details{
}
\value{
  Integer vector containing the locations of vector which match the pattern using regexpr.
}
\references{ }
\author{Matthew Dowle from data.table package.}
\note{}
\seealso{\code{\link{regexpr}} }
\examples{

# get all the months that contain ary:
month.name[month.name \%like\% "ary"]

# get all the months that begin with J:
month.name[month.name \%like\% "^J"]

}
\keyword{}
