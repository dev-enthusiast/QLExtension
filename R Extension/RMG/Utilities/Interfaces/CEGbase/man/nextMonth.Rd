\name{nextMonth}
\alias{nextMonth}
\alias{currentMonth}
\alias{prevMonth}
\title{Get the begining day of prev/current/next month from a given date.}
\description{
  Get the begining day of the next month, the previous month beginning
  and current month begining from a given date.  Just
  annoying to have to write the same code over and over. 
}
\usage{
  nextMonth( fromDate=Sys.Date() )

  currentMonth( fromDate=Sys.Date() )

  prevMonth( fromDate=Sys.Date() )
}
\arguments{
   \item{fromDate}{An R Date.}
}
\details{
}
\value{
  An R Date.
}
\references{ }
\author{Adrian Dragulescu}
\note{}
\examples{

nextMonth( as.Date("2012-06-13") )
#[1] "2012-07-01"

currentMonth( as.Date("2012-06-13") )
#[1] "2012-06-01"

prevMonth( as.Date("2012-06-13") )
#[1] "2012-05-01"

}
\keyword{}
