# Some statistics on the historical performance
# RR - paths returns, as given by FTR.getPathReturns
#

FTR.histPortfolioPerformance <- function(paths, RR, region="NEPOOL",
                                         scale=1000)
{
  MAP <- eval(parse(text=paste(region, "$MAP", sep="")))
  
  if (!("source.name" %in% names(paths)))
    paths <- merge(paths, FTR.PathNamesToPTID(paths, MAP, to="name"))
  if (!("source.zone" %in% names(paths)))
    paths <- merge(paths, FTR.PathNamesToPTID(paths, MAP, to="zone"))

  cRR <- RR; cRR[is.na(cRR)] <- 0
  cRR <- cumsum(cRR)
#  matplot(cRR, type="l")
  
#  print(total.cRR <- rowSums(cRR))

  paths <- merge(paths, data.frame(path.no=as.numeric(colnames(cRR)),
                                   cRR=as.numeric(tail(cRR,1))))
  
  # plot them 
  .plot.cumulative.payoff(cRR)

  # best/worst performers
  aux <- .bestworst.paths(paths, cRR, scale=scale)
  paths.Neg <- aux[[1]]
  paths.Pos <- aux[[2]]
  
  return(list(cRR, paths.Neg, paths.Pos))
}

#################################################################
# 
.bestworst.paths <- function( paths, cRR, scale=1000 )
{
  now.cRR <- as.numeric(tail(cRR,1))
  names(now.cRR) <- colnames(cRR)
  plot(sort(now.cRR))

  # bad paths
  ind.Neg <- which(now.cRR < -scale)
  paths.Neg <- subset(paths, path.no %in% as.numeric(names(ind.Neg)))
  if (length(ind.Neg)>0){
    windows()
    plot(cRR[, names(ind.Neg)], col="blue",
      main=paste("Paths that lost more than $", scale, sep=""))
    cat("\n\n   Worst paths:\n")
    print(paths.Neg[order(paths.Neg$cRR),])
  }
 
  # good paths
  ind.Pos <- which(now.cRR > scale)
  paths.Pos <- subset(paths, path.no %in% as.numeric(names(ind.Pos)))
  if (length(ind.Pos)>0){
    windows()
    plot(cRR[, names(ind.Pos)], col="blue",
      main=paste("Paths that made more than $", scale, sep=""))
    cat("\n\n   Best paths:\n")
   print(paths.Pos[order(-paths.Pos$cRR),])
  }
  
  return(list(paths.Neg, paths.Pos))
}


#################################################################
# cRR - cumulative hourly return
.plot.cumulative.payoff <- function(cRR, ...)
{
   total.cRR <- rowSums(cRR)
   dates <- index(cRR)
   # show the weekdays/weekends with
   # polygon
   wdays <- as.POSIXlt(dates)$wday
   ind.we <- wdays %in% c(0,6)

   ind.start <- which(diff(ind.we)==1)
   ind.end <- which(diff(ind.we)==-1)
   if (ind.start[1] > ind.end[1]) ind.start <- c(1, ind.start)
   if (ind.end[length(ind.end)] < ind.start[length(ind.start)])
     ind.end <- c(ind.end, length(ind.we))  
   if (length(ind.start)!=length(ind.end))
     stop("Problems finding the weekends...")
   
   ry <- range(total.cRR/1000)
   plot(dates, total.cRR/1000, col="blue",
        ylab="Cumulative payoff, K$", type="n", ...)
   for (i in 1:length(ind.start))
     rect(dates[ind.start[i]], ry[1], dates[ind.end[i]], ry[2],
        col="gray", border=NA)
   lines(dates, total.cRR/1000, col="blue", type="o")
  
}
