# this function calculates Heston parameters theta, gamma; alpha is set to 1 in k-space
# options$showPlots == TRUE # show plots
# options$RefineTimeInterval == FALSE # select time interval that has linear dependence of var on time

source("H:/user/R/RMG/Finance/Heston/kHeston.R")
FitHeston <- function(y,options) {
#  browser()
  nK <- 100
  # find theta
  varX <- array(NA, dim = options$DT)
  for (i in 1:options$DT) { varX[i] <- sum((diff(y, lag = i))^2)/(length(y) - 1 - i) }
  theta <- sum(varX[1:options$DT]*c(1:options$DT))/(options$DT*(options$DT+1)*(2*options$DT+1)/6)
  if(options$RefineTimeInterval == TRUE) {
    lastError <- abs(varX[options$DT] - theta*options$DT)
    fSigma <- 1.5*sd(varX[1:options$DT] - theta*c(1:options$DT))
    while(lastError > fSigma) {
      options$DT <- options$DT -1
      theta <- sum(varX[1:options$DT]*c(1:options$DT))/(options$DT*(options$DT+1)*(2*options$DT+1)/6)
      lastError <- abs(varX[options$DT] - theta*options$DT)
      fSigma <- 1.5*sd(varX[1:options$DT] - theta*c(1:options$DT))
    }
  }
  if (options$showPlots == TRUE) {
    windows()
    plot(c(1:options$DT),varX[1:options$DT], xlab = "Time", ylab = "var")
    lines( c(1:options$DT), theta*c(1:options$DT), lwd=2, col="blue")
    grid()
  }
  ecf  <- array(0, dim = c(options$DT,nK))
  k    <- array(0, dim = c(options$DT,nK))
  nKth <- array(0, dim = options$DT)
  for (i in 1:options$DT) {
    xxx <- ECF(diff(y, lag = i),nK)
    ecf[i,] <- xxx[[1]]
    k[i,]   <- xxx[[2]]
    nKth[i] <- xxx[[3]]
  }
  xxx <- HestonFitK(ecf, k, nKth, c(1:options$DT), theta, options)
  a   <- xxx[[1]]
  g   <- xxx[[2]]
  return(list(xxx[[1]], xxx[[2]], theta, options$DT)) # a, g, theta
}

HestonFitK <- function(ecf,k,nKth,DT,theta,options){
#  browser()
  options$nFactorG <- 5
  options$nFactorA <- 1
  xxx <- HestonFitKga(ecf,k,nKth,DT,theta,1,DT[1],options)
#  options$nFactorG <- 1
#  options$nFactorA <- 1.5
#  xxx <- HestonFitKga(ecf,k,nKth,DT,theta,1,xxx[[2]],options)
  return(list(xxx[[1]],xxx[[2]],xxx[[3]]))
}

HestonFitKga <- function(ecf,k,nKth,DT,theta,aX,gX,options){
#  browser()
  chiX <- cfVSecf(ecf,k,nKth,DT,aX,gX,theta,options$showPlots)
  gMin <- gX/options$nFactorG
  gMax <- gX*options$nFactorG
  aMin <- aX/options$nFactorA
  aMax <- aX*options$nFactorA
  chiMin <- cfVSecf(ecf,k,nKth,DT,aMin,gMin,theta,FALSE)
  chiMax <- cfVSecf(ecf,k,nKth,DT,aMax,gMax,theta,FALSE)
  nIt <- 0
  while (abs(chiMax - chiMin)/max(chiMax,chiMin) > 1e-5 & nIt < 20) {
    nIt <- nIt + 1
    if(chiMin > chiX & chiMax > chiX ) {
      gXX <- (gMin + gMax)/2
      aXX <- (aMin + aMax)/2
      chiXX <- cfVSecf(ecf,k,nKth,DT,aXX,gXX,theta,FALSE)
      if((gXX - gX > 1e-10 & abs(aXX - aX) <1e-10) | (abs(gXX - gX) < 1e-10 & aXX - aX > 1e-10) ) {
        if(chiXX < chiX) {
          gMin   <- gX
          aMin   <- aX
          chiMin <- chiX
          gX     <- gXX
          aX     <- aXX
          chiX   <- chiXX
        } else {
          gMax   <- gXX
          aMax   <- aXX
          chiMax <- chiXX
        }
      } else {
        if(chiXX < chiX) {
          gMax   <- gX
          aMax   <- aX
          chiMax <- chiX
          gX     <- gXX
          aX     <- aXX
          chiX   <- chiXX
        } else {
          gMin   <- gXX
          aMin   <- aXX
          chiMin <- chiXX
        }
      }
    } else {
      if(chiMin < chiMax) {
        gMax <- gX
        aMax <- aX
        chiMax <- chiX
        gX   <- gMin
        aX   <- aMin
        chiX <- chiMin
        gMin <- gMin/options$nFactorG
        aMin <- aMin/options$nFactorA
        chiMin <- cfVSecf(ecf,k,nKth,DT,aMin,gMin,theta,FALSE)
      } else {
        gMin <- gX
        aMin <- aX
        chiMin <- chiX
        gX   <- gMax
        aX   <- aMax
        chiX <- chiMax
        gMax <- gMax*options$nFactorG
        gMax <- aMax*options$nFactorA
        chiMax <- cfVSecf(ecf,k,nKth,DT,aMax,gMax,theta,FALSE)
      }
    }
  }
  if(options$showPlots == TRUE)
    chiX <- cfVSecf(ecf,k,nKth,DT,aX,gX,theta,options$showPlots)
  return(list(aX,gX,chiX))
}

# Characteristic vs Emprical Characteristic Functiotions
cfVSecf <- function(ecf,k,nKth,DT,a,g,theta,bFlag) {
  chi <- 0
  cf <- array(NA, dim = c(length(DT),max(nKth)))
#  browser()
  for (iDT in 1:length(DT)) {
    for (iK in 1:nKth[iDT]) {
      cf[iDT,iK] <- kHeston(a,g,theta,DT[iDT],k[iDT,iK])
#        dCF <- cf[iDT,iK] - ecf[iDT,iK]
      dCF <- log(cf[iDT,iK]) - log(ecf[iDT,iK])
      chi <- chi + dCF*dCF
    }
  }
  if(bFlag == TRUE) {
    windows()
    plot(k[1,],ecf[1,], type = "n",
    main = bquote(paste("CF vs ECF,", chi == .(format(chi, digits = 3)), ", ",
    gamma == .(format(g, digits = 3)), ", ", alpha == .(format(a, digits = 3)))),
    xlab = "k", ylab = "CF")
    for (iDT in 1:length(DT)){
      points(k[iDT,1:nKth[iDT]], ecf[iDT,1:nKth[iDT]], lwd = 3, col = "red")
      lines(k[iDT,1:nKth[iDT]], cf[iDT,1:nKth[iDT]], lwd = 3, col = "blue")
    }
    grid()
  }
  return(chi)
}

ECF <- function(dx,nK) {
#browser()
  k   <- array(0, dim = nK)
  dxMax <- max(abs(dx))
  nIt <- 1
  while(nIt < 10) {
    ecf <- array(0, dim = nK)
    kMax <- 20*nIt/dxMax
    dK <- kMax/(nK-1)
    k = dK * c(0:(nK-1))
    ecf[1] <- length(dx)
    for (ik in (2:nK) ) {
      for (idx in 1:length(dx))
        ecf[ik] <- ecf[ik] + cos(k[ik]*dx[idx])
    }
    ecf <- ecf/ecf[1]
    if( ecf[nK] < 0.1)
      break
    nIt <- nIt + 1
  }
  nKth <- nK
  for (iK in 2:nK ) {
    if ( ecf[iK] < 0.05 ){
      nKth <- iK-1
      break
    }
  }
  return(list(ecf,k,nKth))
}