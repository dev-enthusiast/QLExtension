# Implementation of the spread option model from Haug.  Only calls. 
# The payoff is: max(F1-HR*F2-K,0)
# Where data is a data frame with columns
#       data = [F1 F2 V1 V2 Rho K Tx DF HR]
#   F1, F2 the prices of the two underliers
#   V1, V2 the volatilities of the two underliers
#   K      the strike
#   Tx     the time to expiration
#   Rho    the correlation
#   DF     the discount factor
#   HR     the heatrate (multiplier for F2)
#   options$what <- c(value=1, deltas=0, vegas=0, gammas=0, delta.rho=0)  # what to return
#   options$pert   = 0.001 by how much to perturb multiplicatively
#   options$CI     = 0.05 the precision of the 95% CI for MC method
#
# Written by Adrian Dragulescu on 4-Nov-2004


#---------------------------------------------------------------
method.kirk <- function(data){
  Fb  <- data$F1/(data$HR*data$F2 + data$K)         # effective price
  vol2.mod <- data$V2*data$HR*data$F2/(data$HR*data$F2 + data$K) 
  vol  <- data$V1^2 + vol2.mod^2 - 2*data$Rho*data$V1*vol2.mod
  vol  <- sqrt(ifelse(vol<0, 0, vol))       # take care of rounding error 
  vT   <- vol*sqrt(data$Tx) 
  d1   <- log(Fb)/vT + vT/2;  d2 <- d1 - vT
  Nd1 <- pnorm(d1, mean=0, sd=1) 
  Nd2 <- pnorm(d2, mean=0, sd=1)
  res <- data$DF*(data$F1*Nd1 - (data$HR*data$F2+data$K)*Nd2)
  return(list(value=res))
}
#---------------------------------------------------------------
method.MC <- function(data){
  S     <- 2048  # number of simulations
  N     <- nrow(data)
  res   <- array(NA, dim=N)
  ci.95 <- array( 1, dim=N)
  payoff=NULL
  for (n in 1:N){
    while (options$CI < ci.95[n]){
      S <- S*2
      W   <- matrix(rnorm(2*S), nrow=2)
      W   <- rbind(W[1,], data$Rho*W[1,] + sqrt(1-data$Rho^2)*W[2,])
      mu1 <- (data$r - 0.5*data$V1^2)*data$Tx 
      F1  <- data$F1*exp(mu1 + data$V1*sqrt(data$Tx)*W[1,])
      mu2 <- (data$r - 0.5*data$V2^2)*data$Tx
      F2  <- data$F2*exp(mu2 + data$V2*sqrt(data$Tx)*W[2,])
      new.payoff <- exp(-data$r*data$Tx)*pmax(F1-F2-data$K,0)
      payoff     <- c(payoff, new.payoff) 
      res[n]     <- mean(payoff)
      ci.95[n]   <- 1.96*sd(payoff)/sqrt(S)
      if (S>10^10){ print("Too many simulations, exiting.")
        options$CI <- 0}
    }
  }
  return(list(value=res,ci.95))
}
#---------------------------------------------------------------
method.Carmona <- function(data){
  phi <- acos(data$Rho)
  v1t <- data$V1*sqrt(data$Tx)
  v2t <- data$V2*sqrt(data$Tx)
  k   <- data$K*data$DF
  rootFun <- function(theta, delta, gamma, beta, alpha, kappa, phi){
   browser()
   (1/(delta*cos(theta)))*log(-beta*kappa*sin(theta+phi)
    /(gamma*(beta*sin(theta+phi)-delta*sin(theta)))) - delta*cos(theta)/2
    -(1/(beta*cos(theta+phi)))*log(-delta*kappa*sin(theta)
    /(alpha*(beta*sin(theta+phi)-delta*sin(theta)))) + beta*cos(theta+phi)/2
  }
  browser()
  test.theta <- seq(0,2*pi,by=pi/8)
  test.root  <- rootFun(test.theta, v1t, data$F1, v2t, data$F2, k, phi)

  rootFun(0, v1t, data$F1, v2t, data$F2, k, phi)
  thetaS <- uniroot(rootFun, c(0,2*pi), lower=NULL, upper=NULL, tol=10^-4,
     maxiter=1000, v1t, data$F1, v2t, data$F2, k, phi)
}

#---------------------------------------------------------------
method.Exact <- function(data){
  source("H:/user/R/RMG/Utilities/Math/quadGaussFun.R")
  integrand <- function(F2){
    mu <- (r-V1^2/2)*Tx + rho*(V1/V2)*(log(F2/F2.0)-(r-V2^2/2)*Tx)
    d2 <- (log(F1.0/(F2+K)) + mu)/(V1*sqrt(Tx*(1-rho^2)))
    d1 <- d2 + V1*sqrt(Tx*(1-rho^2))
    pdf.F2 <- dlnorm(F2,log(F2.0)+(r-0.5*V2^2)*Tx, V2*sqrt(Tx))
    BS1    <- F1.0*exp(mu-drift1.t)*pnorm(d1)-(F2+K)*DF*pnorm(d2)
    return(pdf.F2*BS1)
  }
  for (n in 1:dim(data)[1]){
    F1.0 <- data$F1[n]; F2.0 <- data$F2[n]; V1   <- data$V1[n];
    V2   <- data$V2[n]; Tx   <- data$Tx[n]; rho  <- data$Rho[n];
    K    <- data$K[n];  r    <- data$r[n];  DF   <- exp(-r*Tx)
    drift1.t <- (r-V1^2*(1-rho^2)/2)*Tx
#    data$value[n] <- quadGaussFun(integrand,0,Inf)
    data$value[n] <- integrate(integrand,lower=0,upper=Inf)$value
  }
  return(data)  
}
dollar.price <- function(data, options){
   return(switch(options$method,
     "kirk"   = method.kirk(data),
     "MC"     = method.MC(data),
     "exact"  = method.Exact(data)))
}
#---------------------------------------------------------------
finite.diff <- function(dataPP, dataNN, h, options){
  valPP <- dollar.price(dataPP, options)
  valNN <- dollar.price(dataNN, options)
  return((valPP$value-valNN$value)/(2*h))
}
#---------------------------------------------------------------
finite.diff2 <- function(dataPP, data, dataNN, h, options){
  valPP <- dollar.price(dataPP, options)
  val   <- dollar.price(data,   options)
  valNN <- dollar.price(dataNN, options)
  return((valPP$value -2*val$value + valNN$value)/(h^2))
}
#---------------------------------------------------------------
#                             MAIN
#---------------------------------------------------------------
spread.option <- function(data, options){

if (nargs()==1){options <- NULL}  
if (length(data$K)==0){data$K <- array(0, dim=dim(data)[1])}
if (length(options$kirk)==0){options$method <- "kirk"}
if (length(options$pert)==0){options$pert <- 0.0001}
if (length(data$HR)==0){data$HR <- 1}
if (length(options$CI)==0){options$CI <- 0.05}
if (length(options$what)==0){
  options$what <- c(value=1, deltas=0, vegas=0, gammas=0, delta.rho=0)}

if (options$what["value"]==1){
  data$value <- dollar.price(data, options)$value}

if (options$what["deltas"]==1){   #----------------------- DELTAS 
  dataPP <- dataMM <- data
  h <- dataPP$F1*options$pert
  dataPP$F1 <- data$F1 + h;  dataMM$F1 <- data$F1 - h
  data$delta1  <- finite.diff(dataPP,dataMM,h,options)
  
  dataPP <- dataMM <- data
  h <- dataPP$F2*options$pert
  dataPP$F2 <- data$F2 + h;  dataMM$F2 <- data$F2 - h  
  data$delta2  <- finite.diff(dataPP,dataMM,h,options)
}
if (options$what["vegas"]==1){   #----------------------- VEGAS
  dataPP <- dataMM <- data
  h <- dataPP$V1*options$pert
  dataPP$V1 <- data$V1 + h;  dataMM$V1 <- data$V1 - h
  data$vega1  <- finite.diff(dataPP,dataMM,h,options)
  
  dataPP <- dataMM <- data
  h <- dataPP$V2*options$pert
  dataPP$V2 <- data$V2 + h;  dataMM$V2 <- data$V2 - h  
  data$vega2  <- finite.diff(dataPP,dataMM,h,options)
}
if (options$what["gammas"]==1){   #----------------------- GAMMAS 
  dataPP <- dataMM <- data
  h <- dataPP$F1*options$pert
  dataPP$F1 <- data$F1 + h;  dataMM$F1 <- data$F1 - h
  data$gamma1 <- finite.diff2(dataPP,data,dataMM,h,options)
  
  dataPP <- dataMM <- data
  h <- dataPP$F2*options$pert
  dataPP$F2 <- data$F2 + h;  dataMM$F2 <- data$F2 - h  
  data$gamma2  <- finite.diff2(dataPP,data,dataMM,h,options)
}
if (options$what["delta.rho"]==1){ #----------------------- RHO
  dataPP <- dataMM <- data
  h <- dataPP$Rho*options$pert
  dataPP$Rho <- data$Rho + h;  dataMM$Rho <- data$Rho - h
  data$delta.Rho  <- finite.diff(dataPP,dataMM,h,options)
}

#---------------------------------------------------------------
if (options$method=="MC"){data$ci.95 <- res$ci.95}
return(as.data.frame(data))
}
