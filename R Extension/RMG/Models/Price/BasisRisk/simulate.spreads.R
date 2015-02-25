# Simulate zonal spreads 
# dMM has all the daily prices at zones
#
# Written by Adrian Dragulescu on 7-Mar-2005


simulate.spreads <- function(dMM, fdata, save, options){

  sim.spread <- array(NA, dim=c(options$fdata$noDays, options$noZones, options$noSims))
  uSeasons   <- as.character(unique(fdata$season))
  ind   <- which(colnames(dMM) %in% options$zone.names)
  
  options$table.season <- matrix(NA, nrow=4, ncol=length(ind))
  aux <- cbind(stack(dMM[,ind]), season=rep(dMM$season, length(ind)))
  aux <- na.omit(aux)
  aux$ind <- ordered(aux$ind, levels=options$zone.names) 
  (options$table.season <- table(aux$season, aux$ind)) #historical days by season & zone
  
  for (season in uSeasons){
     ind.season <- which(fdata$season==season)
     options$sample.size <- length(ind.season)*options$noSims
     res        <- simulate.spread.bySeason(dMM, season, options)
     options    <- res[[2]]
     sim.spread[ind.season, , ] <- aperm(array(res[[1]], dim=c(length(ind.season),
                                   options$noSims, options$noZones)), c(1,3,2))
  }
  dimnames(sim.spread) <- list(as.character(fdata$date), options$zone.names,
                              paste("sim",1:options$noSims, sep="."))
  return(sim.spread)  
}

#  stack(as.data.frame(sim.spread[1:3, ,1:2]))


#  plot(XX[,1], type="l")
#  lines(sim.spread[,1], col="red", type="l")
    
