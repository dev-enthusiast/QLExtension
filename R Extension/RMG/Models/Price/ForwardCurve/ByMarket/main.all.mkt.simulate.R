#
#
#
#

main.all.mkt.simulate <- function(){

  MarketData <- options <- NULL
  MarketData$asOfDate <- as.Date("2007-07-03") #Sys.Date()-1; 

  source("H:/user/R/RMG/Energy/VaR/PE/set.R")
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  options <- overnightUtils.loadOvernightParameters(MarketData$asOfDate, 
    connectToDB=FALSE)

  file <- paste(options$save$datastore.dir, "Positions/forc.positions.",
                MarketData$asOfDate, ".RData", sep="")
  load(file)
  QQ <- unique(QQ.all[, c("CURVE_NAME", "CONTRACT_MONTH")])
  names(QQ) <- c("curve.name", "contract.dt")
  QQ$contract.dt <- as.Date(format(as.Date(QQ$contract.dt), "%Y-%m-01"))
  QQ$curve.name  <- as.character(QQ$curve.name)
  QQ <- unique(QQ)
  QQ$mkt <- sapply(strsplit(QQ$curve.name, " "), function(x){x[2]})
  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  simulate.base.curves(QQ, options)


  
  
##   ind <- which(QQ$curve.name == "COMMOD NG MIDAMCG PHYSICAL")
##   QQ$curve.name[ind] <- "COMMOD NG MIDAM CG PHYSICAL"

##   uCurves <- sort(unique(QQ$curve.name))
##   mkt   <- sapply(strsplit(QQ$curve.name, " "), function(x){x[2]})
##   uMkts <- sort(unique(mkt))

## #  for (m in 1:length(uMtks)){
##   m <- 10
##   QQ.mkt <- subset(QQ, mkt==uMkts[m])
##   QQ.mkt <- QQ.mkt[order(QQ.mkt$curve.name, QQ.mkt$contract.dt), ]

##   pricing.dts  <- seq(as.Date("2007-07-01"), by="month", length.out=60)

##   MarketData$Initial <- QQ.mkt
##   MarketData$curve.name <- as.character(sort(unique(QQ.mkt$curve.name)))
  
##   source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/make.MarketData.R")
##   MarketData <- make.MarketData(MarketData, options)

##   source("H:/user/R/RMG/Models/Price/ForwardCurve/ByMarket/calc.correlation.R")
##   CorMat <- calc.correlation(QQ.mkt, options) 

    
#  }

  
  

}
