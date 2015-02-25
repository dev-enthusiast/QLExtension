#
# Generate Brownian Motion paths: dx_t = mu*dt + sigma*dW_t
#
# where: M          is the number of time-steps
#        T          is the time to expiration (scalar)  
#        options$x0 is the initial value of the assset (scalar)
#        options$N  is the number of simulations
#        
#
# returns x the simulated asset value (matrix, size Mx(N+1))
#
# written by Adrian Dragulescu on 10-Feb-2003
# ported to R on 12-May-2003

simulateBM <- function(mu,sigma,T,M,options){

  if (nargs()==4){options <- list()}
  if (length(options$N)==0){options$N <- 1} # default 1 simulation
  if (length(options$x0)==0){options$x0 <- 0} # start from 0
  
  x <- array(0,c(options$N,M+1))
  dW <- rnorm(M*options$N,0,sigma) # generate random deviates
  dim(dW) <- c(options$N,M)        # reshape them
  x[,1] <- options$x0              # the initial value

  dt <- T/M
  mudt <- mu*dt*array(1,c(options$N,1)) 
  sigmadt2 <- sigma*sqrt(dt)

  for (i in 1:M){
    x[,i+1] <- x[,i]+ mudt + sigmadt2*dW[,i]
  }
  x   # the simulated paths
}
