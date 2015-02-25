# basic plotting stuff for an FTR path
# Where:
#   hist = number of days you want to inspect
#   wdim = plot window dimensions in inches
#

TCC.plot <- function(ptid1, ptid2, MAP, hist=365, wdim=c(12,9))
{
  if (!("package:SecDb" %in% search()))
    require(SecDb)

  DA1c <- MAP[MAP$ptid == ptid1, "tsdb.symbol"]
  if (is.na(DA1c))
    stop("Cannot find tsdb sybmol for ptid1.")
  DA2c <- MAP[MAP$ptid == ptid2, "tsdb.symbol"]
  if (is.na(DA2c))
    stop("Cannot find tsdb sybmol for ptid2.")

  DA1 <- gsub("_MCC_", "_LBM_", DA1c) 
  DA2 <- gsub("_MCC_", "_LBM_", DA2c)   
  RT1 <- gsub("^NYPP_DAM_", "NYPP_RT_", DA1) 
  RT2 <- gsub("^NYPP_DAM_", "NYPP_RT_", DA2)
 # RT1 <- paste("HourlyAvg(", RT1 ,")", sep="")  doesn't work!
 # RT2 <- paste("HourlyAvg(", RT2 ,")", sep="")
    
  start.dt <- Sys.time() - 24*3600*hist
  end.dt   <- Sys.time() + 24*3600
  symbols  <- c(DA1, DA2, DA1c, DA2c)
  MM  <- FTR:::FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
  dMM <- aggregate(MM, as.Date(format(index(MM), "%Y-%m-%d")), mean,
                   na.rm=TRUE)

  symbols  <- c(RT1, RT2)
  aux <- FTR:::FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
  aux <- aggregate(aux, as.Date(format(index(aux), "%Y-%m-%d")), mean,
                   na.rm=TRUE)
  dMM <- cbind(dMM, aux)
  
  # begining of the month vertical bars
  h0 <- which(as.numeric(format(index(dMM), "%d"))==1)

  # do the plots
  windows(wdim[1], wdim[2])
  layout(matrix(1:3, 3, 1))
  par(mar=c(2,4,1.5,1))
  # plot DA2c-DA1c
  plot(dMM[,DA2c]-dMM[,DA1c], col="blue", xlab="", cex.main=1,  
       main=paste(ptid1, " to ", ptid2, sep=""), 
       ylab=paste("congDA", ptid2, "- congDA", ptid1), type="h")
  abline(h=0, col="#BEBEBE80")
  abline(v=index(dMM)[h0], col="#BEBEBE80")
  
  # plot (DA2-DA1)-(RT2-RT1)
  dDART <- (dMM[,DA2]-dMM[,DA1]) - (dMM[,RT2]-dMM[,RT1]) 
  plot(dDART, col="blue", xlab="",
       ylab=paste("DA/RT (", ptid2, "-", ptid1, ")", sep=""), 
       cex.main=1, type="h")
  abline(h=0, col="#BEBEBE80")
  abline(v=index(dMM)[h0], col="#BEBEBE80")  # transparent gray

  path <- data.frame(path.no=1, source.ptid=ptid1, sink.ptid=ptid2,
    class.type="FLAT")
  CPSP <- FTR.get.CP.for.paths(path, NYPP, add.SP=TRUE)
  if (length(grep("-", CPSP$auction)))
    CPSP <- CPSP[-grep("-", CPSP$auction),]
  CPSP$month <- FTR.AuctionTerm(auction=CPSP$auction)$start.dt
  CPSP <- CPSP[order(CPSP$month),]

  if (nrow(CPSP)!=0){
    plot(range(CPSP$month), range(c(CPSP$CP, CPSP$SP)), type="n",
       ylab=paste(ptid2, "-", ptid1))
    points(CPSP$month, CPSP$CP, col="red") 
    points(CPSP$month, CPSP$SP, col="blue", pch=3) # a cross 
    abline(h=0, col="#BEBEBE80")
    legend("topleft", col=c("red", "blue"), pch=c(1,3), legend=c("CP", "SP"),
         text.col=c("red", "blue"))
  }
  
#  on.exit(par(opar))

  return(invisible(CPSP))
}


##   plot(da.1to2, plot.type="single", col=c("blue"),
##        main="", xlab="", ylab=paste("DA", ptid2, "- DA", ptid1))
##   abline(v=index(MM)[h0], col="gray", lty=3)
##   legend(x="topleft", legend=c(ptid1, ptid2), text.col=c("blue", "red"),
##          lwd=1, col=c("blue", "red"))

##   # plot inc1-inc2
##   dInc <- inc1 - inc2
##   plot(dInc, col="blue", xlab="",
##       ylab=paste("inc", ptid1, "+ dec", ptid2), cex.main=1)
##   abline(h=0, col="gray")
##   abline(v=index(MM)[h0], col="gray", lty=3)
  
