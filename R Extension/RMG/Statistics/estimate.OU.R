# Estimate the parameters of an OU process via mle.  The idea is to
# express the distribution of x_{n+1} given x_n.  This conditional
# distribution is Gaussian.
#  dx_t = -\gamma*(x_t - \theta)dt + \sigma dW_t
# The exact solution is 
#  x_t = x_0 + \theta(1-e^{-\gamma t})
#      + \sigma\sqrt{(1-e^{-2\gamma t})/(2\gamma)} dz
#
# The estimated parameters are: theta = [gamma, sigma, theta]
#
# Where: options$dt is the time step (scalar)
#
# Written by Adrian Dragulescu on 8-Nov-2004

estimate.OU.mle <- function(x, options){

  require(stats4)  # for mle
  llike.ou <- function(gamma,sigma,theta){
    M <- length(x)-1
    aux <- exp(-gamma*dt)
    mu  <- theta + aux*(x[1:M]-theta)
    sd  <- sigma*sqrt((1-aux^2)/(2*gamma))
    z   <- (x[2:(M+1)]-mu)/sd
    return(M*log(2*pi)+0.5*sum(z^2 + 2*log(sd)))  # log likelihood
  }
  dt <- options$dt                 # the time step
  M <- length(x)-1                 # number of increments     
  reg <- lm(x[2:M] ~ x[1:(M-1)])   # autocorrelation
  gamma.0 <- -log(as.numeric(coefficients(reg)[2]))/dt 
  sigma.0 <- sqrt(2*gamma.0*var(x))
  theta.0 <- mean(x)  

  theta.0 <- list(gamma=gamma.0, sigma=sigma.0, theta=theta.0)
  res <- mle(llike.ou, start=theta.0, method="L-BFGS-B",
             lower=c(0,0,-Inf), control=list(trace=3))
  summary(res)
#   prof.res <- profile(res); plot(prof.res)
#   cind <- confint(res);     cind
  return(list(coef(res), res))
}

## estimate by linear regression. 
estimate.OU.regress <- function(x, options, resid=F, sd=F){
  require(MASS)
  dt <- options$dt                 # the time step
  M <- length(x)                 # input length
  x[abs(x)<1e-6] <- 0
  lret <- diff(x)
  
  res <- NULL       # output results
  std <- c(0,0,0)   # initialize std
  names(std) <- c("gamma", "sigma", "theta")
  ## some procedure to protect error
  if(all(lret==0)) {## input is a flat line
    res$coeff <- c(1,0,x[1])
    names(res$coeff) <- c("gamma", "sigma", "theta")
    if (resid){res$resid <- lret}
    if (sd){res$sd <- std}
    return(res)
  }

  ## regression
  fit <- lm(lret~x[-M])
  cc  <- coef(fit)
  ss  <- summary(fit)
  std.lm <- ss$coefficients[,2]
  ## problem could happen if there're outliers in the data
  ## such as a quick spike up and down will cause
  ## estimated cc[2] less than -1, causes taking log
  ## on negative numbers.
  ## switch to rlm in that case.
  ## I don't alway use rlm because it's much slower
  ## Remove following rlm part because it's unstable.
  ## Sometimes it produces NAs for estimates 
  ##  if(cc[2] <= -1) {
  ##    fit <- rlm(lret~x[-M])
  ##    cc <- coef(fit)
  ##    if(is.na(cc[2])) ## rlm returns NA sometimes, weird
  ##      cc[2] <- -1e-5
  ##  }
  ## if cc[2] still < -1, make it small negative number and issue warning

  ## Note cc[2] should be a small negative number.
  ## If it is not, make following fake numbers:
  ## gamma: a small positive number
  ## theta: median of x
  ## sigma: sd of lret
  ## This way the process is almost a GBM (given gamma is very small)
  ## but there's weak force to drive it back to long term median.

  if (any(is.na(cc)) | cc[2]<= -1 | cc[2]>0) {
    warning("Estimating OU failed ... \n")
    theta <- median(x)
    gamma <- 1e-5
    sigma <- sd(lret)
  }
  else {
    theta <- -cc[1]/cc[2]
    gamma <- -log(1+cc[2]) / dt
    sigma <- sd(resid(fit))*sqrt(2*gamma/(1-(1+cc[2])^2))

    std["gamma"] <- (1/(1+cc[2]))*(1/dt)*std.lm[2]
    std["theta"] <- sqrt((std.lm[1]/cc[2])^2 + (cc[1]*std.lm[2]/cc[2]^2)^2)
  }
  
  res <- NULL
  res$coeff <- c(gamma, sigma, theta)
  names(res$coeff) <- c("gamma", "sigma", "theta")
  if (resid){res$resid <- resid(fit)} 
  if (sd){res$sd <- std}
  
  return(res)
}


estimate.OU <- function(x, options){
  if (length(options$method)==0){options$method <- "mle"}
  if (length(options$dt)==0){options$dt <- 1}
  res <- switch(options$method,
    mle     = estimate.OU.mle(x, options),
    regress = estimate.OU.regress(x, options))
  if(res[[1]]["gamma"]<0)
    warning("Negative estimated gamma, result is invalid\n")
  return(res)
}


