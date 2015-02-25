###############################################
## function to report some key statistics
## of the simulation result for a single curve
##
## output should be be either in simple text or latex
## I'll leave it for now.
###############################################
reports.sim <- function(curvenames, FP.options, type=c("txt", "latex")) {
  
  type <- match.arg(type)
  if(type=="latex")
    require(xtable)
  browser()
  ## load one historical prices to get number of historical days
  allmkt <- unique(sapply(curvenames, function(x) strsplit(x, " ")[[1]][2]))
  load(paste(FP.options$hpdata.dir, "all.", allmkt[1], ".hp.RData", sep=""))
  nhistDay <- dim(hp)[2]-2
    
  nc <- length(curvenames)
  
  ## init holder for prices 
  simres <- array(NA, dim=c(nc,length(FP.options$sim.days),FP.options$D,
                        FP.options$nsim))
  allhp <- array(NA, dim=c(nc, FP.options$D, nhistDay))
                    
  ## load in simulation result and historical prices
  for(i in 1:nc) {
    load(paste(FP.options$targetdata.dir, curvenames[i], ".sim.RData", sep=""))
    ## sometimes dimensions of res are different, careful
    ncontract <- dim(res)[2]
    simres[i,,(FP.options$D-ncontract+1):FP.options$D,] <- log(res)
  }
  ## load in historical prices
  ## first load in all data for those markets
  allmkt <- unique(sapply(curvenames, function(x) strsplit(x, " ")[[1]][2]))
  tmphp <- NULL
  for(i in 1:length(allmkt)) {
    load(paste(FP.options$hpdata.dir, "all.", allmkt[i], ".hp.RData", sep=""))
    tmphp[[allmkt[i]]] <- hp
  }
  ## then load for data for those curves - this way we don't
  ## have to load data again and again if they are from the same market
  npricingday <- dim(tmphp[[1]])[2]
  for(i in 1:nc) {
    mkt <- strsplit(curvenames[i], " ")[[1]][2]
    idx <- tmphp[[mkt]][,1]==curvenames[i]
    ncontract <- sum(idx)
    allhp[i,(FP.options$D-ncontract+1):FP.options$D,] <- as.matrix(tmphp[[mkt]][idx, 3:npricingday])
  }
  ## need to be careful here. 
#  dimnames(allhp) <- list(NULL, as.character(tmphp[[mkt]][idx,2]), NULL)

  result.single <- result.multi <- NULL
#  rm(tmphp)
#  gc()
  ### first do single curve reports
  ## number of days with daily simulation
  ndaily <- sum(diff.bday(FP.options$sim.days)==1) + 1 
  for(i in 1:nc) {
    cat("\n\t Report for ", curvenames[i], "\n")
    
    hp.thiscurve <- log(allhp[i,,])
    ## 1. daily vol
    ## historical 
    vol.hist <- apply(hp.thiscurve, 1, function(x) sd(diff(x, na.rm=T),na.rm=T))
    ## simulated - note only the daily data can be used
    tmp <- matrix(NA, nrow=FP.options$nsim, ncol=FP.options$D)
    for(isim in 1:FP.options$nsim) {
      tmp[isim,] <- apply(simres[i,1:ndaily,,i], 2, function(x) sd(diff(x), na.rm=T))
    }
    vol.sim <- colMeans(tmp)
    cat("\nDaily volatilites of forward vs. historical prices: \n")
    aa <- data.frame(vol.hist, vol.sim)
    print(t(aa[1:7,]), digit=4)

    result.single[[curvenames[i]]]$daily.vol <- aa

    ## 2. weekly vol
    ## historical 
    vol.hist <- apply(hp.thiscurve, 1, function(x) sd(diff(x, 5),na.rm=T))
    ## simulated 
    tmp <- matrix(NA, nrow=FP.options$nsim, ncol=FP.options$D)
    for(isim in 1:FP.options$nsim) {
      tmp[isim,] <- apply(simres[i,1:ndaily,,i], 2, function(x) sd(diff(x,5),na.rm=T))
    }
    vol.sim <- colMeans(tmp)
    cat("\nWeekly volatilites of forward vs. historical prices: \n")
    aa <- data.frame(vol.hist, vol.sim)
    print(t(aa[1:7,]), digit=4)
  
    result.single[[curvenames[i]]]$weekly.vol <- aa
    
    ## 3. 10-day vol
    ## historical 
    vol.hist <- apply(hp.thiscurve, 1, function(x) sd(diff(x, 10),na.rm=T))
    ## simulated note only the daily data can be used
    tmp <- matrix(NA, nrow=FP.options$nsim, ncol=FP.options$D)
    for(isim in 1:FP.options$nsim) {
      tmp[isim,] <- apply(simres[i,1:ndaily,,i], 2, function(x) sd(diff(x,10),na.rm=T))
    }
    vol.sim <- colMeans(tmp)
    cat("\nBi-weekly volatilites of forward vs. historical prices: \n")
    aa <- data.frame(vol.hist, vol.sim)
    print(t(aa[1:7,]), digit=4)

    result.single[[curvenames[i]]]$biweekly.vol <- aa
    cat("\n===========================================================\n")
  } ## finish report single curves
  if (length(curnames)==1){
    return(list(single=result.single))
  }
  
  ## do multi-curve reports
  ## 1. correlations in historical prices
  ## loop for contract date
  cnames <- sapply(curvenames, function(x) substr(x, 8,100))
  ct.dt <- dimnames(res)[[2]]
  for(i in 1:FP.options$D) {
    tmpres <- NULL
    ## historical
    tmp <- t(allhp[,i,])
    colnames(tmp) <- cnames
    tmpres$cor.hist <- cor(tmp, use="pairwise.complete.obs")
    ## simulated
    tmpcor <- 0
    for(isim in 1:FP.options$nsim) {
      tmpcor <- tmpcor + cor(t(simres[,,i,isim]), use="pairwise.complete.obs")
    }
    tmpres$cor.sim <- tmpcor/FP.options$nsim
    dimnames(tmpres$cor.sim) <- dimnames(tmpres$cor.hist)
    ## assign
    result.multi[[ct.dt[i]]] <- tmpres
  }

  ## final result
  result <- list(single=result.single, multi=result.multi)
  return(result)
  
}

