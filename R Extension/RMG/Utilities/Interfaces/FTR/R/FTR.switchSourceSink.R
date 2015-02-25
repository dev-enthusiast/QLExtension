FTR.switchSourceSink <- function( paths, path.no )
{
  ind <- which(paths$path.no %in% path.no)
  paths.0   <- paths[-ind, ]   
  paths.inv <- paths[ind, ]     # paths you want to invert

  # flip all variables that have source/sinks
  vars <- grep("source.", colnames(paths))
  for (v in 1:length(vars)){
    vname.source <- colnames(paths)[vars[v]]
    vname.sink   <- gsub("source.", "sink.", vname.source)
    aux <- paths.inv[,vname.sink]
    paths.inv[,vname.sink] <- paths.inv[,vname.source]
    paths.inv[,vname.source] <- aux
  }
    
  # flip buy with sell 
  if ("buy.sell" %in% colnames(paths)){
    paths.inv$buy.sell <- ifelse( paths.inv$buy.sell == "SELL",
                                     "BUY", "NA")
  }
  # flip is.counterflow
  if ("is.counterflow" %in% colnames(paths)){
    paths.inv$is.counterflow <- ifelse( paths.inv$is.counterflow == TRUE,
      FALSE, ifelse(paths.inv$is.counterflow == FALSE, TRUE,
                    paths.inv$is.counterflow))
  }
  paths <- rbind(paths.0, paths.inv)
  paths <- paths[order(paths$path.no), ]
  
  return(paths)
}
