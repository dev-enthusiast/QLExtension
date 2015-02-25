# Calculate the average historical return and volatility of a stock
# for a given time interval (default = 1 yr).
#
# written by Adrian Dragulescu on 24-Apr-2005

xls.hist.ret.vol <- function(ticker, lag){

if (nargs()==1){lag <- 365}

source("c:/R/Work/MyLibrary/Finance/General/downloadYahoo.R")
endDate   <- Sys.Date()
startDate <- endDate-lag
res       <- downloadYahoo(ticker, startDate, endDate)

N <- nrow(res)                      # number of observations
r <- log(res[2:N,1]/res[1:(N-1),1]) # the log-returns
ind <- which(r==-Inf)
if (!(length(ind)==0)){
  paste("Error, there are %i log-returns = -Inf.", length(ind))
}

mu     <- mean(r)*252
sigma  <- sd(r)*sqrt(252)

return(list(mu=mu, sigma=sigma))
}


