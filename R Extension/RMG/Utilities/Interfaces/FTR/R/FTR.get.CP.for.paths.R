`FTR.get.CP.for.paths` <-
function( Paths, ISO.env, add.SP=FALSE, panelName=FALSE )
{
  if (length(Paths$path.no)==0){Paths$path.no <- 1:nrow(Paths)}
  ind <- which(names(Paths) %in% c("path.no", "class.type",
    "source.ptid", "sink.ptid", "auction"))
  Paths <- unique(Paths[, ind])
  
  res <- melt(Paths, id=which(!(names(Paths) %in% c("source.ptid", "sink.ptid"))))
  names(res)[which(names(res)=="value")] <- "ptid"
  
  res <- merge(res, ISO.env$CP[,c("class.type", "auction", "ptid",
                           "clearing.price.per.hour")], all.x=TRUE)
  res <- na.omit(res)
  if (nrow(res)==0)
    stop("Cannot find any auction results in ISO.env$CP.  Exiting.")
  ind <- grep("^source.ptid", res$variable)  
  res$clearing.price.per.hour[ind] <- -res$clearing.price.per.hour[ind]
  names(res)[which(names(res)=="clearing.price.per.hour")] <- "value"

  res <- cast(res, path.no + auction ~ variable, sum, fill=NA)
  res$CP <- res$sink.ptid + res$source.ptid
  res <- res[,c("path.no", "auction", "CP")]
  res <- na.omit(res)
  res <- data.frame(res)

  if (add.SP & nrow(res)>0){
    rLog("Get the historical settle prices.")
    if (!("auction" %in% names(Paths))){
      mAuctions <- sort(unique(res$auction))
      aux <- expand.grid.df(Paths, data.frame(auction=mAuctions))
    } else {
      aux <- Paths
    }

    SP <- FTR.get.SP.for.paths(aux, ISO.env)

    rLog("Merge settle and clearing prices.")
    res <- merge(res, SP[,c("path.no", "auction", "SP")])
    res <- res[order(res$path.no),]
    rownames(res) <- NULL
  }

  if (panelName){
    Paths$panelName <- paste(Paths$class.type, " ", Paths$sink.ptid, "/",
      Paths$source.ptid, sep="")
    Paths$panelName <- gsub("PEAK", "", Paths$panelName)
    res <- merge(res, Paths[,c("path.no", "panelName")], by="path.no", all.x=TRUE)
  }
  return(res)
}



##     hSP <- FTR.get.hSP.for.paths(Paths, ISO.env, melt=F)
##     SP <- aggregate(hSP, list(format(index(hSP), "%Y-%m-01")), mean, na.rm=T)
##     if (ncol(hSP)==1){colnames(SP) <- colnames(hSP)}
##     SP <- melt(as.matrix(SP))
##     SP <- SP[is.finite(SP$value), ]
##     colnames(SP) <- c("month", "path.no", "SP")
##     SP$path.no <- as.numeric(as.character(SP$path.no))


##     ind <- grep("-1Y",res$auction)
##     if (length(ind)>0){
##       browser()
      
## #      res <- res[-ind, ]  # only keep the monthlies
##     }

    
##     CP <- cbind(res, month=FTR.AuctionTerm(auction=res$auction)$start.dt)
##     CP <- CP[,-which(colnames(CP)=="auction")]







##   if (is.null(auction)){
##     auction.name <- auction
##     res <- subset(res, auction %in% auction.name)
##   }
