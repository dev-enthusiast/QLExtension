# Create a new symbol in TSDB.
# x = tsdbCreate(name, path, realTimeFlag, description, dataSource);
#   name         - name of the new symbol
#   path         - file system path to the bin file for the data
#   realTimeFlag - 1 for real-time data (hourly); 0 for daily data
#   description: String  - symbol description
#   dataSource: String   - a comment about the source of the data
# Returns 1 on success and 0 on failure.
#

tsdbCreate <- function(symbol, path, description, dataSource, realTimeFlag=TRUE)
{
  if (nchar(symbol) > 31)
    stop("The number of characters in name has to be < 31.")
    
  res <- secdb.invoke("_lib elec matlab dispatch fns", "tsdb::create",
    symbol, path, realTimeFlag, description, dataSource)

  res
}
