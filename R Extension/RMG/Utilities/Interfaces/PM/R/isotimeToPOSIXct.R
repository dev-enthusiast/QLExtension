# convert a ["Date", "Hour.Ending"] data.frame to a POSIXct datetime
# The extra hour in the Fall appears as HourEnding 2X
# In spring, HourEnding 2 is missing. 
#
# Tried different ways.  This implementation, works correctly
#

###################################################################
# X["Date", "HourEnding"], both columns are strings
#
isotimeToPOSIXct <- function(X, extraHourToken="02X")
{
  if (!all(c("Date", "Hour.Ending") %in% names(X)))
    stop("Need Date and Hour.Ending to convert to POSIXct datetime!")
  
  X$datetime <- as.POSIXct(paste(X$Date, " ", 
    X$Hour.Ending, ":00:00", sep=""), format="%Y-%m-%d %H:%M:%S")

  ind <- grep("24", X$Hour.Ending)
  if (length(ind)>0) {
    X$datetime[ind] <- as.POSIXct(paste(X$Date[ind], " 23:59:59", sep="")) + 1
  }
  
  ind <- grep(extraHourToken, X$Hour.Ending)
  if (length(ind)>0){
    X$datetime[ind] <- as.POSIXct(paste(X$Date[ind], " 01:00:00", sep="")) + 3600
  }
  
  #X$sec <- as.numeric(X$datetime)
  
  X  
}


###################################################################
#
.test <- function()
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.regulation.R")

  X <- .get_prelim_data(as.Date("2010-11-06"), as.Date("2010-11-07"))
  isotimeToPOSIXct(X)
  
  X <- .get_prelim_data(as.Date("2011-03-12"), as.Date("2011-03-13"))
  isotimeToPOSIXct(X)

  # spring forward 
  X <- data.frame(Date=rep(c("2011-03-13"), 3),
                  Hour.Ending=c("01", "03", "04"))
  isotimeToPOSIXct(X)

  # fall back
  X <- data.frame(Date=rep(c("2013-11-03"), 5),
                  Hour.Ending=c("01", "02", "02X", "03", "04"))
  y <- isotimeToPOSIXct(X)
  as.numeric(y$datetime)    # obs. are separated by 3600 
  
  
}

