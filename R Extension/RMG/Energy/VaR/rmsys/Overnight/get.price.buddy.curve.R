# Implement a buddy curve algorithm for prices.
# The function is called from fill.prices.vols.overnight.R
#
# Written by Adrian Dragulescu on 14-Dec-2006

get.price.buddy.curve <- function(PP, den){

  indDates  <- grep("-", colnames(PP))
  ind.isna  <- which(is.na(PP), arr.ind=T)
  if (length(ind.isna)==0){return(NULL)}
  ind.incompletes <- unique(ind.isna[,"row"])
  iPP <- PP[ind.incompletes, ]             # incomplete prices
  curve.split   <- strsplit(iPP$curve.name, " ")
  mkt <- sapply(curve.split, function(x){x[2]})
  #-------------------------------------add denomination-----------
  aux <- merge(iPP[, c("curve.name","contract.dt")], den, all.x=T)
  aux$denominated[which(is.na(aux$denominated))] <- "USD"
  iPP$denominated <- paste("USD/", aux$denominated, sep="")
      
  bc <- iPP[, c("curve.name", "contract.dt")]
  bc$buddy.name <- "COMMOD NG EXCHANGE"
  bc$rule       <- "basis"
  for (r in 1:nrow(bc)){                   # fix one problem curve at a time!
    ind <- which(is.na(iPP[r,indDates]))
    if (any(diff(c(0,ind))>1)){
      cat(paste("Curve ", iPP[r,"curve.name"], " has holes for ",
      "contract ",iPP[r,"contract.dt"],"inside the historical range.",sep=""))
    }
    bPP <- subset(PP, (curve.name==bc$buddy.name[r] & contract.dt==iPP$contract.dt[r]))
    if (nrow(bPP)==0){  # if you cannot find bc, move the contract out one month
      next.mth <- seq(iPP$contract.dt[r], by="month", length.out=2)[2]
      bPP <- subset(PP, (curve.name==bc$buddy.name[r] & contract.dt==next.mth))
    }
    if (nrow(bPP)==0){  # if you cannot find bc, move the contract out two months
      next.mth <- seq(iPP$contract.dt[r], by="month", length.out=3)[3]
      bPP <- subset(PP, (curve.name==bc$buddy.name[r] & contract.dt==next.mth))
   }
   if (bc$rule[r] == "basis"){
     basis <- 0                      
     if (max(ind)<options$calc$no.hdays){ # we had a curve with no hist prices at all!
      basis <- iPP[r,indDates][max(ind)+1] - bPP[indDates[max(ind)+1]]}
      iPP[r,indDates[ind]] <- bPP[indDates[ind]] + as.numeric(basis)
    }
    iPP$comment[r] <- paste("Prc_", iPP[r,"curve.name"], "_",
      iPP[r,"contract.dt"], " = Prc_", bPP$curve.name, "_", bPP$contract.dt,
      " ", sprintf("%+f", basis), sep="")
  }
  
  return(iPP)  
}

