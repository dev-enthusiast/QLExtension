\name{Reuters/Nymex/SecDb month codes}
\alias{}
\title{Map a month to a Reuters code and back.}
\description{Map a month to a Reuters code and back.}
\usage{
  CEGterm:::.monthToReutersCode( monthString )

  CEGterm:::.reutersCodeToMonth( monthLetter )
}
\arguments{
  \item{monthString}{An abreviated month name, (e.g. "JAN")}
  \item{monthLetter}{A letter representing the month, (e.g. "F" for Jan).}
}
\value{
An R string.
}
\author{John Scillieri}
\note{This function is not vectorized, so it can take one string only.
  It is also not exported, so you need to qualify the name with the
  CEGterm::: namespace.}
\seealso{The function format.dateMYY in package SecDb.}
\examples{

CEGterm:::.monthToReutersCode("JAN")  # returns "F"

CEGterm:::.reutersCodeToMonth("F")    # returns "JAN"

}
