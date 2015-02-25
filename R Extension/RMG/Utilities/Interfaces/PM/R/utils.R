#
#  .expandSpreadRule
#  .expDecay - used if you need to assymptote to a fixed value
#              in the future
#  .makeSecDbDelivPt.MAP
#  .propagateFilter  - used in forecastLossFactors
#  
#



#################################################################
# X is a data.frame as read from PM_node_mark.xls sheet="HARDCODED"
# Just expand the spread that you will add to all the months necessary. 
#
.expandSpreadRule <- function(X, asOfDate)
{
  # expand empty buckets;  older code using xlsReadWrite had "" instead of NA's!
  ind <- which(is.na(X$bucket) | X$bucket=="")
  if (length(ind)>0) {
    aux <- X[ind,]
    aux$bucket <- NULL
    aux <- expand.grid.df(data.frame(bucket=c("5X16", "2X16H", "7X8")), aux)
    X <- rbind(X[-ind,], aux)
  }
  
  X <- cbind(row=1:nrow(X), X)
  
  # expand start.dt and end.dt's
  startDate     <- as.Date(secdb.dateRuleApply(asOfDate, "+1e+1d"))
  expandedMths <- lapply(split(X[,c("start.dt", "end.dt")], X$row),
    function(x){seq(x$start.dt, x$end.dt, by="1 month")})
  expandedMths <- melt(expandedMths)
  names(expandedMths) <- c("month", "row")
  X <- merge(X, expandedMths, all=TRUE)

  X <- subset(X, month >= startDate)
  
  return(X[,c("delivPt", "parentDelivPt", "bucket", "month", "value")]) 
}

#################################################################
# Take a stationary time series and multiply it by an exponentially
# decaying factor so you can "shift" the mean of the time series from xBar to
# a new xInf.  Useful when you want to implement regime switching.
#  
# y = x + (xInf - xBar)(1-\exp(-\lambda(t-t0)))
#
# CAUTION: lambda needs to be given in inverse units of t0!
# 
.expDecay <- function(X, t0, lambda, xInf, 
   tVariable="month", value="value")
{
  x <- X[,value]
  t <- X[,tVariable]
  ind <- which(t > t0)    # X may contain historical data too!
  if (length(ind)==0)
    stop("You have no forward data to deform!")
  xBar <- mean(x)
  dt   <- as.numeric(t-t0)
  y <- x + (xInf - xBar)*(1 - exp(-lambda*dt))
  
  X[ind,value] <- y   # the shifted curve!

  return(X)
}


#################################################################
# propagate forward a filter.  x is a vector.
# coeff is a vector of coefficients
# 
.propagateFilter <- function(x, coeff, periodsAhead=12)
{
  if (length(x)<length(coeff)){
    stop("Length of time series too short.  Cannot extrapolate.")
  }

  N  <- length(x)
  Nc <- length(coeff)
  y <- c(x, rep(NA, periodsAhead))
  for (p in 1:periodsAhead){
    y[N+p] <- sum(coeff*y[(N+p-Nc):(N+p-1)])
  }
   
  return(y[(N+1):length(y)])
}


#################################################################
# Join smoothly the pmView with recent history.  Sometimes, some
# nodes show a gap because of volatile history.
# histPeriods = how many months to go back 
#
.smoothJoinPMView <- function(pmView, ptid, histPeriods=3,
   lambda=1/365, buckets=c("2X16H", "7X8", "5X16"))
{
  if (length(lambda) < length(buckets))
    lambda <- rep(lambda, length.out=length(buckets))
  
  # get historical settle prices for congestion relative to Hub
  endDate   <- as.Date(format(Sys.Date(), "%Y-%m-01"))-1  # end of current month
  startDate <- seq(endDate+1, by="-12 months", length.out=2)[2] # last year
  symbol   <- paste("nepool_smd_da_", ptid,
    "_congcomp - nepool_smd_da_4000_congcomp", sep="")
  res <- ntplot.bucketizeByMonth(symbol, buckets, startDate, endDate)

  hSP <- data.frame(cast(res, month ~ bucket, I, fill=NA))
  hSP <- zoo(hSP[,-1], hSP[,1])
  #hSP <- hSP[, c("X2X16H", "X7X8", "X5X16")]
  hSP <- tail(hSP, histPeriods)

  for (b in 1:length(buckets)){
    ind <- which(pmView$component=="congestion" & pmView$bucket==buckets[b])
    CC  <- pmView[ind,]
    fwdMean  <- mean(CC$value[1:12])
    lasthSP  <- as.numeric(mean(hSP[,paste("X", buckets[b], sep="")]))
    CC$value <- CC$value - fwdMean + lasthSP  # rescale to last value
  
    bux <- PM:::.expDecay(CC, tail(index(hSP),1), lambda[b], fwdMean,
      tVariable="month", value="value")
    #plot(pmView[ind,"value"],type="o",ylim=c(-5,0));lines(bux$value,col="blue")
#browser()    
    pmView[ind,] <- bux
  }
  
  pmView
}
