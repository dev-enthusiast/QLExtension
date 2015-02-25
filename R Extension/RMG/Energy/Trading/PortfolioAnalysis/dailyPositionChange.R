
options             <- NULL
options$asOfDate    <- as.Date("2006-11-13")
options$previousDate    <- as.Date("2006-11-10")
options$varMapping  <- "Jeff Addison"
options$dir <- "C:/temp/"
options$file     <- paste(options$dir, "Positions_", options$varMapping, "_",
                                    as.character(options$previousDate), ".csv", sep = "")
source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")

if(file.exists(options$file) == FALSE) {

} else {
  data <- read.csv(options$file)
}                                    

options$pricing.dt.start  <- options$asOfDate
options$pricing.dt.end    <- options$pricing.dt.start
options$contract.dt.start <- min(as.Date(data$VALUATION_MONTH))
options$contract.dt.end   <- max(as.Date(data$VALUATION_MONTH))
options$curve.names       <- as.character(unique(data$LOCATION_NM))
futurePrices <- read.prices.from.CPSPROD(options)
colnames(futurePrices)[which(colnames(futurePrices) == "COMMOD_CURVE")] <- "LOCATION_NM"
colnames(futurePrices)[which(colnames(futurePrices) == "START_DATE")]   <- "VALUATION_MONTH"
futurePrices$VALUATION_MONTH <- as.character(futurePrices$VALUATION_MONTH)
futurePrices$PRICING_DATE <- as.character(futurePrices$PRICING_DATE)
futurePrices <- futurePrices[,-which(colnames(futurePrices) == "PRICING_DATE")]
colnames(futurePrices)[which(colnames(futurePrices) == "PRICE")] <- "ASOFDATE_PRICE"

data <- merge(data, futurePrices)
data <- data.frame(data, DELTA_CHANGE = data$DELTA*(data$ASOFDATE_PRICE- data$PRICE))

dataDelta <- aggregate(data$DELTA_CHANGE, list(TYPE = data$TYPE, VALUATION_MONTH = as.character(as.Date(data$VALUATION_MONTH)),
                                          LOCATION_NM = data$LOCATION_NM), sum)
dataDelta <- dataDelta[-which(abs(dataDelta$x) < 10),]
colnames(dataDelta)[which(colnames(dataDelta) == "x")] <- "DELTA_VALUE_CHANGE"
write.table(dataDelta, file = paste(options$dir, "DELTA_VALUE_CHANGE_", options$varMapping, "_",
                                    as.character(options$asOfDate), ".csv", sep = ""), append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)



                                          