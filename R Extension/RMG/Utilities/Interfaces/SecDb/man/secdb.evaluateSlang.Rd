\name{secdb.evaluateSlang}
\alias{secdb.evaluateSlang}
\title{Evaluate SecDb Slang Code}
\description{
Executes arbitrary Slang code and returns the result as a converted R type.
}
\usage{
secdb.evaluateSlang( slangCodeString )
}
\arguments{
  \item{slangCodeString}{a character string representing the Slang code to execute}
}
\value{Returns the SecDb value type in a compatible R format (list, vector, etc).}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get a given value type from a specified security
slangCode = 'GetValue("Bal Close Attrition AcqCost", "Date Created")'
valueType = secdb.evaluateSlang( slangCode )

}
\details{Because SecDb types can be created dynamically, it is possible that the
conversion from a SecDb value to R type would be unsupported.  This code 
attempts to use the most suitable R type for the SecDb objects it knows about. 
In the event of an unsupported type, please contact CCG RMG Analytics and we 
will look into adding the functionality. If a SecDb object is unsupported, 
the C++ conversion routine will return NULL back to the R code.
}
\note{This call will also return NULL if your Slang code does not properly 
validate.
}