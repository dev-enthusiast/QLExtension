loadFutureCurves <- function(options) {

  options$pricing.dt.end    <- options$pricingAsOfDate
  options$fileFuturePrices <- paste(options$dir, "futurePrices",
                                    as.character(options$portfolio), ".csv", sep = "")
  if(file.exists(options$fileFuturePrices) == FALSE) {
    options <- calculateDateRange(options, 98, 0)
    futurePrices <- read.prices.from.CPSPROD(options)
    write.table(futurePrices, file = options$fileFuturePrices,
                append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  } else {
    futurePrices <- read.csv(options$fileFuturePrices)
    futurePrices$PRICING_DATE <- as.Date(futurePrices$PRICING_DATE)
    futurePrices$START_DATE <- as.Date(futurePrices$START_DATE)
    dateRange <- range(futurePrices$PRICING_DATE)
    if(dateRange[2] < options$pricingAsOfDate){
      options$pricing.dt.start  <- dateRange[2]+1
      futurePrices <- rbind(futurePrices,read.prices.from.CPSPROD(options))
    } else {
      Nx <- length(which(unique(futurePrices$PRICING_DATE) <=  options$pricingAsOfDate))
      if(Nx < options$nHist) {
        options <- calculateDateRange(options, options$nHist - Nx + 10, Nx)
        options$pricing.dt.end <- dateRange[1] - 1
        if(options$pricing.dt.end >= options$pricing.dt.start)
          futurePrices <- rbind(futurePrices,read.prices.from.CPSPROD(options))
      }
    }
   write.table(futurePrices, file = options$fileFuturePrices,
                append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  }
  return(futurePrices)
}

calculateDateRange <- function(options, N, Nx) {
  while(Nx != options$nHist) {
    options$pricing.dt.start  <- options$pricingAsOfDate - N
    days <- seq(as.Date(options$pricing.dt.start), as.Date(options$pricingAsOfDate), by = 1)
    nWeekend <- which(weekdays(days) == "Sunday" | weekdays(days) == "Saturday")
    Nx <- length(days) - length(nWeekend)
    if( Nx < options$nHist) {N <- N + 1 }
    if( Nx > options$nHist) {N <- N - 1 }
  }
  return(options)
}