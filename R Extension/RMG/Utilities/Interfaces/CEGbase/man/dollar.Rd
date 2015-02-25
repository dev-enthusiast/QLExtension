\name{dollar}
\alias{dollar}
\title{Format a number to a currency style.}
\description{Format a number to a currency style.}
\usage{
dollar(x, digits=2, do.dollar=TRUE)

dollarShortForm(x, digits=2 do.dollar=TRUE)
}
\arguments{
  \item{x}{A numeric vector to format.}
  \item{digits}{Number of decimals you want to display.}
  \item{do.dollar}{Add the dollar sign in front of the number.}
}
\value{
The output mimics the default Excel format for dollar currency.   For
\code{dollarShortForm} the usual multiple letter is used for each 1000
multiplier.  See examples.
}
\author{Adrian Dragulescu, John Scillieri}
\note{}
\seealso{}
\examples{

dollar(c(-1024.515, 1024.515))
#[1] "$(1,024.52)" "$1,024.52"

dollarShortForm(c(-1024.515, 1024.515, 1000001.25, 11000000))
#[1] "$(1.02k)" "$1.02k"   "$1.00MM"  "$11.00MM"

}