FTR.makeBids <- function( paths, bid.limits, sSP, options,
  bids.quantile=NULL, qmax.map=NULL, expbase=10)
{
  if (!("hours" %in% names(paths)))
    paths$hours <- options$hours[paths$class.type]
  
  ind <- which(names(bid.limits) %in% c("Qmax","ratio.risk.reward"))
  if (length(ind)>0)
    bid.limits <-  bid.limits[,-ind]         # just in case    
  
  # simulated returns
  RR  <- sSP - matrix(bid.limits$max.bid, nrow=nrow(sSP),
                      ncol=ncol(sSP))
  rownames(RR) <- rownames(sSP)

  # basic stats
  RR.bs <- cbind(t(apply(RR, 1, function(x){summary(na.omit(x))})),
           t(apply(RR, 1, quantile, probs=c(0.05, 0.95))))
  RR.bs <- cbind(RR.bs, IQR=RR.bs[,"3rd Qu."]-RR.bs[,"1st Qu."])
  RR.bs <- cbind(RR.bs, sigma.lt.0=RR.bs[,"Median"] - RR.bs[,"1st Qu."])
  ind   <- which(RR.bs[, "sigma.lt.0"]==0)
  if (length(ind)>0)RR.bs[ind, "sigma.lt.0"] <- 10^-10
  RR.bs <- cbind(RR.bs, ratio= RR.bs[,"Median"]/RR.bs[,"IQR"])

  ind <- which(RR.bs[,"IQR"]==0 & RR.bs[,"Median"]==0)
  if (length(ind)>0) RR.bs[ind,"ratio"] <- 0.5  # doesn't really matter

  # assign the Qmax for all paths 
  QQ   <- .assignBidQuantity( RR.bs, qmax.map )

  # add the Qmax
  bid.limits <- merge(bid.limits, QQ)
  
  # make the bids given Qmax
  bids <- .makeBids( bid.limits, sSP, bids.quantile, expbase )

##   bids <- merge(paths[,c("path.no", "class.type", "source.ptid",
##     "sink.ptid", "source.name", "sink.name", "hours")], bids)
  bids <- merge(paths, bids)
  bids$bid.price.month <- bids$bid.price * bids$hours
  
  return(list(bids, bid.limits))
}
  

################################################################
# support a general matrix bids.quantile,
# although it will be rarely used. 
.makeBids <- function(bid.limits, sSP, bids.quantile=NULL, expbase=10)
{
  if (is.null(bids.quantile))
    bids.quantile <- c(0, 1/20, 1/12, 1/8, 1/6, 1/5, 1/4)
  if (is.vector(bids.quantile))
    bids.quantile <- matrix(bids.quantile,
      ncol=7, nrow=nrow(sSP), byrow=T)
  if (is.matrix(bids.quantile))
    bids.quantile <- as.list(as.data.frame(t(bids.quantile)))
  
  # calculate the price quantiles
  N <- length(bids.quantile[[1]])   # number of bids
  aux <- as.list(as.data.frame(t(sSP)))
  bid.prices <- mapply(quantile, aux, probs=bids.quantile)
  rownames(bid.prices) <- NULL   # dim = c(N, no.paths)
  
  EWEL <- .EWEL(sSP, bid.prices)  
  r <- expbase^(-EWEL[,,"EL"]/EWEL[,,"EW"])
  r <- sweep(r, 1, apply(r,1,sum), FUN="/")  # normalize

  QQ <- r * matrix(bid.limits$Qmax, nrow=nrow(r), ncol=ncol(r))
  rownames(QQ) <- bid.limits$path.no
  colnames(QQ) <- ncol(r):1
  QQ <- melt(QQ)
  names(QQ) <- c("path.no", "bid.no", "mw")
  QQ$path.no <- as.numeric(as.character(QQ$path.no))
  QQ$bid.no  <- as.numeric(as.character(QQ$bid.no))

  # Flat bids (no variation!) get special attention 
  ind <- which(bid.prices[1,] == bid.prices[N,])
  if (length(ind)>0){  
    rLog(paste("These paths have all bid prices identical:",
               paste(ind, collapse=", ")))
    rLog("I will take each bid down by -$0.02.")
    for (r in 1:(N-1))                           # <--- HARDCODED!
      bid.prices[r,ind] <- bid.prices[N,ind] - 0.02*(N-r)
    QQ$mw[which(QQ$path.no %in% ind)] <- 5       # <--- HARDCODED!
  }
  scaled.bid.prices <- bid.prices
##   # scale down the bid prices in case the max.bid < Q(1/4)
##   ind <- ((bid.prices[1,] == bid.limits$min.bid) &
##           (bid.prices[N,] == bid.limits$max.bid))
##   if (any(!ind)){ 
##     rLog(paste("Adjusting the min limit for", length(which(!ind)),
##                "paths."))
##     for (p in which(!ind))
##       scaled.bid.prices[,p] <- approx(c(bid.prices[1,p],
##                                         bid.prices[N,p]),
##            c(bid.limits$min.bid[p], bid.limits$max.bid[p]),
##            bid.prices[,p])$y
##   }
  colnames(scaled.bid.prices) <- bid.limits$path.no
  rownames(scaled.bid.prices) <- nrow(bid.prices):1
  SBP <- melt(scaled.bid.prices)
  names(SBP) <- c("bid.no", "path.no", "bid.price")
  SBP$path.no <- as.numeric(as.character(SBP$path.no))
  SBP$bid.no  <- as.numeric(as.character(SBP$bid.no))

  bids <- merge(QQ, SBP)  # put them together
  bids <- bids[order(bids$path.no, bids$bid.no), ]
  rownames(bids) <- NULL
  
  bids <- bids[,c("path.no", "bid.no", "bid.price", "mw")]
  return(bids)  
}


################################################################
# Where RR - simulated path returns, dim=(noPaths, noSims)
# use the ratio=median/(median-Q(1/4)) to assign Qmax
#
.assignBidQuantity <- function(RR.bs, qmax.map=NULL)
{
  if (is.null(qmax.map)){
    qmax.map <- data.frame(ratio=c(2,1,0.5,0,-1),
                   Qmax =c(200,100,50,25,5))
  }
  qmax.map <- qmax.map[order(qmax.map$ratio), ]
  
  if (any(RR.bs[,"ratio"] < -1))
    stop("There are paths that have risk ratio < -1.  Please check.")
  
  QQ <- qmax.map$Qmax[findInterval(RR.bs[,"ratio"], qmax.map$ratio)]
  QQ <- data.frame(path.no=as.numeric(rownames(RR.bs)), 
                   ratio.risk.reward=RR.bs[,"ratio"], Qmax=QQ) 
  rownames(QQ) <- NULL
  
  return(QQ)
}

################################################################
# Take the quantity down some more according to a ratio
# ratio is a data.frame with columnnames =c("path.no", "ratio")
#
.scaleBidQuantity <- function(bids, bid.limits, ratio)
{
  bids <- merge(bids, ratio)
  bids[,c("mw", "Qmax", "cum.mw")] <-
    bids[,c("mw", "Qmax", "cum.mw")] * bids$ratio
  bids$ratio <- NULL


  bid.limits <- merge(bid.limits, ratio)
  bid.limits$Qmax <- bid.limits$Qmax * bid.limits$ratio
  bid.limits$ratio <- NULL

  return(list(bids, bid.limits))  
}

################################################################
# Calculate expected win & expected loss relative to a bid
#
.EWEL <- function(sSP, bid.prices)
{
  rLog("Calculating the EL/EW ratio for each path and bid ...")
  sSP <- t(apply(sSP, 1, sort))

  EWEL <- array(NA, dim=c(dim(bid.prices)[2], dim(bid.prices)[1], 2),
                 dimnames=list(NULL, NULL, c("EW", "EL")))
  
  for (p in 1:nrow(sSP)){          # loop over paths
    for (b in 1:nrow(bid.prices)){ # loop over bids
      ind <- which(sSP[p,] <= bid.prices[b,p])
      EWEL[p,b,2] <- bid.prices[b,p] - mean(sSP[p,ind])    # EL
      EWEL[p,b,1] <- mean(sSP[p, -ind]) - bid.prices[b,p]  # EW
    }
  }
  rLog("Done.")
  # r <- EWEL[,,"EL"]/EWEL[,,"EW"];  matplot(t(r), type="l")

  return(EWEL)
}
