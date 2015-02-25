`FTR.PathNamesToZone` <-
function( Paths, MAP )
{
  if (length(Paths$path.no)==0){Paths$path.no <- 1:nrow(Paths)}
  Paths <- Paths[,c("path.no", "source.ptid", "sink.ptid")]
  
  res <- melt(Paths, id=which(!(names(Paths) %in% c("source.ptid", "sink.ptid"))))
  names(res)[ncol(res)] <- "ptid"
  res <- merge(res, MAP[,c("zone","ptid")], all.x=TRUE)
  res <- res[,-1]  # remove the ptid column
  names(res)[which(names(res)=="zone")] <- "value"

  res <- cast(res,  ... ~ variable, I)   
  names(res)[which(names(res)=="source.ptid")] <- "source.zone"
  names(res)[which(names(res)=="sink.ptid")] <- "sink.zone"    

  return(res)
}

