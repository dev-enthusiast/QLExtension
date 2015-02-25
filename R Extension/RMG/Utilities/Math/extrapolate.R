# Do extrapolation.  Pass sorted x values 
# x <- c(1,2,3)
# y <- c(2,4,6)
# source("H:/user/R/RMG/Utilities/Math/extrapolate.R")
# extrapolate(x,y,1:5)
#
# Written by Adrian Dragulescu on 16-May-2006


extrapolate <- function(x,y,xout,method){
                               
  if (nargs()==3){method <- "linear"}
  if (length(x)<2){
    cat('You only have one element.  Cannot extrapolate!')
    return(y)
  }
  yout <- array(NA, dim=length(xout))
  ind.max.xout <- which(xout > max(x))
  if (length(ind.max.xout)>0){          # extrapolate to the right
    ind <- which.max(x)
    y1  <- y[ind-1]; y2 <- y[ind]
    x1  <- x[ind-1]; x2 <- x[ind]
    if (method=="linear"){
      slope <- (y2-y1)/(x2-x1)
      inter <- (y1*x2-x1*y2)/(x2-x1)
      yout[ind.max.xout] <- slope*xout[ind.max.xout] + inter
    }
  }
  ind.min.xout <- which(xout < min(x))  # extrapolate to the left
  if (length(ind.min.xout)>0){
    ind <- which.min(x)
    y1 <- y[ind]; y2 <- y[ind+1]
    x1 <- x[ind]; x2 <- x[ind+1]
    if (method=="linear"){
      slope <- (y2-y1)/(x2-x1)
      inter <- (y1*x2-x1*y2)/(x2-x1)
      yout[ind.min.xout] <- slope*xout[ind.min.xout] + inter
    }
  }
  ind.in <- (1:length(xout))
  if (length(c(ind.min.xout, ind.max.xout))>0){
    ind.in <- (1:length(xout))[-c(ind.min.xout, ind.max.xout)]
  }
  yout[ind.in] <- approx(x,y,xout[ind.in],method)$y
  
  return(yout)
}
