# Convert from cartesian to polar coordinates and back.   
#
# x.in <- c(0.95, 0.82, 0.3)
# source("H:/user/R/RMG/Utilities/Math/polar.coordinates.R")
# polar.coordinates(x.in, "to.polar")
#
# Last modified by Adrian Dragulescu on 6-Mar-2007

polar.coordinates <- function(x.in, direction){
  
  if (direction=="to.polar"){
    N <- length(x.in)
    if (!isTRUE(all.equal(sqrt(sum(x.in^2)), 1.0))){
      x.in <- x.in/sqrt(sum(x.in^2))}  # normalize it 
    x.out <- array(NA, dim=N-1)
    x.out[1] <- acos(x.in[1])
    for (d in 2:(N-1)){
      x.out[d] <- acos(x.in[d]/prod(sin(x.out[1:(d-1)])))}
  }
  if (direction=="to.cartesian"){
    N <- length(x.in) + 1   
    x.out <- array(NA, dim=N)  # there are N-1 polar coordinates
    x.out[1] <- cos(x.in[1])
    for (d in 2:(N-1)){
      x.out[d] <- cos(x.in[d])*prod(sin(x.in[1:(d-1)]))
    }
    x.out[N] <- prod(sin(x.in[1:(N-1)]))
  }
  return(x.out)  
}
