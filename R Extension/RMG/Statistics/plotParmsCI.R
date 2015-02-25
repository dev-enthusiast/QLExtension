# Plots contour plots of 95% confidence intervals for pairs of parameters,
# as predicted by the linear approximation around the solution.
#
# Reference:  Bates, D. "Nonlinear regression analysis and its application"
#
# last modified by Adrian Dragulescu on 18-Mar-2004


plotParmsCI <- function(nls.res, options){

  if (nargs()==4){ options <- list()}
  if (length(options$range)==0){options$range <- 0.10}   # 10 percent
  if (length(options$points)==0){options$points <- 30}   # 30 points
  if (length(options$together)==0){options$together <- 1}# plots together
  if (length(options$alpha)==0){options$alpha <- 0.05}   # 5 percent
  if (length(options$pairs)==0){options$pairs <- c(1,2)}
  if (length(options$layout)==0){options$layout <- c(1,2)}

  N <- nrow(options$pairs)   # number of plots  
  layout(matrix(1:N, options$layout[1], options$layout[2]))
  layout.show(N)
 
  for (n in 1:N){
    par(mar=c(3.5,3.5,1.5,1.5))  # distance between the plots    
    plot(ellipse(nls.res, level=0.99,
                 which=c(options$pairs[n,1],options$pairs[n,2])),
         type="l", las=1, mgp=c(1.5,0.5,0), pch=20, col="blue",
         xlab=bquote(theta[.(options$pairs[n,1])]),
         ylab=bquote(theta[.(options$pairs[n,2])]), )
    lines(ellipse(hyp.res,
          which=c(options$pairs[n,1],options$pairs[n,2])), col="blue")
    points(coef(hyp.res)[options$pairs[n,1]],
           coef(hyp.res)[options$pairs[n,2]], pch="+", cex=2)
#    abline(v=thetaSol[p], lty=2)
  }
  layout(1)  # revert to normal layout
  title("95% and 99% joint confidence regions")
}
