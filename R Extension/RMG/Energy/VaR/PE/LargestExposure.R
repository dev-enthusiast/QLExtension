################################################################################
# File Namespace
#
LargestExposure = new.env(hash=TRUE)


################################################################################
# Return a list of the counterparties, sorted by maximum exposure
# 
LargestExposure$byCounterparty(asOfDate, useDirectMode=TRUE)
{
  dataDir = paste("R:/PotentialExposure/", asOfDate, "/overnight/RData", sep="")
  filePattern = ifelse(useDirectMode, "_DIRECT.RData$", "_INDIRECT.RData$") 
  fileList = list.files(dataDir, pattern=filePattern, full.names=TRUE)
  maxValue = data.frame(path=fileList)
  
  # find the maximum value in each file and bind them all together
  maxValue = apply(maxValue, 1, .getMax)
  maxValue = do.call(rbind, maxValue)
  
  # clean it up
  maxValue$asOfDate = NULL
  maxValue$type = NULL
  maxValue$what = NULL
  maxValue = subset(maxValue, value!=0)
  maxValue = unique(maxValue)
  
  # sort it
  maxValue = maxValue[ order(maxValue$value, maxValue$name, 
    maxValue$contract.dt, decreasing=TRUE), ]
    
  return(maxValue)
}
  
  
################################################################################
.getMax <- function(path)
{
  load(as.character(path))
  rowIndex =  which(aux$value==max(aux$value))
  return(aux[rowIndex,])
}

