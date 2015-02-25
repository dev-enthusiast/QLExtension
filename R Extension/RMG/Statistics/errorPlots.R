# Plots 4 standard error plots, as suggested by NIST.
# 
# Where:   x   is the error vector, size Nx1
#
# written by Adrian Dragulescu on 2-Feb-2004


errorPlots <- function(err,options){

  if (nargs()==1){ options <- list()}
  if (length(options$together)==0){options$together <- 1}# plots together

  layout(t(matrix(1:4,2,2)))
  layout.show(4)

  par(mar=c(2.5,2.5,1.5,0.5))  # distance between the plots    
  plot(err, type="l", main="Sequence plot")

  N <- length(err)
  plot(err[1:(N-1)], err[2:N], main="Lag plot")

  hist(err, main="Histogram")

  qqnorm(err, main="Normal quantile plot")
  layout(1)
}
