# Get elec prices for several SecDb locations.
#
#

secdb.getElecPrices <- function( asOfDate, startDate, endDate,
  commodity, location, bucket, serviceType="Energy", useFutDates=FALSE )
{
  df <- expand.grid( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType)
  names(df) <- c("asOfDate", "startDate", "endDate", "commodity",
                 "location", "bucket", "serviceType")
  
  SecDBLib  <- "_Lib Matlab Functions"
  SecDBFun  <- "MATLAB::GetFutCurve"

  res <- vector("list", nrow(df))
  for (i in 1:nrow(df)){
    res[[i]] <- data.frame(i=i, secdb.invoke(SecDBLib, SecDBFun, df$asOfDate[i],
      df$startDate[i], df$endDate[i], as.character(df$commodity[i]),
      as.character(df$location[i]), as.character(df$bucket[i]),
      as.character(df$serviceType[i]), useFutDates))
  }

  if (nrow(df)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  res <- merge(data.frame(i=1:nrow(df), df[,c("asOfDate","location", "bucket",
    "serviceType")]), res)
  res$i <- NULL
  res$bucket       <- as.character(res$bucket)
  res$serviceType  <- as.character(res$serviceType)  # removed the service column!
  res$location     <- as.character(res$location)
  
  names(res)[which(names(res)=="date")] <- "month"
  
  return(res)
}
