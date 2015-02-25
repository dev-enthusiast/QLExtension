FTR.awardPaths <- function(auction, bids, ISO.env)
{
  aux <- unique(bids[,c("path.no", "class.type", "source.ptid",
    "sink.ptid")])
  aux$auction <- auction
  CP <- FTR.get.CP.for.paths(aux, ISO.env)
  if (nrow(CP)==0)
    stop(paste("Could not find this auction", auction,
               "in the clearing price database."))
  
  bids <- merge(bids, CP[,c("path.no", "CP")], all.x=TRUE)

  ind  <- ifelse(bids$CP <= bids$bid.price, TRUE, FALSE)
  awds <- bids[ind, ]
  
  awds <- do.call(rbind, (lapply(split(awds, awds$path.no),
    function(x){x$mw <- sum(x$mw); x <- x[nrow(x),]})))

  # clean some unnecessary columns
  ind <- which(colnames(awds) %in% c("Qmax", "cum.mw", "bid.no",
     "bid.price", "bid.price.month"))
  if (length(ind)>0) awds <- awds[,-ind]
  
  return(awds)
}


