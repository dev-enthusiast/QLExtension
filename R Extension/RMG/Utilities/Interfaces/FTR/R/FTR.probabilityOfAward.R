# Estimate a probability of awards based on past clearing prices
#
#
#



############################################################
# Simple way to estimate probability of awards.
# CP should be sorted increasingly
#
.calcProbabilityOfAward.simple <- function(x, CP)
{
  ind <- max(which(CP < x), 0)
  return(ind/length(CP))
}

.calcProbabilityOfAward <- function(x, CP)
{ # not working yet ...
  dCP  <- density(CP)
  ind  <- max(which(dCP$x < x), 0)
  prob <- dCP$y[ind]
  return(sum(dCP[]))
}




FTR.probabilityOfAward <- function(bids, ISO.env)
{
  if (any(!(c("path.no", "class.type", "source.ptid","sink.ptid",
    "bid.no", "bid.price") %in% names(bids))))
    stop("Missing a required column from bids data.frame.")
  
  aux <- unique(bids[,c("path.no", "class.type", "source.ptid",
    "sink.ptid", "bid.no", "mw", "bid.price")])
  CPSP <- FTR.get.CP.for.paths(aux, ISO.env, add.SP=FALSE)
  CPSP <- CPSP[order(CPSP$path.no, CPSP$CP), ] # sort em for later
  CPSP <- split(CPSP, CPSP$path.no)

  pAwds <- ddply(bids, c("path.no", "bid.no"), function(x, CPSP) {
    data.frame(probAward=.calcProbabilityOfAward.simple(x$bid.price,
                 CPSP[[as.character(x$path.no)]]$CP))
  }, CPSP)

    
  return( pAwds )
}

