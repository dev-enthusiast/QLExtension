# Deal with capacity prices in SecDb
#
# .get_reserve_margins
# .pull_marks_capacity
#
#


################################################################
# get the reserve margins from secdb
#
.get_reserve_margins <- function()
{
  val <- secdb.getValueType("Bal Close PWR NEPOOL Load",
    "Matrix Data Stored")

  RM <- data.frame(eford = unlist(val[[9]]), 
                  fcmld  = unlist(val[[12]]),
                  fcmadj = unlist(val[[13]]))
  
  # eford is used to reflect differences between summer/winter reserve margins
  RM$reserveMargin <- RM$fcmadj/RM$fcmld*(1-RM$eford)

  
  # add the month
  mth <- unlist(secdb.getValueType("Bal Close PWR NEPOOL Load",
    "Contracts"))
  RM <- cbind(month=format.dateMYY(mth, direction=1), RM)
  
  RM
}



################################################################
# git them from secdb
#
.pull_marks_capacity <- function(asOfDate, startDate,
  endDate=as.Date("2020-12-01"), service=c("UCAP"),
  location=c("ROP", "CT", "MAINE", "NEMA", "SEMA", "WMA", "RI"))
{
  rLog("Pull capacity NEPOOL ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- location
  bucket      <- c("FLAT")
  serviceType <- service
  CC <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  
  CC
}


################################################################
################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(plyr)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.R")
  
  asOfDate  <- Sys.Date()
  startDate <- as.Date("2018-06-01")
  endDate   <- as.Date("2021-12-01")

  ucap <- .pull_marks_capacity(asOfDate, startDate, endDate,
    service="UCAP",
    location=c("ROP", "CT", "MAINE", "NEMA", "SEMA", "RI"))

  cast(ucap, month ~ location, I, fill=NA, value="value")
  

  
  print(subset(ucap, month >= as.Date("2015-06-01")), row.names=FALSE)
  
         

         
  
  
}
