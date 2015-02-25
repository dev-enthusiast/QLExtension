############################### Gas Daily Daily Option ####################################
# Example
# CCG sells UTILITY a Gas Daily Daily Option
# Type: Call
# Term: 9/1/2006 to 10/31/2006
# Volume: Fixed daily at 50,000 MMBtu/day
# Index: Henry Hub
# CCG receives Index + $0.02
# UTILITY will notify CCG between 7:30am-8:00am CST on day prior to delivery.
# Margin = (Index +0.02 - Trader's purchasing price)*volume

# Step I: Simulate spot Prices during the term of this deal (i.e. simulate HH Index prices)
# using current fwd curve, historical correlations, and volatilities.

# deal.index is the symbol of a given index (e.g. IGBBL00 for NYMEX)
# deal.start and deal.end are the start and end dates for this deal: "mm/dd/yyyy"
# deal.volume is the daily volume at least 1 futures contract (10000 MMBtu)
# fixed.volume = TRUE. Whether it is a fixed volume or flexible
# num.years = 3. The number of years of historical data to estimate required parameters.
# intraday.dist ="Uniform" or "triangular"
# noSims: number of MC simulations
# spot.vol: historical daily spot volatilities for each calendar Month.
# directory : where all R functions are saved.


# Application of the following function : GDD.evaluate
#
#GDD.evaluate(deal.Index = "IGBBL00",deal.start = "9/1/2006",deal.end = "10/31/2006"
#   , deal.volume = 50000, fixed.volume = TRUE, num.years=3, intraday.dist ="Uniform", noSims=1000, spot.vol, 
#   directory="S:\\Risk\\Temporary\\Spot Simulation\\", ToPlot.ratios=FALSE)

GDD.evaluate <- function(deal.Index = "IGBBL00",deal.start = "9/1/2006",deal.end = "10/31/2006"
   , deal.volume = 50000, fixed.volume = TRUE, num.years=3, intraday.dist ="Uniform", noSims=1000, spot.vol, 
   estm.vol="Yes", directory="S:\\Risk\\Temporary\\Spot Simulation\\", ToPlot.ratios=FALSE) {
       
  deal.start = as.Date(deal.start, "%m/%d/%Y") 
  deal.end = as.Date(deal.end, "%m/%d/%Y") 
  deal.length = as.numeric(as.Date(deal.end) - as.Date(deal.start))+1
  deal.contracts = 50000/10000

  if(fixed.volume){volume.type="Fixed"}else{volume.type="Flexible"}
  
  # Obtain historical Index, High and Low prices for Henry Hub
  source(paste(directory,"get.Index.r", sep=""))
  hist.index <- get.Index(Index=deal.Index , time.period=list(start.mm.dd=format(as.Date(deal.start),"%m/%d"),
  end.mm.dd=format(as.Date(deal.end), "%m/%d") , start.year=as.numeric(format(Sys.Date(), "%Y"))-num.years)
  ,Verbose=FALSE)

  # Obtain current fwrd curve as of last trading date
  months.in.term = unique(format(as.Date(rownames(hist.index)),"%b"))
  
  source(paste(directory, "get.hist.FwdCurve.r", sep=""))
  current.FC <- get.hist.FwdCurve(commodity1="Close of NG",start.mo.FC=format(deal.start,"%m"),
  start.year.FC=format(deal.start,"%Y"), num.months=length(months.in.term),
  start.date.hist=format(Sys.Date()-10, "%m/%d/%Y"), Verbose=FALSE, ShowPrices=TRUE)[[3]]

  # Obtain the historical correlations and spot volatility
  source(paste(directory, "FEA.spot.prompt.analysis.r", sep=""))
  corr.vol.data <- monthly.vol.corr(Index= paste("Index of ",deal.Index, sep="") , Prompt="Close of NG", 
  start.date.hist=paste("01/01/",as.numeric(format(Sys.Date(), "%Y"))-num.years , sep=""),
  Verbose=FALSE, Plot=F, directory=directory)

  # Create MarketData and options  to simulate spot prices
  MarketData <- options <- NULL

  options$noSims <- noSims    # Number of simulations
  options$nodays <-  deal.length
  options$nopaths <- 4    # Number of paths to be plotted

  MarketData$mr <- corr.vol.data[[1]]
  MarketData$SP.corr <- corr.vol.data[[2]][,ncol(corr.vol.data[[2]])]
  MarketData$Spot.vol <- corr.vol.data[[3]][,ncol(corr.vol.data[[3]])]
  
  if (estm.vol=="Yes"){
    MarketData$Spot.vol <- corr.vol.data[[3]][,ncol(corr.vol.data[[3]])]
    }else{MarketData$Spot.vol <- spot.vol}

  MarketData$last.spot <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),1]
  MarketData$last.fwd <- corr.vol.data[[4]][nrow(corr.vol.data[[4]]),2]
  MarketData$AsOf <- as.Date(rownames(corr.vol.data[[4]])[nrow(corr.vol.data[[4]])])
  MarketData$Pricing.dates <- seq(as.Date(deal.start, format="%Y-%m-%d"),by="day",length.out=deal.length)
  MarketData$current.FC <- current.FC

  # Simulate Index Prices
  jpeg(paste(directory, "SimPrices.jpg", sep=""), width = 640, height = 520)
  sim.spots <- simulate.spots.FEA(MarketData, options, RemoveWkd=FALSE)
  dev.off()
  
  # Step II: Simulate from Historical Low and high prices for Henry Hub during the term of this contract
  
  # Separate by month (Sep and Oct)
  high.ratio <- NULL
  low.ratio <- NULL
  for(i in 1:length(months.in.term)){
    entry= which(format(as.Date(rownames(hist.index)),"%b" )==months.in.term[i])
    high.ratio[[i]] = hist.index[entry,2]/hist.index[entry,1]  # The min value of high.ratio is 1
    low.ratio[[i]] =  hist.index[entry,3]/hist.index[entry,1]  # The max value of low.ratio is 1

    if(ToPlot.ratios){
      windows()
      par(mfrow=c(2,2))
      hist(high.ratio[[i]], main=paste("Histogram of High/Index for ",months.in.term[i],sep=""))
      plot(density(high.ratio[[i]]), main=paste("Density of High/Index for ",months.in.term[i],sep=""))
      hist(low.ratio[[i]], main=paste("Histogram of Low/Index for ",months.in.term[i],sep=""))
      plot(density(low.ratio[[i]]), main=paste("Density of Low/Index for ",months.in.term[i], sep=""))
    }
  }

  # Simulate high and low values for Sep. and Oct.

  margin <- matrix(NA, nc=options$noSims, nr=length(MarketData$Pricing.dates))

  for (d in 1:length(MarketData$Pricing.dates)){
    entry = which(months.in.term==format(as.Date(MarketData$Pricing.dates[d]),"%b"))
    sim.high = sim.spots[d,]*sample(high.ratio[[entry]], options$noSims, replace = TRUE)
    sim.low = sim.spots[d,]*sample(low.ratio[[entry]], options$noSims, replace = TRUE)

    if(intraday.dist =="Triangular"){
    source("S:\\Risk\\Temporary\\Spot Simulation\\TriangDist.r")
    apply.triang <- function(i, n, sim.low, sim.mode, sim.high){ rtriang(n, min.val=sim.low[i], mode.val=sim.mode[i], max.val=sim.high[i])}

    # Using a triangular distribution for intraday distribution is less conservative than Uniform distr.
    intraday <- apply(matrix(1:options$noSims), 1, apply.triang, n=1, sim.low=sim.low, sim.mode=sim.spots[d,], sim.high=sim.high)
    }else{
    # Using a Uniform draw is more conservative.
    intraday = runif(options$noSims,sim.low,sim.high)
    }

    margin[d,] = (sim.spots[d,]-intraday)
  }


    # Add option premium ??
    # if(length(deal.premium)!=0){margin = margin + deal.premium}

    
    # Is the volume fixed or random?
    # If random, then the counterparty will requests the max volume to be delivered when it is NOT economical for CCG.
    
    if(fixed.volume){
    deal.margin <- apply(margin,2,sum)/(deal.length)
    }else{
    random.volume = matrix(10000*sample(1:deal.contracts,options$noSims*length(MarketData$Pricing.dates),  replace = TRUE)
    ,nc=options$noSims, nr=length(MarketData$Pricing.dates))

    # for negative margins, volume is max (i.e. deal.volume)
    for( k in 1:length(MarketData$Pricing.dates)){random.volume[k,which(margin[k,]<0)] <- deal.volume}

    tot.margin = margin*random.volume
    deal.margin <- apply(tot.margin,2,sum)/apply(random.volume,2,sum)
    }


    # Ststisitcs
    deal.summary <- t(cbind(names(summary(deal.margin)),summary(deal.margin)) )
    deal.quantiles <- t(cbind(names(quantile(deal.margin, probs=c(0,0.025,0.05,0.075,0.1,0.5,0.9,0.95,0.975,0.99,0.995, 1)))
    ,round(quantile(deal.margin, probs=c(0,0.025,0.05,0.075,0.1,0.5,0.9,0.95,0.975,0.99,0.995, 1)),4)))
    
    jpeg(paste(directory, "ExpectedPayoff.jpg", sep=""), width = 640, height = 520)
    par(mfrow=c(2,1))
    hist(deal.margin,xlab="", main ="Histogram of Expected Margin/MMBtu", sub=paste(intraday.dist," Intraday Dist and ", volume.type, " Volume", sep=""), col.main="Blue")
    boxplot(deal.margin, main ="Box-Plot of Expected Margin/MMBtu", col.main="Blue")
    dev.off()
    
   results <- list("StatSummary"= deal.summary, "Quantiles"= deal.quantiles)
return(results)
}    


