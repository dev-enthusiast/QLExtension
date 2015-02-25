# Estimate the parameters of an GBM process via mle.  The idea is to
# express the distribution of log(x_{n+1}/x_n).  This conditional
# distribution is normal.
#   dx_t = mu*x_t*dt + sigma*x_t*dW_t 
# The estimated parameters are: theta = [mu, sigma]
#
# Where: options$dt is the time step (scalar)
#
# Written by Adrian Dragulescu on 8-Nov-2004


estimate.GBM <- function(x, options){

  require(stats4)  # from mle 
  llike.gbm <- function(mu,sigma){
    M <- length(x)-1
    r <- log(x[2:(M+1)]/x[1:M])          # returns
    mean  <- (mu - 0.5*sigma^2)*dt
    std   <- sigma*sqrt(dt)
    z     <- (r-mean)/std
    return(M*log(2*pi)+0.5*sum(z^2 + 2*log(std))) # log likelihood
  }

  
  #-----------------------------------------------MAIN---------
  if (length(options$dt)==0){options$dt <- 1}
  dt <- options$dt                 # the time step
  M <- length(x)-1                 # number of increments     
  r <- log(x[2:(M+1)]/x[1:M])
  sigma.0 <- sd(r)/sqrt(dt)
  mu.0    <- mean(r)/dt + 0.5*sigma.0^2
  
  theta.0 <- list(mu=mu.0, sigma=sigma.0); theta.0
  res <- mle(llike.gbm, start=theta.0, method="L-BFGS-B",
             lower=c(-Inf,10^(-10)), control=list(trace=3))
  summary(res)
#   prof.res <- profile(res); plot(prof.res)
#   cind <- confint(res);     cind
  return(res)
}






#   x <- rnorm(100,0,1)
#   llike.gauss <- function(theta){
#     err <- (x-theta[1])/theta[2]
#     return(sum(err^2 + 2*log(theta[2])))
#   }
  
#   sol <- optim(c(0,1), llike.gauss, NULL, method="L-BFGS-B",
#          control=list(trace=3))
#   sol$par


