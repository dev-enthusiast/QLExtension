# Construct the list necessary for the simulation of Futures prices. 
# Adds the prices, vols and corr matrix for the curves you're interested in.
# Requires:
#   MarketData$asOfDate <- as.Date("2007-06-06")
#   MarketData$Initial  -  a data frame with curve.name and contract.dt columns.
#   MarketData$pricing.dt  <- seq(as.Date("2007-07-01"), by="month", length.out=5)
#
# First version by Adrian Dragulescu on 6-Jun-2007

make.MarketData <- function(MarketData, options=NULL){

  if (length(options)==0){
    source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
    options <- overnightUtils.loadOvernightParameters(MarketData$asOfDate, 
      connectToDB=FALSE)
  }
  #------------------------------add the forward.prices --------------------------- 
  cat("Get as of date prices ...")
  filename <- paste(options$save$datastore.dir, "Prices/hPrices.", options$asOfDate,
                    ".RData", sep="")
  load(filename)
  MarketData$Initial <- merge(hP, MarketData$Initial, all.y=T)
  names(MarketData$Initial)[3] <- "price"
  cat("Done.\n")
  
  #------------------------------add the historical forward.vols ------------------
  cat("Get as of date vols (corrected by outliers) ...")
  filename <- paste(options$save$prices.mkt.dir, "all.historicalVol.RData", sep="")
  load(filename)
  historicalVol <- historicalVol[, -which(names(historicalVol)
                                          %in% c("ClosingPrice", "PriceChange"))]
  MarketData$Initial <- merge(historicalVol, MarketData$Initial, all.y=T)
  names(MarketData$Initial)[3] <- "vol"
  cat("Done.\n")
  MarketData$Initial$hvol <- NULL

  #------------------------------construct the correlation matrix ----------------- 
  source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/calc.correlation.R")
  cat("Calculating correlations (from next 24 contracts) ... \n")
  res <- calc.correlation(MarketData$Initial[, c("curve.name", "contract.dt")], options)
  MarketData$Rho  <- res[[1]]; MarketData$Initial$hvol <- res[[2]]
  ind <- which(is.na(MarketData$Initial$vol))
  MarketData$Initial$vol[ind] <- MarketData$Initial$hvol[ind]
  MarketData$Initial$hvol <- NULL
  cat("Done.\n")  

  #----------------------------- fix correlation matrix ---------------------------
  source("H:/user/R/RMG/Statistics/correct.cor.matrix.R")
  MarketData$Rho <- correct.cor.matrix(MarketData$Rho)[[1]]
  
  return(MarketData)
}
## If you want to get the price history ... 
##   QQ <- expand.grid(curve.name=MarketData$curve.name, contract.dt=MarketData$contract.dt)
##   QQ <- cbind(QQ, position = 1, vol.type = "NA")
##   QQ$curve.name <- as.character(QQ$curve.name)
##   hPP <- aggregate.run.prices.mkt(QQ, options)  # get historical prices


