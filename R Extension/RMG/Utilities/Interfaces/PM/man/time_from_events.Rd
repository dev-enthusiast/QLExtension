\name{time_from_events}
\alias{time_from_events}
\title{Create a zoo timeseries from a sequence of events.}
\description{
Function to go from a set of discrete events to an aggregated timeseries.
For example, given a set of outages, and a quantity of mw out, calculate
the total mw out at a given point in time and return this curve.
}
\usage{
  time_from_events(startEvent, endEvent, valueEvent, index, FUN, ...)
}
\arguments{
   \item{startEvent}{A vector of datetimes/dates that represent begining
	 of the event.}
   \item{endEvent}{A vector of datetimes/dates that represent end of the
	 event.}
   \item{valueEvent}{A vector of numeric values that quantify the
	 event.}
   \item{index}{A vector of datetimes/dates.  All elements of
	 \code{startEvent} and  \code{endEvent} are part of this vector.}
   \item{FUN}{A function to aggregate the events with.  For example sum
	 to calculate the sum of all values across events.}
   \item{\ldots}{Additional arguments to passed to \code{FUN}.}
}
\details{
  For convenience this function depends on the zoo package. 
}
\value{
  A zoo timeseries. 
}
\references{ }
\author{Adrian Dragulescu.}
\examples{



}
\keyword{}
