# Application of FEA.spot.prompt.analysis.r
source("S:\\Risk\\Temporary\\Spot Simulation\\FEA.spot.prompt.analysis.r")
# Obtain the correlation, volatility data

# estimate the correlations and volatilities
corr.vol.data <- monthly.vol.corr(Index= "Index of IGBBL00", Prompt="Close of NG", start.date.hist="01/01/2001", Verbose=FALSE, Plot=F,directory="S:\\Risk\\Temporary\\Spot Simulation\\")
source("S:\\Risk\\Temporary\\Spot Simulation\\getFCFromLIM.r") # Obtains the FC
source("S:\\Risk\\Temporary\\Spot Simulation\\get.hist.index.prompt.r") # Obtains historical index data

# Specify MarketData and options
MarketData <- options <- NULL

options$noSims <- 1000     # Number of simulations
options$nodays <- 365  # Number of days in year
options$nopaths <- 4 # plot 10 random simulated paths


MarketData$mr <- corr.vol.data[[1]]
MarketData$SP.corr <- corr.vol.data[[2]][,ncol(corr.vol.data[[2]])]
MarketData$Spot.vol <- corr.vol.data[[3]][,ncol(corr.vol.data[[3]])]
MarketData$last.spot <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),1]
MarketData$last.fwd <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),2]
MarketData$AsOf <- as.Date(rownames(corr.vol.data[[4]])[nrow(corr.vol.data[[4]])])
MarketData$Pricing.dates <- seq(as.Date(MarketData$AsOf)+1,by="day",length.out=options$nodays)
MarketData$current.FC <- getFCFromLIM(theSymbol="Close of NG", mosFwd=12,monthNames=paste("F(",1:12,"m)", sep=""), Verbose =F)[[2]]



# Simulate paths of spot prices
windows()
sim.spots <- simulate.spots.FEA(MarketData, options, RemoveWkd=FALSE)

# Verify that the simulated spot averages out to the assumed forward?
avg.spots = rep(NA, length(MarketData$current.FC))

# Divide the simulated spot prices into 12-months intervals
end.points= floor(seq(1,nrow(sim.spots),length=length(MarketData$current.FC)+1))
for(i in 1:(length(end.points)-1)){
  if(i == (length(end.points)-1)){days.seq = end.points[i]:end.points[i+1]
  }else{days.seq = end.points[i]:(end.points[i+1]-1)}

  avg.spots[i] = mean(sim.spots[days.seq,])
}
cbind("Avg Spot"=avg.spots, "FC"=MarketData$current.FC )


