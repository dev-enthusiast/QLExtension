# A wrapper for the ntplot function in SecDb
# Should to the db call directly using odbc ...
#


tlp.prices <- function(id, bucket=c("5X16", "2X16H", "7X8"),
  region="NEPOOL", zone="HUB", scenario="Base", model="DA")
{
  res <- vector("list", length(bucket))
  for (r in 1:length(res)){
    aux <- cbind(bucket=bucket[r],
      data.frame(secdb.invoke("_lib ntplot functions",
      "NTPLOT::TLPPrices", as.character(id), bucket[r], region, zone,
      scenario, model)))
    res[[r]] <- aux
  }

  if (length(res)==1)
    res <- res[[1]]
  else
    res <- do.call(rbind, res)

  res
}


