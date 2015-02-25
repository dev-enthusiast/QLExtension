# Load portfolio in 2 steps: 
# 1. Options
#    i. Check if there is a file with current options positions, if not creates and saves it.
# 2. Linear Position, using Adrian's function read.Deltas.from.CPSPROD
# EXAMPLE
# options$asOfDate  <- as.Date("2006-11-15")
# options$portfolio   <- "Power Trading Portfolio"
# options$dir <- "H:/user/R/RMG/Energy/Trading/DailyOptionReport/Data/"
# data <- loadPortfolio(options)
# Anatoley Zheleznyak 11/16/2006


loadPortfolio <- function(options) {
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/booksToVarMapping.R")
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")  
  source("H:/user/R/RMG/Utilities/writeOptionsPositions.R")
  source("H:/user/R/RMG/Utilities/read.deltas.from.CPSPROD.R")
  
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