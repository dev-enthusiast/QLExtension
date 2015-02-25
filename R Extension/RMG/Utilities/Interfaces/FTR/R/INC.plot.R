# basic plotting stuff for a ptid
# Where:
#   hist = number of days you want to inspect
#   wdim = plot window dimensions
#


.make_RTDA_symbs <- function(ptid)
{
  if (ptid < 60000) {
    # a NEPOOL node, this is the usual pattern
    DA  <- paste("NEPOOL_SMD_DA_", ptid, "_LMP", sep="") 
    RT  <- paste("NEPOOL_SMD_RT_", ptid, "_LMP", sep="")  
  } else if (ptid %in% 61752:61847) {
    # a NYPP zone node.  You can only inc/dec at a zone in NYPP
    DA  <- paste("NYPP_DAM_ZONE_LBM_", ptid, sep="") 
    RT  <- paste("NYPP_RT_ZONE_LBM_", ptid, sep="")    # this is 5 min data!  
  }
  c(DA, RT)
}

INC.plot <- function(ptid1, ptid2, hist=10, wdim=c(12,7))
{
  if (!("package:SecDb" %in% search()))
    require(SecDb)

  aux <- .make_RTDA_symbs(ptid1)
  DA1 <- aux[1];  RT1 <- aux[2]
  aux <- .make_RTDA_symbs(ptid2)
  DA2 <- aux[1];  RT2 <- aux[2]
  
  symbols  <- c(DA1, RT1, DA2, RT2)
  start.dt <- Sys.time() - 24*3600*hist
  end.dt   <- Sys.time() + 24*3600
  aggHourly <- ifelse(grepl("^NYPP", symbols), TRUE, FALSE)
  MM  <- FTR:::FTR.load.tsdb.symbols(symbols, start.dt, end.dt, aggHourly=aggHourly)
  MM[is.na(MM)] <- 0

  h0 <- which(as.numeric(format(index(MM), "%H"))==0)
  inc1 <- MM[,DA1] -  MM[,RT1]
  inc2 <- MM[,DA2] -  MM[,RT2]

#  opar=par()
  
  windows(wdim[1], wdim[2])
  layout(matrix(1:4, 2, 2))
  par(mar=c(2,4,1,1))
  plot(cbind(inc1, inc2), plot.type="single", col=c("blue", "red"),
       main="", xlab="", ylab="incs")
  abline(h=0, col="gray")
  abline(v=index(MM)[h0], col="gray", lty=3)
  legend(x="topleft", legend=c(ptid1, ptid2), text.col=c("blue", "red"),
         lwd=1, col=c("blue", "red"))

  # plot DA1-DA2
  dDA <- MM[,DA1]-MM[,DA2]
  plot(dDA, col="blue", xlab="",
       ylab=paste("DA", ptid1, "- DA", ptid2), cex.main=1)
  abline(h=0, col="gray")
  abline(v=index(MM)[h0], col="gray", lty=3)

  # plot inc1-inc2
  dInc <- inc1 - inc2
  plot(dInc, col="blue", xlab="",
      ylab=paste("inc", ptid1, "+ dec", ptid2), cex.main=1)
  abline(h=0, col="gray")
  abline(v=index(MM)[h0], col="gray", lty=3)
  
  # plot RT1-RT2
  dRT <- MM[,RT1]-MM[,RT2]
  plot(dRT, col="blue", xlab="",
       ylab=paste("RT", ptid1, "- RT", ptid2), cex.main=1)
  abline(h=0, col="gray")
  abline(v=index(MM)[h0], col="gray", lty=3)
  
#  on.exit(par(opar))

  return(invisible(MM))
}

