# From a chron series, returns a data.frame with year month day
# where dateCH is a date in chron format or a Date
#
# Written by Adrian Dragulescu on 13-Feb-2004


ymd <- function(dateCH, rep=rep){

  if (nargs()==1){rep <- 1}
  if (attributes(dateCH[1])$class=="chron"){
    source("C:/R/Work/MyLibrary/TimeFunctions/chron2posix.R")
    datePOS <- chron2posix(dateCH)
  
    table=NULL
    year <- as.numeric(as.character(years(dateCH)))
    month <- as.numeric(strftime(datePOS, format="%m"))
    day <- as.numeric(days(dateCH))
  }
  if (attributes(dateCH[1])$class=="Date"){
    year  <- as.numeric(format(dateCH, "%Y"))
    month <- as.numeric(format(dateCH, "%m"))
    day   <- as.numeric(format(dateCH, "%d")) 
  }

  
  if (rep==24){
    no.Days <- length(dateCH)
    year  <- rep(year, each=rep)
    month <- rep(month, each=rep)
    day   <- rep(day, each=rep)
    hour  <- rep(0:23, no.Days)
    res   <- data.frame(year, month, day, hour)
  } else {
    res <- data.frame(year, month, day)
  }
  
  return(res)
}
