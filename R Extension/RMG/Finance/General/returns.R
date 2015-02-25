# Given a time-series, the function calculates the returns at
# specified time lags.  If x is a data.frame it considers the
# time-series agreggated along the columns.
#
# Written by Adrian Dragulescu on 23-Jun-2003

returns <- function(x,lag=lag,options){

  if (nargs()==2){options <- list()}
  if (length(options$method)==0){options$method="log"}

  N <- dim(x)[1]
  D <- dim(x)[2]
  res = NULL
  for (d in 1:D){
    xUp <- x[(lag+1):N,d]
    xDn <- x[1:(N-lag),d]
    if (options$method=="log"){      # the log return 
      r <- log(xUp/xDn)
    } else {                         # the arithmetic return
      r <- xUp/xDn - 1}
    res <- cbind(res,r)
  }
  if (is.data.frame(x)){colnames(res) <- colnames(x)}
  return(res)
}
