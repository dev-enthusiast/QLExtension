# Generate Ornstein-Uhlenbeck paths:
#     dx_t = -\gamma(x_t - \theta)dt + \sigma dW_t
#
# The simulation algorithm is not Euler.  It preserves the first and
# second moments exactly. 
#
# where: parms$sigma, parms$gamma, parms$theta
#        parms$x0   is the initial value of the assset (scalar)
#        options$M  is the number of time-steps
#        options$Tx is the time to expiration (scalar)
#        options$S = [1] is the number of simulations
#                                        
# returns x the simulated asset value (matrix, size [S,M+1])
#
# written by Adrian Dragulescu on 24-Jun-2004

simulateOU <- function(parms,options){

  if (length(options$S)==0){options$S <- 1} # default 1 simulation
  if (length(parms$x0)==0){parms$x0 <- 0} # start from 0
 
  sigma <- parms$sigma;  gamma <- parms$gamma
  theta <- parms$theta;
  v <- sigma^2/(2*gamma)                     # asymptotic variance
  x <- array(0,c(options$S,options$M+1))
  dW <- rnorm(options$M*options$S,0,sqrt(v)) # generate random deviates
  dim(dW) <- c(options$S,options$M)          # reshape them
  x[,1]   <- parms$x0                        # the initial value

  dt <- options$Tx/options$M
  f1 <- exp(-gamma*dt)
  f2 <- sqrt(1-f1^2)
 
  for (i in 1:options$M){
    x[,i+1] <- theta + f1*(x[,i]-theta) + f2*dW[,i]
  }
  return(x)   # the simulated paths
}
