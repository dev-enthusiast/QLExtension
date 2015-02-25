#options <- NULL
#options$DT <- 11
#options$percentile <- 5
#options$showPlots <- 0 # for show plots 
#options$showPlots <- 1 # not show plots 
#load("c:/temp/4Heston.Rdata")
# y <- xxx
HestonModel <- function(y,options) {
#  browser()
  # define parameters
  lastError <- 1
  fSigma <- 0
  nK <- 100
  # find theta
  varX <- array(NA, dim = options$DT)
  for (i in 1:options$DT) { varX[i] <- sum((diff(y, lag = i))^2)/(length(y) - 1- i) }
  lastError <- 1
  fSigma <- 0          
  while(lastError > fSigma) {
    options$DT <- options$DT -1
    theta <- sum(varX[1:options$DT]*c(1:options$DT))/(options$DT*(options$DT+1)*(2*options$DT+1)/6)
    lastError <- abs(varX[options$DT] - theta*options$DT)
    fSigma <- 1.5*sd(varX[1:options$DT] - theta*c(1:options$DT))
  }
  if (options$showPlots == 0) {
    windows()
    plot(c(1:options$DT),varX[1:options$DT])
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
  x    <- array(NA, dim = c(options$DT,101))
  PDFX <- array(NA, dim = c(options$DT,101))
  xAv  <- array(NA, dim = c(options$DT,50))
  CPDF <- array(NA, dim = c(options$DT,50))
  prct <- array(NA, dim = options$DT)
  quantiles <- array(0, dim <- c(options$DT,2))
  for (i in 1:options$DT) {
    xxx <- HestonX(a,g,theta,k[i,nKth[i]],i)
    x[i,]    <-xxx[[1]]
    PDFX[i,] <-xxx[[2]]
#    browser()
    xxx <- percentile(x[i,],PDFX[i,],options)
    xAv[i,]  <- xxx[[1]]
    CPDF[i,] <- xxx[[2]]
    prct[i]  <- xxx[[3]]
    ySort <- sort(diff(y, lag = i))
    quantiles[i,] <- quantile(ySort, probs = c(0.01*options$percentile, 1-0.01*options$percentile))    
  }
  if (options$showPlots == 0) {  
    cols <- rainbow(options$DT)
    Xrange <- c(-max(abs(diff(y, lag = options$DT))), max(abs(diff(y, lag = options$DT))))
    Yrange <- c(0,1)
    windows()
    plot(Xrange,Yrange, type = "n", xlab = "Delta Value (M$)",  ylab = "CPDF",
    main = bquote(paste("Heston Fit CPDF: ", gamma == .(format(g, digits = 3)), ", ", 
                        alpha == .(format(a, digits = 3)), ", ", theta == .(format(theta, digits = 3)))))
    for(i in 1:options$DT) {
      ySort <- sort(diff(y, lag = i))
#     quantiles <- quantile(ySort, probs = c(0.01*options$percentile, 1-0.01*options$percentile))
      CP <- (c(1:length(ySort))-0.5)/length(ySort)
      points(ySort,CP, lwd = 2, col = cols[i])
      lines(-xAv[i,],CPDF[i,], lwd = 2, col = cols[i])
      lines(xAv[i,],(1-CPDF[i,]), lwd = 2, col = cols[i])
      abline(v = - prct[i], lwd = 1, col = cols[i])
      abline(v = prct[i], pch = 22, lwd = 1, col = cols[i])    
      abline(v = quantiles[i,1], pch = 22, lwd = 1, col = cols[i])    
      abline(v = quantiles[i,2], pch = 22, lwd = 1, col = cols[i])      
    }
    grid()
 
    windows()
    Yrange <- c(1e-3,max(PDFX))
    plot(Xrange,Yrange, type = "n", log = "y", xlab = "Delta Value (M$)",  ylab = "PDF", 
    main = bquote(paste("Heston Fit PDF: ", gamma == .(format(g, digits = 3)), ", ", 
      alpha == .(format(a, digits = 3)), ", ", theta == .(format(theta, digits = 3)))))
    for(i in 1: options$DT) {
      lines(x[i,],PDFX[i,], lwd = 3, col = cols[i])
      lines(-1*x[i,],PDFX[i,], lwd = 3, col = cols[i])
      abline(v = prct[i], lwd = 3, col = cols[i])
      abline(v = -prct[i], lwd = 3, col = cols[i])
    }
    grid()
  }  
  return(list(a,g,theta,x,PDFX,prct,quantiles))
}

percentile <- function(x,PDFX,options) {
#  browser()
  CPDF <- array(0, dim = ((length(x)-1)/2))
  Xav  <- array(0, dim = ((length(x)-1)/2))
  m <- 0
  for (i in ((length(x)-1)/2):1){
    m <- m + 1
    X <- c(x[2*i+1],x[2*i],x[2*i-1])
    Y <- c(PDFX[2*i+1],PDFX[2*i],PDFX[2*i-1])
    xxx <- parabolicCoeff(X,Y)
    dInt <- (X[1]-X[3])*((xxx[[1]]/3)*(X[3]*X[3] + X[3]*X[1]+ X[1]*X[1]) + (xxx[[2]]/2)*(X[3]+X[1]) + xxx[[3]])
    Xav[m] <- X[3]
    if( m == 1){ CPDF[m] <- dInt }
    if (m > 1) {
      CPDF[m] <- CPDF[m-1] +dInt
      if(CPDF[m] > 0.01*options$percentile & CPDF[m-1] < 0.01*options$percentile)
#        browser()
        xPrct <- (X[3]+(X[1]-X[3])*(CPDF[m] - 0.01*options$percentile)/dInt)
    }                                  
  }
  return(list(Xav,CPDF,xPrct))
}

parabolicCoeff <- function(x,y) {
  R21 <- (y[2]-y[1])/(x[2]-x[1])                        
  R32 <- (y[3]-y[2])/(x[3]-x[2])
  A <-  (R21-R32)/(x[1]-x[3])
  B <- R21-A*(x[2]+x[1])
  C <- y[1]-A*x[1]*x[1]-B*x[1]
  return(list(A,B,C))
}

# Heston Function in x-space
HestonX <- function(a,g,theta,kMax,DT) {
  if(g < 1e-5) {
    xMax <- 10*sqrt(theta*DT/(2*a))
    nx <- 100
    dx  <- xMax/nx    
    PDFX <- array(0, dim = (nx +1))
    x <- dx*(0:nx)
    x[1] <- 1e-8
    for( i in 1:(nx+1)) {
      y <- x[i]*sqrt(2*a/(theta*DT))
      PDFX[i] <- 1/(sqrt(pi)*gamma(a))*2^(0.5-a)*sqrt(2*a/(theta*DT))*y^(a-0.5)*besselK(y, 0.5-a)
    } 
#    Simpson(PDFX,nx+1,dx)   
  } else {
#    browser()
    while (PDFkHeston(a,g,theta,kMax,DT) > 1e-5) # find kMax
      kMax <- kMax + 10
    dx <- 0.2/kMax
    xMax <- 100/kMax
    dK <- 0.1/xMax
    nK <- 2*round(round(kMax/dK)/2)
    nKp1 <- nK + 1
    k <- dK*c(0:nK)
    PDFK   <- array(0, dim = nKp1)
    IntFun <- array(0, dim = nKp1)
    for(i in 1:nKp1)
      PDFK[i] <- PDFkHeston(a,g,theta,k[i],DT)
    nx <- 100
    dx  <- xMax/nx
    # find  the x limit
    for (j in 1:nKp1)
      IntFun[j] <- cos(k[j]*xMax)*PDFK[j]
    PDFxMax <- 1/pi*Simpson(IntFun,nKp1,dK)
    while(PDFxMax < 2e-5){
      xMax <- xMax - dx
      for (j in 1:nKp1)
        IntFun[j] <- cos(k[j]*xMax)*PDFK[j]
      PDFxMax <- 1/pi*Simpson(IntFun,nKp1,dK)
    }
  while(PDFxMax > 2e-5){
      xMax <- xMax + dx
      for (j in 1:nKp1)
        IntFun[j] <- cos(k[j]*xMax)*PDFK[j]
      PDFxMax <- 1/pi*Simpson(IntFun,nKp1,dK)
    }
    dx  <- xMax/nx
    x <- dx*c(0:nx)
    PDFX <- array(0, dim = (nx +1))
    for( i in 1:(nx+1)) {
      for (j in 1:nKp1)
        IntFun[j] <- cos(k[j]*x[i])*PDFK[j]
      PDFX[i] <- 1/pi*Simpson(IntFun,nKp1,dK)
    }
  }  
  return(list(x,PDFX))
}

Simpson <- function(intFunction,N,dx) {
  indEven <- 2*c(1:((N-1)/2))
  indOdd  <- 1 + 2*c(1:((N-2)/2))
  intRes <- (dx/3)*(intFunction[1]+intFunction[N] + 4*sum(intFunction[indEven]) + 2*sum(intFunction[indOdd]))
  return(intRes)
}
  
# Heston Fit
HestonFitK <- function(ecf,k,nKth,DT,theta,options){
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
  chiMin <- cfVSecf(ecf,k,nKth,DT,aMin,gMin,theta,1)
  chiMax <- cfVSecf(ecf,k,nKth,DT,aMax,gMax,theta,1)  
  nIt <- 0  
  while (abs(chiMax - chiMin)/max(chiMax,chiMin) > 1e-5 & nIt < 20) {
    nIt <- nIt + 1
    if(chiMin > chiX & chiMax > chiX ) {
      gXX <- (gMin + gMax)/2
      aXX <- (aMin + aMax)/2      
      chiXX <- cfVSecf(ecf,k,nKth,DT,aXX,gXX,theta,1)
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
        chiMin <- cfVSecf(ecf,k,nKth,DT,aMin,gMin,theta,1)
      } else {
        gMin <- gX
        aMin <- aX
        chiMin <- chiX
        gX   <- gMax
        aX   <- aMax
        chiX <- chiMax
        gMax <- gMax*options$nFactorG
        gMax <- aMax*options$nFactorA
        chiMax <- cfVSecf(ecf,k,nKth,DT,aMax,gMax,theta,1)
      }
    }
  }
  if(options$showPlots == 0)
    chiX <- cfVSecf(ecf,k,nKth,DT,aX,gX,theta,options$showPlots)
  return(list(aX,gX,chiX))
}

# Characteristic vs Emprical Characteristic Functiotions
cfVSecf <- function(ecf,k,nKth,DT,a,g,theta,nFlagFig) {
  chi <- 0
  cf <- array(NA, dim = c(length(DT),max(nKth)))
#  browser()
  for (iDT in 1:length(DT)) {
	 for (iK in 1:nKth[iDT]) {
        cf[iDT,iK] <- PDFkHeston(a,g,theta,k[iDT,iK],DT[iDT])
#        dCF <- cf[iDT,iK] - ecf[iDT,iK]
        dCF <- log(cf[iDT,iK]) - log(ecf[iDT,iK])
        chi <- chi + dCF*dCF
    }
  }

  if(nFlagFig == 0) {
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

PDFkHeston <- function(a,g,theta,k,DT) {
  if (k == 0) {
      Ftk <- 0
  } else {
    if(g < 1e-6) {
      Ftk <- - log(1 + k*k*theta*DT/(2*a))     
    } else {
      gt <- g*DT
      omega <- sqrt(1 + 2*k*k*theta/(g*a))
      omegaGammaTo2 <- omega*gt/2
      if (omegaGammaTo2 > 10) {
        Ftk <- gt/2*(1-omega) - log((omega*omega+2*omega+1)/(4*omega))
      } else {
        Ftk <- gt/2 - log(cosh(omegaGammaTo2) + (omega*omega + 1)/(2*omega)*sinh(omegaGammaTo2))
      }
    }  
  }
  return(exp(a*Ftk))
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