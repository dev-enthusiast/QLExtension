

FTR.performance <- function(paths, ISO.env)
{
  paths <- paths[order(paths$path.no), ]
  must.have <- c("auction", "class.type", "sink.ptid", "source.ptid", "mw", "CP")
  if (any(!(must.have %in% colnames(paths))))
    stop(paste("Missing column", must.have[!(must.have %in% colnames(paths))],
         "in variable paths."))
  
  if (length(paths$path.no)==0){paths$path.no <- 1:nrow(paths)}

  # get the start.dt/end.dt by path
  aux   <- data.frame(path.no=paths$path.no, FTR.AuctionTerm(auction=paths$auction))
  paths <- cbind(paths, aux[,-1])
  
  res <- vector("list", length=nrow(aux))
  names(res) <- aux$path.no
  for (r in 1:nrow(aux)){
    res[[r]] <- seq(aux[r,2], aux[r,3], "day")
  }
  res <- data.frame(melt(res))
  rownames(res) <- NULL
  colnames(res) <- c("day", "path.no")
  
  hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=T)
  colnames(hSP)[3] <- "SP"
  
  # pick only the dates you're interested in
  hSP <- merge(hSP, res)

  # add the number of hours per path
  aux <- ISO.env$HRS
  colnames(aux)[which(colnames(aux)=="contract.dt")] <- "start.dt"
  paths <- merge(paths, aux)


  
  # add the 
  res <- merge(hSP)
  
  
  hSP <- split(hSP[,c("day", "va")], )

  
  


  
  
}

.fun.expand <- function(x){
   x <- sort(x)
   return(seq(x[1], x[2], by="day"))
}
