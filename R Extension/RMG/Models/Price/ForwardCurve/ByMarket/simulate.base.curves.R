# Simulate base curves.  Load base curves from the spreadsheet. 
#
#
#
# First version by Adrian Dragulescu on 5-Jul-2007


simulate.base.curves <- function(QQ, options){

  require(RODBC)
  file <- "H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/control.xls"
  con  <- odbcConnectExcel(file)
  Base <- sqlFetch(con, "main")
  odbcCloseAll()

  names(Base) <- c("mkt", "curve.name")
  Base$curve.name <- toupper(Base$curve.name)
  Base <- na.omit(Base)

  #------------------------ get the min, max contract date by market ------
  max.date <- aggregate(QQ$contract.dt, by=list(mkt=QQ$mkt), max)
  min.date <- aggregate(QQ$contract.dt, by=list(mkt=QQ$mkt), min)
  max.date$max.contract.dt <- as.Date("1970-01-01") + max.date$x
  min.date$min.contract.dt <- as.Date("1970-01-01") + min.date$x
  max.date <- merge(Base, max.date)
  min.date <- merge(Base, min.date)
  range.date <- merge(max.date, min.date, by="mkt")

  #------------------------ get the MarketData structure ------------------
  MarketData <- MarketData$Initial <- NULL
  MarketData$asOfDate <- options$asOfDate
  MarketData$pricing.dt <- get.pricing.dts(MarketData$asOfDate,
                                           max(max.date$max))
  MarketData$curve.name <- as.character(Base$curve.name)
  for (r in 1:nrow(range.date)){
    dates <- rev(seq(range.date[r,"max.contract.dt"],
                     range.date[r, "min.contract.dt"], by="-1 month"))
    aux <- data.frame(curve.name=range.date[r,"curve.name"], contract.dt=dates)
    MarketData$Initial <- rbind(MarketData$Initial, aux)
  }

  MarketData$Initial <- na.omit(MarketData$Initial) # NG exchange cash month ?
  
  
  source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/make.MarketData.R")
  MarketData <- make.MarketData(MarketData, options)

  options$noSims <- 1000
  options$save$price.sims <- TRUE
  source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/simulate.Futures.1F.R")
  sPP <- simulate.Futures.1F(MarketData, options)

  sPP <- do.call("rbind", sPP)
  rownames(sPP) <- NULL
  
  head(sPP)
  

}
