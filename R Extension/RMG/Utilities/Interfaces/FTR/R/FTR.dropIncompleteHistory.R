`FTR.dropIncompleteHistory` <-
function( paths, hSP )
{
  dropped <- NULL
  if (class(hSP)=="list"){
    buckets <- names(hSP)
    for (bucket in buckets){
      ind.not.na <- !apply(is.na(hSP[[bucket]]), 2, any)
      hSP[[bucket]] <- hSP[[bucket]][,ind.not.na]
      dropped <- c(dropped, as.numeric(names(which(!ind.not.na))))
    }
  }
  if (length(dropped)>0){
    rLog("Dropped these paths due to incomplete prices:")
    print(subset(paths, path.no %in% dropped))
    paths <- subset(paths, !(path.no %in% dropped))
  }

  return(list( paths, hSP ))
}

