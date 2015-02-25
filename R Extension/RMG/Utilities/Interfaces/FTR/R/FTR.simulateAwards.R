# probs is a matrix dim=(noBids, noPaths) with the probability of being
# awarded The element probs[i,j] is the probability of being
# awarded bid "i-1" for path "j".  probs[1,j] is the probability
# of not being awarded path "j". 
#
FTR.simulateAwards <- function(bids, probs, noSims=10)
{
  noPaths <- length(unique(bids$path.no))
  if (noPaths != ncol(probs))
    stop(paste("The number of cols of the probs matrix",
               "not equal to the number of paths. "))
  
  cProbs <- apply(probs, 2, cumsum)

  # generate random variables  
  rand <- matrix(runif(noPaths*noSims), ncol=noPaths)
  res  <- matrix(NA, nrow=noSims, ncol=noPaths)
  colnames(res) <- colnames(probs)

  # find if a path gets awarded or not in this simulation 
  for (i in seq_len(ncol(probs))){
    res[,i] <- findInterval(rand[,i], cProbs[,i])
  }
  res <- melt(res)
  res <- subset(res, value!=0)  # don't get awarded
  names(res) <- c("sim", "path.no", "bid.no")
  res$path.no <- as.numeric(as.character(res$path.no))

  if (!("cum.mw" %in% names(bids))){
     bids <- bids[order(bids$path.no, -bids$bid.price),]     
     bids$cum.mw <- as.numeric(sapply(split(bids$mw, bids$path.no),
                                   cumsum))
   }
  
  awds <- merge(bids[,c("path.no","bid.no","cum.mw","bid.price")],
                res)
  awds <- awds[order(awds$sim, awds$path.no), ]

  # just call it mw from now on
  names(awds)[which(names(awds)=="cum.mw")] <- "mw"
  
  return(awds)
}
