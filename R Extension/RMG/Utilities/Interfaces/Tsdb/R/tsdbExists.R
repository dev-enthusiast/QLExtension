# Check if a symbol exists
# @param symbol the name of the tsdb symbol
# @return a boolean if the symbol exists or not

tsdbExists <- function(symbol)
{
  res <- secdb.invoke("_lib elec matlab dispatch fns", "tsdb::exists",
    symbol)

  as.logical(res)
}


.test_tsdbExists <- function()
{
  tsdbExists("nepool_load_hist")

  tsdbExists("goo goo gaa gaa")
  
}
