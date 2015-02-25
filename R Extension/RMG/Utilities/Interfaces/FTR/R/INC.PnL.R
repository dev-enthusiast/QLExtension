# Price inc/decs 
#
#  THIS IS VIRTUALLY UNUSABLE BECAUSE OF BAD DOCUMENTATION!
#  LEARN AND DON'T DO IT AGAIN!
#  

####################################################################
# 
#
INC.PnL <- function(QQ, region, hPP=NULL)
{
  if (any(!(c("ptid", "source.schedule", "sink.schedule", 
     "bucket", "start.dt", "mw") %in% names(QQ)))){
    stop("Missing one of the required columns. See help file.")
  }
  if (class(QQ$start.dt)[1] != "POSIXt"){
    cat("Converting start.dt to POSIX dates.\n")
    QQ$start.dt <- as.POSIXct(paste(QQ$start.dt, "00:00:00"))
  }
  ind <- which(is.na(QQ$ptid))
  if (length(ind)>0){
    rLog("\nHave unmapped positions!")
    out <- capture.output(print(QQ[ind,]))
    sendEmail("OVERNIGHT_REPORTS@constellation.com",
      "adrian.dragulescu@constellation.com", "Missing ptids in indec QQ!", out)
    QQ <- QQ[-ind,]
  }
  
  ptids <- sort(unique(QQ$ptid))
  start.dt <- as.POSIXct(paste(min(QQ$start.dt), "00:00:00"))
  end.dt <- as.POSIXct(paste(max(QQ$start.dt+24*3600), "00:00:00"))
  if (is.null(hPP))
    hPP <- .inc.dec.tsdbprices(ptids, region, start.dt,
                             end.dt)

  QQ$PnL <- NA
  for (r in 1:nrow(QQ)){
    ind <- grep(paste("_",QQ[r,"ptid"],"_",sep=""), colnames(hPP))
    if (length(ind)!=2){
      rLog("Inside INC.PnL: cannot find both DA and RT prices")
      browser()
    }
    hP <- hPP[,ind]
    QQ$PnL[r] <- as.numeric(try(.price.1.incdec(QQ[r,], hP, region)))
  }
  
  return(QQ)
}


###############################################################
#  Each inc/dec is for one day only, start.dt==end.dt
#  not good to reuse because it shows by bucket instead of hours!  
#
.price.1.incdec <- function(qq, hP, region)
{
  # it's hour ending so, I advance a second the start date
  ind.b <- secdb.getHoursInBucket(region, as.character(qq$bucket),
      qq$start.dt+1, qq$start.dt+24*3600)
  hP <- window(hP, index=ind.b$time)

  ind.source <- grep(qq$source.schedule, colnames(hP))
  ind.sink   <- grep(qq$sink.schedule, colnames(hP))
  res <- sum(hP[,ind.sink] - hP[,ind.source])*qq$mw
  return(res)
}

###############################################################
#
.inc.dec.tsdbprices <- function( ptids, region, start.dt, end.dt)
{
  if (region=="NEPOOL"){
    tsdb.symbols <- c(
      paste("NEPOOL_SMD_RT_", ptids, "_LMP", sep=""),
      paste("NEPOOL_SMD_DA_", ptids, "_LMP", sep=""))
  }
  hPP <- FTR.load.tsdb.symbols( tsdb.symbols, start.dt, end.dt)

  return(hPP)
}
