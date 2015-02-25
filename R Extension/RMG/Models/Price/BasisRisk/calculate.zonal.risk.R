# Calculate the basis risk by multiplying the Q's with the price spreads. 
#
#
# Written by Adrian Dragulescu on 16-Mar-2005


calculate.zonal.risk <- function(fdata, sim.spread, Q, save, options){

  #-----------------------Flatten the 3D sim.spread for easy calculations-----------
  aux <- data.frame(spread=as.numeric(sim.spread[, , ]),
         date=rep(fdata$date[1:options$fdata$noDays], options$noSims*options$noZones),
         zone=rep(rep(options$zone.names,each=options$fdata$noDays), options$noSims), 
         sim =rep(1:options$noSims, each=options$fdata$noDays*options$noZones))
  aux$yyyymm <- format(aux$date, "%Y-%m")
  aux$zone   <- factor(aux$zone, levels=options$zone.names) 
  
  #----------------------Calculate the average risk by month, zone, and sim---------
  res <- tapply(aux$spread, list(aux$yyyymm, aux$zone, aux$sim), mean)
  res[,,1]

  Q <- Q[,-1,]                          # remove the reference zone
  ind <- which(options$fdata$months %in% rownames(Q))
  Q <- Q[ind,,]

  fs <- options$forward.marks[,-1]
  for (z in 1:options$noZones){
    fs[,z] <- options$forward.marks[,z+1]-options$forward.marks[,1]
  }
  fwd.spread <- array(fs, dim=c(dim(fs), options$noSims),
                      dimnames=c(dimnames(fs), list(1:options$noSims)))
  
  acc.types <- dimnames(Q)[[3]]
  risk      <- array(NA, dim=c(dim(res), length(acc.types)+1),
    dimnames=c(dimnames(res), list(c(acc.types,"all"))))
  for (accounting in acc.types){
    Q.3D <- array(Q[,,accounting], dim=c(length(rownames(Q)),
                  options$noZones,options$noSims))
    risk[,,,accounting] <- (res[ind, , ]-fwd.spread) * Q.3D
  }
  risk[,,,"all"] <- apply(risk, c(1,2,3), sum, na.rm=T)
  return(list(risk, aux))
}


