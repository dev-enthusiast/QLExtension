# Monitor the LSCP costs by zone.  Run once a month, at the beginning of month
#
# Written by Adrian Dragulescu on 7-Mar-2011


#########################################################################
#
.send_email <- function(out, header, subject)
{
  if (!is.null(out)) {
    x <- tail(out, 13)
    x <- x[is.finite(x[,2]),]
    x[,2:ncol(x)] <- sapply(x[2:ncol(x)], round, 2)
    
    out <- capture.output(print(x, row.names=FALSE))
    #out <- c(paste(header, "\n", sep=""), out)
    #to <- paste(c("necash@constellation.com"), sep="", collapse=",")
    to <- "adrian.dragulescu@constellation.com"
    from <- "OVERNIGHT_PM_REPORTS@constellation.com"
    sendEmail(from, to, subject, out)
  } 
}

  
############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(SecDb)
require(zoo)

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.rmr.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Ancillaries/lib.ncpc.R") 
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.load.R")
  

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_monitor_lscp.R")
rLog("Start at ", as.character(Sys.time()))

HH  <- .pull_hist_rmr(long=FALSE)
out <- .report(HH)

subject <- "Latest LSCP costs ($/MWh)"
header  <- ""
.send_email(out, header, subject)


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


