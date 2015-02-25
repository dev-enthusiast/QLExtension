loadPortfolio <- function(options) {
  options$fileAsOfDate     <- paste(options$dir, "OptionsPositions_", options$portfolio, "_",
                                    as.character(options$asOfDate), ".csv", sep = "")
  dir.create(options$dirOut,  showWarnings = FALSE)
  books      <- get.portfolio.hierarchy(options$portfolio, options$asOfDate)
  booksToVar <- booksToVarMapping(books,options$asOfDate)
  if(file.exists(options$fileAsOfDate) == FALSE) {
    dataAsOfDate <- writeOptionsPositions(books, booksToVar, options$fileAsOfDate, options$asOfDate)
  } else {
    dataAsOfDate <- read.csv(options$fileAsOfDate)
    if(("QUANTITY" %in% colnames(dataAsOfDate)) == FALSE )
      dataAsOfDate <- writeOptionsPositions(books, booksToVar, options$fileAsOfDate, options$asOfDate)
  }
  if( length(options$books) > 1 | (length(options$books) == 1 & is.na(options$books[1]) == FALSE) ) {
    dataAsOfDate <- subset(dataAsOfDate, BOOK %in% options$books)
  } else { options$books <- get.portfolio.hierarchy(options$portfolio, options$asOfDate) }
  # linear positions
  options$books <- as.character(options$books)
  resDelta <- read.deltas.from.CPSPROD(options)
  resDelta$COMMOD_CURVE <- toupper(as.character(resDelta$COMMOD_CURVE))
  resDeltaX <- resDelta[, c("BOOK", "COMMOD_CURVE", "CONTRACT_DATE", "DELTA_VALUE")]
  colnames(resDeltaX)[which(colnames(resDeltaX) == "COMMOD_CURVE")] <- "LOCATION_NM"
  colnames(resDeltaX)[which(colnames(resDeltaX) == "CONTRACT_DATE")] <- "VALUATION_MONTH"
  colnames(resDeltaX)[which(colnames(resDeltaX) == "DELTA_VALUE")] <- "DELTA"
  optionsDelta <- dataAsOfDate[, c("BOOK", "DELTA", "VALUATION_MONTH", "LOCATION_NM")]
  optionsDelta$LOCATION_NM <- as.character(optionsDelta$LOCATION_NM)
  optionsDelta$DELTA <- -optionsDelta$DELTA
  optionsDelta <- aggregate(optionsDelta$DELTA, list(BOOK = optionsDelta$BOOK,
                                          VALUATION_MONTH = as.character(as.Date(optionsDelta$VALUATION_MONTH)),
                                          LOCATION_NM = optionsDelta$LOCATION_NM), sum)
  colnames(optionsDelta)[which(colnames(optionsDelta) == "x")] <- "DELTA"
  resDeltaX$VALUATION_MONTH <- as.character(as.Date(resDeltaX$VALUATION_MONTH))
  xxx <- rbind(optionsDelta, resDeltaX)
  linearDelta <- aggregate(xxx$DELTA, list(BOOK = xxx$BOOK,
                                          VALUATION_MONTH = as.character(as.Date(xxx$VALUATION_MONTH)),
                                          LOCATION_NM = xxx$LOCATION_NM), sum)
  colnames(linearDelta)[which(colnames(linearDelta) == "x")] <- "DELTA"
  indX <- which(abs(linearDelta$DELTA) < 1)
  linearDelta <- linearDelta[-which(abs(linearDelta$DELTA) < 1),]
  linearDeltaX <- data.frame(linearDelta, VEGA = NA, TYPE = "FORWARD", STRIKE = NA, QUANTITY = NA)
  linearDeltaX <- merge(linearDeltaX, booksToVar)
  colnames(dataAsOfDate)[which(colnames(dataAsOfDate) == "CALL_PUT_TYPE")] <- "TYPE"
  dataAsOfDate$VALUATION_MONTH <- as.character(dataAsOfDate$VALUATION_MONTH)
  data <-  rbind(linearDeltaX,dataAsOfDate[,colnames(linearDeltaX)])
  return(data)
}