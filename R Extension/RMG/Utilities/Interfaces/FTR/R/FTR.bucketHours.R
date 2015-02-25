`FTR.bucketHours` <-
function(region="NEPOOL", start.dt=as.POSIXlt("2003-01-01 01:00:00"),
         end.dt=as.POSIXlt("2013-01-01 00:00:00"), buckets=NULL)
{
  if (is.null(buckets))
    buckets   <- c("flat", "peak", "offpeak", "2x16h", "7x8", "5x16")

  res <- NULL
  for (ind.b in seq_along(buckets)){
    aux <- secdb.getBucketMask(region, buckets[ind.b], start.dt, end.dt)
    aux <- aux[aux[,2], ]
    aux$month <- format(aux$time-1, "%Y-%m-01")
    aux <- data.frame(table(aux$month))
    names(aux) <- c("month", buckets[ind.b])
    rownames(aux) <- NULL
    if (ind.b==1){
      res <- aux
    } else {
      res <- merge(res, aux)
    }
  }

  res <- melt(res, id=1)
  names(res) <- c("contract.dt", "class.type", "hours")
  res$class.type  <- toupper(as.character(res$class.type))
  res$contract.dt <- as.Date(as.character(res$contract.dt))
  res$class.type  <- gsub("^PEAK", "ONPEAK", res$class.type)
  
  return(res)  
}

# Older function.  Not used now.  Maybe in the future...
#
## `.FTR.countHoursAuction` <-
## function( region="NEPOOL" )
## {
##   rLog("Count the hours by bucket and auction term ...")
##   CP <- data.frame(start.dt=seq(as.Date("2003-01-01"), by="month",
##                      length.out=120))
##   CP$end.dt <- CP$start.dt
##   CP <- rbind(CP, data.frame(start.dt=seq(as.Date("2003-01-01"), by="year",
##     length.out=10), end.dt=seq(as.Date("2003-12-01"), by="year",
##     length.out=10)))
##   CP$auction <- FTR.AuctionTerm(CP$start.dt, CP$end.dt)
  
##   long.CP <- NULL
##   for (r in 1:nrow(CP)){
##     aux <- data.frame(auction = CP[r,"auction"],
##                       month = seq(CP[r,"start.dt"], CP[r,"end.dt"], by="month"))
##     long.CP <- rbind(long.CP, aux)
##   }
  
##   buckets   <- c("flat", "peak", "offpeak")

##   start.dt <- as.POSIXlt("2003-01-01 00:00:00")
##   end.dt   <- as.POSIXlt("2012-12-31 23:00:00")
##   res <- NULL
##   for (ind.b in seq_along(buckets)){
##     aux <- secdb.getBucketMask(region, buckets[ind.b], start.dt, end.dt)
##     aux <- aux[aux[,2], ]
##     aux$month <- as.Date(format(aux$time, "%Y-%m-%01"))

##     aux <- data.frame(table(aux$month))
##     names(aux)[1] <- c("month")
##     aux$month  <- as.Date(aux$month)

##     aux <- merge(aux, long.CP)
##     aux <- aggregate(aux$Freq, list(aux$auction), sum)
##     names(aux) <- c("auction", buckets[ind.b])
    
##     rownames(aux) <- NULL
##     if (ind.b==1){
##       res <- aux
##     } else {
##       res <- merge(res, aux)
##     }
##   }

##   res <- melt(res, id=1)
##   names(res) <- c("auction", "class.type", "hours")
##   res$class.type <- toupper(as.character(res$class.type))
##   res$class.type <- gsub("^PEAK", "ONPEAK", res$class.type)
##   rLog("Done.")
  
##   return(res)
## }

