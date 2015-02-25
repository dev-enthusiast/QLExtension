# Get settle prices for several SecDb locations.
#
#

secdb.getSettlePrices <- function(startDate, endDate,
  location, container="BAL Close PWR NEPOOL PM")
{
  #container <- paste("BAL Close PWR", region, "PM")
  sd <- format(startDate, "%d%b%y")
  ed <- format(endDate, "%d%b%y")
  
  res <- vector("list", length(location))
  for (i in 1:length(location)){
    slang <- paste('Price Fixes by Time Dated Actual("', container,
      '", "', location[i], '", date("', sd, '"), date("', ed, '"));', sep="")
    aux <- secdb.evaluateSlang(slang)
    if (!is.null(aux))
      res[[i]] <- data.frame(location=location[i], aux)
  }

  res <- do.call(rbind, res)

  return(res)
}
