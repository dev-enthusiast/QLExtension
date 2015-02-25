FTR.makePositionsForVaR <- function(filepath, ISO.env)
{
  if (!file.exists(filepath))
    stop("Cannot find position file.  Please check filepath.")
  
  bids <- .loadBids(filepath, ISO.env)
  
  # get the total bid quantity
  bids <- aggregate(as.numeric(bids$mw), as.list(bids[,1:6]), sum)
  colnames(bids)[7] <- "mw"  

  nodeNames <- sort(unique(c(bids$source, bids$sink)))
  commods   <- FTR.nodeToCommod(nodeNames, ISO.env)
  
  QQ <- .makePositionsFromBids(bids, commods, ISO.env)
  
  return(QQ)
}

.loadBids <- function(filepath, ISO.env)
{
  cat(paste("Reading bids file: ", filepath,"\n"))
  if (ISO.env$region=="NEPOOL"){
    aux <- read.csv(filepath, skip=1)
    aux <- aux[-nrow(aux), ]             # remove comment line

    colnames(aux) <- tolower(colnames(aux))
    aux <- aux[, c("begin", "end", "class", "buysell", "source", "sink", "mw")]
    aux$begin <- as.Date(aux$begin)
    aux$end   <- as.Date(aux$end)
  } 
  
  return(aux)   
}




########################################################################
`.makePositionsFromBids` <-
function(bids, commods, ISO.env)
{
  cat("Make positions from bids ...\n")
  lbids <- NULL                           # long bids
  # expand the contract months
  for (b in 1:nrow(bids)){
    contract.dt <- seq(bids$begin[b], bids$end[b], by="month")
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
  names(class.map) <- c("class", "class.type")
  
  lbids <- merge(lbids, class.map, all.x=T)
  ind   <- which(names(lbids) %in% c("class"))
  lbids <- lbids[,-ind]

  # add the hours
  lbids <- merge(lbids, ISO.env$HRS, all.x=T)
  names(lbids)[which(names(lbids) == "class.type")] <- "bucket"
  
  # add bucket to commods
  commods$bucket <- sapply(strsplit(commods$curve.name, " "), function(x){x[3]})
  commods$bucket <- ifelse(commods$bucket=="2X16", "2X16H", commods$bucket)

  # add curve.name to lbids
  lbids <- merge(lbids, commods[,c("location", "curve.name", "bucket")], all.x=T)
  lbids$position <- lbids$mw * lbids$hours
    
  QQ <- lbids[, c("curve.name", "contract.dt", "position")]
  QQ$vol.type <- "NA"

  res <- aggregate(QQ$position, list(contract.dt=QQ$contract.dt), sum)
  if (any(abs(res$x)) > 1){
    stop("Source and sink bids don't net out to zero!  Exiting")
  }

  # Aggregate QQ one more time!
  QQ <- aggregate(QQ[,"position"], as.list(QQ[,c("curve.name", "contract.dt",
                                              "vol.type")]), sum)
  colnames(QQ)[4] <- "position"
  
  cat("Done.\n")
  return(QQ)  
}

