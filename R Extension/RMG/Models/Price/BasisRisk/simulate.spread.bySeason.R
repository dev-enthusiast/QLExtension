# Simulate the spread, one season at a time. 
#
# Written by Adrian Dragulescu on 10-Mar-2005

simulate.spread.bySeason <- function(dMM, season, options){

  ind.Z  <- which(colnames(dMM) %in% options$zone.names)
  ind.H  <- which(colnames(dMM) == options$ref.name)
  X      <- dMM[,ind.Z]-dMM[,ind.H]        # the spread to the reference location
  XX     <- X[which(dMM$season==season), ] # pick only this season
    
  loptions=options
  loptions$noSims <- options$sample.size
  sspread <- sample.from.Ndim(XX, loptions)
  colnames(sspread) <- options$zone.names
 
  return(list(sspread, options))
}
