# Simulate N random variables by sampling from the joint probability distribution.
# The algorithm bins the observations into their quantiles and then bootstraps in 
# the quantile space.  Extra variability is added by uniformly sampling inside
# each quantile.  The [min, max] range in each variable is extended by doing
# an entropy approach like the one suggested by Vonod (see meboot package) but on
# quantiles. 
#
# Where: X - is a data frame with N variables
#        ensembles - boolean, maintain the same quantile time order as X
#                  when simulating
#        options$historical - if you want to bootstrap only
#        options$noSims - no of points to return
#
# Returns: Y the simulated variables, as a matrix c(noSims, N)
#            if ensembles=TRUE, an array with dims c(dim(X), noSims)
#
# AAD (2-Jun-08): added a parameter for returning ordered observations.
#
# AAD (19-Oct-07): added forecast, extended the
#   ranges using the entropy approach, and add a transition matrix.
#
# First version by Adrian Dragulescu on 9-Jul-2005

.demo.sample.from.Ndim <- function(){

  asOfDate <- Sys.Date()-1
  curve.name <- c("COMMOD NOX Y10 PHYSICAL", "COMMOD NOX C08 PHYSICAL")

  options <- NULL
  options$calc$hdays <- seq(asOfDate, by="-1 day", length.out=300)
  QQ <- expand.grid(curve.name=curve.name,
    contract.dt = seq(as.Date("2008-01-01"), by="month", length.out=12))

  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")
  hP <- get.prices.from.R(QQ, options)
  X  <- t(hP[,-(1:2)])

  options$no.cuts <- 10
  res <- sample.from.Ndim(X[,1:2], options)
  hist(res[,1], 50)
  windows(); hist(X[,1], 50)

  
  
  X0 <- X[nrow(X),]
  dX <- apply(X, 2, diff)
  
#  options$time.fwd <- c(1, 3, 5, 10, 20, 40, 60, 80)
  options$time.fwd <- 1:60
  options$noSims  <- 1000
  options$no.cuts <- 10
  
  res <- propagate.sample.Ndim.changes(X0, dX, options)

  windows(); bringToTop(2)
  plot(X[,1], type="l")
  
  windows(); bringToTop(3)
  matplot(t(res[1:100, 1, ]), type="l", lwd=1)

}


sample.from.Ndim <- function(X, ensembles=FALSE, options=NULL)
{
  if (length(options$historical)==0){options$historical <- FALSE}
  if (length(options$no.cuts)==0){options$no.cuts <- 10}
  if (length(options$noSims)==0){options$noSims <- 100}
  myQuantiles <- function(x, prob.cuts){quantile(as.numeric(x),
                               probs=prob.cuts, na.rm=T)}
  myFindInt <- function(x, probs){
    findInterval(x, myQuantiles(x,probs), rightmost.closed=T)} 
  options$prob.cuts <- c(0, seq(1/options$no.cuts/2, 1-1/options$no.cuts/2,
                                by=1/options$no.cuts), 1)
  N <- ncol(X)
  
  if (ensembles){
    scale <- nrow(X)
  } else {
    scale <- 1
  }
  
  if (!options$historical){
    bin.X <- apply(X, 2, myFindInt, options$prob.cuts)

    if (ensembles){
      Y <- bin.Y <- matrix(t(bin.X), ncol=N, nrow=nrow(X)*options$noSims, byrow=T)
    } else {      
      ind.Y  <- sample(1:nrow(X), options$noSims, replace=T)
      Y <- bin.Y <- bin.X[ind.Y,,drop=F]     # sampled bins
    }
      
    qX    <- apply(X, 2, myQuantiles, options$prob.cuts)
    upper <- lower <- Y
    for (c in 1:N){
      upper[,c] <- qX[bin.Y[,c]+1, c]
      lower[,c] <- qX[bin.Y[,c], c]
    }  
    eta <- matrix(runif(N*options$noSims*scale), ncol=N) # uniform samples for F(X)
    Y <- eta*(upper-lower) + lower
    # Extend the simulation range using the entropy argument  
    for (c in 1:N){               
      ind   <- which(X[,c] > qX[nrow(qX)-1,c])
      theta <- mean(X[ind,c]) - qX[nrow(qX)-1,c]  # parameter of the Upper Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == (length(options$prob.cuts)-1))
        Y[ind,c] <- qX[nrow(qX)-1,c] + (-theta*log(1-eta[ind,c]))
      }
      ind   <- which(X[,c] < qX[2,c])
      theta <- qX[2,c] - mean(X[ind,c])           # parameter of the Lower Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == 2)
        Y[ind,c] <- qX[2,c] - (-theta*log(1-eta[ind,c]))
      }
    }
  } else {  # bootstrap from historical values only
    ind.Y  <- sample(1:nrow(X), options$noSims, replace=T)
    Y <- X[ind.Y,]    
  }
  if (ensembles){
    Y <- array(Y, dim=c(scale, options$noSims, N))
    Y <- aperm(Y, c(1,3,2))
  }
  
  return(Y)
}


###############################################################
# Propagate an initial vector a number of forward time 
# steps.
# X0    - the initial vector, size N
# dX    - the historical changes, matrix size [H, N]
# nsims - the number of simulations
# time.fwd - the forward time steps, same units as the changes, 
#            c(1, 2, 3, 5, 10, 20, ...)  
#
propagate.sample.Ndim.changes <- function(X0, dX, options, acceptance.fun=NULL)
{
  loptions <- options
  nsim <- options$noSims
  N  <- length(X0)
  X0 <- matrix(X0, nrow=nsim, ncol=N, byrow=T)
  Y  <- array(NA, dim=c(nsim, N, length(options$time.fwd)))  # prediction
  dt <- diff(c(0, options$time.fwd))
  for (ind in 1:length(options$time.fwd)){
    reject <- rep(TRUE, nsim)     # accept/reject changes depending on
    while (any(reject)){           # a given acceptance function.
      nsim <- length(reject)
      loptions$noSims <- nsim * dt[ind]    
      aux <- sample.from.Ndim(dX, loptions)
      aux <- aperm(array(t(aux), c(N, nsim, dt[ind])), c(2,1,3))
      if (dim(aux)[3]>1){
        aux <- apply(aux, c(1,2), sum) # aggregate the increments
      } else {
        aux <- aux[,,,drop=T]
      }
      pY <- X0 + aux              # proposed simulated values
      accept <- validate.values(pY, acceptance.fun, options$boundaries)
      Y[accept,,ind] <- pY[accept,]  # collect the results 
      reject[accept] <- FALSE     # these sims have been accepted
    }
    X0  <- Y[,,ind]               # the new initial values
  }
  return(Y)  # size [nsim, length(X0), length(time.fwd)]
}

validate.values <- function(pY, acceptance.fun="fermi.dirac", boundaries)
{
#  prob <- fermi.dirac(pY, 0, std)
  return(rep(TRUE, nrow(pY)))        # do nothing for now                
}

fermi.dirac <- function(x, location, scale){   # logistic distribution
  return(1/(exp((x-location)/scale)+1))  
}



###############################################################
# Calculate the transtion matrix between different quantiles.
# Needed if you want to take care of autocorrelation. 
make.transition.matrix <- function(bin.X)
{ 
  TM <- vector("list", length=ncol(bin.X)) # one matrix per curve.name
  for (ind in 1:ncol(bin.X)){
    aux <- as.numeric(bin.X[,ind])
    MM  <- table(aux[-length(aux)], aux[-1])
    MM  <- sweep(MM, 2, colSums(MM), "/")  # calculate the probabilities
    TM[[ind]] <- MM 
  }
  return(TM)
}



###############################################################
# Propagate an initial vector a number of forward time 
# steps.
# X     - the historical levels, matrix size [H, N]
# nsims - the number of simulations
# time.fwd - the forward time steps, same units as the changes, 
#            c(1, 2, 3, 5, 10, 20, ...)  
#
propagate.multi.step.levels <- function(X, nsim=1000, time.fwd){

  if (length(options$historical)==0){options$historical <- FALSE}
  if (length(options$historical)==0){options$do.paths   <- FALSE}
  if (length(options$no.cuts)==0){options$no.cuts <- 10}
  myQuantiles <- function(x){quantile(x, probs=options$prob.cuts, na.rm=T)}
  myFindInt <- function(x){findInterval(x, myQuantiles(x),rightmost.closed=T)} 
  options$prob.cuts <- seq(0,1,1/options$no.cuts)

  N <- ncol(X)
  Y <- bin.Y <- matrix(NA, nrow=options$noSims, ncol=N) # simulated values
  bin.X <- apply(X, 2, myFindInt)

  TM <- make.transition.matrix(bin.X)
  X0 <- bin.X[nrow(bin.X),]

  for (c in 1:length(TM)){
    

  }
  

  

  ind.Y  <- sample(1:nrow(X), options$noSims, replace=T)
  


  


  X0 <- matrix(X0, nrow=nsim, ncol=N, byrow=T)
  Y  <- array(NA, dim=c(nsim, N, length(time.fwd)))  # prediction
  dt <- diff(c(0, time.fwd))
  for (ind in 1:length(time.fwd)){
    loptions$noSims <- nsim * dt[ind]
    aux <- sample.from.Ndim(dX, loptions)
    aux <- aperm(array(t(aux), c(N, nsim, dt[ind])), c(2,1,3))
    if (dim(aux)[3]>1){
      aux <- apply(aux, 3, sum)  # aggregate the increments
    } else {
      aux <- aux[,,,drop=T]
    }
    Y[,,ind] <- X0 + aux
    X0  <- Y[,,ind]              # the new initial value
  }
  return(Y)  # size [nsim, length(X0), length(time.fwd)]
}


## propagate.one.step.levels <- function(X0, TM){


## }











## sample.from.Ndim.MLE <- function(X, options=NULL){

##   # Implement the MLE extension on the min/max values...
 
## }


#  x <- x + (10^-10)*runif(length(x)) # to make sure that quantiles are distinct
#   eta.sy  <- runif(options$noSims)  # simulate uniform y
#   sim.y   <- array(NA, dim=options$noSims)
#   for (bin in 1:options$no.cuts.x){                 
#     ind   <- which(bin.sx==bin)
#     sy    <- sort(y[which(bin.x==bin)])  # y from the given x band cut
#     ly    <- length(sy)
#     cdf.y <- approx(seq(0, 1, by=1/length(sy)) , c(sy[1], sy), eta.sy[ind])
#     sim.y[ind] <- cdf.y$y
#   }
#   return(data.frame(sim.x=options$sim.x, sim.y=sim.y))

#    bin.Y[,n] <- myFindInt(Y[,n], myQuantiles(X[,n]))

##    TM <- array(0, c(options$no.cuts, options$no.cuts, ncol(bin.X)))
##    aux <- factor(bin.X[,ind], levels=1:options$no.cuts)
##     normalize <- function(x){
##       if (sum(x)>0){x <- x/sum(x)}
##       return(x)
##     }
##     MM <- apply(MM, 2, normalize)

