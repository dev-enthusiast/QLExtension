runUSims <- function(directory, FCurve, noSims, noPaths, mr, SP.corr, Spot.vol, interp) {

    wkdir <- directory

    if(file.exists(paste(wkdir,"sims2.csv", sep=""))) file.remove(paste(wkdir,"sims2.csv", sep=""))
    if(file.exists(paste(wkdir,"SimulatedUSpots.jpg", sep=""))) file.remove(paste(wkdir,"SimulatedUSpots.jpg", sep=""))

    MarketData <- options <- NULL
    #
    options$noSims <- noSims              # Number of simulations
    options$nodays <- 12*length(FCurve)   # Should be really called "Number of Points"; the number to the right makes sense for this app where we care for monthly quantities
    options$nopaths <- noPaths            # number of simulated paths to plot
    options$interp <- interp              # number of simulated paths to plot
    #   
    #
    MarketData$mr <- mr                     # 7.5
    MarketData$SP.corr <- rep(SP.corr, 12)  # 0.74 was found through history; Here we set all months to the same level
    MarketData$Spot.vol <- rep(Spot.vol,12) # a reasonable weekly number is 0.01.  All months set to equal annual vols
    MarketData$last.spot <- FCurve[1]       # initial condition for spots
    MarketData$last.fwd <- FCurve[1]        # initial condition
    MarketData$AsOf <- Sys.Date()-1         #as.Date(format(as.Date(AsOf),'%Y-%m-%d')  )
    
    MarketData$Pricing.dates <- seq(as.Date(MarketData$AsOf)+1,by="month",length.out=options$nodays)
    
    # Change the annual FC to one that is monthly with all 12 months having the same (annual) value
    for(i in 1:length(FCurve)) {
      for(j in 1:12) {
        MarketData$current.FC <- append(MarketData$current.FC, FCurve[i])
      }
    }
  
    if(file.exists(paste(wkdir,"sims2.csv", sep=""))) file.remove(paste(wkdir,"sims2.csv", sep=""))
 												
    SimSpots <- simulate.spots.U(MarketData, options, wkdir)
    
    write.csv(SimSpots, file = paste(wkdir,"sims2.csv", sep=""))
    
    #print(SimSpots)
    
    return(SimSpots)

  }