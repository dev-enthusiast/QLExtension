# Main function to transform a set of data files into some output data files.
#
# keys is a list keys[["name1"]] <- c("file_1", "file_2", ...)
# Conquer loops over all the keys.  All the files under 
#
conquer <- function(keys, FUN, preFileName="", overwrite=TRUE, ...)
{

  funArgs <- formals(FUN)
  if (!("file" %in% names(as.list(funArgs)))){
    stop("Conquer function FUN, does not have a file= keyword argument.")
  }
  
  if (length(grep("/$", preFileName)==1)){
    dirNameOut <- preFileName
  } else {
    dirNameOut <- dirname(preFileName)
  }
  if (is.na(file.info(dirNameOut)$isdir))
      dir.create(dirNameOut)     
  
  keyNames <- names(keys)
  for (k in keyNames){
    rLog("Conquering key:", k)
    fileNameOut <- paste(preFileName, k, ".RData", sep="")
    if (file.exists(fileNameOut)){
      rLog("Key has been processed before.  Skipping.")
      next
    }
     
    values <- keys[[k]]        # values are usually filenames ...
    
    res <- vector("list", length=length(values))
    for (ind in 1:length(values)){
      rLog("Working on file", values[ind])
      if (!file.exists(values[ind]))
        stop("File", values[ind], "does not exist!")
      res[[ind]] <- FUN(file=values[ind], ...) 
    }
    res <- do.call(rbind, res)  
    rownames(res) <- NULL       
    rLog("Processed key:", k)

    if (nrow(res)>0){   # the function may not return anything
      if (!overwrite){
        this.res <- res
        load(fileNameOut)
        res <- rbind(res, this.res)  # append to existing results
      }
      # save the transformed data
      res <- try(save(res, file=fileNameOut))
      if (class(res)!="try-error")
        rLog("Wrote key to", fileNameOut)
    }
  }
}

