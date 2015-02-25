# Extend the forward price simulations to contract dates beyond the ones 
# that are simulated.  Where
#   curve.name and max.contract can be vectors
#
# Written by Adrian Dragulescu on 1-Oct-2007

source("H:/user/R/RMG/Utilities/Database/R/read.forward.price.sims.R")

.demo.extend.price.sims <- function(){
  
  path.to.sims <- "R:/SimPrices/Network/2007-09-26/interactive/test_AAD/result/"
  curve.names   <- c("COMMOD NG EXCHANGE")
  max.contracts <- as.Date("2017-12-01")     # max contract to extend to
  
  asOfDate <- as.Date("2007-09-26")
  load(paste("S:/All/Risk/Data/VaR/prod/Prices/hPrices.",  
    asOfDate, ".RData", sep=""))                  # load asOfDate prices 
  hP <- subset(hP, curve.name %in% curve.names)
  names(hP)[grep("-", names(hP))] <- "price"
  
  source("H:/user/R/RMG/Utilities/Database/R/extend.price.sims.R")
  res <- extend.price.sims(curve.names, max.contracts, hP, path.to.sims)
  
  return(res)
  
}

#################################################################################
# Add contract dates past the 4 yr we simulate. hP is a data frame with 
# columns: curve.name, contract.dt, price
#
extend.price.sims <- function(curve.names, max.contracts, hP, path.to.sims, env=NULL)
{

  seP <- vector(mode="list", length=length(curve.names)) # simulated extended prices
  names(seP) <- curve.names
  for (ind in 1:length(curve.names)){

    res = read.forward.price.sims( curve.names[ind], 
            path.to.filehash = path.to.sims, env=env )[1,,,]

    contract.dts <- seq(as.Date(dimnames(res)[[2]][1]), max.contracts[ind], by="month")
    
    # If we don't need to extend the simulation, go to the next curve
    simulatedContracts = colnames(res)
    if( max.contracts <= simulatedContracts[length(simulatedContracts)] )
    {
      rLog("No Need To Extend For Curve:", curve.names[ind])
      seP[[ind]] = res
      next
    }
    
    rLog("Extending Simulations For:", curve.names[ind])
    dim.sP  <- dim(res)
    dim.seP <- dim.sP; dim.seP[2] <- length(contract.dts)
    

    seP[[ind]] <- array(NA, dim=dim.seP, dimnames=list(dimnames(res)[[1]],
      as.character(contract.dts), dimnames(res)[[3]]))
    ind.max <- length(dimnames(res)[[2]])
    seP[[ind]][,1:ind.max,] <- res           # assign the old prices
    
    F0 <- subset(hP, (hP$curve.name == curve.names[ind] &
      hP$contract.dt %in% contract.dts))
    F0 <- F0[order(F0$contract.dt),]
    if (length(contract.dts)!=nrow(F0))
    { 
      stop("Don't have prices as far as you want to extend!  Please check.")
    }

    F0.last <- F0[ind.max,"price"]
    dP  <- res[,ind.max,] - F0.last   # changes in prices for the last sim. contract
    #extend for the extra contract months  for the first 4 years of pricing dates
    for (ind.ex in (ind.max+1):length(contract.dts)){  # loop over the months you extend
      seP[[ind]][,ind.ex,] <- F0[ind.ex,"price"] + dP
    }    
  }
  return(seP)  # return simulated extended prices as a list.
}

#    F0.last <- subset(F0, contract.dt == as.Date(dimnames(res)[[2]][ind.max]))$price

#######################################################################################
## This function will consider the need to extend the curves due to longer contracts (>48 months)
## However, it won't extend directly, like in extend.price.sims. Instead, it will calculate a dP matrix
## and return F0 and dP matrix for extension in the calling host
## The purpose of this is to avoid the possibility of extending the result matrix to a very large one
## written by Jing Bai, 2008-05-19

extend.onecurve.sims <- function(curve.name, max.contract, hP, path.to.sims, env=NULL)
{
  seP <- vector(mode="list", length=2)
  names(seP)=c(curve.name, "dP.matrix")
  res = read.forward.price.sims( curve.name, 
            path.to.filehash = path.to.sims, env=env )[1,,,]
  contract.dts <- seq(as.Date(dimnames(res)[[2]][1]), max.contract, by="month")
  seP[[1]] <- res
    
  # If we don't need to extend the simulation, go to the next curve
  simulatedContracts = colnames(res)
  if( max.contract <= simulatedContracts[length(simulatedContracts)] )
  {
    rLog("No Need To Extend For Curve:", curve.name)
    seP[[2]] <- NA
    return(seP)
  }
  #otherwise, need to extend simulation, but return a price change matrix instead of extending right away
  rLog("Need to extend Simulations For:", curve.name)
  F0 <- subset(hP, (hP$curve.name == curve.name &
                hP$contract.dt %in% contract.dts))
  F0 <- F0[order(F0$contract.dt),]
  if (length(contract.dts)!=nrow(F0))
  { 
    rLog("Don't have prices as far as you want to extend!  Please check.")
    return(NULL)
  }
  ind.max <- length(dimnames(res)[[2]])
  F0.last       <- F0[ind.max,"price"]
  dP            <- res[,ind.max,] - F0.last 
  seP$dP.matrix <- dP
  return(seP)
}