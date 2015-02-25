# Define a Bid object.  A Bid is a Path with a bid price.
#
#


#############################################################
# Construct an FTR/CRR/TCC bid
# @param source.ptid, sink.ptid integers
# @param mw
# @param auction, a CRR auction object
# @param bucket a String the bucket.  For Nepool: "ONPEAK" or "OFFPEAK"
# @param contract, either "OBLIGATION" or "OPTION"
# @param bid.price in $/MWh
#
Bid <- function(source.ptid,
                sink.ptid,
                bucket, 
                auction,
                bid.price,
                mw=1,
                contract="OBLIGATION")
{
  path <- Path(source.ptid, sink.ptid,
               bucket, auction,
               mw=mw, contract=contract)
  

  structure(list(source.ptid = path$source.ptid,
                 sink.ptid   = path$sink.ptid,
                 source.name = path$source.name,
                 sink.name   = path$sink.name,
                 bucket      = path$bucket,
                 auction     = path$auction,
                 mw          = path$mw,
                 bid.price   = bid.price, 
                 contract    = path$contract
            ), class=c("CRR_Bid", "CRR_Path"))

}




#############################################################
# have an as.data.frame method
as.data.frame.CRR_Bid <- function(x, ...) {
  data.frame(auction     = x$auction$auctionName,
             source.ptid = x$source.ptid,
             sink.ptid   = x$sink.ptid,
             source.name = x$source.name,
             sink.name   = x$sink.name,
             bucket      = x$bucket,
             mw          = x$mw,
             bid.price   = x$bid.price,
             contract    = x$contract)
 #as.data.frame(as.list(unclass(x))) 
}



#############################################################
#############################################################
#
.test_Path <- function()
{
  require(CEGbase)
  require(SecDb)
  require(FTR)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")

  source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/CRR_Auction.R")
  source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/Path.R")
  source("H:/user/R/RMG/Utilities/Interfaces/CRR/R/get_hourly_SP.R")
  
  FTR.load.ISO.env(region="NEPOOL")
  auction <- parse_auctionName("F13", region="NEPOOL")
  
  paths <- list(Path(source.ptid = 4000,
                  sink.ptid   = 4001,
                  bucket      = "PEAK",
                  auction     = auction),
             Path(source.ptid = 4000,
                  sink.ptid   = 4001,
                  bucket      = "OFFPEAK",
                  auction     = auction))
  do.call(rbind, lapply(paths, as.data.frame))


  FTR.load.ISO.env(region="NYPP")
  auction <- parse_auctionName("F13", region="NYPP")  
  p1 <- Path(source.ptid = 61752,
             sink.ptid   = 61758,
             bucket      = "FLAT",
             auction     = auction)
  as.data.frame(p1)
  
  
  FTR.load.ISO.env(region="PJM")
  p2 <- list(Path(source.name = "G385-1 TR1",
                  sink.name   = "55 HEGEW CIDG",
                  class.type  = "ONPEAK",
                  region      = "PJM"),
             Path(source.name = "458 GREE TR71",
                  sink.name   = "LAPORTE ROLLI",
                  class.type  = "ONPEAK",
                  region      = "PJM"))
  
  do.call(rbind, lapply(p2, as.data.frame))


  
}

