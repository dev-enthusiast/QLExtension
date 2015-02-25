# An improved version of intersect.series.  Do not use intersect.series
# anymore.
#
# Given two time series returns the common values
# Where date.x and date.y are R Dates
#
# Written by Adrian Dragulescu on 30-Dec-2004

intersect.ts <- function(dates.x, x, dates.y, y){

  commonDates <- intersect(dates.x, dates.y)
  ix <- which(dates.x %in% commonDates)
  x  <- x[ix]
  iy <- which(dates.y %in% commonDates)
  iy <- y[iy]

  return(list(data.frame(dates=commonDates, x, y), ix, iy))
}
