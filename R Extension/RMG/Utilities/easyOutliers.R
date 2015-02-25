################################################################################
# This function takes a data set and confidence interval and determines if it
# has any outliers past the specified interval.  It then iteratively removes 
# those outliers until a final data set is returned with outliers removed.
#
easyOutliers.removeOutliers <- function(dataSet, confidenceInterval=.95)
{
  if(!require(outliers))
  {
    source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
    load.packages("outliers")
  }

  #The grubbs test requires sorted data
  sortedData = sort(dataSet)
  
  # start the loop
  hasOutlier = TRUE
  while (hasOutlier)
  {
    # run the Grubb tests until no outliers are found 
    # (removing the outliers as the test progresses to further refine the data)
    testResults = grubbs.test(sortedData,type=10)
    
    # Get the confidence results of the tests
    hasOutlier = ifelse(confidenceInterval < (1 - testResults$p.value), 
      TRUE, FALSE)
    
    if (hasOutlier)
    {
      # We have a valid outlier, extract it from the result string
      # (I have no idea why it isn't available as an accessor)
      resultText = noquote(strsplit(testResults$alternative, " "))
      valToRemove = grep("[-]?[0-9]*.?[0-9]+", resultText[[1]], 
        extended=TRUE, value=TRUE)
    
      #remove it from the sorted & original data set 
      sortedData = sortedData[sortedData!=valToRemove]
      dataSet = dataSet[dataSet!=valToRemove]
    }
    
  }
  
  return(dataSet) 
}


################################################################################
# This function determines if an outlier is present in the data set
#
easyOutliers.hasOutlier <- function(dataSet, confidenceInterval=.95)
{
  if(!require(outliers))
  {
    source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
    load.packages("outliers")
  }

  #The grubbs test requires sorted data
  sortedData = sort(dataSet)

  # run the Grubb tests for outliers
  testResults = grubbs.test(sortedData,type=10)
  
  # Get the confidence results of the tests
  hasOutlier = ifelse(confidenceInterval < (1 - testResults$p.value), 
    TRUE, FALSE)
    
  return(hasOutlier)
}