
# A contrete example of the functions in this file is provided in "FEA.analysis.example.r" (Same directory)
# Monthly.vol.corr estimates the monthly correlation of log-returns of spot to prompt based on a mean reversion-rate model
# It also computes the monthly 'naive' correlation between log-returns of spot and log-returns of prompt
# Additionally, it calculates the dailty volatility of spot's log-returns for a given month.
# It also provides extra outputs e.g. the mean-reversion rate, the latest available spot price..etc


monthly.vol.corr <- function(Index= "Index of IGBBL00", Prompt="Close of NG", start.date.hist="01/01/2001", 
Verbose=FALSE, Plot=T, directory){
  source(paste(directory, "get.hist.index.prompt.r", sep=""))

  hist.data <- get.hist.index.prompt(Index, Prompt, start.date.hist, Verbose=FALSE)
  dates <- as.Date(rownames(hist.data)) #n
  
  spot <- hist.data[,1]
  prompt.mo <- hist.data[,2]
  prompt.ret = diff(log(prompt.mo)) # log-returns of prompt-month prices
  spot.ret = diff(log(spot))  # log-returns of spot prices
  
  # FEA model
  log.sp <- log(spot/prompt.mo)  # n
  diff.log.sp <- diff(log.sp)   #n-1
  delta.t <- as.numeric(diff(dates)/365)
  log.sp.dt <- log.sp[-length(log.sp)]*delta.t
  lm1 <- summary(lm(diff.log.sp~-1+log.sp.dt))
  mr = - lm1$coeff[1]
  #resi =  lm1$residuals
  resi = diff.log.sp - mr* log.sp.dt
  
  if(Plot==T){
  par(mfrow=c(2,1))
  y.range= range(hist.data)
  plot(dates, spot, ylim=y.range,type="l", col="red",frame.plot=TRUE, xlab=paste("Historical Spot and Prompt Prices", "\n", "from ",start.date.hist, " to Present", sep=""), main=" Time Series Plot of Spot and Prompt Prices")
  lines(dates,prompt.mo, col="orange", type="l")
  legend(x="topleft", inset=0,legend=c("Spot", "Prompt"), lty=1, text.col=c("red" ,"orange"),col = c("red" ,"orange"))
  
  ret.range = range(prompt.ret,spot.ret, log.sp)
  plot(dates,log.sp,ylim=ret.range, col="green", type="l",xlab=paste("Historical Spot and Prompt log-returns", "\n", "from ", start.date.hist, " to Present", sep=""), main=" Time Series Plot of log-Returns")
  lines(dates[-1],prompt.ret, col="orange", type="l")
  lines(dates[-1],spot.ret, col="red", type="l")
  legend(x="topleft", cex=0.58,inset=0,legend=c("spot to prompt","Prompt","Spot"), lty=1, text.col=c("green", "orange", "red"),col = c("green", "orange", "red"))
  
  windows()
  par(mfrow=c(2,2))
  plot(lm1$residuals, main="Scatter Plot of Residuals")
  plot(log.sp.dt,lm1$residuals, main="Residuals vs. log.sp.dt")
  qqnorm(scale(lm1$residuals))
  abline(a=0,b=1)
  hist(lm1$residuals)
  }
  # Compute corr. and vol.  
  unique.months <- months(as.Date(paste("2006-",1:12,"-1", sep="")), TRUE)
  unique.yr <- unique(format(dates[-1], "%Y"))
  
  cor.mat = matrix(NA, nr=length(unique.months),nc=length(unique.yr)+1)
  rownames(cor.mat) = unique.months
  colnames(cor.mat) = c(unique.yr, "Avg.")
  
  vol.mat = matrix(NA, nr=length(unique.months),nc=length(unique.yr)+4)
  rownames(vol.mat) = unique.months
  colnames(vol.mat) = c(unique.yr, "Min", "Max", "Runif","Avg" )
  
  # Compute the monthly corr and volatility 
  for (k in 1:length(unique.yr)){
    
    for(i in 1:length(unique.months)){
      entry <- which(format(dates[-1],"%b")==unique.months[i] & format(dates[-1],"%Y")==unique.yr[k])
      if(length(entry !=0)){
      spot.temp = spot.ret[entry]
      res.temp = resi[entry]
      prompt.temp = prompt.ret[entry]
      cor.mat[i,k] = cor(res.temp,prompt.temp, method = "pearson")
      #cor.mat[i,2] = cor(spot.temp,prompt.temp, method = "pearson") 
      vol.mat[i,k] = sqrt(var(spot.temp))     #Daily volatility 
      }
    }
  }
  
  # Add average correlation for a given month
  cor.mat[,ncol(cor.mat)] = rowMeans(cor.mat, na.rm = TRUE)
  
  
  # Compute the min, max, runif(1, min, max) and mean
  vol.mat[,length(unique.yr)+1] <- apply(vol.mat[,1:length(unique.yr)], 1, min,  na.rm=TRUE) 
  vol.mat[,length(unique.yr)+2] <- apply(vol.mat[,1:length(unique.yr)], 1, max,  na.rm=TRUE)
  vol.mat[,length(unique.yr)+3] <- runif(length(unique.months), vol.mat[,length(unique.yr)+1], vol.mat[,length(unique.yr)+2])
  vol.mat[,length(unique.yr)+4] = rowMeans(vol.mat, na.rm = TRUE)  

results = list("mr rate"=mr, "Corr. Spot and Prompt"=cor.mat, "Daily Vol of Spot"= vol.mat, "Hist. Prices" =hist.data)
return(results)
}    


                                                        

# Simulate Spot prices: This simulation is based on the FEA's model that assumes that spot price mean-reverts to the prompt-month forward.
# FEA's model assumes that the fwd curve is stochastic.
# Pleas see FEA User Guide, Technical Reference page 63
# The Inputs
# 1. MarketData is a list
#MarketData$mr: the mean reversion rate
#MarketData$SP.corr: a vector of monthly correlation between spot and prompt 
#MarketData$Spot.vol: daily volatility of spot prices, for a given month (i.e. the volatility of daily prices of month Jan)
#MarketData$last.spot: recent hist spot price
#MarketData$last.fwd: recent hist spot price
#MarketData$AsOf: date of latest available prices.
# MarketData$Pricing.dates: sequence of dates "YYYY-mm-dd"
#MarketData$current.FC: current forward curve or use output of function 'getFCFromLIM'

# All elements of MarketData, except current Fwd curve, could be the output of function 'monthly.vol.corr'. Please see example below!

#options$noSims <- 100     # Number of simulations
#options$nodays <- 365  # Number of days in year. This should be consistent with the number of futures contracts in the fwd curve.
#options$nopaths <- 5 # Number of random chosen simulated paths to be plotted.

# RemoveWkd = True or False; whether to remove weekend prices from simulated paths


# Application

#corr.vol.data <- monthly.vol.corr(Index= "Index of IGBBL00", Prompt="Close of NG", start.date.hist="01/01/2001", Verbose=FALSE, Plot=F, directory="S:\\Risk\\Temporary\\Spot Simulation\\")

#MarketData <- options <- NULL
#
#options$noSims <- 1000     # Number of simulations
#options$nodays <- 365  # Number of days in year
#options$nopaths <- 3 # plot 10 random simulated paths
#
#
#MarketData$mr <- corr.vol.data[[1]]
#MarketData$SP.corr <- corr.vol.data[[2]][,ncol(corr.vol.data[[2]])]
#MarketData$Spot.vol <- corr.vol.data[[3]][,ncol(corr.vol.data[[3]])]
#MarketData$last.spot <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),1]
#MarketData$last.fwd <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),2]
#MarketData$AsOf <- as.Date(rownames(corr.vol.data[[4]])[nrow(corr.vol.data[[4]])])
#MarketData$Pricing.dates <- seq(as.Date(MarketData$AsOf)+1,by="day",length.out=options$nodays)
#MarketData$current.FC <- getFCFromLIM(theSymbol="Close of NG", mosFwd=12, monthNames=paste("F(",1:12,"m)", sep=""), Verbose =F)[[2]]
#
#
#SimSpots <- simulate.spots.FEA(MarketData, options, RemoveWkd=FALSE)


simulate.spots.FEA <- function(MarketData, options, RemoveWkd= TRUE){

  if (length(options$noSims)==0){options$noSims <- 50}
  if (length(options$nodays)==0){options$nodays <- 365}
  if (length(options$nopaths)==0){options$nopaths <- 5}

  
  FC.length =  length(MarketData$current.FC)
  
  #options$nodays <- 21* FC.length # 21 trading days per month
    
  # Create a sequence of pricing dates
  pricing.dates = MarketData$Pricing.dates
   
  # Smooth the FC (constant each month)
  temp.FC= c(MarketData$current.FC, MarketData$current.FC[FC.length])
  
  # Constant FC 
  fwd <- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="constant")$y
  
  # Perform cubic spline interpolation
  #fwd <-  spline(x=1:(FC.length+1), y=temp.FC, n=options$nodays)$y
  
  # Linear Interpolation
  #fwd <- approx(1.5:(FC.length+0.5), MarketData$current.FC, method="linear", xout=seq(1.5,FC.length+0.5,length.out=options$nodays),f=0.5)
  #fwd <- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="linear")$y
  
  SimPrices <- array(NA, dim=c(options$nodays,options$noSims), dimnames=list(as.character(pricing.dates), 1:options$noSims))
  AsOf <- MarketData$AsOf 
  S0 <- rep(MarketData$last.spot ,options$noSims)
  F0 <- MarketData$last.fwd
  
  #options$nodays 
  for (d in 1:options$nodays){
    delta.t = as.numeric(pricing.dates[d] - AsOf)
    entry = as.numeric(format(AsOf, "%m"))
    spot.vol =  MarketData$Spot.vol[entry]
    SP.corr = MarketData$SP.corr[entry]
    
    z1 <- rnorm(options$noSims)
    z2 <- rnorm(options$noSims)

    #Following mathematically correct but simulated values oscillate between 0 and inf
    # Perhaps the best way to resolve this is to actually solve the original SDE instead of using Euler's approx. I am working on it!!!
    #aux <- exp((1-MarketData$mr*delta.t)*log(S0/F0) + spot.vol*(SP.corr*z1*sqrt(delta.t) + sqrt(delta.t*(1-SP.corr^2))*z2)) 

    # Mathematically Incorrect
    aux <- exp((1/MarketData$mr*delta.t)*log(S0/F0) + spot.vol*(SP.corr*z1*sqrt(delta.t) + sqrt(delta.t*(1-SP.corr^2))*z2)) # Not math. correct
    
    SimPrices[d,] <- fwd[d]*(aux)
    
    AsOf <- pricing.dates[d]
    S0 <- SimPrices[d,]
    F0 <- fwd[d]
  }
  
  if(RemoveWkd){
  # Remove Weekends simulations
  rm.days = which(format(pricing.dates,"%a")=="Sat" | format(pricing.dates,"%a")=="Sun")
  prices.no.wkd = SimPrices[-rm.days,]

  # Plot the fwrd curve and few spot simulated paths
  which.sim <- sample(x=1:options$noSims, size=options$nopaths, replace=FALSE,prob=rep(1/options$noSims, options$noSims))
  y.range = range(fwd[-rm.days], prices.no.wkd[,which.sim])
  plot(pricing.dates[-rm.days], fwd[-rm.days], ylim=y.range, type="p", col="blue", lwd=2, main="Fwd Curve and Simulated Spot Prices")
  color = c("mediumturquoise","black" ,"orange","deeppink", "coral", "red", "black", "maroon",
  "darkgrey", "darkblue", "darkgreen", "brown") 
  for( i in 1:options$nopaths){
  lines(pricing.dates[-rm.days], prices.no.wkd[,which.sim[i]], type="l", col=color[i], lty=1, lwd=1)}
  #lines(pricing.dates[-rm.days],fwd.constant[-rm.days],type="p", col="darkblue", lwd=1)  
  return(prices.no.wkd)
  }else{
  which.sim <- sample(x=1:options$noSims, size=options$nopaths, replace=FALSE,prob=rep(1/options$noSims, options$noSims))
  y.range = range(fwd,SimPrices[,which.sim])
  plot(pricing.dates, fwd,ylim=y.range, type="p", col="blue", lwd=2, main="Fwd Curve and Simulated Spot Prices")
  color = c("mediumturquoise","black" ,"orange","deeppink", "coral", "red", "black", "maroon",
  "darkgrey", "darkblue", "darkgreen", "brown") 
  for( i in 1:options$nopaths){
  lines(pricing.dates, SimPrices[,which.sim[i]], type="l", col=color[i], lty=1, lwd=1)}
  #lines(pricing.dates,fwd.constant,type="p", col="darkblue", lwd=1)
  return(SimPrices)
  }
}




############################# PART II: THIS MODELING APPROACH ASSUMES 
# 1. SPOT PRICES FOLLOW A GBM   
# 2. THE FORWARD CURVE IS GIVEN AND THEREFORE NOT STOCHASTIC
# 3. E[S(T)]=F(0,T)


###  TO DO: REMOVE WEEKENDS   ####
simulate.spots.GBM <- function(MarketData, options){

  if (length(options$noSims)==0){options$noSims <- 50}
  if (length(options$nodays)==0){options$noSims <- 365}
  if (length(options$nopaths)==0){options$noSims <- 5}
  
  
  FC.length =  length(MarketData$current.FC)
       
  # Create a sequence of pricing dates
  pricing.dates = seq(as.Date(MarketData$AsOf)+1,by="day",length.out=options$nodays)
  # Please note delta.t=1 

  # Smooth the FC (constant each month)
  temp.FC= c(MarketData$current.FC, MarketData$current.FC[FC.length])
  fwd <- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="constant")$y
  
  SimPrices <- array(NA, dim=c(options$nodays,options$noSims), dimnames=list(as.character(pricing.dates), 1:options$noSims))
  AsOf <- MarketData$AsOf 
  S0 <- rep(MarketData$current.spot ,options$noSims)
  #F0 <- fwd[1]
  F0 <- MarketData$last.fwd
  
  for (d in 1:options$nodays){
    entry = as.numeric(format(as.Date(AsOf), "%m"))
    spot.vol =  MarketData$Spot.vol[entry]
       
    z1 <- rnorm(options$noSims)
  
    aux <- exp(-0.5*spot.vol^2 + spot.vol*z1) 
    SimPrices[d,] <- S0*(fwd[d]/F0)*(aux)
    
    AsOf <- pricing.dates[d]
    S0 <- SimPrices[d,]
    F0 <- fwd[d]
  }
  
  # Plot the fwrd curve and few spot simulated paths
  which.sim <- sample(x=1:options$noSims, size=options$nopaths, replace=FALSE,prob=rep(1/options$noSims, options$noSims))
  y.range = range(fwd, SimPrices[,which.sim])
  plot(pricing.dates, fwd,ylim=y.range, type="p", col="blue", lwd=.2, main="Fwd Curve and Simulated Spot Prices")
  
  color = c("mediumturquoise","black" ,"orange","deeppink", "coral", "red", "black", "maroon",
  "darkgrey", "darkblue", "darkgreen", "brown") 
  
  for( i in 1:options$nopaths){
  lines(pricing.dates, SimPrices[,which.sim[i]], type="l", col=color[i], lty=1, lwd=1)}
  
return(SimPrices)
}


#windows()
#SimPricesGBM <- simulate.spots.GBM(MarketData, options)
  


