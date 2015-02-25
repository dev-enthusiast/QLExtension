
FTR.getPathReturns <- function(paths, start.dt, end.dt=Sys.time(),
  ISO.env=ISO.env, melt=FALSE, hourly=FALSE, buddy=.get_buddy_node())
{
  if (length(paths$path.no)==0){paths$path.no <- 1:nrow(paths)}
  if (length(paths$mw)==0){paths$mw <- 1}
  if (length(paths$CP)==0){paths$CP <- 0}
  if (as.POSIXct(start.dt) > as.POSIXct(end.dt))
    stop("Starting date is greater than end date.  Exiting.")
  if (any(paths$sink.ptid == paths$source.ptid))
    stop("Source and sink should not be the same!")
  
  paths <- paths[order(paths$path.no), ]
  
  must.have <- c("class.type", "sink.ptid", "source.ptid", "CP")
  if (any(!(must.have %in% colnames(paths))))
    stop(paste("Missing column", must.have[!(must.have %in% colnames(paths))],
         "in variable paths.")) 

  res <- NULL
  if (hourly){
    res <- .getPathReturnsHourly(paths, start.dt, end.dt=end.dt,
       ISO.env=ISO.env, melt=melt, buddy=buddy)
  } else {
    res <- .getPathReturnsDaily(paths, start.dt, end.dt,
       ISO.env=ISO.env, melt=melt, buddy=buddy)
  }
  return(res)
}

#---------------------------------------------------------------------
.getPathReturnsDaily <- function(paths, start.dt, end.dt=Sys.Date(),
   ISO.env, melt, buddy)
{
  hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=FALSE, buddy=buddy)
  hSP <- hSP[(index(hSP)>=as.Date(start.dt) & index(hSP) <= as.Date(end.dt)),,drop=FALSE]
  
  CP  <- matrix(paths$CP, nrow=nrow(hSP), ncol=nrow(paths), byrow=T)
  hSP <- hSP - CP  # the returns
  hSP <- hSP*matrix(paths$mw, nrow=nrow(hSP), ncol=nrow(paths), byrow=T)

  hrs <- matrix(24, nrow=nrow(hSP), ncol=nrow(paths))  # for NYPP 
  if (ISO.env$region == "NEPOOL"){
    ind.bucket <- .get.Bucket.Mask(ISO.env$region, start.dt,
                                   index(hSP)[length(index(hSP))])
    ind.bucket <- aggregate(ind.bucket[,-1],
      list(as.Date(format(ind.bucket$time, "%Y-%m-%d"))), sum)

    hrs[, which(paths$class.type=="ONPEAK")]  <- ind.bucket[,"ONPEAK"]
    hrs[, which(paths$class.type=="OFFPEAK")] <- ind.bucket[,"OFFPEAK"]
  }
  hSP <- hSP * hrs
  
  if (melt){
    hSP <- melt(as.matrix(hSP))
    colnames(hSP)[1:2] <- c("datetime", "path.no")
    hSP$path.no  <- as.numeric(as.character(hSPpath.no))
    hSP$datetime <- as.Date(as.character(hSP$datetime))
  }
  
  return(hSP)  
}

#---------------------------------------------------------------------
.getPathReturnsHourly <- function(paths, start.dt, end.dt=Sys.time(),
   ISO.env=ISO.env, melt=FALSE, buddy)
{
  if ("Date" %in% class(start.dt))
    start.dt <- as.POSIXct(paste(format(start.dt, "%Y-%m-%d"), "01:00:00"))
  aux <- melt(paths[,c("path.no", "source.ptid", "sink.ptid")], id=1)
  colnames(aux)[3] <- "ptid"
  
  # add the tsdb symbols 
  MAP <- merge(ISO.env$MAP[, c("ptid", "tsdb.symbol")], aux)

  # load the tsdb symbols
  tsdb.symbols <- sort(unique(MAP$tsdb.symbol))
  hP <- FTR.load.tsdb.symbols(tsdb.symbols, start.dt, end.dt)

  # correct for NYPP negative congestion signs
  if (ISO.env$region=="NYPP")
    hP <- -hP
  
  # get the bucket indices
  ind.bucket <- .get.Bucket.Mask(ISO.env$region, start.dt,
                                 index(hP)[length(index(hP))])

  ind <- merge(MAP[,c("tsdb.symbol","path.no")],
               data.frame(tsdb.symbol=colnames(hP), in.hP=TRUE), all.x=TRUE)
  if (any(is.na(ind$in.hP))){
    ind2 <- subset(ind, is.na(ind))
    cat(paste("Could not extract paths:", sort(unique(ind2$path.no))), "\n")
    browser()    
  }

  #path returns
  PR <- zoo(matrix(NA, nrow=nrow(hP), ncol=nrow(paths)), index(hP))
  colnames(PR) <- paths$path.no  
  for (p in 1:nrow(paths)){
    aux <- subset(MAP, path.no==paths$path.no[p])
    tsdb.sink   <- aux$tsdb.symbol[aux$ptid == paths[p,"sink.ptid"]]
    tsdb.source <- aux$tsdb.symbol[aux$ptid == paths[p,"source.ptid"]]
    
    PR[,p] <- paths$mw[p] * (hP[,tsdb.sink] - hP[,tsdb.source] - paths[p, "CP"])
    PR[,p][!ind.bucket[,paths$class.type[p]]] <- NA
  }

  if (melt) {
    PR <- melt(as.matrix(PR))
    colnames(PR)[1:2] <- c("datetime", "path.no")
    PR$path.no  <- as.numeric(as.character(PR$path.no))
    PR$datetime <- as.POSIXct(as.character(PR$datetime))
  }
  
  return(PR)  
}

.get.Bucket.Mask <- function(region, start.dt, end.dt)
{
  if (region=="NEPOOL"){    
    ind  <- merge(secdb.getBucketMask("NEPOOL", "PEAK", start.dt, end.dt), 
                  secdb.getBucketMask("NEPOOL", "OFFPEAK", start.dt, end.dt))
    colnames(ind)[2] <- "ONPEAK"  # ah!
  }
  if (region=="NYPP"){
    ind <- secdb.getBucketMask("NYPP", "FLAT", start.dt, end.dt)
  }
  return(ind)
}
