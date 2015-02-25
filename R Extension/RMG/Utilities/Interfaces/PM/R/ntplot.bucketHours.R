# A wrapper for the ntplot function in SecDb
#
#


ntplot.bucketHours <- function(bucket, startDate, endDate, region="NEPOOL",
  period="Month", stat="Sum", includeZeros=TRUE)
{  
  res <- vector("list", length(bucket))
  for (r in seq_along(res)){
    res[[r]] <- cbind(bucket=bucket[r], data.frame(secdb.invoke(
      "_lib ntplot functions", "NTPLOT::BucketHours", bucket[r], region,
      period, stat, includeZeros)))
  }

  if (length(bucket)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  if (!missing(startDate))
    res <- subset(res, date >= startDate)

  if (!missing(endDate))
    res <- subset(res, date <= endDate)
  
  res
}
