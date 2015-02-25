# A wrapper for the ntplot function in SecDb
#
#

ntplot.bucketizeByDay <- function(symbol, bucket, startDate, endDate,
  region="NEPOOL", SamplingPeriod=3600, stat="Mean")
{
  df <- expand.grid(symbol, bucket)

  res <- vector("list", nrow(df))
  for (r in 1:nrow(df)){
    res[[r]] <- cbind(symbol=df[r,1], bucket=df[r,2],
      data.frame(secdb.invoke("_lib ntplot functions",
      "NTPLOT::BucketizeByDay", symbol=as.character(df[r,1]),
      bucket=as.character(df[r,2]), region, as.character(SamplingPeriod), stat, 1)))
  }

  if (nrow(df)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  if (!missing(startDate)) {
    res <- subset(res, date >= startDate)
  }
    
  if (!missing(endDate))
    res <- subset(res, date <= endDate)
  
  names(res)[3] <- "day"
  
  res
}

## secdb.evaluateSlang('Link("_lib NTPLOT Functions");')
##     slangCode <- paste('@NTPLOT::BucketizeByMonth("', df[r,1],
##                        '", "', df[r,2], '");', sep="") 
##     res[[r]]  <- cbind(symbol=df[r,1], bucket=df[r,2],
##                        data.frame(secdb.evaluateSlang(slangCode)))
