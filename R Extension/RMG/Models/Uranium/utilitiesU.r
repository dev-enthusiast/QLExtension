simulate.spots.U <- function(MarketData, options, wkdir){

  FC.length =  length(MarketData$current.FC)

  # Create a sequence of pricing dates
  pricing.dates = MarketData$Pricing.dates

  # Smooth the FC (constant each month)
  temp.FC= c(MarketData$current.FC, MarketData$current.FC[FC.length])

  # Constant FC
  if(options$interp) {
      fwd <- approx(1:length(FCurve), FCurve, n=options$nodays, method="linear")$y 
      original <<- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="constant")$y
    } else  {
      fwd <- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="constant")$y
      original <- approx(x=1:(FC.length+1), y=temp.FC, n=options$nodays, method="constant")$y
    }

  SimPrices <- array(NA, dim=c(options$nodays,options$noSims), dimnames=list(as.character(pricing.dates), 1:options$noSims))
  AsOf <- MarketData$AsOf
  S0 <- rep(MarketData$last.spot ,options$noSims)
  F0 <- MarketData$last.fwd

  #options$nodays
  for (d in 1:options$nodays){
    delta.t = as.numeric(pricing.dates[d] - AsOf) /365
    entry = as.numeric(format(AsOf, "%m"))
    spot.vol =  runif(1, min=.25*MarketData$Spot.vol[entry], max = 1.75*MarketData$Spot.vol[entry])
    SP.corr = MarketData$SP.corr[entry]

    z1 <- rnorm(options$noSims)
    z2 <- rnorm(options$noSims)

    aux <- exp((1/MarketData$mr*delta.t)*log(S0/F0) + spot.vol*(SP.corr*z1*sqrt(delta.t) + sqrt(delta.t*(1-SP.corr^2))*z2)) # Not math. correct

    SimPrices[d,] <- fwd[d]*(aux)
    if (d==1) SimPrices[d,] <- fwd[d] *rnorm(1,1,.15) # First day usually can be rather unsettled, so we force it to be close to the FWD

    AsOf <- pricing.dates[d]
    S0 <- SimPrices[d,]
    F0 <- fwd[d]
  }

  if(file.exists(paste(wkdir,"SimulatedUSpots.jpg", sep=""))) file.remove(paste(wkdir,"SimulatedUSpots.jpg", sep=""))

  which.sim <- sample(x=1:options$noSims, size=options$nopaths, replace=FALSE,prob=rep(1/options$noSims, options$noSims))
  y.range = range(fwd,SimPrices[,which.sim])
  jpeg(paste(wkdir, "SimulatedUSpots.jpg", sep=""), width = 640, height = 520)
  plot(pricing.dates, fwd, ylim=y.range, type="p", col="blue", lwd=2, main="Fwd Curve and Simulated Spot Prices")
        color = c("mediumturquoise","black" ,"orange","deeppink", "coral", "red", "black", "maroon",
        "darkgrey", "darkblue", "darkgreen", "brown")
  lines(pricing.dates, original, ylim=y.range, type="p", col="coral", siz=3)
  for( i in 1:options$nopaths){
    lines(pricing.dates, SimPrices[,which.sim[i]], type="l", col=color[i], lty=1, lwd=1)}
  dev.off()

  SimSpots <<- SimPrices
  
  return(SimPrices)

}


