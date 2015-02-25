#
#
# .get_bucket_index    
# get_hourly_SP        get hourly settle price for an auction
# tsdb_symbol_fun      get the congestion symbol by region, ptid
#


#############################################################
# 
.get_bucket_index <- function(startDt, endDt, region) {
  list(
    PEAK = which(secdb.getBucketMask( region, "Peak", startDt, endDt)[,2]==1),
    OFFPEAK = which(secdb.getBucketMask( region, "Offpeak", startDt, endDt)[,2]==1),
    FLAT = which(secdb.getBucketMask( region, "Flat", startDt, endDt)[,2]==1)
  )  
}



#############################################################
# Get hourly settle prices for a region only
# 
# @param paths is a list of Path objects, or a data.frame with a path.no
# @result is a list of zoo timeseries
#
get_settle_price_hourly <- function( paths )
{
  if (inherits(paths, "CRR_Path")) 
    paths <- list(paths) 
  
  region <- unique(sapply(paths, function(x){x$auction$region}))
  if (length(region) > 1)
    stop("Only paths from one region are allowed.")
  
  if (!(region %in% c("NEPOOL", "NYPP", "SPP"))) 
    stop("Region ", region, "not supported yet.")

  if (region == "NYPP") {
    multiplier <- -1
  } else {
    multiplier <- 1
  }

  for (i in seq_along(paths))
    paths[[i]]$path.no <- i           # in case there are multiple auctions 

  sp <- vector("list", length=length(paths))

  # calculate the SP by auction
  aux <- split(paths, sapply(paths, function(x){x$auction$auctionName}))
    
  for (i in seq_along(aux)) {
    #rLog("-- Working on auction", names(aux)[i])
    pathsA <- aux[[i]]
    startDt <- as.POSIXct(paste(pathsA[[1]]$auction$startDt, "01:00:00"))
    endDt   <- as.POSIXct(paste(min(pathsA[[1]]$auction$endDt+1, Sys.Date()+2),
                                "00:00:00"))

    if (startDt > as.POSIXct(paste(Sys.Date()+1, "01:00:00"))) {
      rLog("Start date is in the future!")
      return(rep(NA, length=length(pathsA)))
    }

    fun <- tsdb_symbol_fun(region)

    # get the prices for all the unique ptids
    ptids <- sort(unique(unlist(lapply(pathsA,
      function(x){c(x$source.ptid, x$sink.ptid,
                    x$source.ptid.buddy, x$sink.ptid.buddy)}))))
 
    hSP   <- FTR:::FTR.load.tsdb.symbols(fun(ptids), startDt, endDt)
    if (ncol(hSP) != length(ptids)) 
      stop("Not all ptids had symbols!  Check your ptids!")
    colnames(hSP) <- ptids

    # you may not have data in yet  AAD 10/09/2014
    endDt <- min(endDt, index(hSP)[length(index(hSP))])
    ind.bucket <- .get_bucket_index(startDt, endDt, region)
    
    for (p in seq_along(pathsA)) {
      #rLog("---- Working on path", p)
      path <- pathsA[[p]]
      ptid.source  <- path$source.ptid
      ptid.sink    <- path$sink.ptid
      bucket       <- path$bucket      

      source.price <- hSP[,as.character(ptid.source)]
      if ("source.ptid.buddy" %in% names(path)) {
        rLog("Using buddy ", path$source.ptid.buddy, " for ", ptid.source)
        ind <- index(hSP) >= path$source.ptid.buddy.startDt
        source.price[ind] <- hSP[ind,as.character(path$source.ptid.buddy)]
      }
      # get only the hours for this bucket, all the prices you have 
      source.price <- source.price[ind.bucket[[bucket]]]
      
      sink.price <- hSP[,as.character(ptid.sink)]
      if ("sink.ptid.buddy" %in% names(path)) {
        rLog("Using buddy ", path$sink.ptid.buddy, " for ", ptid.sink)
        ind <- index(hSP) >= path$sink.ptid.buddy.startDt
        sink.price[ind] <- hSP[ind,as.character(path$sink.ptid.buddy)]
      }
      # get only the hours for this bucket, all the prices you have
      sink.price <- sink.price[ind.bucket[[bucket]]]
                               
      sp[[path$path.no]] <- multiplier*(sink.price - source.price)
    }
  }
  
  
  sp
}


#############################################################
# Return the function that gives you the symbol
#
# @param region a String with region name, e.g.  NYPP, PJM, NEPOOL
# @param ptid is a vector of ptids
# @returns a function from ptids to strings
#
tsdb_symbol_fun <- function(region)
{
  switch(region,
    "NEPOOL" = function(ptid) {
      paste("nepool_smd_da_", ptid, "_congcomp", sep="")
    },
    "NYPP"   = function(ptid) {
      root <- ifelse(ptid %in% c(61752:61762, 61844:61847),
                 "nypp_dam_zone_mcc_", "nypp_dam_gen_mcc_")
      paste(root, ptid, sep="")
    },
    "SPP"    = function(ptid) {
      this.ptid <- ptid
      subset(SPP$MAP, ptid %in% this.ptid )$DAC.Symbol
    },     
    stop("Unsupported region ", region))
}


#############################################################
#############################################################
#
.test_get_hourly_SP <- function()
{

  .tsdb_symbol_fun("NEPOOL")( c(4000,4001) )
  
  .tsdb_symbol_fun("NYPP")( c(61752,61753) )

  

  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  FTR.load.ISO.env(region="NEPOOL")
  paths <- FTR:::.get_favorite_paths()
  
  auction <- parse_auctionName( "U13" )
  
  hsp <- get_settle_price_hourly(paths)


}





#############################################################
# 
## .get_bucket_index <- function(startDt, endDt, region) {
##   list(
##     PEAK = zoo(1,
##       secdb.getHoursInBucket( region, "Peak", startDt, endDt)$time),
##     OFFPEAK = zoo(1,
##       secdb.getHoursInBucket( region, "Offpeak", startDt, endDt)$time),
##     FLAT = zoo(1,
##       secdb.getHoursInBucket( region, "Flat", startDt, endDt)$time)
##   )  
## }
