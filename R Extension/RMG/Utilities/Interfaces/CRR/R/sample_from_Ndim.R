# Simulate N random variables by sampling from the joint probability
# distribution.  The algorithm bins the observations into their
# quantiles and then bootstraps in the quantile space.  Extra
# variability is added by uniformly sampling inside each quantile.
# The [min, max] range in each variable is extended by doing
# an entropy approach like the one suggested by Vonod
# (see meboot package) but on quantiles. 
#
# @param X a numerical matrix with N variables on columns
# @param historicalOnly if you want to bootstrap from historical values
#   only
# @param ensembles a boolean.   If TRUE the return data Y maintains the indexing 
#   order from X[1,] (so only modest variance in simulated values is achieved).
#   If ensembles=FALSE, you lose the time-ordered results, and the variability
#   of the simulated distribuition increases a lot.  You should use this if your
#   input data X comes from mixing of distributions via weighting, and the order
#   of the result Y is no longer important.
# @param noSims numeric number of simulations to return.  
# @param noCuts how many cuts you use in the probability space
# @param jitterAmount by how much you should jitter identical values
#   in a quantile bin.
# @return Y the simulated variables. If ensembles=FALSE an array [noSims, N],
#   or if ensembles=TRUE an array with dims [dim(X), N, noSims]
#
# First version by Adrian Dragulescu on 9-Jul-2005
# Retouched on 14-Oct-2013, improved docs
#
sample_from_Ndim <- function(X, historicalOnly=FALSE, ensembles=TRUE,
  noSims=1000, noCuts=10, jitterAmount=10^-10)
{
  if (class(X)[1] == "zoo")
    X <- as.matrix(X)          # maybe not the best way to do it
  
  myQuantiles <- function(x, prob.cuts) {
    quantile(as.numeric(x), probs=prob.cuts, na.rm=T)
  }
  myFindInt <- function(x, probs) {
    findInterval(x, sort(myQuantiles(x,probs)), rightmost.closed=T)
  }
  
  N <- ncol(X)  # number of variables
  prob.cuts <- c(0, seq(1/noCuts/2, 1-1/noCuts/2, by=1/noCuts), 1)

  adjustNoSims <- if (ensembles) nrow(X) else 1
  
  # if you haved less than 10 distinct values for a variable, jitter it
  nUniques <- apply(X, 2, function(x){ length(unique(x))} )
  ind <- which(nUniques <= trunc(nrow(X)/noCuts))
  if (length(ind)>0)
    X[,ind] <- jitter(X[,ind], jitterAmount)

  
  if ( historicalOnly ) {
    # sample from historical values only
    ind.Y  <- sample(1:nrow(X), noSims*adjustNoSims, replace=T)
    Y <- X[ind.Y,]    
    
  } else {
    # bucket each observation into its corresponding quantile
    bin.X <- apply(X, 2, myFindInt, prob.cuts)  

    if (ensembles) {
      Y <- bin.Y <- matrix(t(bin.X), ncol=N, nrow=nrow(X)*noSims, byrow=T)
    } else {      
      ind.Y  <- sample(1:nrow(X), noSims, replace=T)
      Y <- bin.Y <- bin.X[ind.Y,,drop=F]     # sampled bins
    }

    qX    <- apply(X, 2, myQuantiles, prob.cuts)
    upper <- lower <- Y
    for (c in 1:N) {
      upper[,c] <- qX[bin.Y[,c]+1, c]
      lower[,c] <- qX[bin.Y[,c], c]
    }
    # uniform samples for F(X), 0 < eta < 1
    eta <- matrix(runif(N*noSims*adjustNoSims), ncol=N) 
    Y <- eta*(upper-lower) + lower
    
    # Extend the simulation range using the entropy argument  
    for (c in 1:N) {               
      ind   <- which(X[,c] > qX[nrow(qX)-1,c])
      theta <- mean(X[ind,c]) - qX[nrow(qX)-1,c]  # parameter of the Upper Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == (length(prob.cuts)-1))
        Y[ind,c] <- qX[nrow(qX)-1,c] + (-theta*log(1-eta[ind,c]))
      }
      ind   <- which(X[,c] < qX[2,c])
      theta <- qX[2,c] - mean(X[ind,c])           # parameter of the Lower Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == 2)
        Y[ind,c] <- qX[2,c] - (-theta*log(1-eta[ind,c]))
      }
    }
  } 

  if (ensembles) {
    # reshape Y to size [nrow(X), N, noSims] 
    Y <- array(Y, dim=c(adjustNoSims, noSims, N))  
    Y <- aperm(Y, c(1,3,2))                   
  } 
  
  return(Y)
}


################################################################
################################################################
#
.test <- function()
{
  require(CEGbase)
  require(SecDb)
  require(FTR)
  source("H:/user/R/RMG/Utilities/Interfaces/FTR2/R/sample_from_Ndim.R")
  
  symbs <- c("PJMDAC_G3851_138_KV_TR2_12_COME", "PJMDAC_55_HEGEW_138_KV_CIDGRF_C",
    "PJMDAC_458_GREE_138_KV_TR71_12_", "PJMDAC_LAPORTE_34_KV_ROLLINGP_A")

  startDt <- as.POSIXct("2013-05-01 01:00:00")
  endDt   <- as.POSIXct(paste(Sys.Date(), "00:00:00"))

  X <- FTR.load.tsdb.symbols(symbs, startDt, endDt, newvars=NULL)

  source("H:/user/R/RMG/Utilities/Interfaces/FTR2/R/sample_from_Ndim.R")
  Y1 <- sample_from_Ndim(X)

  Y2 <- sample_from_Ndim(X, historicalOnly=TRUE)
    
  Y3 <- sample_from_Ndim(X, ensembles=FALSE)

  Y4 <- sample_from_Ndim(X, historicalOnly=TRUE, ensembles=FALSE)


  symbs <- c("PJMDAC_G3851_138_KV_TR2_12_COME")
  X <- FTR.load.tsdb.symbols(symbs, startDt, endDt, newvars=NULL)
  


}
