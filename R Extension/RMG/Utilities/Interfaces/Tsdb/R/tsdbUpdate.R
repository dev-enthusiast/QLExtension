# Update symbol name's data with curve.
# stat = tsdbUpdate(name, curve)
#   symbol  - name of the symbol to update
#   x - a data.frame with columns = c("time", "value")
# Returns 1 on success and 0 on failure.
#

tsdbUpdate <- function(symbol, x)
{
  if (!all(c("time", "value") %in% names(x)))
    stop("Input data.frame needs columns time and value!")

  secdb.invoke("_lib r functions", "R::TsdbUpdate", symbol,
    as.list(as.numeric(x$time)), as.list(x$value))
  
}




  ## if (res & fillDST){
  ##   res <- secdb.invoke("_lib elec matlab dispatch fns",
  ##     "tsdb::addDSTToTSDBSymbol", name, sdbdate(curve$time[1]),
  ##     sdbdate(curve$time[nrow(curve)]))
  ##   if (!res)
  ##     error("Failed to add DST to symbol")
  ## }


  ## N <- nrow(x)
  ## chunk_size <- 1000
  ## no_chunks  <- N %/% chunk_size + 1

  ## allres <- TRUE
  ## for (i in 1:no_chunks) {
  ##   ind <- ((i-1)*chunk_size+1):min(i*chunk_size, N)
  ##   cmd <- paste('TsdbUpdate("', symbol, '", TCurve([[',
  ##     paste(as.numeric(x$time[ind]), sep="", collapse=", "), "], [",
  ##     paste(sprintf("%f", x$value[ind]), sep="", collapse=", ")
  ##     ,']]));', sep="")  

  ##   try(res <- secdb.evaluateSlang(cmd))
  ##   allres  <- allres & res
  ##   if (!res)
  ##     cat("Insert failed for chunk ", i, ".  Try again!\n")
  ## }

  ## allres
