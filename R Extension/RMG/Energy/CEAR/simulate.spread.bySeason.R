# Simulate the spread, one season at a time. 
#
# Written by Adrian Dragulescu on 10-Mar-2005

simulate.spread.bySeason <- function(dMM, season, options){

  ind.Z  <- which(colnames(dMM) %in% options$zone.names)
 # ind.H  <- which(colnames(dMM) == options$ref.name)
  X      <- dMM[,ind.Z] #-dMM[,ind.H]    # the spread to the reference location
  ind.s  <- which(dMM$season==season)  # index of season
  ind.z1 <- which.max(apply(options$table.season, 2, sum)) 
  XX.ref <- X[ind.s,ind.z1] # select historical spread data for this season
  if (length(na.omit(XX.ref))<85){
    msg <- paste("Not enough data for the reference spread ", names(ind.z1),
    " in season ", season, ".  Using all historical data available.\n", sep="")
    cat(msg); flush.console()
    XX.ref <- na.omit(X[,ind.z1])
  } 
  options$sim.x <- sample.from.1D(XX.ref, options)$sim.x
 
  #-------------------------------------Do the 2D simulation----
  sspread <- matrix(NA, ncol=options$noZones, nrow=options$sample.size)
  sspread[,ind.z1] <- options$sim.x
  ind <- which(options$zone.names != names(ind.z1))
  for (z in ind){           # loop over remaining zones
    XX.2D <- X[ind.s, c(ind.z1, z)]  # the dependent spread
    if (nrow(na.omit(XX.2D))<85){
      msg <- paste("Not enough data for the spread ", options$zone.names[z],
      " in season ", season, ".  Using all historical data available.\n", sep="")
      cat(msg); flush.console()
      XX.2D <- na.omit(X[,c(ind.z1, z)])
    }
       
    res <- sample.2D.table(XX.2D[,1], XX.2D[,2], options)
    sspread[,z] <- res[,2]
  }
  colnames(sspread) <- options$zone.names
  options$sim.x     <- NULL
 
  return(list(sspread, options))
}
