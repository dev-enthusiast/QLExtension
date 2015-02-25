# Estimate and report Stage2 vlr for CCG and CNE loads
#
#
#
#

#################################################################
#
.report_vlr2 <- function(vlr)
{
  vlr$day <- as.Date(format(vlr$time-1))
  res.day <- ddply(vlr, c("day", "company"), function(x){
    c(mwh.bid.da = round(sum(x$load.bid.da)),
      mwh.rt = round(sum(x$load.rt)),
      dart = round(mean(x$dart),2),
      vlr2 = round(sum(x$vlr)))
  })
  head(res.day)
  
  ## aux <- melt(res.day, id=c(1,2,5))
  ## res.day <- cast(aux, day + dart ~ company + variable, I, fill=NA)
  res.day <- res.day[order(-as.numeric(res.day$day)),]

  res.day
}


#################################################################
options(width=200)  # make some room for the output
options(stringsAsFactors=FALSE) 
require(methods)   
require(CEGbase)
require(SecDb)
require(zoo)
require(reshape)
require(lattice)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.demandbids.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.loads.compare.actuals.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.vlr.R")


rLog(paste("Start proces at ", Sys.time()))
returnCode <- 0    # succeed = 0

#startDt <- as.POSIXct(paste(currentMonth(), "01:00:00")
#startDt <- as.POSIXct("2014-01-01 01:00:00")
startDt <- as.POSIXct(paste(format(Sys.Date()-35), "01:00:00"))
endDt   <- Sys.time()
vlr <- calculate_stage2_vlr(startDt, endDt)

vlr.day <- .report_vlr2(vlr)
out <- c(capture.output(print(vlr.day, row.names=FALSE)),
         "\n\nProcmon Job: nepool_vlr2_estimate",
         "Contact: Adrian Dragulescu")
                      
to <- paste(c("adrian.dragulescu@constellation.com",
    "william.ewing@constellation.com", "ankur.salunkhe@constellation.com", 
    "kate.norman@constellation.com"), sep="", collapse=",")

sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
#  "NECash@constellation.com",
  to, 
  paste("VLR stage 2 estimate as of", as.character(Sys.Date())), out)

q( status = returnCode )
