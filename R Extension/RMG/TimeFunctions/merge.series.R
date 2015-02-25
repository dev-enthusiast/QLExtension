# Given two time series merge the values on the union of dates
# Where date.x and date.y are R Dates
#
# Written by Adrian Dragulescu on 15-Feb-2005

merge.series <- function(dates.x, x, dates.y, y){

  commonDates <- union(dates.x, dates.y)
  res <- data.frame(dates=commonDates, NA, NA)

  ind        <- which(commonDates  %in% dates.x)
  res[ind,2] <- x
  ind        <- which(commonDates  %in% dates.y)
  res[ind,3] <- y

  return(res)
}
