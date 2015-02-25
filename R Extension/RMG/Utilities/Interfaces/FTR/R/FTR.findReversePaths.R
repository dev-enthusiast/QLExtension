FTR.findReversePaths <- function( paths )
{
  if ("path.no.rev" %in% colnames(paths))   # remove it if there ...
    paths$path.no.rev <- NULL
  
  ind <- which(duplicated(paths[,c("path.no", "source.ptid", "sink.ptid",
                             "class.type")]))
  if (length(ind)>0)
    stop("Paths are not unique: path.no, source.ptid, sink.ptid, class.type.")
  
  direct  <- paths[,c("path.no", "source.ptid", "sink.ptid", "class.type")]
  inverse <- direct
  names(inverse)[1:3] <- c("path.no.rev", "sink.ptid", "source.ptid")
  
  aux <- merge(direct, inverse)

  paths <- merge(paths, aux[,c("path.no","path.no.rev")], all.x=TRUE)

##   aux <- subset(aux, path.no < path.no.rev)
##   reversePaths <- sort(unique(aux$path.no.rev))
  
  return( paths )
}
