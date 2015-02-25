# Get a load object
# 
# loadName  <- "RiProG10"
# loadClass <- "Large DS"
# asOfDate <- Sys.Date()

secdb.getLoadObject <- function(loadName, loadClass, asOfDate=Sys.Date())
{
  SecDBLib  <- "_Lib NTPLOT Functions"
  SecDBFun  <- "NTPLOT::ShowLoad"

  res <- secdb.invoke(SecDBLib, SecDBFun, as.character(loadName),
    asOfDate, as.character(loadClass))
  res <- as.data.frame(res)

  res$time <- as.POSIXlt(res$time)
  
  return(res)
}
