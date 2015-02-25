#
#
#



rm(list=ls())
MarketData <- options <- NULL
MarketData$asOfDate <- as.Date("2007-06-22")# Sys.Date()-1; #as.Date("2007-06-09") #
MarketData$curve.names <- c("Commod PWJ 5x16 West Physical", "Commod NG Exchange",
                            "Commod WTI Exchange", "Commod PWY 5x16 West Physical") 
MarketData$contract.dt <- seq(as.Date("2008-01-01"), by="month", length.out=60)
MarketData$pricing.dt  <- seq(as.Date("2007-07-01"), by="month", length.out=5)

source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/make.MarketData.R")
MarketData <- make.MarketData(MarketData, options)


source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/simulate.Futures.1F.R")
res <- simulate.Futures.1F(MarketData, options)

matplot(res[1,1,,], type="l")

#========================================================================
#       
#========================================================================

require(RODBC)
file <- "H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/control.xls"
con  <- odbcConnectExcel(file)
data <- sqlFetch(con, "main")
odbcCloseAll()

MM <- data.frame(curve.name=toupper(data[,"Base#Curve"]))
MM <- na.omit(MM)

MarketData <- options <- NULL
MarketData$asOfDate <- as.Date("2007-07-02") # Sys.Date()-1; #as.Date("2007-06-09") #
MarketData$curve.name <- as.character(MM$curve.name)
MarketData$contract.dt <- seq(as.Date("2007-08-01"), by="month", length.out=60)

MarketData$Initial <- expand.grid(curve.name  = MarketData$curve.name,
                                  contract.dt = MarketData$contract.dt)
MarketData$pricing.dt  <- seq(as.Date("2007-08-01"), by="month", length.out=60)




