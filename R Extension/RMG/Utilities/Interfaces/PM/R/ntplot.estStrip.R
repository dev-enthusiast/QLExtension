# A wrapper for the ntplot function in SecDb
#
#


ntplot.estStrip <- function(symbol, startDate, endDate)
{
  res <- vector("list", length(symbol))
  for (r in 1:length(res)){
    res[[r]] <- cbind(symbol=symbol[r],
      data.frame(secdb.invoke("_lib ntplot functions",
      "NTPLOT::EstStrip", as.character(symbol[r]), startDate, endDate)))
  }

  if (length(res)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  res
}


