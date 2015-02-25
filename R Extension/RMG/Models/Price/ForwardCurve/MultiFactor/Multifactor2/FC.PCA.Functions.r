#A collection of routines to collect, clean, and transform the data.
source("FC.PCA.Data.Functions.r")
# version check
PCAfuncVersion = 3
# requires a marketData list as defined in above source.

# to generate simulations first set marketData = transformHistData(rawData) in FC.PCA.Data.Functions
# then call PCSims(marketData) for each pricing date each dT = (pricingDate - asOfDate) 
# simulations are returned as shown below:
    #PCSimData$PC : principal component object returned by R  
    #PCSimData$simData = simulated curve for the given season (if any)  
    #PCSimData$returnSims = raw returns unadjusted for the X0 seed
    #PCSimData$eta2 = eta2       # GBM adjustment returned for verification.
    #PCSimData$pct_explained = pct explained.
    #PCSimData$PCstdev = PCstdev  # annualized std deviation of scaled PCs
    #PCSimData$dataStdev = Xstdev   # std dev of actual data
    #PCSimData$PCVolFunctions = PC volatility functions for plotting.

#the default seasons are the NG season:
# list(c(5:9),c(11,12,1:3),c(4,10)) ==> (summ,wint,shld)

# however, if no bySeason is passed, it will not apply any seasonality at all.

# generatingDistribution:
#           0:    normal distribution
#           #>0:  t-dist with generatingDistribution degrees of freedom; useful for fat symetric tails
#           -1:   samples from the historical distribution

# note that the sampling method is antithetic and generates a -z for every Z
# for the historical distribution it generates equal samples above and below the median.
# if NSims is odd then the samples will contain a 50% percentile shock to maintain symmetry above and below the median

 
PCSims = function(marketData, NSims = 250, dT = 1, generatingDistribution=0, bySeason = NULL) {
    # note that the assumption here is that the time between rows is dt =1 
    # therefore dt scales relative to the time period of the rows.
    # so for example if the the data is weekly, setting dt = 52 gives a one year time.
    # the return object is PCSims list
    

    modelType = marketData$modelType  #how to generate the sims.
    
    
    if (!is.null(bySeason)) {
      # only do the sims on the given season.
      X = getSeason(marketData,bySeason)
      
    }  else  {
      X =  marketData$data
    }
    
    X= na.omit(X)   #make sure we get rid of NaNs
    # seed row
    
    # first we rescale the data
    Nobs = nrow(X);
    p = ncol(X);      #no columns of X
    
    onesCol = matrix(1,Nobs,1)
    
    XMean = apply(X,2,mean)
    Xhat= X - onesCol %*% XMean
    
    XCov = cov(X)  # X covarance matrix, used below for the Geo brownian motion adjustment
    
    Xstdev = sqrt(diag(diag(XCov)))
    Xhat =  Xhat %*% solve(Xstdev)    # we need to scale each column by the std deviation   we cannot use prcomp(scale=TRUE) because we need to use the scale later.
    colnames(Xhat) = colnames(marketData$data)
    
    PCObject=prcomp(Xhat,center=FALSE,scale=FALSE)
    
    PC = PCObject$rotation        # principle components
    PCstdev = diag(PCObject$sdev)      # variance
    SC = PCObject$x                     # rotated values
    
    # How much variation in the dataset is explained by each PC?
    PCvar = PCObject$sdev ^2
    
    pct_explained = (PCvar)/sum(PCvar) *100
    # we want to make sure that the sims are antithetic: that is for every z we have a -z
    # if NSims is odd we add a shock of zero (the base).
    
    if (NSims>1) {
      NSimsUp = NSims%/%2
      NSimsBase = NSims-2*NSimsUp
    }  else {
      NSimsUp = NSims
      NSimsBase = 0
    } 
    
    
    if (generatingDistribution>=1) {
      #stick a t-dist in here with generating Distribution df
      Z = array(rt(p * NSimsUp,generatingDistribution),dim=c(NSimsUp,p))
      Z= rbind(Z,-Z)
      if (NSimsBase>0)  {
        Z = rbind(Z,array(0,dim=c(NSimsBase,p)))  
      }
      
      
      # replace this with some other distribution if needed.
      XSim = Z %*% PCstdev %*% t(PC) %*% Xstdev  
      
    } else if (generatingDistribution==-1) {
      # Historical distribution
      
      # this little piece of code generates sims from the empirical cdf.
      Z= array(runif(NSimsUp*p),dim=c(NSimsUp,p))
      Z= rbind(Z,1-Z)
      if (NSimsBase>0)  {
        Z = rbind(Z,array(0.5,dim=c(NSimsBase,p)))  
      }
      
      PCsimSc = matrix(0,NSims,p)
      
      for (k in 1:p) {
        PCkcdf = ecdf(SC[,k])
        PCkx=knots(PCkcdf)
        PCky = PCkcdf(PCkx)
        PCkcdfAppx=approxfun(PCky,PCkx,rule=2)
        PCsimSc[,k]= PCkcdfAppx(Z[,k])
        #note that this will already be adjusted for the std dev no need to rescale.
      }

      XSim = PCsimSc %*% t(PC) %*% Xstdev  
      
      
    } else { 
      # default is standard normal
      Z = array(rnorm(p * NSimsUp),dim=c(NSimsUp,p))
      Z= rbind(Z,-Z)
      if (NSimsBase>0)  {
        Z = rbind(Z,array(0,dim=c(NSimsBase,p)))  
      }
      # replace this with some other distribution if needed.
      XSim = Z %*% PCstdev %*% t(PC) %*% Xstdev 
    }
    eta2 =  matrix(0,NSims,p)
    
    if (modelType == 2) {
      # have to compute the variance for the geometric brownian motion factor
      #PCscaled = PC %*% PCstdev;
      #TVar = PCscaled %*% t(PCscaled);
      #XSimVar = t(Xstdev) %*% TVar %*% Xstdev;
      # But this is just the variance of X! which we already computed above!
      XSimVar = XCov
      
      ones = matrix(1,NSims,1)
      eta2 = -0.5* ones %*% t(diag(XSimVar))
      
    }
    # seed row
    X0 = marketData$seedRow
    
    row.names(XSim) = c(1:NSims)
    colnames(XSim) = colnames(X0)
    simData = simDataX0(XSim,eta2,dT,X0,modelType)    # this is the actual simulated curve adjusted to the X0 seed value
    colnames(simData) = colnames(X0)                                                  #XSims is the simulated **CHANGES** relative to X0 - i.e. you can use XSims to 
                                                      # on the fly reseed the simulations at some other X0hat
    
    
    PCSimData = list()
    PCSimData$PCVolFunctions = t(PCstdev %*% t(PC) %*% Xstdev)
    row.names(PCSimData$PCVolFunctions) = row.names(PCObject$rotation)
    colnames(PCSimData$PCVolFunctions) = colnames(PCObject$rotation)
    
    PCSimData$seasonData= X                          # return the seasonal data                                            
    PCSimData$PC = PCObject                          # an R PC object
    PCSimData$simData = simData
    PCSimData$changeSims = XSim
    PCSimData$eta2 = eta2                            # the GBM adjustment - returned for testing
    PCSimData$pct_explained = pct_explained
    PCSimData$PCstdev = PCstdev                      # Std sed of actual PCs
    PCSimData$dataStdev = diag(Xstdev)                     # std dev of underlying data.
    return(PCSimData)

}

simDataX0 = function(XSim,eta2,dT,X0, modelType) {
  NSims =nrow(XSim)
  XSimdt = (XSim * sqrt(dT)+ eta2 * dT);
  
  if (modelType == 2) {
      simData = (matrix(1,NSims,1) %*% X0) * exp(XSimdt)
    } else {
      simData = (matrix(1,NSims,1) %*% X0)  +  XSimdt
    }
  row.names(simData) = row.names(XSim)
  return(simData)

}

multiPCSims = function(marketData, NSims = 250, generatingDistribution=0, pricingDates) {
    #pricingDates is a vector of Dates to generate sims
    asofDate = marketData$seedRowDate
    pricingDatesMatrix = pricingDatesCalcdT(asofDate,pricingDates,marketData$months.in.season)
    numDates = length(pricingDates)
    
    multiSimData = list()
    for (i in 1:numDates) {
      multiSimData = list(multiSimData,pcSims(marketData,NSims,dT=pricingDatesMatrix[i,1],generatingDistribution,bySeason = pricingDatesMatrix[i,2])) 
      
    }
    return(multiSimData)
    
}


PCSimGenerate = function (dataFileName, dT, NSims, getRow=-1, pctPlot=95,writeToFiles = FALSE,generatingDistribution=0,modelType=1,plotPCAs = TRUE ) {

  rawPCdata = read.csv(paste(dataFileName,".csv",sep=""), row.names=1)
  # Start simulation at F0
  histDataStructure = setupHistData(rawPCdata,modelType,getRow)
  PCSimsData = PCSims(histDataStructure,NSims,dT,generatingDistribution)
  pct_expl = PCSimsData[[3]] #$pct_explained as a % 
  # How much variation in the dataset is explained by each PC?
  
  if (plotPCAs) {
    # Plot of the eigenvalues
    y.range = range(cumsum(pct_expl),.9*pct_expl[1])
    plot(cumsum(pct_expl), ylim=y.range,xlab="PC", ylab="", type="b",col="mediumturquoise", lwd=2,main="Cum. Variation Explained by PCA")
    
    PC = PCSimsData[[4]] #$PC
    PCstdev = PCSimsData[[5]]#$PCstdev
      
    windows()
    
    # number to plot
    #nPlot = 10
    
    nPlot = sum(cumsum(pct_expl)<=pctPlot) # plot the PCs which explain pct% of the variance
    nPlot = max(nPlot,1)
    
    PCScaled = PC %*% PCstdev
    y.range = range(PCScaled)*1.1
    
    i=1
    plotColors = c("red", "green", "mediumturquoise","coral","darkorchid","black")
      
    plot(PCScaled[,i]   ,xlab="Column", ylab="Factor Loadings", ylim=y.range, type="l", lty=1, lwd=2, col=plotColors[i])      
    if (nPlot>1) {
      for( j in 2:nPlot) {lines(1:nrow(PCScaled[]),PCScaled[,j], lty=1, lwd=2, col=plotColors[j]) }
    }
    legend(cex=0.6,x="topleft", inset=0, legend=paste("Factor",1:nPlot,": ",format(pct_expl[1:nPlot],digits=2,nsmall=1),"%",sep=""), lty=rep(1,nPlot),lwd=rep(2,nPlot), col=plotColors[1:nPlot] ,text.col=plotColors[1:nPlot])
  }
  
  if (writeToFiles) {
    write.csv(XSimdt,paste(fileName," SimResults.csv",sep=""))
    write.csv(PC,paste(fileName," PC Components.csv",sep=""))
    write.csv(PCstdev,paste(fileName," PC Std Dev.csv",sep=""))
    write.csv(Z,paste(fileName," randnos.csv",sep=""))
    write.csv(XSim,paste(fileName," eta.csv",sep=""))
    write.csv(eta2,paste(fileName," eta2.csv",sep=""))
    write.csv(simData,paste(fileName," simData.csv",sep=""))
  
  }
  res = return(list("Raw Data"=rawPCdata,"Hist Data"=histDataStructure,"PCSimData"=PCSimsData,"Pct Expl"=pct_expl))
  return(res)

}



