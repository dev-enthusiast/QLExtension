# Given a x is a named vector(values) with bucket names.
# Create an hourly data.frame from it.
# Month is a begining of a month R date
#
#
#

###################################################################
# you can run it once with x = c(1,1,1) to get the bucket mask!
#
expandMonthlyBucketsHourly <- function(month, x, region="NEPOOL",
  agg=FALSE, bucketMask=NULL)
{
  startDt <- as.POSIXct(paste(format(month), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(seq(month, by="1 month",
    length.out=2)[2]), "00:00:00"))
  
  if (is.null(bucketMask)){
    for (i in seq_along(x)){
      aux <- secdb.getBucketMask(region, names(x)[i], startDt, endDt)
      if (i == 1){
        bucketMask <- aux
      } else {
        bucketMask <- cbind(bucketMask, aux[,2])
        names(bucketMask)[ncol(bucketMask)] <- names(x)[i]
      }
    }
  }
  
  res <- bucketMask
  for (bucket in names(x))
    res[,bucket] <- res[,bucket]*x[bucket]

  if (agg){
    res[,2] <- apply(res[,2:ncol(res)], 1, sum)
    res <- res[,1:2]
    names(res)[2] <- "value"
  }
  
  res
}


###################################################################
# return them in long format
.expandMonthlyBucketsDaily <- function(month, x, region="NEPOOL",
  agg=FALSE, bucketMask=NULL)
{
  # first expand to Hourly, and then collapse to daily
  hh <- expandMonthlyBucketsHourly(month, x, region=region,
    agg=agg, bucketMask=bucketMask)
  hh$time <- as.Date(format(hh$time-1, "%Y-%m-%d"))
  colnames(hh)[1] <- "date"
  
  dd <- subset(melt(unique(hh), id=1), value != 0)
  
  #cast(dd, time ~ variable, I, fill=0)
  dd
}


###################################################################
#
## expandMonthlyBuckets <- function(month, x, region="NEPOOL",
##   agg=FALSE, to=c("hourly", "daily"))
## {
##   switch(to,
##     hourly = .expandMOnthlyBucketsHourly(month, x,
##       region=region, agg=agg),
##     daily = .expandMOnthlyBucketsDaily(month, x,
##       region=region, agg=agg),
##     rLog("Unknown to argument", to)     
##   )
## }


##############################################################
.test <- function()
{
  require(SecDb)
  require(reshape)
  
  month  <- as.Date("2011-05-01")
  x <- structure(c(50,25,10), names=c("5X16", "2X16H", "7X8"))
  
  res <- expandMonthlyBucketsHourly(month, x, agg=FALSE, bucketMask=NULL)


  

}
