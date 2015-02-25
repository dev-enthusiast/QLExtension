# return the hourly settle price for the period you want
#   paths - is the usual data.frame
# 
# The difference beteen this version and FTR.get.hSP.for.paths is
# that this goes to tsdb and pulls the data hourly.
#

FTR.calc.SP.for.paths <- function( paths, start.dt, end.dt,
   buddy=.get_buddy_node() )
{
  # get the bucket indices
  ind.bucket <- FTR:::.get.Bucket.Mask("NEPOOL", start.dt, end.dt)

  ptids <- sort(unique(c(paths$source.ptid, paths$sink.ptid)))

  if (!is.null(buddy)) {
    ptidBuddy <- unique(na.omit(buddy[as.character(ptids)]))
    ptids <- sort(c(ptids, ptidBuddy))
  }
  
  # load the tsdb symbols
  symbs <- paste("nepool_smd_da_", ptids, "_congcomp", sep="")
  hP <- FTR.load.tsdb.symbols(symbs, start.dt, end.dt)
  if (ncol(hP) == length(ptids)) {
    colnames(hP) <- as.character(ptids)
  } else {
    stop("cannot find some ptid!")
  }
  
  res <- rep(NA, length=nrow(paths))
  for (i in seq_along(res)) {
    ind <- ind.bucket[,paths$class.type[i]]
    sourcePrice <- hP[,as.character(paths$source.ptid[i])]
    sinkPrice   <- hP[,as.character(paths$sink.ptid[i])]
    
    if (!is.null(buddy) && !is.na(buddy[as.character(paths$source.ptid[i])])) {
      buddyPtid <- buddy[as.character(paths$source.ptid[i])]
      indB <- length(takeWhile(sourcePrice, is.na))
      sourcePrice[1:indB] <- hP[1:indB, as.character(buddyPtid)]
    }
    
    if (!is.null(buddy) && !is.na(buddy[as.character(paths$sink.ptid[i])])) {
#      browser()
      buddyPtid <- buddy[as.character(paths$sink.ptid[i])]
      indB <- length(takeWhile(sinkPrice, is.na))
      sinkPrice[1:indB] <- hP[1:indB, as.character(buddyPtid)]
    }
 
    RR  <- sinkPrice - sourcePrice
    res[i] <- mean(RR[ind], na.rm=TRUE)
  }
  paths$SP <- res
  
  
  return(paths)
}



################################################################################
# Paths will have at least c("source.ptid", "sink.ptid", "class.type").
# Return historical prices by path, each column corresponds to a row in   
# the Paths data.frame.  Returns a list by bucket.
#
.FTR.calc.SP.for.pathsBatch <- function( Paths, hSP )
{
  buckets <- unique(Paths$class.type)

  res <- NULL
  for (bucket in buckets){
    start.dts <- format(as.Date(index(hSP[[bucket]])), "%Y-%m-01")
    aux <- aggregate(hSP[[bucket]], list(start.dts), mean, na.rm=T) 
    auction <- FTR.AuctionTerm(rownames(aux), rownames(aux))
    rownames(aux) <- auction
    
    aux <- melt(as.matrix(aux))
    names(aux) <- c("auction", "path.no", "SP")
    res <- rbind(res, aux)
  }
  return(res)
}
