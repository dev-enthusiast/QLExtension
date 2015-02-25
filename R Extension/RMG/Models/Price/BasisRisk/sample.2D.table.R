# Where x and y are historical data points. 
#
#
# Last modified by Adrian Dragulescu on 6-May-2005


sample.2D.table <- function(x,y,options){

  myQuantiles <- function(x){quantile(x, probs=options$prob.cuts.x, na.rm=T)}
  if (length(options$sim.x)==0){
    res <- sample.from.1D(x, options)
    options$sim.x <- res$sim.x
  }
  if (length(options$no.cuts.x)==0){options$no.cuts.x <- 10}
  options$prob.cuts.x <- seq(0,1,1/options$no.cuts.x)

  x <- x + (10^-10)*runif(length(x))     # to make sure that quantiles are distinct
  bin.x   <- findInterval(x, myQuantiles(x), rightmost.closed=T)
  cut.sx  <- myQuantiles(options$sim.x)
  bin.sx  <- findInterval(options$sim.x, cut.sx, rightmost.closed=T)# bin simulated x
  eta.sy  <- runif(options$sample.size)  # simulate uniform y
  sim.y   <- array(NA, dim=options$sample.size)
  for (bin in 1:options$no.cuts.x){                 
    ind   <- which(bin.sx==bin)
    sy    <- sort(y[which(bin.x==bin)])  # y from the given x band cut
    ly    <- length(sy)
    cdf.y <- approx(seq(0, 1, by=1/length(sy)) , c(sy[1], sy), eta.sy[ind])
    sim.y[ind] <- cdf.y$y
  }
  if (save$Analysis$plots){
    N <- length(x)
    pdf(options$plot$filename, width=6.0, height=4.5)
    plot(x,y, col="blue", xlab=options$plot$label.x, ylab=options$plot$label.y,
       main=options$plot$main, cex.main=1)
    points(options$sim.x[1:N], sim.y[1:N], col="red", pch=2)
    legend("topright", legend=c("Hist","Sim"),
         col=c("blue", "red"), pch=c(1,2), text.col=c("blue","red"))
    dev.off()
  }
  return(data.frame(sim.x=options$sim.x, sim.y=sim.y))
}


