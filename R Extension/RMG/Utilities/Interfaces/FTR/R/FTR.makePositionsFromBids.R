`FTR.makePositionsFromBids` <-
function(bids, commods, ISO.env)
{
  rLog("Make positions from bids ...")
  lbids <- NULL                           # long bids
  # expand the contract months
  for (b in 1:nrow(bids)){
    contract.dt <- seq(bids$begin[r], bids$end[r], by="month")
    lbids <- rbind(lbids, expand.grid.df(data.frame(contract.dt), bids[b,]))
  }
  ind <- which(names(lbids) %in% c("begin", "end"))
  lbids <- lbids[,-ind]
  
  # stack the source/sink on top of each other
  lbids <- melt(lbids, id=c(1:3,6))  
  lbids$mw <- ifelse(lbids$variable=="source", -lbids$mw, lbids$mw)
  lbids$mw <- ifelse(lbids$buysell=="SELL", -lbids$mw, lbids$mw)   
  lbids <- lbids[,c("contract.dt", "class", "mw", "value")]
  names(lbids)[4] <- "name"

  # add the SecDB location name 
  lbids <- merge(lbids, ISO.env$MAP[,c("location", "name")], all.x=T)
  
  
  # break into these buckets, 2x16H is captured
  class.map <- data.frame(matrix(c("OFFPEAK", "OFFPEAK", "ONPEAK",
                                   "2X16H", "7X8", "5X16"), ncol=2))
  names(class.map) <- c("class", "bucket")
  
  lbids <- merge(lbids, class.map, all.x=T)
  ind   <- which(names(lbids) %in% c("class"))
  lbids <- lbids[,-ind]

  # add the hours
  lbids <- merge(lbids, ISO.env$HRS, all.x=T)

  # add bucket to commods
  commods$bucket <- sapply(strsplit(commods$curve.name, " "), function(x){x[3]})
  commods$bucket <- ifelse(commods$bucket=="2X16", "2X16H", commods$bucket)

  # add curve.name to lbids
  lbids <- merge(lbids, commods, all.x=T)
  lbids$position <- lbids$mw * lbids$hours
    
  QQ <- lbids[, c("curve.name", "contract.dt", "position")]
  QQ$vol.type <- "NA"

  res <- aggregate(QQ$position, list(contract.dt=QQ$contract.dt), sum)
  if (any(res$x) != 0){
    stop("Source and sink bids don't net out to zero!  Exiting")
  }
  
  rLog("Done.")
  return(QQ)  
}

