# Define a Path object
#
#

.get_name_from_ptid <- function(ptid, MAP)
{
  ind <- which(MAP$ptid == ptid)
  if (length(ind) == 0)
    stop("Cannot get a unique name for ptid ", ptid)

  MAP$name[ind[1]]
}

.get_ptid_from_name <- function(name, MAP)
{
  ind <- which(MAP$name == name)
  if (length(ind) == 0)
    stop("Cannot get a unique ptid for name ", name)

  MAP$ptid[ind[1]]
} 

#############################################################
# Construct an FTR/CRR/TCC
# @param source.ptid, sink.ptid integers
# @param mw
# @param auction, a CRR auction object
# @param bucket a String the bucket.  For Nepool: "ONPEAK" or "OFFPEAK"
# @param contract, either "OBLIGATION" or "OPTION"
# @param ..., other arguments for path classification etc.                      
Path <- function(source.ptid,
                 sink.ptid,
                 bucket, 
                 auction,
                 mw=1,
                 contract="OBLIGATION", ...)
{
  args <- list(...)
  
  if (missing(bucket) && auction$region=="NYPP")
    bucket <- "FLAT"

  if (bucket == "ONPEAK")
    bucket <- "PEAK"
  
  bucket <- toupper(bucket)
  contract <- toupper(contract)

  if ( !inherits(auction, "CRR_Auction") ) 
    stop("Argument auction needs to be a CRR_Auction.")
  
  if ((auction$region == "NEPOOL") && !(bucket %in% c("OFFPEAK", "PEAK")))
    stop("Wrong bucket ", bucket, " for NEPOOL!")
  
  if ((auction$region == "NYPP") && bucket != "FLAT")
    stop("Wrong bucket ", bucket, " for NYPP!")

  MAP <- switch(auction$region,
    "NEPOOL"  =  get("NEPOOL")$MAP,  
    "PJM"     =  get("PJM")$MAP,
    "NYPP"    =  get("NYPP")$MAP,
    "MISO"    =  get("MISO")$MAP,
    "SPP"     =  get("SPP")$MAP,            
    stop("Unimplemented region ", auction$region))

  source.name <- .get_name_from_ptid(source.ptid, MAP)
  sink.name   <- .get_name_from_ptid(sink.ptid, MAP)

  structure(c(list(source.ptid = source.ptid,
                   sink.ptid   = sink.ptid,
                   source.name = source.name,
                   sink.name   = sink.name,
                   bucket      = toupper(bucket),
                   auction     = auction,
                   mw          = mw,
                   contract    = contract), args)
            , class="CRR_Path")

}




#############################################################
# have an as.data.frame method
as.data.frame.CRR_Path <- function(x, ...) {
  as.data.frame(as.list(unclass(x)))
}

  ## auctionName <- x$auction$auctionName
  ## x$auction <- NULL
  ## cbind(auctionName=auctionName, as.data.frame(as.list(unclass(x)))) 
  ## data.frame(auction     = x$auction$auctionName,
  ##            source.ptid = x$source.ptid,
  ##            sink.ptid   = x$sink.ptid,
  ##            source.name = x$source.name,
  ##            sink.name   = x$sink.name,
  ##            bucket      = x$bucket,
  ##            mw          = x$mw,
  ##            contract    = x$contract)


#############################################################
# TODO: 
reverse_path <- function(path)
{
  path
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

