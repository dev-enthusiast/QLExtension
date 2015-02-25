# Given an hourly time series, return the onpeak hours
# Where data is a data frame formatted: year, month, day, hour, x1, x2, x3, ... 
#       res  is a data frame formatted: year, month, day, hour, y1, y2, y3, ...
# hour goes from 0 to 23.  On peak is hours 7:22
#                          Offpeak is hours 0:6 & 23
# 
# Last modified by Adrian Dragulescu on 20-Feb-2004

select.onpeak <- function(data){

  require(chron)  # for is.weekend
  res=NULL        # the output results

  dates  <- paste(data$month, data$day, data$year, sep="/")
  ind.WD <- !is.weekend(dates)
  hourON <- (data$hour>6) & (data$hour<23)
  ind    <- which(hourON & ind.WD)
  
  return(cbind(data[ind,], ind.PEAK=ind))
} 
