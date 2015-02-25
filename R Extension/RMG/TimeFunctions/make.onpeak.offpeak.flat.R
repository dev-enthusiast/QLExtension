# Return the onpeak, offpeak, and flat from hourly series.
# Where data has columns [year month day hour symbols]
#
#  Written by Adrian Dragulescu on 15-Feb-2005

make.onpeak.offpeak.flat <- function(data){

  source("C:/R/Work/MyLibrary/TimeFunctions/bucket.ts.R")
  source("C:/R/Work/MyLibrary/TimeFunctions/merge.series.R")
  data.peak    <- bucket.ts(data, "onpeak")
  data.offpeak <- bucket.ts(data, "offpeak")
  data.flat    <- bucket.ts(data, "daily")

  days.flat <- as.Date(paste(data.flat$year, data.flat$month,
                             data.flat$day, sep="-"))
  days.peak <- as.Date(paste(data.peak$year, data.peak$month,
                             data.peak$day, sep="-"))

  res <- merge.series(days.flat, data.flat[,4],
                      days.peak, data.peak[,4])
  res <- data.frame(res, data.offpeak[,4])
  colnames(res)[2:4] <- c("flat", "onpeak", "offpeak")
  return(res)
}

