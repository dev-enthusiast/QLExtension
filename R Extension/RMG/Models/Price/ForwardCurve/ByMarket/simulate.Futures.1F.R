# Simulate the futures curve using one factor GBM model.
# Where MarketData is a list with relevant data:
#       MarketData$asOfDate  
#       MarketData$forward.prices - current forward curves, matrix size [C,M]
#       MarketData$forward.vols   - current forward volatilities, matrix size [C,M]
#       MarketData$pricing.dt     - the historical dates to simulate, vector size D
#       MarketData$contract.dt    - the contracts to simulate, bom dates, vector size M
#       MarketData$Rho            - the correlation, array size [C,C,M]
#
# Returns: SimPrices, a 4D array, size [C,D,M,S]
#
# Written by Adrian Dragulescu on 20-Jan-2006

simulate.Futures.1F <- function(MarketData, options){

  if (length(options$noSims)==0){options$noSims <- 50}
  
  MarketData$pricing.dt <- c(MarketData$asOfDate, MarketData$pricing.dt) 
  C <- length(MarketData$curve.name)      # number of curve names
  D <- length(MarketData$pricing.dt)      # number of pricing dates
  M <- length(MarketData$contract.dt)     # number of contract months
  if (any(MarketData$contract.dt<MarketData$asOfDate)){
    cat("Some contracts have already expired!  Exiting."); flush.console()
    return(NULL)
  }
  
#  RMat <- array(NA, dim=c(C,C,M))                     # rotation matrix
#  for (m in 1:M){RMat[,,m] <- t(chol(MarketData$Rho[,,m]))}
  RMat <- t(chol(MarketData$Rho, pivot=TRUE))  # rotation matrix
  dimnames(RMat) <- dimnames(MarketData$Rho)

  eta <- array(NA, dim=c(C, M, options$noSims))
  max.no.sims <- trunc(memory.limit()/object.size(MarketData$Initial))
  cat("The maximum no of sims you can do is ", max.no.sims, ".\n", sep="")

  SimPrices <- vector(mode="list", length=D)
  names(SimPrices) <- MarketData$pricing.dt
  SimPrices[[1]] <- cbind(pricing.dt=rep(options$asOfDate, nrow(MarketData$Initial)),
                          MarketData$Initial)
  for (d in 2:D){                                 # loop over pricing.dts
    cat("Simulating prices for pricing date ", as.character(MarketData$pricing.dt[d]),
        "...", sep="")
    gc()
    dt  <- as.numeric(MarketData$pricing.dt[d] - MarketData$pricing.dt[d-1])/365
    z   <- matrix(rnorm(C*options$noSims), nrow=C, ncol=options$noSims)
    eta <- RMat %*% z                             # correlated increments 

    VV  <- subset(MarketData$Initial, contract.dt >= MarketData$pricing.dt[d])
    MDI <- subset(cbind(ind=1:nrow(SimPrices[[d-1]]),
                        SimPrices[[d-1]][, c("curve.name", "contract.dt")]),
                        contract.dt >= MarketData$pricing.dt[d])
    cm  <- nrow(MDI)
    ind.eta <- data.frame(curve.name=MarketData$curve.name, ind.eta=1:C)
    MDI <- merge(MDI, ind.eta)  # find how to allocate the shock by commodity

    aux <- sqrt(dt)*eta
    aux <- aux[MDI$ind.eta, ] * VV$vol  # this is OK because of 1 factor model!  
    aux <- exp(aux)             
    
    if (d==2){
      SimPrices[[1]] <- matrix(SimPrices[[1]][,"price"], nrow=nrow(SimPrices[[1]]),
                                                         ncol=options$noSims)
      colnames(SimPrices[[1]]) <- paste("sim.", 1:options$noSims, sep="")
    } 
    ind.sims <- grep("sim", colnames(SimPrices[[d-1]]))
    aux <- SimPrices[[d-1]][MDI$ind,ind.sims]*aux           # <-- these are the prices
    SimPrices[[d]] <- cbind(pricing.dt=rep(MarketData$pricing.dt[d], nrow(MDI)),
                       MDI[, c("curve.name", "contract.dt")], aux)
    
    if (options$save$price.sims){
      if (length(options$save$price.sims.dir)==0){
        options$save$price.sims.dir <- "R:/SimPrices/"
      }
      file <- paste(options$save$price.sims.dir,  "Base/sprices.asOf.",
                    MarketData$pricing.dt[d], ".RData", sep="")
      sPP <- SimPrices[[d]]
      rownames(sPP) <- NULL
      save(sPP, file=file)
      SimPrices[[d-1]] <- list()
    }
    cat("Done\n")
  } # end of loop over pricing.dts
  SimPrices[[1]] <- NULL
  return(SimPrices)
}
