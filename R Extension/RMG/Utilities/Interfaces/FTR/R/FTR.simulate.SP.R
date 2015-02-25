FTR.simulate.SP <- function( hSP, options, noSims=5000, noSamples=10,
  noEns=100, reversePaths=NULL, check=TRUE )
{
  if (!is.null(reversePaths)){
    hSP <- hSP[, -which(colnames(hSP) %in%
                       as.character(reversePaths$path.no.rev))]
  }
  
  ISO.env  <- eval(parse(text=paste(options$region)))  # inherit it 
  loptions <- NULL
  loptions$noSims <- noEns
  loptions$no.cuts <- 100  # how many quantiles I use 

  if (length(grep("-", options$auction))>0) { # if multi-month auction
    months <- seq(options$start.dt, options$end.dt, by="1 month")
    months.YMM <- format.dateMYY(months, -1)
  } else {
    months.YMM <- options$auction
  }
  
  no.batches <- noSims/(noEns*noSamples)
  sSP <- array(0, dim=c(ncol(hSP), noSims)) # set to zero for multi-months
  rownames(sSP) <- colnames(hSP)

  for (m in months.YMM){
    rLog(paste("Simulating month", m))
    moptions <- FTR.makeDefaultOptions(m, ISO.env)
    for (b in 1:no.batches){
      rLog(paste("Simulating batch", b, "of", no.batches))
      seSP <- .sample.from.Ndim(as.matrix(hSP), ensembles=TRUE, loptions)
      seSP <- aperm(seSP, c(3,2,1))
  
      # cut simulated history in several ways
      ind.dates.sims <- .get.starting.dates(index(hSP), moptions, noSamples)
    
      # calculate the monthly mean
      aux <- array(NA, dim=c(noEns, ncol(hSP), noSamples))
      for (d in 1:noSamples){
        ind <- ind.dates.sims[d]:(ind.dates.sims[d]+moptions$days["FLAT"]-1)
        aux[,,d] <- rowMeans(seSP[,,ind], na.rm=T, dims=2)
      }
      aux <- aperm(aux, c(2,1,3))
      # return in shape [ncol(hSP), noSims*noSamples]
      aux <- array(aux, dim=c(dim(aux)[1], dim(aux)[2]*dim(aux)[3]))
  
      ind <- ((b-1)*noEns*noSamples+1):(b*noEns*noSamples)
      sSP[,ind] <- sSP[,ind] + aux
    }
  }
  if (length(months.YMM)>1) sSP <- sSP/length(months.YMM) # average it
  
  if (!is.null(reversePaths)){   # append the reverse paths
    rSP <- array(0, dim=c(nrow(reversePaths), noSims))
    rSP <- - sSP[as.character(reversePaths$path.no), ] 
    rownames(rSP) <- as.character(reversePaths$path.no.rev)

    aux <- rbind(sSP, rSP)
    rownames(aux) <- c(rownames(sSP), rownames(rSP))
    sSP <- aux[order(as.numeric(rownames(aux))), ]   # order them ascendingly
  }  

  if (check){
    ind <- which(apply(is.na(sSP), 1, any))
    rLog(paste("Paths with NA's:", paste(ind, sep="", collapse="', '")))
  }

  sSP <- round(sSP, 4)
  return(sSP)
}


################################################################
# Get starting dates for the simulated "months" by assigning a
# higher weight to similar months, and to last 3 months. 
#
.get.starting.dates <- function( hdates, options, noSims )
{
  hdates <- hdates[-((length(hdates)-options$days["FLAT"]):length(hdates))]
  
  month  <- as.POSIXlt(hdates)$mon + 1 
  weight <- rep(1, length(hdates))

  # last 3 months get a weight of 2
  ind.lm <- which(hdates > seq(options$asOfDate, by="-3 months",
                               length.out=2)[2])
  weight[ind.lm] <- 2
  
  # similar months get a weight of 3
  ind.sm <- which(month %in% options$nby.months)
  weight[ind.sm] <- 3

  cF <- c(0, cumsum(weight)/sum(weight))
  
  rand <- runif(noSims)

  ind.dates.sims <- cut(rand, cF, labels=FALSE)
  if (any(is.na(ind.dates.sims))){
      browser()
  }
  
  return(ind.dates.sims)
}


################################################################
# Find maximum contiguous complete price data for all paths.
# Report on it, and cut the prices
#
.filter.contiguous.hSP <- function(paths, hSP, report=TRUE, cut=TRUE)
{
  ind.rm  <- NULL
  buckets <- split(paths$path.no, paths$class.type)

  range.buckets <- matrix(NA, ncol=2, nrow=length(buckets),
    dimnames=list(names(buckets), c("min", "max")))
  for (b in names(buckets)){
    aux <- hSP[, as.character(buckets[[b]])]
    ind <- which(apply(is.na(aux), 1, all))# if there are NA's for all paths
    if (length(ind)>0) aux <- aux[-ind,]
    max.na <- apply(is.na(aux), 2, function(x){
      if (any(x)) max(which(x)) else 1})

    start.dt <- index(aux)[max.na]
    ind <- which(start.dt > (Sys.Date() - 800))
    if (length(ind)>0){
      rLog("History shorter than 2 years.  Removing several paths.")
      ind.rm <- c(ind.rm, colnames(aux)[ind])
      aux <- aux[,-ind]
    }
    range.buckets[b,] <- as.character(range(index(aux)))
  }
  range.max <- as.Date(c(max(range.buckets[,"min"]),
                         min(range.buckets[,"max"])))

  ndays <- range.max[2]-range.max[1]
  if (as.numeric(ndays) < 730){
    stop(paste("You only have ", as.numeric(ndays),
               " contiguous historical days!", sep=""))
  } else {
    rLog(paste("Subsetting historical prices from ", range.max[1], 
               " to ", range.max[2], ".", sep=""))
    hSP <- window(hSP, start=range.max[1], end=range.max[2])
  }
  if (!is.null(ind.rm)){
    paths <- subset(paths, !(path.no %in% ind.rm))
    hSP   <- hSP[, which(!(colnames(hSP) %in% ind.rm))]
    cat(paste("Removed: ", paste(ind.rm, sep="", collapse=", "), "\n"))
  }
  
  return(list(hSP, paths))
}
  
################################################################
# Simulate N random variables by sampling from the joint probability distribution.
# The algorithm bins the observations into their quantiles and then bootstraps in 
# the quantile space.  Extra variability is added by uniformly sampling inside
# each quantile.  The [min, max] range in each variable is extended by doing
# an entropy approach like the one suggested by Vonod (see meboot package) but on
# quantiles. 
#
# Where: X - is a data frame with N variables
#        ensembles - boolean, maintain the same quantile time order as X
#                  when simulating
#        options$historical - if you want to bootstrap only
#        options$noSims - no of points to return
#
# Returns: Y the simulated variables, as a matrix c(noSims, N)
#            if ensembles=TRUE, an array with dims c(dim(X), noSims)
#
# AAD (2-Jun-08): added a parameter for returning ordered observations.
#
# AAD (19-Oct-07): added forecast, extended the
#   ranges using the entropy approach, and add a transition matrix.
#
# First version by Adrian Dragulescu on 9-Jul-2005

.sample.from.Ndim <- function(X, ensembles=FALSE, options=NULL)
{
  if (length(options$historical)==0){
    options$historical <- FALSE   # use "enhanced" history
  }
  if (length(options$no.cuts)==0){options$no.cuts <- 10}
  if (length(options$noSims)==0){options$noSims <- 100}
  if (length(options$jitter.default)==0){options$jitter.default <- 10^-10}
  myQuantiles <- function(x, prob.cuts){quantile(as.numeric(x),
                               probs=prob.cuts, na.rm=T)}
  myFindInt <- function(x, probs){
    findInterval(x, sort(myQuantiles(x,probs)), rightmost.closed=T)} 
  options$prob.cuts <- c(0, seq(1/options$no.cuts/2, 1-1/options$no.cuts/2,
                                by=1/options$no.cuts), 1)
  N <- ncol(X)
  
  if (ensembles){
    scale <- nrow(X)
  } else {
    scale <- 1
  }

  nUniques <- apply(X, 2, function(x){length(unique(x))})
  ind <- which(nUniques <= trunc(nrow(X)/10))
  if (length(ind)>0)
    X[,ind] <- jitter(X[,ind], options$jitter.default)
  
  if (!options$historical){
    bin.X <- apply(X, 2, myFindInt, options$prob.cuts)

    if (ensembles){
      Y <- bin.Y <- matrix(t(bin.X), ncol=N, nrow=nrow(X)*options$noSims, byrow=T)
    } else {      
      ind.Y  <- sample(1:nrow(X), options$noSims, replace=T)
      Y <- bin.Y <- bin.X[ind.Y,,drop=F]     # sampled bins
    }
      
    qX    <- apply(X, 2, myQuantiles, options$prob.cuts)
    upper <- lower <- Y
    for (c in 1:N){
      upper[,c] <- qX[bin.Y[,c]+1, c]
      lower[,c] <- qX[bin.Y[,c], c]
    }  
    eta <- matrix(runif(N*options$noSims*scale), ncol=N) # uniform samples for F(X)
    Y <- eta*(upper-lower) + lower
    # Extend the simulation range using the entropy argument  
    for (c in 1:N){               
      ind   <- which(X[,c] > qX[nrow(qX)-1,c])
      theta <- mean(X[ind,c]) - qX[nrow(qX)-1,c]  # parameter of the Upper Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == (length(options$prob.cuts)-1))
        Y[ind,c] <- qX[nrow(qX)-1,c] + (-theta*log(1-eta[ind,c]))
      }
      ind   <- which(X[,c] < qX[2,c])
      theta <- qX[2,c] - mean(X[ind,c])           # parameter of the Lower Exponential
      if (is.finite(theta)){
        ind <- which(bin.Y[,c] == 2)
        Y[ind,c] <- qX[2,c] - (-theta*log(1-eta[ind,c]))
      }
    }
  } else {  # bootstrap from historical values only
    ind.Y  <- sample(1:nrow(X), options$noSims, replace=T)
    Y <- X[ind.Y,]    
  }
  if (ensembles){
    Y <- array(Y, dim=c(scale, options$noSims, N))
    Y <- aperm(Y, c(1,3,2))
  }
  
  return(Y)
}
