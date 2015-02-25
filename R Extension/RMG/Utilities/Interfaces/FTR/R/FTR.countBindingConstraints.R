`FTR.countBindingConstraints` <-
function( paths, BC, nby.months )
{
  uPaths <- unique(paths[,c("source.ptid", "sink.ptid")])
  uNodes <- sort(unique(c(uPaths$source.ptid, uPaths$sink.ptid)))
  tsdb.symbols <- paste("NEPOOL_SMD_DA_", uNodes, "_CongComp", sep="")
  
  rLog("Read prices from tsdb")
  start.dt <- ISOdatetime(2003,4,1,0,0,0)
  end.dt   <- Sys.time()

  ind.ONPEAK  <- secdb.getHoursInBucket("NEPOOL", "peak", start.dt, end.dt)
  ind.OFFPEAK <- secdb.getHoursInBucket("NEPOOL", "offpeak", start.dt, end.dt)
  ind.bucket <- rbind(cbind(ind.ONPEAK,  class.type="ONPEAK"),
                      cbind(ind.OFFPEAK, class.type="OFFPEAK"))

  sres <- vector("list", length=nrow(uPaths))
  for (r in 1:nrow(uPaths)){
    rLog(paste("Working on path", r, "of", nrow(uPaths)))
    symbols <- paste("NEPOOL_SMD_DA_", c(uPaths$source.ptid[r],
      uPaths$sink.ptid[r]), "_CongComp", sep="")
    hPP <- FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
    spread <- round(hPP[,2] - hPP[,1], 2)
    ind <- which(spread == 0)
    not.relevant <- unique(subset(BC,
       time %in% index(spread)[ind])$constraint.name)
    
    spread <- spread[which(spread != 0)]

    spread <- data.frame(time=index(spread), value=coredata(spread))
    spread <- merge(spread, ind.bucket)
    spread$month <- as.POSIXlt(spread$time)$mon+1
    spread <- merge(spread, BC)
    spread <- subset(spread, class.type==paths$class.type[r])

    spread <- subset(spread, !(constraint.name %in% not.relevant))
    if (nrow(spread)==0){next}
    spread$nby.month <- FALSE
    spread$nby.month[spread$month %in% nby.months] <- TRUE
    
    res <- cast(spread, nby.month + constraint.name ~ .,
                function(x){c(length(x), mean(x, na.rm=T))})
    colnames(res)[3:4] <- c("count", "mean")

    sres[[r]] <- cbind(path.no=paths$path.no[r], res)
    
    filename <- paste("Constraints/", uPaths$source.ptid[r], ".to.",
                      uPaths$sink.ptid[r], ".csv", sep="")
    write.csv(res, file=filename, row.names=F)
  }
  sres <- do.call(rbind, sres)
  rownames(sres) <- NULL
  
  return(sres)
}

