\name{secdb.getOptionPremium}
\alias{secdb.getOptionPremium}
\title{Read Option Premiums from SecDb}
\description{
  Read the monthly or term option premiums for a given elec curve in SecDb.
}
\usage{
  secdb.getOptionPremium(region="NEPOOL", loc="hub", bucket="5x16",
     freq="Day", OptType="Call", StartDate=nextMonth(Sys.Date()), NumMth=1,
     Strike=50, ExpRule="+0b", AsOfDate=as.Date(Sys.Date()), Aggregate=FALSE)}
\arguments{
  \item{Region}{FERC region name, Default="NEPOOL"}
  \item{loc}{SecDb location name from Calc, Default="Hub"}
  \item{bucket}{SecDb bucket name, Default="5x16"}
  \item{freq}{'Day' for Daily option, 'Month' for Monthly Option, Default="Day"}
  \item{OptType}{Put/Call, Default="Day"}
  \item{StartDate}{Start Date of the Option, Default=nextMonth(Sys.Date())}
  \item{NumMth}{Number of months in the option.}
  \item{Strike}{Option Strike Price.}
  \item{ExpRule}{Expiration Date Rule. Note NEPOOL we use "-1b".}
  \item{AsOfDate}{valuation date.}
  \item{Aggregate}{boolean, get term option price if TRUE, Monthly option price
    if FALSE.}
}
\value{Returns a dataframe with month numbers and corresponding option
  premiums if not then just the term option premium.}
\author{Ankur}
\examples{


# get the option premium
op <- secdb.OptPremium(region = "NEPOOL", loc = "Hub", StartDate = as.Date("2015-01-01"), NumMth = 2, Strike = 186, ExpRule = "-1b", AsOfDate = as.Date("2014-10-31"), Aggregate = FALSE)
op
#        date    value
#1 2015-01-01 31.98847
#2 2015-02-01 29.33738


}
