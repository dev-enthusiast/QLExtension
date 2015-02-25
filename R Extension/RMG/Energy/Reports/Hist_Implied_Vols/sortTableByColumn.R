################################################################################
# Take a table and sort it by the column specified, returning the optional
# number of results.
#
sortTableByColumn <- function(tableData, column, resultsToReturn=-1, decreasing=TRUE)
{
  tableData = na.omit(tableData)
  column = na.omit(column)

  sortedData = sort(column, index.return=TRUE, decreasing=decreasing)
  if (resultsToReturn != -1)
  {
    indexes = head(sortedData$ix,n=resultsToReturn)
  }else
  {
    indexes = sortedData$ix
  }

  sortedTable = tableData[indexes,]

  return(sortedTable)
}