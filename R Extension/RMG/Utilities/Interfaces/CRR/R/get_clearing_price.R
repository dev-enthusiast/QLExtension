#


#############################################################
# Calculate the clearing price for a given auction for a list
# of paths for a given region.
# @params paths
# @result a numeric vector for each path
#
get_clearing_price <- function( paths )
{
  if (inherits(paths, "CRR_Path"))    # if it's only one path 
    paths <- list(paths)
  
  region <- unique(sapply(paths, function(x){x$auction$region}))
  if (length(region) > 1)
    stop("Only paths from one region are allowed.")

  auctions <- unique(sapply(paths, function(x){x$auction$auctionName}))
  
  CP <- .load_CP_archive(region, auctions)
  if ( nrow(CP) == 0 ) 
    rWarn("Cannot find clearing prices for the auction you requested in the archive.")
  
  
  aux <- do.call(rbind, lapply(paths, as.data.frame))
  aux <- cbind(path.no=1:nrow(aux), aux)
  aux$auction <- aux$auction.auctionName

  CP$source.ptid  <-  CP$sink.ptid  <- CP$ptid
  
  # add the clearing price for the source
  res1 <- merge(unique(aux[,c("auction", "bucket", "source.ptid")]),
    CP[,c("auction",  "source.ptid", "bucket", "clearing.price.per.hour")])
  colnames(res1)[which(colnames(res1)=="clearing.price.per.hour")] <- "source.CP"
  aux <- merge(aux, res1, all.x=TRUE)
                
  # add the clearing price for the sink
  res2 <- merge(unique(aux[,c("auction", "bucket", "sink.ptid")]),
    CP[,c("auction",  "sink.ptid", "bucket", "clearing.price.per.hour")], all.x=TRUE)
  colnames(res2)[which(colnames(res2)=="clearing.price.per.hour")] <- "sink.CP"
  aux <- merge(aux,res2)

  aux$CP <- aux$sink.CP - aux$source.CP
  aux <- aux[order(aux$path.no),]
  
  aux$CP
}


##############################################################
# Probably will need a better solution when the data is very
# large, e.g. PJM
# @param region
# @param auctions, a vector of auctionNames, e.g. c("F14", "G14").
#   If NULL, return them all.  Not OK for big data ... 
# @return a data.frame with data for these auctions
#
.load_CP_archive <- function(region, auctions=NULL)
{
  CP <- switch(region,
    "NEPOOL" = {
      if (exists("NEPOOL") && inherits(NEPOOL, "environment")
          && "CP" %in% ls(NEPOOL)) {
        NEPOOL$CP
      } else {
        load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/ftrcp.RData")
        CP
      } 
    },
    "NYPP" = {
      if (exists("NYPP") && inherits(NYPP, "environment")) {
        if ( "CP" %in% ls(NYPP)) {
          NYPP$CP
        } else {
          load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/ftrcp.RData")
          NYPP$CP <- CP
          CP
        }
      } else {
        load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/ftrcp.RData")
        CP
      } 
    },
    stop("Region ", region, " is not supported.")           
    )

  colnames(CP)[which(colnames(CP)=="class.type")] <- "bucket"   
  CP$bucket[CP$bucket == "ONPEAK"] <- "PEAK"

  if (!is.null(auctions))
    CP <- subset(CP, auction %in% auctions)

  CP
}
