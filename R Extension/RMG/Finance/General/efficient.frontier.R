# Calculate the efficient frontier given a series of standard
# deviations, x and returns, y.
# Return two arrays: the x-frontier, xF and the y-frontier, yF. 
# Algorithm: go recursively, dividing the points by 2. 
#
# last modified by Adrian Dragulescu on 28-Apr-2004

efficient.frontier <- function(x,y){

#  x=rnorm(20); y=rnorm(20)
  ind <- order(x)
  x <- x[ind]; y <- y[ind];    # sort along x
  yF <- cummax(y)              # get the running maximum
  xF <- x
  sw <- 0
  while (sw==0){
    sw <- 1                    # exit when there are no more changes
    N <- length(xF)
    indOut <- NULL
    indL=1; indM=2; indR=3
    while (indR <= N){
      yMid <- yF[indL] + ((yF[indR]-yF[indL])/(xF[indR]-xF[indL]))*(xF[indM]-xF[indL])
      if (yF[indM] <= yMid){     # eliminate the middle point
        sw <- 0                
        indOut <- c(indOut, indM)
        indM <- indM+1
        indR <- indR+1
      } else { 
        indL <- indM           # validate the mid
        indM <- indL+1
        indR <- indM+1   
      }
    }                          # c(indL, indM, indR)
    if (length(indOut)!=0){
      xF <- xF[-indOut]
      yF <- yF[-indOut]
    }
  }
#   plot(x,y); points(xF,yF, type="b", col="blue")
  res=NULL
  res <- data.frame(xF=xF, yF=yF)
  return(res)
}
