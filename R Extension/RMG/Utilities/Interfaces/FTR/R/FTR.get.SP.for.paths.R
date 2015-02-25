


`FTR.get.SP.for.paths` <- function( Paths, ISO.env )
{
  ind <- which(names(Paths) %in% c("path.no", "class.type",
    "source.ptid", "sink.ptid", "auction"))
  if (length(ind) < 5)
    stop(paste("Argument Paths missing a required field: path.no,",
               "class.type, source.ptid, sink.ptid, auction!"))
  
  Paths <- unique(Paths[, ind])
  Paths <- cbind(Paths, FTR.AuctionTerm(auction=Paths$auction))

  # Work with path.no not indices, as rows may shift...
  # paths with monthly Auctions
  indM  <- unique(Paths[which(nchar(Paths$auction)==3),"path.no"])  
  # paths with yearly Auctions
  indY  <- unique(Paths[grep("-1Y", Paths$auction),"path.no"])
  # paths with 6M auctions
  ind6M <- unique(Paths[grep("-6M", Paths$auction),"path.no"])
  
  hSP <- FTR.get.hSP.for.paths(Paths, ISO.env, melt=F)

  res <- NULL
  # get monthly auctions
  if (length(indM)>0){
    SP <- aggregate(hSP[,as.character(indM), drop=F],
      list(format(index(hSP), "%Y-%m-01")), mean, na.rm=T)
    if (ncol(SP)==1){colnames(SP) <- as.character(indM)}
    SP <- melt(cbind(start.dt=index(SP), as.data.frame(SP)), id=1)
    SP <- SP[is.finite(SP$value), ]
    colnames(SP) <- c("start.dt", "path.no", "SP")
    SP$path.no  <- as.numeric(as.character(SP$path.no))
    SP$start.dt <- as.Date(as.character(SP$start.dt))
    PathsM <- merge(Paths[which(nchar(Paths$auction)==3),], SP)
    res <- PathsM
  }

  # get annual auctions.  FIX if annual auctions don't start in Jan
  if (any(substr(Paths$auction[grep("-1Y", Paths$auction)], 1, 1) != "F")){
    cat("Annual auctions that don't start in Jan are not implemented yet.\n")
    Paths <- Paths[-grep("-1Y", Paths$auction),]
  }
  if (length(indY)>0){
    SP <- aggregate(hSP[,as.character(indY), drop=F],
      list(format(index(hSP), "%Y-01-01")), mean, na.rm=T)
    if (ncol(SP)==1){colnames(SP) <- as.character(indY)}
    SP <- melt(cbind(start.dt=index(SP), as.data.frame(SP)), id=1)
    SP <- SP[is.finite(SP$value), ]
    colnames(SP) <- c("start.dt", "path.no", "SP")
    SP$path.no  <- as.numeric(as.character(SP$path.no))
    SP$start.dt <- as.Date(as.character(SP$start.dt))
    PathsY <- merge(Paths[grep("-1Y", Paths$auction), ], SP)
    res <- rbind(res, PathsY)
  }
 
  # get 6M auctions
  if (length(ind6M)>0){
    cat("6M auctions not implemented yet.  Sorry.\n")
    Paths <- Paths[-grep("-6M", Paths$auction),]    
  }

  res <- res[, c("path.no", "class.type",
    "source.ptid", "sink.ptid", "auction", "SP")]
  res <- res[order(res$path.no), ]
  rownames(res) <- NULL
  
  return(res)
}
