\name{secdb.HR.getOptionPremium}
\alias{secdb.HR.getOptionPremium}
\title{Read Heat Rate Option Premiums from SecDb}
\description{
  Read the monthly or term HR option premiums for a given elec curve and gas location in SecDb.
}
\usage{
secdb.HR.OptPremium(region="NEPOOL", loc="hub", gasCommod="NG ALGCG",
gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call",
StartDate=nextMonth(Sys.Date()), NumMth=1, Strike=0, HR="ATM",
ExpRule="-1b", AsOfDate=as.Date(Sys.Date()), Aggregate = FALSE)
}
\arguments{
  \item{Region}{FERC region name, Default="NEPOOL"}
  \item{loc}{SecDb location name from Calc, Default="Hub"}
  \item{gasCommod}{SecDb Gas commod name from Calc, Default="NG ALGCG"}
  \item{gasLoc}{SecDb Gas location name from Calc, Default="GAS-DLY MEAN"}
  \item{bucket}{SecDb bucket name, Default="5x16"}
  \item{freq}{'Daily' for Daily option, 'Monthly' for Monthly Option, Default="Daily"}
  \item{OptType}{Put/Call, Default="Call"}
  \item{StartDate}{Start Date of the Option, Default=nextMonth(Sys.Date())}
  \item{NumMth}{Number of months in the option, Default=1}
  \item{Strike}{Option Strike Price}
  \item{HR}{Heat Rate of the option. Default = "ATM"}
  \item{ExpRule}{Expiration Date Rule. Note NEPOOL we use "-1b"}
  \item{AsOfDate}{valuation date. Default = Sys.Date()}
  \item{Aggregate}{boolean, get term option price if TRUE, option price By month if FALSE. Default = FALSE}
}
\value{Returns a dataframe with month numbers and corresponding HR option
  premiums if not then just the term option premium. Also returns the power and gas price}
\author{Ankur}
\examples{


# get the ATM HR option premium for 12 months
op <- secdb.HR.OptPremium(region="NEPOOL", loc="hub", gasCommod="NG ALGCG", gasLoc="GAS-DLY MEAN", bucket="5x16", freq="Daily", OptType="Call", StartDate=nextMonth(Sys.Date()), NumMth=12, Strike=0, HR="ATM", ExpRule="-1b", AsOfDate=as.Date(Sys.Date()), Aggregate = TRUE)
op
##         HR    power      gas month    HROpt
## 1 10.66985 74.11816 6.946506  Term 8.143078



}
