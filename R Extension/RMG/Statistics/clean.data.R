# Simple algorithm to catch spikes in data.  The spike is replaced by the
# average of the nearest neighbors.
# Where:   options$replace.NA=1 replaces with NA
# Returns: x     - the cleaned data series
#          messg - the index of replaced points
#
# Written by Adrian Dragulescu on 11-Jan-2005

clean.data <- function(x, options){
  test.spike <- function(ind, messg){
    dx.left  <- abs(as.numeric(x[ind]-x[ind-1]))
    dx.right <- abs(as.numeric(x[ind+1]-x[ind]))
    if ((dx.left > 5*std)||(dx.right > 5*std)){   #  a spike 
      x[ind] <- (x[ind+1]+x[ind-1])/2
      clean  <- 0
      messg  <- c(messg, ind)
    }
    return(list(x, clean, messg))
  }
  
  if (nargs()==1){options=NULL; options$replace.NA <- 0}
  clean <- 0; messg=NULL
  while (clean==0){
    clean   <- 1
    std     <- sd(diff(x))
    ind.min <- as.numeric(which(x==min(x, na.rm=TRUE)))
    aux <- test.spike(ind.min, messg)
    x <- aux[[1]]; clean <- aux[[2]]; messg <- aux[[3]]
   
    std     <- sd(diff(x))
    ind.max <- as.numeric(which(x==max(x, na.rm=TRUE)))
    aux <- test.spike(ind.max, messg)
    x <- aux[[1]]; clean <- aux[[2]]; messg <- aux[[3]]   
  }
  if (options$replace.NA==1){x[messg] <- NA}
  return(list(cleaned.data=x, ind.replaced=messg))
}
