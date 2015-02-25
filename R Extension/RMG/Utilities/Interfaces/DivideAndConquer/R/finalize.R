# Take a bunch of files, perform an operation on them, and bring the 
# results back in memory
#
finalize <- function(files, FUN, agg="rbind", ...)
{
  res <- vector("list", length=length(files))
  for (f in files){
    if (!file.exists(f))
      stop("File", f, "does not exist!")
    rLog("Working on", f)
    res[[f]] <- FUN(file=f, ...)
  }
  if (agg=="rbind"){
    res <- do.call(rbind, res)
    rownames(res) <- NULL
  }
  
  return(res)
}
