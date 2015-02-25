#
# Where buddyDF is a data.frame(ptid, buddyPtid)
# for example:
# data.frame(
#   ptid      = c(40327, 40328),
#   buddyPtid = c(1691,   1691)) 
#


###################################################################
#
FTR.get.hSP.for.paths <-
function( Paths, ISO.env, melt=FALSE, start.dt=NULL, end.dt=NULL,
  buddy=.get_buddy_node() )
{
  if (length(Paths$path.no)==0){Paths$path.no <- 1:nrow(Paths)}
  Paths <- Paths[order(Paths$path.no), ]

  hSP <- zoo(matrix(NA, ncol=nrow(Paths), nrow=nrow(ISO.env$hP)),
             index(ISO.env$hP))
  colnames(hSP) <- Paths$path.no

  Paths$bucket.sink   <- paste(Paths$class.type, Paths$sink.ptid, sep=".")
  Paths$bucket.source <- paste(Paths$class.type, Paths$source.ptid, sep=".")

  hP <- as.matrix(ISO.env$hP) # for convenience, hopefully faster access too
  cnames <- colnames(hP)
  for (p in 1:nrow(Paths)) {
    
    sinkPrice   <- hP[, Paths$bucket.sink[p]]
    sourcePrice <- hP[, Paths$bucket.source[p]]

    if ( !is.na(buddy[as.character(Paths$source.ptid[p])])) {
      buddyPtid <- buddy[as.character(Paths$source.ptid[p])]
      ind <- length(takeWhile(sourcePrice, is.na))
      cname <- paste(Paths$class.type[p], buddyPtid, sep=".")
      sourcePrice[1:ind] <- hP[1:ind, cname]
    }
    
    if ( !is.na(buddy[as.character(Paths$sink.ptid[p])])) {
      buddyPtid <- buddy[as.character(Paths$sink.ptid[p])]
      ind <- length(takeWhile(sinkPrice, is.na))
      cname <- paste(Paths$class.type[p], buddyPtid, sep=".")
      sinkPrice[1:ind] <- hP[1:ind, cname]
    }
    
    hSP[,p] <- sinkPrice - sourcePrice     
  }

  if (!is.null(start.dt) && !is.null(end.dt)){
    hSP <- hSP[(index(hSP)>=start.dt & index(hSP) <= end.dt),,drop=FALSE]
  }
  
  if (melt){
    hSP <- melt(as.matrix(hSP))
    names(hSP)[1:2] <- c("day", "path.no")
    hSP$day <- as.Date(hSP$day)
    hSP$path.no <- as.numeric(as.character(hSP$path.no))
    hSP <- na.omit(hSP)
  }
  
  hSP
}
