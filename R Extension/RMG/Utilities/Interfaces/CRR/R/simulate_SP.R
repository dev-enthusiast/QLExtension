#


################################################################
# Simulate settle prices for one month only
# 
# @param paths the paths data.frame
#
#
.simulate_SP_month <- function(paths, month=as.Date("2013-11-01"),
  hSP=NULL, noSims=1000, noCuts=NULL, hrs=NULL)
{
  #browser()
  if ( is.null(hSP) )
    stop("Please provide sampling history hSP")

  if ( is.null(hrs)) {
    auctionName <- format.dateMYY(month, -1)
    hrs <- count_hours_auction(auctions=auctionName, region=paths$region[1])
    hrs <- structure(hrs$hours, names=hrs$class.type)
  }

  
  # do each bucket separately
  dlply(paths, c("class.type"), function( paths, hSP, noSims ) {
    X <- do.call(cbind, hSP[paths$path.no])
    colnames(X) <- paths$path.no
    
    if (is.null(noCuts))
      noCuts <- round(sqrt(nrow(X)))
    
    hrsBucket <- hrs[[paths$class.type[1]]]
    
    # need to simulate hrsBucket * noSims observations
    sX <- sample_from_Ndim( X,
                            noSims = noSims*hrsBucket,  
                            noCuts = noCuts,
                            ensembles = FALSE )
    sX <- array(sX, dim=c(hrsBucket, noSims, ncol(X)))  
    sX <- aperm(sX, c(1,3,2))  
    
    avgSim <- apply(sX, c(2,3), mean)
    rownames(avgSim) <- paths$path.no
    
    list(sp    = avgSim,
         hours = hrsBucket)
  }, hSP, noSims)
    
}


################################################################
# Simulate settle prices.
# Split a longer auction into individual months.
# Simulate separately different buckets.
# @param auction an FTR_Auction object
# @param
# @param hSP a zoo timeseries
# @return sSP a matrix of simulated Settle Prices, size [path.no, noSims]
#
simulate_SP_auction <- function(paths, auction, hSP=NULL,
  noSims=1000, noCuts=NULL)
{
  if (class(auction) != "FTR_Auction")
    auction <- parse_auctionName(auction, region=paths$region[1])
  
  if (is.null(hSP)) 
    stop("Please provide sampling history hSP")

  # simulate settles by month
  mSP <- lapply(auction$months,
    function(month, paths, hSP, noSims, noCuts) {
      c(list(month=month),
        .simulate_SP_month(paths, month, hSP, noSims, noCuts))
  }, paths, hSP, noSims, noCuts)

  # aggregate the months 
  uBuckets <- unique(paths$class.type)
  sSP <- matrix(NA, nrow=nrow(paths), ncol=noSims)
  rownames(sSP) <- paths$path.no
  
  for (bucket in uBuckets) {
    res <- hours <- 0
    for (m in 1:length(mSP)) {             # loop over the months
      mHours <- mSP[[m]][[bucket]]$hours 
      res    <- res + mSP[[m]][[bucket]]$sp * mHours
      hours  <- hours + mHours
    }
    
    sSP[rownames(res),] <- res/hours
  }

  
  sSP
}



################################################################
################################################################
# 
.test <- function()
{
  source("H:/user/R/RMG/Utilities/Interfaces/FTR2/R/simulate_SP.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR2/R/sample_from_Ndim.R")
  
  # use 3 months of history
  auction <- "M13-3M"
  hSP <- get_hourly_SP(paths, auction)
  lapply(hSP, summary)
  
  sSP <- simulate_SP_auction(paths, auction, hSP=hSP)
  apply(sSP, 1, summary)
  

}



