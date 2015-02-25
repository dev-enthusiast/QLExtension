# Implement a buddy curve algorithm for vols.
# The function is called from fill.prices.vols.overnight.R
#
# Written by Adrian Dragulescu on 14-Dec-2006

get.vol.buddy.curve <- function(VV, den){

  indDates  <- grep("-", colnames(VV))
  ind.isna  <- which(is.na(VV), arr.ind=T)
  if (length(ind.isna)==0){return(NULL)}
  ind.incompletes <- unique(ind.isna[,"row"])
  iVV <- VV[ind.incompletes, ]
  curve.split   <- strsplit(iVV$curve.name, " ")
  mkt <- sapply(curve.split, function(x){x[2]})
  #--------------------------------add USD/USD denomination for all vols-----
  iVV$denominated <- "USD/USD"
      
  bc <- iVV[, c("curve.name", "contract.dt")]
  bc$buddy.name <- "COMMOD NG EXCHANGE"
  bc$action     <- "ratio"
  for (r in 1:nrow(bc)){                   # fix one problem curve at a time!
    ind <- which(is.na(iVV[r,indDates]))
    if (any(diff(c(0,ind))>1)){
      cat(paste("Vol for ", iVV[r,"curve.name"], " type ", iVV[r,"vol.type"],
      " has holes for contract ", iVV[r,"contract.dt"],
      " inside the historical range.\n",sep=""))
    }
    bVV <- subset(VV, (curve.name==bc$buddy.name[r] & contract.dt==iVV$contract.dt[r]))
    if (nrow(bVV)==0){  # Exchange expired? Move the contract out one month
      next.mth <- seq(iVV$contract.dt[r], by="month", length.out=2)[2]
      bVV <- subset(VV, (curve.name==bc$buddy.name[r] & contract.dt==next.mth))
    }
    next.mth <- iVV$contract.dt[r]
    while (nrow(bVV)==0){# Very far out? Move closer by one year. It shouldn't fail.
      next.mth <- seq(next.mth, by="-1 year", length.out=2)[2]
      bVV <- subset(VV, (curve.name==bc$buddy.name[r] & contract.dt==next.mth))
    }
    if (bc$action[r] == "ratio"){
      ind.R <- pmin(ind+1,options$calc$no.hdays+1)
      ratio.R <- iVV[r,indDates][ind.R]/bVV[indDates[ind.R]]
      err <- try(ratio.R <- as.numeric(ratio.R[max(which(!is.na(ratio.R)))]), silent=T)
      if (class(err)=="try-error"){ratio.R <- 1}
      iVV[r,indDates[ind]] <- bVV[indDates[ind]]*ratio.R
    }
    iVV$comment[r] <- paste("Vol_", iVV[r,"vol.type"], "_", iVV[r,"curve.name"],
      "_", iVV[r,"contract.dt"], " = ", round(ratio.R,6), "*Vol_M_", bVV$curve.name,
      "_", bVV$contract.dt, sep="")
  }
  return(iVV)  
}
