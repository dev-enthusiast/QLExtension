\name{toHourly}
\alias{toHourly}
\alias{toDaily}
\title{Aggregate a zoo time series to hourly, daily frequency.}
\description{
  Aggregate (average) a zoo time series to hourly, daily frequency.
}
\usage{
 toHourly(X, HE=TRUE, \dots)

 toDaily(X, HE=TRUE, \dots)
}
\arguments{
  \item{X}{A time-series zoo object.}
  \item{HE}{a boolean.  If \code{TRUE} it assumes your time-series is in
	hour ending convention. }
  \item{\dots}{Other arguments to be passed to function \code{mean}.
	For example NA treatment, trim, etc. }
}
\details{}
\value{
  Another zoo object.
}
\author{Adrian Dragulescu}
\seealso{}
\examples{}

