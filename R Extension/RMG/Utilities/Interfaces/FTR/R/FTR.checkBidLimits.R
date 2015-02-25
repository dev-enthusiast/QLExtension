# Where sSP is the simulated settle prices dim (noPaths, noSims)
#
FTR.checkBidLimits <- function( sSP, bid.limits )
{
  sSP.bs <- t(apply(sSP, 1, function(x){summary(na.omit(x))}))
  sSP.bs <- data.frame(path.no=as.numeric(rownames(sSP.bs)), sSP.bs)

  sSP.bs <- merge(sSP.bs, bid.limits[,c("path.no","max.bid","min.bid")])
  sSP.bs <- sSP.bs[order(sSP.bs$path.no), ]

  # big problem: max.bid > median
  ind <- which(sSP.bs$max.bid > sSP.bs$Median)
  if (length(ind)>0){
    rLog("These paths need lower bids...")
    print(sSP.bs[ind,])
    rLog("Moving the highest bid to Q(,3/8) ...")
    bid.limits$original.max.bid <- bid.limits$max.bid
    aux <- apply(sSP[ind,], 1, quantile, probs=3/8, na.rm=T)
    bid.limits[ind,"min.bid"] <- (bid.limits[ind,"min.bid"] 
      - bid.limits[ind,"max.bid"] + aux)
    bid.limits[ind,"max.bid"] <-  aux
    rLog("Done")  
  }

  bid.limits <- .adjust.Bids(sSP.bs, bid.limits)
  
  return(bid.limits)
}

############################################################## 
# If 25q is away from the median, adjust
#
.adjust.Bids <- function(sSP.bs, bid.limits)
{
  d1 <- sSP.bs$max.bid - sSP.bs$X1st.Qu.
  d2 <- sSP.bs$Median  - sSP.bs$max.bid
  ind <- which(d1 > d2)
  if (length(ind)>0){
    rLog(paste("Moving the max.bid down for", length(ind), "paths."))
    bid.limits$max.bid[ind] <- bid.limits$max.bid[ind] - d1[ind]/2
    bid.limits$min.bid[ind] <- bid.limits$min.bid[ind] - d1[ind]/2     
  }

  return(bid.limits)
}
