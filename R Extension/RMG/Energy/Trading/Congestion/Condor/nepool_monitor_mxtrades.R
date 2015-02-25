# Monitor MX trades and how their PnL
#
#


############################################################################
#
.trade_summary <- function( day )
{
  QQ <- .get_mx_trades( day )
  qq <- subset(QQ, trade.date == day)

  cast(qq, contract.dt ~ bucket, function(x)round(sum(x)), value="delta")
  
}

  
############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(SecDb)
require(reshape)

returnCode <- 0   # 0 means success

rLog("Start at ", as.character(Sys.time()))

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.mx.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

bal <- PM:::secdb.runBalReport( asOfDate, "MXNEACC" )

if ( !is.null(bal) && abs(round(bal$New.Trades)) > 0 ) {
  rLog("New trades with MX ")

  aux <- capture.output(.trade_summary( asOfDate))
  
  out <- c(paste("New trades PnL for ", format(asOfDate), "=",
    dollar(round(bal$New.Trades))), "\n\nPositions:\n", aux)

  to <- paste(c(
  #  "Vikram.Krishnaswamy@constellation.com",
    "steven.webster@constellation.com", 
    "heejin.ryu@constellation.com",
    "adrian.dragulescu@constellation.com"), sep="", collapse=",")
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "MX new trades PnL", out)
  
} else { 
  rLog("No new trades found!")
}



rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

