# Options expire on the third Saturday of the month, except when the
# first of the month is a Saturday.  Then they expire on the 4th Saturday.
#
# Create the expiration.Calendar.Rdata file
#
# Written by Adrian Dragulescun on 10-Aug-2004

library(chron)
source("C:/R/Work/MyLibrary/TimeFunctions/ymd.R")
setwd("C:/R/Work/Finance/Trading")
today <- as.Date(Sys.Date())
startDate <- as.Date("2004-08-01")
endDate <- as.Date("2011-01-01")

bomDays <- seq(startDate, endDate, by="1 month")
monthsLabel <- format(bomDays,  "%b%y")

expiration.date=NULL
for (m in 1:length(bomDays)){
  startMonth <- bomDays[m]
  endMonth <- seq(startMonth, startMonth+35, by="1 month")[2]
  days.in.month <- seq(startMonth, endMonth, by="day")
  ch.days <- chron(format(days.in.month, "%m/%d/%y"))
  y.m.d. <- ymd(ch.days)   
  dow <- day.of.week(y.m.d.[,2], y.m.d.[,3], y.m.d.[,1])
  ind.Sat <- which(dow==6)
  if (ind.Sat[1]==1){
    expiration.date[m] <- as.character(days.in.month[ind.Sat[4]])
  } else {
    expiration.date[m] <- as.character(days.in.month[ind.Sat[3]])
  }
}
expiration.date <- as.data.frame(expiration.date)
rownames(expiration.date) <- monthsLabel
colnames(expiration.date) <- "Expiration"
expiration.date

save(expiration.date, file="expiration.date.Rdata")



