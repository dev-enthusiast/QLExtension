FTR.basicStats <- function( hSP, ISO.env )
{
  rLog("DEPRECATED!!! ")
  
  # calculate some statistics
  rLog("Calculate quantiles ...")
  probs <- c(0, 0.01, 0.05, 0.25, 0.5, 0.75, 0.95, 0.99, 1)
  aux <- FTR.pathApply( NULL, hSP, ISO.env, quantile, prob=probs, na.rm=T )
  names(aux)[-1] <- paste("p", 100*probs, sep="")
  paths <- aux

  rLog("Calculate mean ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env,  mean, na.rm=T )
  names(aux)[2] <- "mean"
  paths <- merge(paths, aux, all.x=T)
  
  rLog("Calculate sd ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env,  sd, na.rm=T )
  names(aux)[2] <- "sd"
  paths <- merge(paths, aux, all.x=T)

  rLog("Calculate EWin ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, EWin)
  names(aux)[2] <- "EWin"
  paths <- merge(paths, aux, all.x=T, sort=F)

  rLog("Calculate ELoss ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, ELoss)
  names(aux)[2] <- "ELoss"
  paths <- merge(paths, aux, all.x=T, sort=F)

  rLog("Calculate days below, at, above zero ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, Probs.0, counts=TRUE )
  names(aux)[2:4] <- c("days.lt.0", "days.eq.0", "days.gt.0")
  paths <- merge(paths, aux, all.x=T, sort=F)

  return(paths)
}
