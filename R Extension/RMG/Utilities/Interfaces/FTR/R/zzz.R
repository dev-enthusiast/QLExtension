


#################################################################
# Moved here on 9/26/2013 - not exporting anymore
#
.FTR.balloonplot <- function( paths, what="area", value="mw",
   main=NULL, title=NULL, table=FALSE, diag=TRUE)
{
  paths$count <- 1
  if (length(paths$mw)==0){paths$mw <- 1}
  TT <- NULL
  
  source.nm <- paste("source.", what, sep="")
  sink.nm   <- paste("sink.",   what, sep="")
  if (any(!(c(value, source.nm, sink.nm) %in% names(paths))))
    stop("Not all the variables exists in the data.frame.  Check!")

  res <- aggregate(paths[,value],
    as.list(paths[,c("class.type", source.nm, sink.nm)]), sum)
  colnames(res)[4] <- "value"

  if (table == TRUE){
    uWhat <- sort(unique(unlist(c(res[,2:3]))))
    res[,2] <- factor(res[,2], levels=uWhat)
    res[,3] <- factor(res[,3], levels=uWhat)
    TT <- eval(parse(text=paste("cast(res, class.type + ",
      source.nm, " ~ ", sink.nm,  ", sum, na.rm=TRUE, add.missing=TRUE, ", 
      " margins=TRUE)", sep="")))
    ind <- which(TT$class.type=="(all)")
    TT  <- TT[-ind[-length(ind)], ]
  } else {
    if (diag){
      dotcolor <- rep("skyblue", nrow(res))
      dotcolor[res[,2]==res[,3]] <- "yellow"
    }
    balloonplot(y=list(bucket=res$class.type, source=res[, source.nm]),
              x=list(sink=res[,sink.nm]),
              z=round(res[,"value"]), sorted=T, 
              main=main, colmar=1, dotcolor=dotcolor)
    title(main=list(title, cex=0.9), line=0.5)
  }
 
  invisible(TT) 
}
