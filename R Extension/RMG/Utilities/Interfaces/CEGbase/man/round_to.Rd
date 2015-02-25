\name{round_to}
\alias{round_to}
\title{Round a number a nearest integer.}
\description{Round a number a nearest integer.}
\usage{
  round_to(x, to=25)
}
\arguments{
  \item{x}{A numeric vector to format.}
  \item{to}{Number you want to round to.}
}
\value{
  Note that you can use the usual \code{round} function to round to the
  nearest hundred, \code{round(x, -2)}, or other power of ten. 
}
\author{Adrian Dragulescu}
\note{}
\seealso{}
\examples{

# round to the nearest 25
round_to(c(1412.1, 1430, 1445, 1455, -1412, -1430), to=25)



}