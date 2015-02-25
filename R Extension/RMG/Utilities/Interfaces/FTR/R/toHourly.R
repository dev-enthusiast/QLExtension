# Aggregate a sub-hourly zoo object to hourly frequency. 
# HE = TRUE use hour ending convention, e.g. all prices between 00:00:01 
#   and 01:00:00 are aggregated to hour 01:00:00, etc. 
#    = FALSE use hour beginning convention, e.g. all prices between 00:00:00 
#   and 00:59:59 are aggregated to hour 00:00:00, etc. 
# @param X a zoo ts
# 
toHourly <- function( X, HE=TRUE, ...)
{
  if (HE){
    ind <- as.POSIXct(format(index(X)-1, "%Y-%m-%d %H:00:00")) + 3600
  } else {
    ind <- as.POSIXct(format(index(X), "%Y-%m-%d %H:00:00"))
  }
  Xh  <- aggregate(X, ind, mean, ...)

  return(Xh)
}

# WOULD BE NICE TO MAKE THIS GENERIC

## toHourly.zoo <- function( X, HE=TRUE, fun=function(z){mean(z)}, ... )
## {
##   if (HE){
##     ind <- as.POSIXct(format(index(X)-1, "%Y-%m-%d %H:00:00")) + 3600
##   } else {
##     ind <- as.POSIXct(format(index(X), "%Y-%m-%d %H:00:00"))
##   }
##   Xh  <- aggregate(X, ind, fun, ...)

##   Xh
## }


# Aggregate a sub-daily zoo object to daily frequency. 
# HE = TRUE use hour ending convention, e.g. all prices between day1 00:00:01 
#   and day 2 00:00:00 are aggregated to day1, etc. 
#    = FALSE use hour beginning convention, e.g. all prices between
#   day1 00:00:00 and day1 00:59:59 are aggregated to day1, etc. 
#
toDaily <- function( X, HE=TRUE, ...)
{
  if (HE){
    ind <- as.Date(format(index(X)-1, "%Y-%m-%d 00:00:00"))
  } else {
    ind <- as.Date(format(index(X), "%Y-%m-%d 00:00:00"))
  }
  Xh  <- aggregate(X, ind, mean, ...)

  return(Xh)
}

