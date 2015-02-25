# Sample from a 1D distribution.
#
#
# Last modified by Adrian Dragulescu on 6-May-2005


sample.from.1D <- function(x, options){

  if (nargs()==1){options=NULL}  
  if (length(options$noSims)==0){options$sample.size <- 200}
  if (length(options$eta.x)==0){options$eta.x <- runif(options$sample.size)}
  
  N <- length(x)
  cdf.x <- seq(0, 1, by=1/(N-1))      # map [min, max] to [0, 1]
  sx    <- sort(x)
  sim.x <- approx(cdf.x, sx, xout=options$eta.x)$y  # simulated x
  
  return(data.frame(sim.x, eta=options$eta.x))
}

#   require(sm)
#   sm.x <- sm.density(x, display="none")
#   h    <- sm.x$eval.points[2]-sm.x$eval.points[1]
  
#   sm.x$cdf <- cumsum(sm.x$estimate)*h
#   sim.x <- approx(sm.x$cdf, sm.x$eval.points, xout=eta.x)$y  # simulated x
