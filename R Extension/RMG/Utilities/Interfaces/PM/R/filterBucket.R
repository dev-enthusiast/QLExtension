# filter a zoo object by bucket
# return a list of zoo's objects
#
#

filterBucket <- function(x, buckets=c("5X16", "2X16H", "7X8"),
  region="NEPOOL")
{
  res <- vector("list", length=length(buckets))
  names(res) <- buckets
  
  startTime <- index(x)[1]
  endTime   <- tail(index(x),1)
    
  for (bucket in buckets){
    ind <- secdb.getHoursInBucket(region, bucket, startTime, endTime)
    if (nrow(ind) > 0)
      res[[bucket]] <- x[ind$time,,drop=FALSE]
  }

  res
}


.test_filterBucket <- function()
{
  require(CEGbase)
  require(zoo)
  require(reshape)
  require(SecDb)
  require(PM)

  # incorrect if you pull one day only ...
  day <- as.Date("2010-01-02")
  x <-  FTR:::FTR.load.tsdb.symbols(c("nepool_smd_da_4000_lmp"),
                                       as.POSIXct(format(day, "%Y-%m-%d 01:00:00")),
                                       as.POSIXct(format(day+1, "%Y-%m-%d 00:00:00")))
  y <- filterBucket(x)


  
  
}
