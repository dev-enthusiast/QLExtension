# Generate Brownian Motion paths: dx_t = mu*x_t*dt + sigma*x_t*dW_t
# 
# where: parms$mu, parms$sigma 
#        options$M  is the number of time-steps
#        options$Tx is the time to expiration (scalar)  
#        options$x0 is the initial value of the assset (scalar)
#        options$N  is the number of simulations
#        
# returns x the simulated asset value (matrix, size Mx(N+1))
#
# written by Adrian Dragulescu on 10-Feb-2003
# ported to R on 12-May-2003

simulateGBM <- function(parms,options){

  if (nargs()==4){options <- list()}
  if (length(options$N)==0){options$N <- 1}   # default 1 simulation
  if (length(parms$x0)==0){parms$x0 <- 100}   # start from 100

  mu <- parms$mu; sigma <- parms$sigma
  x <- array(0,c(options$N,options$M+1))
  dW <- rnorm(options$M*options$N,0,1)     # generate random deviates
  dim(dW) <- c(options$N,options$M)        # reshape them
  x[,1]   <- parms$x0                      # the initial value

  dt   <- options$Tx/options$M
  mudt <- mu*dt*array(1,c(options$N,1)) 
  sigmadt  <- sigma*sqrt(dt)
  sigmadt2 <- 0.5*sigmadt^2

  for (i in 1:options$M){
    x[,i+1] <- x[,i]*(1 + mudt + sigmadt*dW[,i])  # Euler
#    x[,i+1] <- x[,i]*exp(mudt-sigmadt2 + sigmadt*dW[,i])
  }
  return(x)   # the simulated paths
}
