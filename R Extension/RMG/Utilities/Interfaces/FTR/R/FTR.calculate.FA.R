# Calculate Financial Assurance for a given set of paths.
#
# .fa_calculate_proxy
# FTR.calculate.FA.NEPOOL
#

##############################################################
# Calculate proxy price
#   source.ptid, sink.pid
#   class.type 
#   histMCC     - is the return data from the DALMP report
#   auctionType - either monthly or annual
#
.fa_calculate_proxy <- function(source.ptid, sink.ptid,
  class.type, histMCC, auctionType=c("monthly", "annual"),
  MAP)
{
  threshold <- switch(auctionType,
    "monthly" = 0.75,
    "annual"  = 0.95,
     NA)
  noMonths <- round(36 * threshold)

  # select only the two nodes you want
  if (source.ptid == 40338) source.ptid <- 1216  # HARDCODED!
  
  rr    <-  subset(histMCC, ptid %in% c(source.ptid, sink.ptid))
  rr    <- rr[rr$class.type == class.type, ]
  aux   <- cast(rr, month ~ ptid, I, fill=NA, value="MCC")
  if (ncol(aux) != 3){
    rLog("I should have 2 columns but I don't!")
    browser()
  }
  noNAs <- apply(aux, 2, function(x)length(which(is.na(x))))

  # check if you have enough data
  noNAs12 <- noNAs[noNAs > 12]
  if (length(noNAs12) > 0) {
    rLog("  Have to go to zonal proxy!")
    #browser()
    if ( noNAs[as.character(source.ptid)] > 12 ) {
      source.zone.id <- subset(MAP, ptid==source.ptid)$zone.id
      if (source.zone.id == sink.ptid) {
        pv <- list(proxyPrice=0)
      } else {
        pv <- .fa_calculate_proxy(source.zone.id, sink.ptid, class.type,
               histMCC, auctionType, MAP) 
      }
    } else {
      browser()
      zone.id <- subset(MAP, name==sink.name)$zone.id
      sink.zone.name <- subset(MAP, ptid==zone.id)$name
      if (sink.zone.name == source.name) {
        pv <- list(proxyPrice=0)
      } else {
        pv <- .fa_calculate_proxy(source.ptid, sink.zone.ptid, class.type,
              histMCC, auctionType)
      }
    } 

    proxyPrice <- pv$proxyPrice
  } else {                         # you have enough data!
    r <- aux[,as.character(sink.ptid)] - aux[,as.character(source.ptid)]
    r <- sort(r, decreasing=TRUE, na.last=FALSE)
    
    proxyPrice <- r[noMonths]
  }

  data.frame(proxyPrice=proxyPrice)
}




##############################################################
# bids what you get from read_ftrbids_csv_file
# histMCC from .read_DALMP_report
#
# There are two components of FA.  Total FA is the sum of the two. 
# 1) Bid FA - from the bids
# 2) SRFA - settlement risk financial assurance
# 
#
FTR.calculate.FA.NEPOOL <- function(bids)
{
  auction <- bids$auction[1]
  auctionType <- if (grepl("-", auction)) "annual" else "monthly"
  rLog("Auction type is", auctionType)

  rLog("Get the DALMP file ... ")
  histMCC <- .read_DALMP_file()   # from lib.NEPOOL.FTR.R

  MAP <- NEPOOL$MAP
  uPaths <- unique(bids[,c("path.no", "source.name", "sink.name",
                            "class.type")])
  uPaths <- merge(uPaths,
    FTR.PathNamesToPTID(uPaths, MAP, from="name", to="ptid"))

  
  # calculate the proxy price
  proxyPrice <- ddply(uPaths, c("path.no", "source.ptid", "sink.ptid",
    "class.type"), function(x) {
      rLog("Working on path.no", x$path.no[1])
      .fa_calculate_proxy(x$source.ptid, x$sink.ptid, x$class.type,
         histMCC, auctionType, MAP)
    })
  bids <- merge(bids, proxyPrice[,c("path.no", "proxyPrice")])


  # add the hours for SRFA
  HRS <- count_hours_auction(auctions=bids$auction[1],
                             buckets=c("PEAK", "OFFPEAK")) 
  bids <- merge(bids, HRS)

  
  # calculate the FA
  bids <- ddply(bids, .(path.no), function(x) {
    x <- x[order(x$bid.price, decreasing=TRUE), ]
    x$cumulativeMW <- cumsum(x$mw)
    
    # Settlement Risk Financial Assurance
    x$SRFA  <- x$hours * x$cumulativeMW * x$proxyPrice

    # Bid Financial Assurance
    x$BidFA <- x$bid.price.month * x$cumulativeMW

    # Total FA
    x$totalFA <- pmax(0, x$SRFA + x$BidFA)
    
    x
  })

  FA <- ddply(bids, .(path.no), function(x){
    x[which.max(x$totalFA),]   # pick up the maximum for each path
  })
  
  FA
}

#################################################################
#
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(FTR)

  FTR.load.ISO.env("NEPOOL")

  filename <- "S:/All/Structured Risk/NEPOOL/FTRs/Monthly Auction Analyses/Long-term Auctions/2012 Annual Auction/uploadBids1.csv"

  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/ftrbids_csv_file.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.calculate.FA.R")

  filename <- "S:/All/Structured Risk/NEPOOL/FTRs/Monthly Auction Analyses/2012/2012-10/uploadBids1.csv"
  bids <- read_ftrbids_csv_file(filename)
  

  FA <- FTR.calculate.FA.NEPOOL(bids)
  sum(FA$totalFA)
  
  
  
}
