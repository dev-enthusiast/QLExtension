# Simulate congestion prices
# Where dMM has all the prices at nodes/zones (hourly or daily)
#
# Written by Adrian Dragulescu on 7-Mar-2005


simulate.spreads <- function(dMM, fdata, save, options){

  loptions   <- NULL
  sim.spread <- array(NA, dim=c(options$fdata$noDays, options$noZones, options$noSims))
  uSeasons   <- as.character(unique(fdata$season))
  ind.zones  <- which(colnames(dMM) %in% options$zone.names)
    
  for (season in uSeasons){
     ind.season  <- which(fdata$season==season)
     loptions$noSims <- length(ind.season)*options$noSims
     ind.hseason <- which(dMM$season==season)
     X <- dMM[ind.hseason, ind.zones]
     Y <- sample.from.Ndim(X, loptions)
     sim.spread[ind.season, , ] <- aperm(array(Y, dim=c(length(ind.season),
                                   options$noSims, options$noZones)), c(1,3,2))
  }
  dimnames(sim.spread) <- list(as.character(fdata$date), options$zone.names,
                              paste("sim",1:options$noSims, sep="."))
  return(sim.spread)  
}
