# Please see 'FC.PCA.example.r' for an example on how to run the functions in this file (i.e. simulate the forward curve 
# based on seasonal principal component analysis 

############ PART I: OBTAIN THE HISTORICAL PRICES OF A GIVEN SET OF FUTURES CONTRACTS  ###########################

# The function "get.hist.FwdCurve" obtains the historical forward prices (if ShowPrices = TRUE) 
# or log-returns ( if ShowPrices = FALSE) for a set of futures contracts
# It returns a list, whose first element is a dataframe of historical returns (no NA), the second one is the date 
# of last available prices of format ("YYYY-mm-dd") and the last element is the latest forward curve.
# The function "get.hist.FwdCurve" calls the following fncs "read.LIM", "Uts.String.right", and "Uts.Other.contract.letters".


# PLEASE MAKE SURE THE FOLLOWING FUNCTIONS ARE SAVED IN THE APPROPRIATE DIRECTORY OR MAKE THE NECESSARY CHANGES!!
source("H:/user/R/RMG/Models/Price/ForwardCurve/MultiFactor/Utils.r")     #Used to be: S:/Risk/Temporary/Fundamentals/  changed at 10/26/2006 9:19:03 AM
source("H:/user/R/RMG/Models/Price/ForwardCurve/MultiFactor/read.LIM.r")   #Used to be:  S:/Risk/Temporary/Fundamentals/   changed at 10/26/2006 9:23:02 AM
source("H:/user/R/RMG/Models/Price/ForwardCurve/MultiFactor/get.hist.FwdCurve.r")   # Used to be: S:\\Risk\\Temporary\\Spot Simulation\\  Changed at 10/26/2006 9:25:07 AM


############ PART II: Perform a Principle Component Analysis (PCA) ###########################
# This PC analysis is based on the seasons of pricing dates. That is if one wants to simulate the fwd curves 
# for Aug06 to Jan07 on July 12, the PCA is performed on the summer seasons in the historical data (obtained in part I)


# Inputs of PC.analysis

# 1. pricing.dates is a daily sequence of dates (format "YYYY-dd-mm") from class "Date" or "POSIXt"
# Example of pricing.dates is seq(as.Date("2006-07-13"),by="day",length.out=as.numeric(as.Date("2006-10-31")-as.Date("2006-07-13")))


# 2. seasons must be a list containing 3 elements. The first element gives the number of seasons. 
# The second element is a list that contains the months in each season (order is maintained). 
# Example of such a list is seasons=list(noSeas=3, months.in.season=list(c(5:9),c(11,12,1:3),c(4,10)))


# 3. hist.data is the output of part I. It is a list whose first element is a dataframe of historical 
# log-returns with rownames being dates in the form "YYYY-mm-dd" and # colnames being the NYMEX symbols of futures contracts (e.g. "Q6" "U6").
# The second element of this list is the last available date of data (will be used in part III)
# The third element of the list is the latest fwd curve (Will be used in part III)


# The following function "months.in.season" return the season number based on a vector of months.
# e.g. seasons=list(c("May", "Jun", "Jul", "Aug", "Sep"),c("Nov", "Dec" ,"Jan", "Feb", "Mar"),c("Apr", "Oct"))
# mo = c("Jul", "Aug" ,"Sep", "Oct", "Dec")
 
months.in.season <- function(seasons, mo){
  which.seas <- rep(NA,length(mo))
  for( i in 1:length(seasons)){
    for( j in 1:length(mo)){
      if(length((1:length(seasons[[i]]))[seasons[[i]] == mo[j]])!=0){which.seas[j] <- i}      # could have used 'which'
    }
  }
  return(which.seas)
}


# Principle component analysis for each season
PC.analysis <-function(pricing.dates, seasons=list(noSeas=3,months.in.season=list(c(5:9),c(11,12,1:3),c(4,10))) , hist.returns)
{
  # Convert month numbers in seasons to month names
  seas.mo <- NULL
  for(k in 1:seasons[[1]])        
    {seas.mo[[k]]<- months(as.Date(paste("2006-",seasons[[2]][[k]],"-1",sep="")), TRUE)}

  # Based on pricing dates, determine seasons
  pricing.mo <- unique(months(pricing.dates, TRUE))  
  which.season <- unique(months.in.season(seas.mo, pricing.mo))
  
  # which.mo <- vector("numeric")
  # for (k in which.season){
  # which.mo <- append(which.mo,seas.mo[[k]])
  # } 
  # which.mo gives the months you need in your hist.returns from part I
  
  ret.all = hist.returns
  
  dates.hist <- as.Date(rownames(ret.all))
  months.hist <- format(dates.hist, "%b")
  
  # Perform PCA for each season
  vol.fnc <- array(NA,c(ncol(ret.all),ncol(ret.all),length(which.season)))
  for (s in 1:length(which.season)){

    # Selectthe data in prices.all where the months correspond to the desired seasons (i.e. which.mo)
    which.row <- vector("numeric")
    for ( i in 1:length(seas.mo[[which.season[s]]])) {
      if(length((1:length(months.hist))[months.hist == seas.mo[[which.season[s]]][i]]) !=0){
          which.row <-  append(which.row,(1:length(months.hist))[months.hist == seas.mo[[which.season[s]]][i]]) }
    }
   
    
    # Select the log-returns based on the current season
    ret <- ret.all[which.row,]
          
    pc = prcomp(ret, retx=FALSE)
    std.dev.pc <- pc$sdev
    eig.vec <- pc$rotation
    rot.data <- pc$x

    # How much variation in the dataset is explained by the first 3 PC?
    variation <- cumsum(std.dev.pc)/sum(std.dev.pc)

    # Plot of the eigenvalues
    windows()
    plot(variation, xlab="PC", ylab="", type="b",col="mediumturquoise", lwd=2,
    main=paste("Cum. Variation Explained by PCA for season", which.season[s], sep=""))

    # Compute the main three factors functions : shifting, twisting and bending ??

    #Compute all factors
    for(j in 1:length(std.dev.pc)){vol.fnc[,j,s] = eig.vec[,j]*std.dev.pc[j]}
  }
  
  season.new <- NULL
  for (k in 1:length(which.season)){
  season.new[[k]] <- seas.mo[[which.season[k]]]
  }
  which.season.new <- months.in.season(season.new, months(pricing.dates, TRUE))
  res=list("PC -Vol Fnc-" = vol.fnc, "Price Season Indicators"= which.season.new)
  return(res)
}



############ PART III: Simulate the Fwd Curve ###########################
# the pc must be a list, whose first element is the PC (volatility functions) for each season, 
# and its second element is a vector of pricing.dates/seasons indicators
# add seaons (the same input as in past II)


#### TO DO: Add messages and NA to contracts that have already expired!!!!

simulate.Futures.PC <- function(MarketData, PC, options){

  if (length(options$noSims)==0){options$noSims <- 50}
  if (length(options$noFac)==0){options$noFac <- 3}

  DD <- length(MarketData$pricing.dt)      # number of pricing dates
  MM <- length(MarketData$contract.dt)     # number of contract months

      
  SimPrices <- array(NA, dim=c(DD,MM,options$noSims), dimnames=list(
     as.character(MarketData$pricing.dt), MarketData$contract.dt, 1:options$noSims))

  if (any(MarketData$contract.dt<MarketData$asOfDate)){
    cat("Some contracts have already expired!  Exiting."); flush.console()
    return(SimPrices)
  }

  
  F0  <- matrix(as.vector(unlist(MarketData$forward.prices)), nr=MM, nc= options$noSims)
  PC.vol.fnc <- PC[[1]]
  PC.ind <- PC[[2]]

  for (d in 1:DD){
    PC.price <- array(t(PC.vol.fnc[,1:options$noFac,PC.ind[d]]), dim=c(options$noFac, MM, options$noSims))
    z <- array(rnorm(options$noFac*MM*options$noSims), dim=c(options$noFac,MM, options$noSims))
    #ind  <- which(MarketData$contract.dt<=MarketData$pricing.dt[d])

    # Please note that delat.t is in days and so is the volatility of each principle component, so there is no need for scaling!!
    delta.t <- as.numeric(MarketData$pricing.dt[d] - MarketData$asOfDate)
    eta <- z*PC.price*sqrt(delta.t)

    # Note eta2 is same for all eta2[,,j]
    eta2 <- -0.5*(delta.t)*PC.price^2

    std <- matrix(NA, nr=options$noSims, nc=MM)
    std2 <- matrix(NA, nr=options$noSims, nc=MM)

    # See rowsum
    for( k in 1:options$noSims){
      std[k,] <-apply(eta[,,k],2,sum)
      std2[k,] <-apply(eta2[,,k],2,sum)
    }

    aux  <- t(exp(std+std2))   # aux is a matrix [MM , options$noSims]
    SimPrices[d,,] <- F0*(aux)
    MarketData$asOfDate <- MarketData$pricing.dt[d]
    F0 <- SimPrices[d,,]
  }
return(SimPrices)
}


