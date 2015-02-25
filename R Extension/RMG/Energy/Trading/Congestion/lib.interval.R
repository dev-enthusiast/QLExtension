# An interval class to manage time intervals
#
# format
# overlaps
# toHourly
# toMonthly
#


is.Interval <- function(x) {
  if (inherits("Interval")) TRUE else FALSE
}

############################################################
# Interval is of this type: [startDt, endDt)
# @param startDt is something that can be converted to POSIXct 
# @param endDt is something that can be converted to POSIXct

Interval <- function(startDt, endDt)
{
  if (class(startDt)[1] == "Date")
    startDt <- paste(format(startDt), "01:00:00")
  if (class(endDt)[1] == "Date")
    endDt <- paste(format(endDt+1), "00:00:00")
  
  startDt <- as.POSIXct(startDt)
  endDt   <- as.POSIXct(endDt)

  if (endDt < startDt)
    stop("Interval end needs to be after interval start!")
  
  structure(list(startDt = startDt,
                 endDt   = endDt),
            class="Interval")
}

print.Interval <- function(x, ...){
  print(paste(format(x$startDt), "->", format(x$endDt)))

  invisible()
}

format.Interval <- function(x, ...){
  paste(format(x$startDt), "->", format(x$endDt))
}


#################################################################
# 
overlaps <- function(x, ...) UseMethod("overlaps")

overlaps.Interval <- function(i1, i2) {
  if (((i1$endDt > i2$startDt) && (i1$startDt < i2$endDt)) ||
      ((i2$endDt > i1$startDt) && (i2$startDt < i1$endDt))) TRUE else FALSE
}

#################################################################
# Get all the hours in this interval
#
toHourly <- function(x, ...) UseMethod("toHourly")

toHourly.Interval <- function(i) {
  seq(i$startDt, i$endDt, by="1 hour")
}

#################################################################
# Get all the months in this interval
#
toMonthly <- function(x, ...) UseMethod("toMonthly")

toMonthly.Interval <- function(i) {
  seq(currentMonth(i$startDt), currentMonth(i$endDt), by="1 month")
}


#################################################################
#################################################################
# 
.test <- function()
{
  library(CEGbase)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.interval.R")

  i1 <- Interval("2014-01-01", "2014-01-02")
  i2 <- Interval("2014-01-01", "2014-01-02 10:00:00")
  i3 <- Interval("2014-01-02 04:00:00", "2014-01-03")
  
  overlaps(i2, i3) # TRUE

  overlaps(i1, i3) # FALSE

  toHourly(i1)

  toMonthly(Interval("2014-01-01", "2014-12-11"))


  

  library(reshape)
  library(zoo)
  
  x <- zoo(matrix(1:12, ncol=3),
           seq(as.Date("2014-01-01"), length.out=4, by="1 month"))
  colnames(x) <- c("x1", "x2", "x3")

  y <- zoo(matrix(c(100,200), ncol=2),
           as.Date("2014-05-01"))
  colnames(y) <- c("x1", "x3")

  rbind.fill(cbind(month=index(x), as.data.frame(x)),
             cbind(month=index(y), as.data.frame(y)))

  

  
  
}
