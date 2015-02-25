# basic plotting stuff for an FTR path
# Where:
#   hist = number of days you want to inspect
#   wdim = plot window dimensions in inches
#

FTR.plot <- function(ptid1, ptid2, bucket="ONPEAK",
                     hist=365, wdim=c(12,9))
{
  if (!("package:SecDb" %in% search()))
    require(SecDb)
  if (!(toupper(bucket) %in% c("ONPEAK", "OFFPEAK")))
    stop("Wrong bucket type!  Only onpeak and offpeak are supported.")
  
  sbucket <- bucket
  if (toupper(sbucket)=="ONPEAK") sbucket <- "PEAK"
  
  root <- "NEPOOL_SMD_"
  DA1  <- paste(root, "DA_", ptid1, "_LMP", sep="") 
  DA2  <- paste(root, "DA_", ptid2, "_LMP", sep="") 
  RT1  <- paste(root, "RT_", ptid1, "_LMP", sep="") 
  RT2  <- paste(root, "RT_", ptid2, "_LMP", sep="") 
  DA1c <- paste(root, "DA_", ptid1, "_CongComp", sep="")
  DA2c <- paste(root, "DA_", ptid2, "_CongComp", sep="")
    
  symbols  <- c(DA1, DA2, RT1, RT2, DA1c, DA2c)
  start.dt <- as.POSIXct(paste(format(currentMonth(Sys.Date() - 3*hist)),
                                      "01:00:00"))
  end.dt   <- Sys.time() + 24*3600
  MM  <- FTR:::FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
  #MM[is.na(MM)] <- 0  # why was this here?! AAD - 8/14/2013

  bucketMask <- secdb.getHoursInBucket("NEPOOL", sbucket, index(MM)[1],
                                    index(MM)[nrow(MM)])
  MM <- MM[bucketMask$time,]


  # aggregate daily
  dMM <- aggregate(MM, as.Date(format(index(MM)-1, "%Y-%m-%d")), mean,
                   na.rm=TRUE)
  # add NA's for weekends
  dMM <- merge(dMM,
    zoo(0, seq(min(index(dMM)), max(index(dMM)), by="1 day")))
  dMM <- dMM[,-ncol(dMM)]
  
  dMM <- dMM[(nrow(dMM)-hist):nrow(dMM),]
  #dMM <- window(dMM, start=Sys.Date()-hist)       # TODO!
  # begining of the month vertical bars
  h0 <- which(as.numeric(format(index(dMM), "%d"))==1)

  # do the plots
  windows(wdim[1], wdim[2])
  layout(matrix(1:3, 3, 1))
  par(mar=c(2,4,1.5,1))
  # plot DA2c-DA1c
  plot(dMM[,DA2c]-dMM[,DA1c], col="blue", xlab="", cex.main=1,  
       main=paste(ptid1, " to ", ptid2, ", ", bucket, " bucket", sep=""), 
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

  # calculate the monthly settle price
  aux <- aggregate(MM[,DA2c]-MM[,DA1c],
    as.Date(format(index(MM)-1, "%Y-%m-01")), mean, na.rm=TRUE)
  #aux <- aux[,DA2c]-aux[,DA1c]
  SP <- data.frame(month=index(aux), SP=as.numeric(aux))

  # get the monthly clearing prices
  path <- data.frame(path.no=1, source.ptid=ptid1, sink.ptid=ptid2,
    class.type=toupper(bucket))
  CPSP <- FTR.get.CP.for.paths(path, NEPOOL, add.SP=FALSE)
  if (length(grep("-", CPSP$auction)))
    CPSP <- CPSP[-grep("-", CPSP$auction),]
  CPSP$month <- FTR.AuctionTerm(auction=CPSP$auction)$start.dt
  CPSP <- CPSP[order(CPSP$month),]
  CPSP <- merge(CPSP, SP)
 
  plot(range(CPSP$month), range(c(CPSP$CP, CPSP$SP)), type="n",
       ylab=paste(ptid2, "-", ptid1))
  points(CPSP$month, CPSP$CP, col="red") 
  points(CPSP$month, CPSP$SP, col="blue", pch=3) # a cross 
  abline(h=0, col="#BEBEBE80")
  legend("topleft", col=c("red", "blue"), pch=c(1,3), legend=c("CP", "SP"),
         text.col=c("red", "blue"))
 
  
#  on.exit(par(opar))
  CPSP[,4:5] <- sapply(CPSP[,4:5], function(x)round(x,2))
  
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
  
