# General collection of moving average functions
# Where: x   is a data frame, with rownames as dates, and one column
#        lag is the time window for the moving average
#        fun is the function that needs to be applied
#            = "histVol" for historical volatilities
#            = "trend"   for historical trend
#            = "runs"    for historical runs
#
# Written by Adrian Dragulescu on 13-Oct-2004

ma.functions <- function(x, lag=30, fun="mean"){

  R   <- nrow(x)
  XX  <- matrix(0, nrow=R-lag+1, ncol=lag)
  aux <- 1:(R-lag+1) 
  for (L in 1:lag){
    ind    <- (L-1) + aux
    XX[,L] <- x[ind,1]
  }
  funApply <- fun
  if (fun=="histVol"){funApply <- function(z){
    ret <- diff(z)/z[-length(z)]
    sqrt(252)*sd(ret[is.finite(ret)], na.rm=TRUE)}}   # historical volatility
  if (fun=="trend"){funApply <- function(z){
    ret <- diff(z)/z[-length(z)]
    sqrt(252)*mean(ret, na.rm=TRUE)}}
  if (fun=="runs"){funApply <- function(z){
    signs <- sign(diff(z)/z[-length(z)])
    sum(signs, na.rm=TRUE)}}

  res <- as.data.frame(apply(XX, 1, funApply))
  rownames(res) <- rownames(x)[ind]
  colnames(res) <- paste(colnames(x),fun,sep=".")
#  plot(res[,1])
  return(res)
}
