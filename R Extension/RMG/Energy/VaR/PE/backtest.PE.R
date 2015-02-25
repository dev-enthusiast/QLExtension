# Tool to keep track of PE performance. 
#
#
# First version by Adrian Dragulescu, 24-Oct-2007
.demo.backcast.PE <- function(){
  rm(list=ls())
  start.dt <- as.Date("2007-01-01")           # start of backtest
  run.name <- "backtest_PWJ_2008"              # no spaces please! 
  QQ <- data.frame(
#    curve.name  = "COMMOD FCO API2 INDEX",
    curve.name  = "COMMOD PWJ 5X16 WEST PHYSICAL",               
    contract.dt = seq(as.Date("2008-01-01"), by="month", length.out=12),
    position = 10000, stringsAsFactors=FALSE)

  options <- NULL
  options$asOfDate <- as.Date("2007-11-07")   # end of backtest
  
  source("H:/user/R/RMG/Energy/VaR/PE/backtest.PE.R")
  backtest.PE(start.dt, run.name, QQ, options)
}
          

backtest.PE <- function(start.dt, run.name, QQ, options=NULL){
  #browser()
  require(SecDb); require(filehash)
  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")

  if (length(options$asOfDate)==0) # end of backtest
     options$asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  options$calc$hdays <- seq(start.dt, options$asOfDate, by="day")
  PP <- get.prices.from.R(QQ[, c("curve.name", "contract.dt")], options)
  options$calc$hdays <- as.Date(names(PP)[grep("-", names(PP))])
  start.dt <- options$calc$hdays[1]
  # simulate prices for the curves in the deal.
  #
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
  FP.options <- ForwardPriceOptions$create(asOfDate=start.dt,
    isInteractive=TRUE, run.name=run.name)
  sim.all(unique(QQ$curve.name), FP.options)

  # calculate simulated exposures ... 
  #
  source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
  PE$options$asOfDate <- start.dt
  PE$BASE_SIM_DIR   <- "//nas-msw-07/rmgapp/SimPrices/Network/"
  PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
    "interactive/", run.name, "/result/", sep="")
  PE$options$fp <- FP.options
  QQ <- cbind(QQ, F0=PP[,as.character(start.dt)])
  res.sPE <- PECalculator$.aggregateSimulations(QQ)
  # PE by quantiles
  #
  qPE.direct <- PECalculator$.summarizeData(res.sPE, mode="direct")  
  qPE.indirect <- -PECalculator$.summarizeData(res.sPE, mode="indirect")
  # calculate historical exposures ... 
  #
  res.hPE <- PECalculator$.peHistCalculation(QQ, PP)
  res.hPE$Hist.Exposure <- res.hPE$Hist.Exposure/1000000

  # plot the results ... 
  #
  rangePlot <- range(as.numeric(qPE.direct),  as.numeric(qPE.indirect),
                     as.numeric(res.hPE$Hist.Exposure))
  pricing.dts <- as.Date(colnames(qPE.direct))
  plot(pricing.dts, y=NULL, xlim=range(pricing.dts), 
    ylim=rangePlot, type="l", main=run.name, 
    ylab="Potential Exposure, M$")
  colorList = c("black", "blue", "red", "black", "blue", "red")
  for( index in 1:nrow(qPE.direct)){
    lines(pricing.dts, qPE.direct[index,], col=colorList[index])
    points(pricing.dts, qPE.direct[index,], pch=index, col=colorList[index])
    lines(pricing.dts, qPE.indirect[index,], col=colorList[index])
    points(pricing.dts, qPE.indirect[index,], pch=index, col=colorList[index])
  }
  lines(res.hPE$pricing.dt, res.hPE$Hist.Exposure, col="gray", lwd=2)

  legend( "topright", legend=c(rownames(qPE.direct), "hist"), 
          col=c(colorList[1:nrow(qPE.direct)], "gray"), 
          text.col=c(colorList[1:nrow(qPE.direct)], "gray"),
          pch=c(1:nrow(qPE.direct),-1) )

  
}

