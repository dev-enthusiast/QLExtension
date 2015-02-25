# Calculate the IRR for an investment.
# Where dates    is a vector of  R Dates
#       cashflow is a numeric vector
#
## source("H:/user/R/RMG/Finance/General/calculate.irr.R")
## dates <- (0:3)*365.25; cashflows=c(-2,1,1,1)
## calculate.irr(dates,cashflows)
#
# Written by Adrian Dragulescu on 8-Oct-2005

calculate.irr <- function(dates, cashflows, options){

  if (nargs()==2){options <- NULL}
  if (length(options$bound)==0){options$bound <- 0.1}
  PV <- function(r, t, cf){sum(cf*exp(-r*t))}
  t  <- as.numeric((dates - dates[1]))/365.25
#  browser()
  options$bound <- 0.1
  sign.changes <- 0
  while (sign.changes!=1){
    rMM   <- matrix(seq(-options$bound, options$bound, by=0.05))
    PV.MM <- apply(rMM, 1, PV, t, cashflows)
    sign.changes  <- length(which(diff(sign(PV.MM))!=0))
    options$bound <- options$bound*2
  }

  if (sign.changes>1){
    res <- NA
  } else {
    res <- uniroot(PV, interval=c(-options$bound,options$bound), , , , ,
                   t, cashflows)$root
    res <- exp(res)-1  # discrete compounding to match Excel!
  }
  return(res)
}
