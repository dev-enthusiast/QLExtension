\name{text_wrap}
\alias{text_wrap}
\title{Fold a long string to a readable width.}
\description{
 Fold a long string to a fixed width format, similar to what
 'fill-paragraph does in Emacs.  The function is not vectorized, not
 very well optimized (but works.)
}
\usage{
  text_wrap(x, WIDTH=70)
}
\arguments{
   \item{x}{A long character string to wrap.}
   \item{WIDTH}{A numerical value for column width.}
}
\details{
}
\value{
  a string that you can output with cat.
}
\references{ }
\author{Adrian Dragulescu.}
\examples{

## x <- "Want key facts about the power system in your state? Check out ISO-NE's updated state profiles and new regional profile. You'll find info on demand growth, fuel mix, DR participation, RPS, transmission development, wholesale prices, and FCA results."
## cat(text_wrap(x))


}
\keyword{}
