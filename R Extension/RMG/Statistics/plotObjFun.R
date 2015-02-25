# Plots the dependence of the objective function around a set of 
# parameters. 
# Where:   X   is the matrix of predictors, size NxP
#          y   is the response, vector size Nx1
#        theta are all the parameters, vector size Px1
#        fname is the name of the model function   
#
# written by Adrian Dragulescu on 18-Jul-2003


plotObjFun <- function(fName,theta,X,y,options){

  if (nargs()==4){ options <- list()}
  if (length(options$range)==0){options$range <- 0.10}   # 10 percent
  if (length(options$points)==0){options$points <- 30}   # 30 points
  if (length(options$together)==0){options$together <- 1}# plots together

  eta <- NULL; S <- NULL
  thetaSol <- theta       
  P <- length(theta)
  cP <- ceiling(sqrt(P))
  if (P > cP*(cP-1)){
    layout(t(matrix(1:cP^2,cP,cP)))
    layout.show(cP^2)
  } else {
    layout(t(matrix(1:(cP*(cP-1)),cP,cP-1)))
    layout.show(P)
  }
  for (p in 1:P){
    theta  <- thetaSol
    ptheta <- seq(theta[p]*(1-options$range), theta[p]*(1+options$range),
                  length=options$points)
    for (k in 1: length(ptheta)){
       theta[p] <- ptheta[k]
       eta  <- fName(theta,X)
       S[k] <- sum(objectiveFun(y,eta))
    }
 #   ind <- c(ceiling(p/cP),(p+1)%%cP+1)
    par(mar=c(2.5,2.5,0.5,0.5))  # distance between the plots    
    plot(ptheta,S, xlab=bquote(theta[.(p)]), 
         mgp=c(1.5,0.5,0), pch=20, col="blue")
    abline(v=thetaSol[p], lty=2)
  }
  layout(1)  # revert to normal layout

}
