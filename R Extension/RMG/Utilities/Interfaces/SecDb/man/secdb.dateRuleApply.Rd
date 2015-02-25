\name{secdb.dateRuleApply}
\alias{secdb.dateRuleApply}
\title{Apply A SecDb Date Rule To An R Date}
\description{
Used to intelligently convert one date to another using a specified rule.
}
\usage{
secdb.dateRuleApply( dateValue, ruleString )
}
\arguments{
  \item{dateValue}{R Date object or valid string representation}
  \item{ruleString}{the rule character array to apply on the given date value}
}
\value{Returns the new R time value with the rule applied}
\examples{
library(SecDb)

secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")

##Get the curve data using the tsdb utility functions
plusOneBusinessDay = secdb.dateRuleApply(Sys.Date(), "+1b")
print(plusOneBusinessDay)

}
\details{This function actually calls the built-in SecDb function 'DateRuleApply'.
It is implemented as a secdb.invoke call of the following code:
    'DateRuleApply(Date("', dateString, '"), "', rule, '")'
All valid SecDb date rules should work and return the correct time.}
