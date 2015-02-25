# TCC auction functionality for NYPP.
# Requires package SecDb, CRR.
#
# 
# is_allowed_ptid      -- check if you can use this ptid for a TCC 
# load_valid_ptids
#



#################################################################
# Check if the ptids are allowed to trade
# @param ptid a vector a ptids
# @return a vector of booleans
#
is_allowed_ptid <- function( ptid )
{
  ptid %in% NYPP$valid_ptids
}


#################################################################
# Get the valid (accepted) points where you can put TCC's
# @param fromDate an R Date.  By default return the last approved list.
#   If not NULL, return all the valid points before the fromDate.
# @return a vector of ptids
#
load_valid_ptids <- function( fromDate=NULL )
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/"
  filename <- paste(DIR, "valid_ptids.RData", sep="")
  load(filename)

  if (!is.null(fromDate)) {
    valid_ptids <- subset(valid_ptids, valid_ptids$fromDate <= fromDate)
  } else {
    lastDate <- max(valid_ptids$fromDate)
    valid_ptids <- subset(valid_ptids, fromDate == lastDate)
  }
  
  NYPP$valid_ptids <- sort(unique(valid_ptids$ptid))
  rLog("Loaded NYPP$valid_ptids with nodes from",
       format(valid_ptids$fromDate[1]))
  
  invisible()
}



#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(FTR)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.TCC.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")

  startDt <- as.Date("2012-11-01")
  endDt   <- as.Date("2012-11-30")
  TCC_Auction(startDt, endDt, round=0)


  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")
  parse_auctionName( c("K13-1Y", "K13-2Y", "K13-1Y-R2") )
  
  parse_auctionName( c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
                       "K13-6M", "M13-1Y-R2") )

  count_hours_auction(c("X10", "K14-6M", "K14-2Y-R2"))


  
  
  
}
