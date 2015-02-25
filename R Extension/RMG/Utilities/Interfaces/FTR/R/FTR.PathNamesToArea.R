`FTR.PathNamesToArea` <-
function( Paths, MAP, what="area" )
{
  if (length(Paths$path.no)==0){Paths$path.no <- 1:nrow(Paths)}
  Paths <- Paths[,c("path.no", "source.ptid", "sink.ptid")]
  
  res <- melt(Paths, id=which(!(names(Paths) %in% c("source.ptid", "sink.ptid"))))
  names(res)[ncol(res)] <- "ptid"
  res <- merge(res, MAP[,c(what,"ptid")], all.x=TRUE)
  res <- res[,-1]  # remove the ptid column
  names(res)[which(names(res)==what)] <- "value"

  res <- cast(res,  ... ~ variable, I)   
  names(res)[which(names(res)=="source.ptid")] <- paste("source.", what, sep="")
  names(res)[which(names(res)=="sink.ptid")] <- paste("sink.", what, sep="")    

  return(res)
}

