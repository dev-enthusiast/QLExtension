`FTR.PathNamesToPTID` <-
function( Paths, MAP, from="ptid", to)
{
  if (length(Paths$path.no)==0){
    Paths$path.no <- 1:nrow(Paths)}

  if (to == "ptid" & from != "name"){
    stop("You can only go to ptid's from names.")
  }
  
  cnames <- c("path.no", paste(c("source.", "sink."), from, sep=""))
  Paths  <- unique(Paths[, cnames])
  res    <- melt(Paths, id=1)
  names(res)[ncol(res)] <- from
  res <- merge(res, MAP[,c(from, to)], all.x=TRUE)
  
  res <- res[,-which(names(res)==from)]     # remove the "from" column
  names(res)[which(names(res)==to)] <- "value"
  
  res <- cast(res,  ... ~ variable, I)
  colnames(res) <- gsub(from, to, colnames(res)) # change to colum names 
  
  res <- data.frame(res[order(res$path.no), ])
  
  return(res)
}

