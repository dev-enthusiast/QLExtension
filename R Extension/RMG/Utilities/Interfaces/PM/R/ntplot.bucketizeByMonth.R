# A wrapper for the ntplot function in SecDb
#
#



ntplot.bucketizeByMonth <- function(symbol, bucket, startDate, endDate,
  region="NEPOOL", stat="Mean")
{
  df <- expand.grid(symbol, bucket)

  res <- vector("list", nrow(df))
  for (r in 1:nrow(df)){
    res[[r]] <- cbind(symbol=df[r,1], bucket=df[r,2],
      data.frame(secdb.invoke("_lib ntplot functions",
      "NTPLOT::BucketizeByMonth", symbol=as.character(df[r,1]),
      bucket=as.character(df[r,2]), region, "3600", stat)))
  }

  if (nrow(df)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  if (!missing(startDate))
    res <- subset(res, date >= startDate)

  if (!missing(endDate))
    res <- subset(res, date <= endDate)
  
  names(res)[3] <- "month"
  
  res
}


.test <- function()
{
  startDate  <- as.Date("2008-01-01")
  endDate    <- as.Date("2008-12-01")
  bucket     <- c("Offpeak", "Peak")
  symbol     <- c("nepool_smd_da_4000_lmp")

  # get the monthly price by bucket
  res <- ntplot.bucketizeByMonth(symbol, bucket, startDate, endDate,
    region="NEPOOL")

  # gas burn at My7
  res <- ntplot.bucketizeByMonth("BG_Mystic7_Gen_NG_MMBTu", "Flat", startDate, endDate,
    region="NEPOOL", stat="Sum")

  
}
