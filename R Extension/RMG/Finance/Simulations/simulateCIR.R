#
# Generate paths given by the Cox-Ingersoll-Ross process:
#       dx_t = -gamma*(x_t- theta)dt + kappa*sqrt(x_t)*dZ_t
#
# where: gamma, theta, kappa are the parameters, with time unit in years
#        T          is the time to expiration (scalar) in days
#        M          is the number of time-steps
#        options$x0 is the initial value of the assset (scalar)
#        
# returns x the simulated asset value (vector, size (M+1))
#
# written by Adrian Dragulescu on 3-Jul-2003

simulateCIR <- function(gamma,theta,kappa,T,M,options){

  if (nargs()==5){options <- list()}
  if (length(options$x0)==0){options$x0 <- theta} # start from theta
  if (length(options$plot)==0){options$plot <- 1} # plot the simulation
  if (length(options$method)==0){options$method <- "Milstein"}
 
  x <- array(0,M+1)
  dW <- rnorm(M,0,1)     # generate random deviates for x
  x[1] <- options$x0     # the initial value
  dt <- T/M
  dt12 <- sqrt(dt)
  dt32 <- dt^(3/2)
  dt2 <- dt^2

  if (options$method=="Milstein"){
    dW2 <-dW^2 - 1  
    for (i in 1:M){
 #     browser()
      mu <- -gamma*(x[i]-theta)
      sigma <- kappa*x[i]
      aux12 <- kappa*sqrt(x[i])*dt12*dW[i]
      aux10 <- (mu - kappa^2/2*dW2[i])*dt
      aux32 <- 0.5*(-gamma*sigma - mu*gamma/(2*x[i]) -
                    kappa^2*sigma/2)*dW[i]*dt32
      aux20 <- 0.5*(-gamma*mu)*dt2
      x[i+1] <- x[i] + aux12 + aux10 + aux32 + aux20 
    } # end of for loop over the time-steps
  } else {
    if (options$method=="Euler"){
       for (i in 1:M){
         x[i+1] <- (x[i] - gamma*(x[i]-theta)*dt
               + kappa*sqrt(x[i])*dt12*dW[i])
       } # end of for loop over the time-steps
    } else {
       print("Unknown method type.") 
    }
  } 

  if (options$plot==1){
    plot(seq(0,T,by=dt),x, type="l", col="blue")
  }
  x
} # end of file

##############################################################
#    sw <- 0
#    while (sw==0){   # if the increments produce a negative x
#      dW[,i] <- t(rnorm(options$N,0,1))
#      x[,i+1] <- (x[,i] - gamma*(x[,i]-theta)*dt
#               + kappa*sqrt(x[,i])*sdt2*dW[,i])
#      if (x[,i+1]>0){sw <- 1}  # if a good v, you pass
#    } 
##############################################################
#     print(i)
#     H <- 1              # number of intervals to simulate extra
#     bux <- sqrt(dt/x[i])
#     epsilon <- -aux*bux - (1-gamma*dt)/(bux*kappa)
#     prob <- pnorm(epsilon)
#     dT <- dt
#     while (prob>(0.01/H)){
#       dT <- dT/2
#       H <- H*2
#       bux <- sqrt(dT/x[i])
#       epsilon <- -aux*bux - (1-gamma*dT)/(bux*kappa)
#       prob <- pnorm(epsilon)
#     }
#     if (H>1){
#       y <- NULL
#       y[1] <- x[i]
#       dZ <- rnorm(H,0,1) # generate random increments
#       for (h in 2:H){
#         y[h] <- (y[h-1] - gamma*(y[h-1]-theta)*dT
#                + kappa*sqrt(y[h-1])*sqrt(dT)*dZ[h])
#       }
#       x[i+1] <- y[H]
#     } else {
#     x[i+1] <- (x[i] - gamma*(x[i]-theta)*dt
#                + kappa*sqrt(x[i])*sdt2*dW[i])
#     }
###################################################################
#   noSteps <- function(M, A){
#      pnorm(A*sqrt(M))-0.01/M
#   }

#   A <- -sqrt(theta/T)/kappa
#   MM <- uniroot(noSteps, c(1,2^20), lower=1, upper=2^20,
#           tol=.Machine$double.eps^0.25, maxiter=1000,A)
#   aux=paste("Recomended step size M: ", as.character(MM$root))
#   print(aux)
 
