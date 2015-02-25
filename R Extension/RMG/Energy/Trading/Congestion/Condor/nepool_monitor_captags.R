# Monitor the captags and report on jumps
#
# Written by Adrian Dragulescu on 08-Mar-2012


############################################################################
#
.find_events <- function(PC, THRESHOLD=5)
{
  aux <- ddply(PC, .(Asset.ID), function(x){
    x <- tail(x[order(x$Trading.Date),])
    x$change.in.MW <- c(NA,diff(x$Asset.Peak.Contribution))
    ind <- which(x$change.in.MW > THRESHOLD)
    if (length(ind) > 0) {
      x[ind,]
    } else {
      NULL
    }
  })
  
  return(aux)  
}

#########################################################################
#
.send_email <- function(res)
{
  subject <- "ATTENTION!  Day on day attrition of 5MW or more"
  out <- capture.output(res)
  to <- paste(c("kate.norman@constellation.com", 
    "necash@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, subject, out)
}

  
############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.captag.monitor.R")

returnCode <- 0   # 0 means success

rLog("Start at ", as.character(Sys.time()))

asOfDate  <- Sys.Date()
startDate <- asOfDate - 45

days <- seq(startDate, asOfDate, by="1 day")
PC   <- .read_peakcontribution_report(days, org="EXGN")

res  <- .find_events(PC, THRESHOLD=5)

if (nrow(res) > 0)
  .send_email(res)


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


