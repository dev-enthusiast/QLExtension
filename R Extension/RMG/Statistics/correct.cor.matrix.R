# Implement an algorithm of correcting the correlation matrix such that the 
# eigenvalues are all positive.  From a paper by Rebonato [99] - "The most
# general methodology to create a valid correlation matrix for risk management
# and option pricing purposes".  Implements two methods, "spectral.cut" which
# is fast and blunt, and "hypersphere" which does some optimization to get  
# close to the original matrix.  
#
# CC  <- matrix(c(1, 0.9, 0.7, 0.9, 1, 0.3, 0.7, 0.3, 1), ncol=3)
# source("H:/user/R/RMG/Statistics/correct.cor.matrix.R")
# correct.cor.matrix(CC)
#
# Last modified by Adrian Dragulescu on 5-Mar-2007


correct.cor.matrix <- function(CC, options){
  
  if (nargs()==1){options <- NULL}
  if (length(options$method)==0){options$method <- "spectral.cut"}

  res <- NULL
  res <- eigen(CC, symmetric=T)
  RR  <- res$vectors
  # rotation matrix RR %*% DD %*% t(RR) = CC     with    RR %*% t(RR)=1
  DD  <- diag(res$values)
  #------------------------------------------------------------------------
  if (options$method=="spectral.cut"){
    evals <- ifelse(res$values>0, res$values, 0)
    TT <- diag(as.numeric(1/(RR^2 %*% evals)))
    BB <- sqrt(TT) %*% RR %*% sqrt(diag(evals))
    corrected.CC <- BB %*% t(BB)
  }
  #------------------------------------------------------------------------
  if (options$method=="hypersphere"){
    source("H:/user/R/RMG/Utilities/Math/polar.coordinates.R")
    res.0 <- correct.cor.matrix(CC)        # do a sprectral.cut first

    min.Fun <- function(theta, CC.0){
      theta <- matrix(theta, (dim(CC.0)[1]-1))
      BB <- t(apply(theta, 2, polar.coordinates, "to.cartesian"))
      CC <- BB %*% t(BB)
      return(sum((CC-CC.0)^2))   # target is the starting C
    }
    BB.0 <- res.0$rotation.mat
    CC.0 <- res.0[[1]]
    theta.0 <- as.numeric(apply(BB.0, 1, polar.coordinates, "to.polar"))
    
    res <- optim(theta.0, min.Fun, gr=NULL, method="Nelder-Mead", lower=-Inf,
       upper=Inf, control=list(), hessian=FALSE, CC.0)
    if (res$convergence!=0){stop("Optimization failed! \n")}
    theta <- matrix(res$par, (dim(CC.0)[1]-1))
    BB <- t(apply(theta, 2, polar.coordinates, "to.cartesian"))
    corrected.CC <- BB %*% t(BB)
    sum(corrected.CC^2)
    evals <- eigen(corrected.CC, only.values=TRUE)$values # all eigenvalues>0
  }
  dimnames(corrected.CC) <- dimnames(CC)
  return(list(CC=corrected.CC, rotation.mat=BB, eigenvals=evals,
              optim.res=res))
}


