# Simulate the futures curve using one factor GBM model.
# Where MarketData is a list with relevant data:
#       MarketData$asOfDate  
#       MarketData$forward.prices - current forward curves, size [C,M]
#       MarketData$forward.vols   - current forward volatilities, size [C,M]
#       MarketData$pricing.dt     - the historical dates to simulate, size D
#       MarketData$contract.dt    - the contracts to simulate, bom dates, size M
#       MarketData$Rho            - the correlation, array size [C,C,M]
#
# Returns: SimPrices, a 4D array, size [C,D,M,S]
#
# Written by Adrian Dragulescu on 20-Jan-2006

simulate.Futures <- function(MarketData, options){

  if (length(options$noSims)==0){options$noSims <- 50}
  
  C <- dim(MarketData$forward.prices)[1]  # number of commodities
  D <- length(MarketData$pricing.dt)      # number of pricing dates
  M <- length(MarketData$contract.dt)     # number of contract months
  SimPrices <- array(NA, dim=c(C,D,M,options$noSims), dimnames=list(
     rownames(MarketData$forward.prices),
     as.character(MarketData$pricing.dt),
     format(MarketData$contract.dt, "%b%y"), 1:options$noSims))
  if (any(MarketData$contract.dt<MarketData$asOfDate)){
    cat("Some contracts have already expired!  Exiting."); flush.console()
    return(SimPrices)
  }
  
  if (is.na(dim(MarketData$Rho)[3])){Rho <- array(MarketData$Rho, dim=c(C,C,M))}
  RMat <- array(NA, dim=c(C,C,M))                     # rotation matrix
  for (m in 1:M){RMat[,,m] <- t(chol(Rho[,,m]))}  

  F0  <- array(MarketData$forward.prices, dim=c(C, M, options$noSims))
  eta <- array(NA, dim=c(C, M, options$noSims))
  for (d in 1:D){
    z <- array(rnorm(C*options$noSims), dim=c(C, options$noSims, M)) 
    for (m in 1:M){eta[,m,] <- RMat[,,m] %*% z[,,m]}  # correlated increments
    ind  <- which(MarketData$contract.dt<=MarketData$pricing.dt[d])
    std  <- MarketData$forward.vols*sqrt(as.numeric(MarketData$pricing.dt[d] -
                                                   MarketData$asOfDate)/365)
    std[,ind] <- NA   # the contracts have already expired
    std2 <- 0.5*std^2
    aux  <- -eta*array(std, dim=c(C, M, options$noSims)) +
      array(std2, dim=c(C, M, options$noSims))
    SimPrices[,d,,] <- F0*exp(aux)
  }

  return(SimPrices)
}
