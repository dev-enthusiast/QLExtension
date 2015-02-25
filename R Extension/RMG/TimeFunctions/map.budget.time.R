# Map a date to a budget time.  Months, Quarters, Years. 
#
# x <- seq(as.Date("2007-01-01"), as.Date("2010-10-01"), by="month")
# source("H:/user/R/RMG/TimeFunctions/map.budget.time.R")
# map.budget.time(x)
#
# Written by Michael Zhang on 01-11-2007

map.budget.time <- function(x, asOfDate){
  
  if (nargs()=1){asOfDate <- Sys.Date()}
  yy  <- format(x, "%Y")
  mm  <- as.numeric(format(x, "%m"))
  mmm <- format(x, "%b")
  qq  <- ifelse(mm <= 3, 1, ifelse(mm <=6, 2, ifelse(mm <=9, 3, 4)))
  
  cyy <- format(asOfDate, "%Y")
  cmm <- format(asOfDate, "%m")
  cqq <- if (cmm <= 3) 1 else if (cmm <= 6) 2 else if (cmm <= 9) 3 else 4

  budget.time <- ifelse(yy == cyy, ifelse(qq == cqq, 
    paste(mmm, yy, sep=""), paste('Q', qq, "-", yy, sep="")),
    paste('Year', yy, sep=""))
                       
  return (budget.time)
}
