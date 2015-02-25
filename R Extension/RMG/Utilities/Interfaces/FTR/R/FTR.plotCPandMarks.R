# Make a plot to compare the clearing prices vs. Marks
# CP comes from FTR.get.CP.for.paths for ONE path only
# marks comes from PM:::.getSecDbMark, aggregate the offpeak
#

FTR.plotCPandMarks <- function(CP, marks,
  ylab="Congestion price, $", ...)
{
  
  if (!("start.dt" %in% names(CP)))
    CP <- cbind(CP, FTR.AuctionTerm(auction=CP$auction))

  if (!("contract.dt" %in% names(marks)))
    stop("Need a contract.dt variable for the fwd congestion.")

  
  months <- sort(unique(c(CP$start.dt, marks$contract.dt)))
  
  op <- par(no.readonly=TRUE)
  plot(as.Date(months), rep(NA, length(months)), ylim=range(c(CP$CP,
    marks$value)), xlim=range(months),
    ylab=ylab, cex.main=1, ...)
  
  # add the monthly auctions
  MM <- CP[-grep("-", CP$auction),]  # the monthly auctions
  points(MM$start.dt, MM$CP, col="blue")

  # add the Cal auctions
  AA <- CP[grep("-", CP$auction),]  # the annual auctions
  for (r in 1:nrow(AA)){
    lines(c(AA$start.dt[r], AA$end.dt[r]), rep(AA$CP[r],2), col="black")
  }

  # add the SecDb marks
  points(marks$contract.dt, marks$value, pch=19, col="red")  
  abline(h=0, col="gray", lty=3)
  
  # add a legend
  par(xpd=TRUE)  # Do not clip to the drawing area
  lambda <- .025
  legend(par("usr")[1], 
    (1 + lambda) * par("usr")[4] - lambda * par("usr")[3],
    legend=c("monthly auction", "annual auction", "SecDb mark"),
    xjust = 0, yjust = 0, text.col=c('blue', 'black', 'red'),
    pch=c(1,NA,19), lty=c(NA,1,NA), 
    col=c('blue', 'black', 'red'), cex=0.90)
  par(op)

  
}
