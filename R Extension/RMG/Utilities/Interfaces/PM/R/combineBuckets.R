# Two functions to make and split prices for different buckets
#
#

###################################################################
# 
#
combineBuckets <- function(X, weight="none", buckets=c("2X16H", "7X8"),
  multiplier=rep(1, length(buckets)), name="OFFPEAK")
{
  if (any(!c("contract.dt", "bucket", "value") %in% names(X)))
    stop("Input X data.fram is missing one of c('contract.dt', 'bucket', 'value')")
  
  #X <- X[,c("contract.dt", "bucket", "value")]
  # don't restrict X to have only these columns.  Used by
  # PM:::getPositionsDeliveryPoint to aggregate offpeak by delivery point. 
  
  ind <- which(X$bucket %in% buckets)
  aux <- X[ind,]

  if (weight=="none"){
    aux <- cast(aux[,-which(names(aux)=="bucket")], ... ~ ., sum)
    names(aux)[ncol(aux)] <- "value"
    aux$bucket <- name
  }

  if (weight=="hour"){
    if (!("hours" %in% names(X))){
      rLog("Hours not specified in input data.frame X.")
      rLog("I'll pull them now using region NEPOOL.")
      HRS <- ntplot.bucketHours(buckets, region="NEPOOL",
         startDate=min(X$contract.dt), endDate=max(X$contract.dt))
      names(HRS)[2:3] <- c("contract.dt", "hours")
      aux <- merge(aux, HRS, by=c("contract.dt", "bucket"))
    }
    
    mult <- data.frame(bucket=buckets, multiplier=multiplier)
    aux  <- merge(aux, mult, by="bucket", all.x=TRUE) 
    aux$value <- aux$value * aux$hours * aux$multiplier

    indC <- which(names(aux) %in% c("bucket", "hours", "multiplier"))
    VH   <- cast(aux[,-indC], ... ~ ., sum)
    #VH   <- cast(aux[,-indC], contract.dt ~ ., sum)
    names(VH)[ncol(VH)] <- "VH"

    aux$value <- aux$hours * aux$multiplier
    indC <- which(names(aux) %in% c("bucket", "hours", "multiplier"))
    H    <- cast(aux[,-indC], ... ~ ., sum)
    names(H)[ncol(H)] <- "hours"
    
    aux <- merge(VH, H)
    aux$value <- aux$VH/aux$hours   # calculate the weighted value
    aux <- aux[,-which(names(aux) %in% c("VH"))]
    aux$bucket <- name
#browser()    
    if (!("hours" %in% names(X)))
      aux$hours <- NULL  
  }

  X <- rbind(X[-ind,], aux)

  return(X)
}

  
