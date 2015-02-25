# Get spot exchange rates as of a certain date.
#
# Last modified by Adrian Dragulescu on 9-Apr-2007

get.spot.fx.rates <- function(options){
  
  cat("Reading in spot exchange rates ... ")  
  query <- paste("select pricing_date, rate_name, rate ",
    "from foitsox.sdb_irfx_rate where pricing_date between to_date('",
    options$calc$hdays[1], "','yyyy-mm-dd') and to_date('", options$asOfDate,
    "','yyyy-mm-dd') and rate_name like '%/%' and ",
    "start_date = pricing_date and start_date = end_date", sep="")
  hFX <- sqlQuery(options$connections$CPSPROD, query)
  colnames(hFX) <- c("pricing.dt", "rate.name", "value")

  hFX <- cast(hFX, rate.name ~ pricing.dt)
  colnames(hFX) <- gsub("\\.", "-", colnames(hFX))
  colnames(hFX) <- gsub("X", "", colnames(hFX))
  rownames(hFX) <- hFX[,"rate-name"]; hFX <- hFX[,-1]
  hFX["USD/USD", ] <- rep(1, ncol(hFX))

  filename <- paste(options$save$datastore.dir, "fx.", options$asOfDate,
    ".RData", sep="")
  save(hFX, file=filename)
  
  cat("Done!\n")
}
