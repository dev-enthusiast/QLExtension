# Add a bucket column to a vector of POSIXct times
#
#
#

###############################################################
# @param x a vector of POSIXct
# @return a vector with the bucket info
#
addBucket <- function(x, buckets=c("5X16", "2X16H", "7X8"),
  region="NEPOOL")
{
  xx <- data.frame(ind=1:length(x), index=x)
  
  res <- vector("list", length=length(buckets))
  names(res) <- buckets
  startTime <- min(x)
  endTime   <- max(x)
    
  for (bucket in buckets){
    indB <- secdb.getHoursInBucket(region, bucket, startTime, endTime)
    res[[bucket]] <- cbind(bucket=bucket, subset(xx, index %in% indB$time))
  }
  res <- do.call(rbind, res)
  res <- res[order(res$ind),]
  res <- merge(res[,c("ind", "bucket")], xx, by="ind", all.y=TRUE)
  
  res$bucket
}


.test_addBucket <- function()
{
  require(CEGbase)
  require(zoo)
  require(reshape)
  require(SecDb)
  require(PM)

  x <- seq(as.POSIXct("2013-01-01 01:00:00"), by="1 hour", length.out=50)
  data.frame(index=x, bucket=addBucket(x))

  
  data.frame(index=x, bucket=addBucket(x, buckets="PEAK"))  # OFFPEAK will be NA!
  
  

  
  
  
  
}
