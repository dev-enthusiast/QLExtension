# Given two time series returns the common values
# Where date.x and date.y are R Dates
#
# Modified by Adrian Dragulescu on 18-Feb-2005

intersect.ts <- function(dates.x, x, dates.y, y){

  commonDates <- intersect(dates.x, dates.y)
  ix <- which(dates.x %in% commonDates)
  x  <- x[ix]
  iy <- which(dates.y %in% commonDates)
  y <- y[iy]

  return(list(data.frame(dates=commonDates, x, y), ix, iy))
}
