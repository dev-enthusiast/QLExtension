# Tool to keep track of PE performance. 
#
#
# First version by Adrian Dragulescu, 24-Oct-2007
.demo.backcast.PE <- function(){
  rm(list=ls())
  start.dt <- as.Date("2007-01-01")           # start of backtest
  run.name <- "backtest_PWY_2008"              # no spaces please! 
  QQ <- data.frame(

    curve.name  = "COMMOD PWY 5X16 PHYSICAL",               
    contract.dt = seq(as.Date("2008-01-01"), by="month", length.out=12),
    position = 10000, stringsAsFactors=FALSE)

  options <- NULL
  options$asOfDate <- as.Date("2007-10-26")   # end of backtest
  
  source("H:/user/R/RMG/Energy/VaR/PE/backtest.PE_regress.R")
  backtest.PE(start.dt, run.name, QQ, options)
  
}

#################################################################################
# To backtest PE on curves saved in file: regression.tests.xls
#
.backtest.regression <- function() {
  rm(list=ls())
  require(RODBC)
  require(reshape)
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/regression.tests.xls")
  tables <- sqlTables(con) 
  data   <- sqlFetch(con, "Specification")  #"Changes" is table name in the workbook
  odbcCloseAll()
  
  #replace the # sign in the column name to "."
  colnames(data) <- gsub("#", ".", colnames(data))
     
  options <- NULL
  options$asOfDate <- as.Date("2007-10-26")   # backtest ending date
  backtest.start.dt <- as.Date("2007-01-01")  # backtest starting date
  source("H:/user/R/RMG/Energy/VaR/PE/backtest.PE_regress.R")  
  
  ncurve =  which(is.na(data$Run.Name))[1]-1 #number of curves to test (before the blank rows in the regression.tests.xls file)                              
  idx  <- seq( 1:ncurve ) 
  run.names <- as.character(data$Run.Name[idx]) #run names from Excel, indicates backtest type  
  curve.names <- toupper(as.character(data$Curve.Long[idx]))
  contract.start.dts <- as.Date(data$Start.Dt[idx])
  contract.end.dts   <- as.Date(data$End.Dt[idx])
  positions <- as.integer(data$Quantity)
  
  for (i in 1:ncurve){
    if(!is.na(curve.names[i])){
      contract.dates <-  seq(contract.start.dts[i], contract.end.dts[i], by="month")  
      QQ <- data.frame(curve.name = curve.names[i], contract.dt = contract.dates, position = positions[i], stringsAsFactors=FALSE)
      backtest.PE(backtest.start.dt, run.names[i], QQ, options)
    }
    else {
    rWarn(paste("the curve: ", run.names[i], " has no symmbol in the Excel workbook", sep="") )
    }
  } #end of curve loops 
}   
##
##
########################################################################
#
#
backtest.PE <- function(backtest.start.dt, run.name, QQ, options=NULL){
  #
  require(SecDb); require(filehash)
  source("H:/user/R/RMG/Utilities/Database/R/get.marks.from.R.R")

  if (length(options$asOfDate)==0) # end of backtest
     options$asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  options$calc$hdays <- seq(backtest.start.dt, options$asOfDate, by="day")
  PP <- get.prices.from.R(QQ[, c("curve.name", "contract.dt")], options) #PP as [T;curve.name, contract.dt, t's]
  options$calc$hdays <- as.Date(names(PP)[grep("-", names(PP))])
  backtest.start.dt <- options$calc$hdays[1] #start date re-adjusted, used as folder name in "R:\SimPrices\Network\"
  # simulate prices for the curves in the deal.
  #
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
  FP.options <- ForwardPriceOptions$create(asOfDate = backtest.start.dt,                   
    isInteractive = TRUE, run.name = run.name)
    
  # assign pre-determined backtest contract dates (by month), only used in backtesting case
  FP.options$backtest.contract.dts = QQ$contract.dt   
  
  # set number of simulations >= 1000 times
  FP.options$nsim=ifelse(FP.options$nsim >=1000, FP.options$nism, 1000)
  
  # extrend forecast month to the last month of the backtested months (# of simulated month may be > 48 months)
  corrected.contract.dts = seq(FP.options$contract.dts[1], QQ$contract.dt[length(QQ$contract.dt)], by = "month")
  if ((mon<-length(corrected.contract.dts)) > FP.options$D){
  FP.options$D = mon
  FP.options$contract.dts = corrected.contract.dts
  }
  
  # calculate simulated exposures ...                     
  sim.all(unique(QQ$curve.name), FP.options)                                                                                                     
  
  source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
  PE$options$asOfDate <- backtest.start.dt
  PE$BASE_SIM_DIR   <- "//nas-msw-07/rmgapp/SimPrices/Network/"
  PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
    "interactive/", run.name, "/result/", sep="")
  PE$options$fp <- FP.options
  QQ <- cbind(QQ, F0=PP[,as.character(backtest.start.dt)])
  res.sPE <- PECalculator$.aggregateSimulations(QQ)
  # PE by quantiles
  #
  qPE.direct <- PECalculator$.summarizeData(res.sPE, mode="direct")  
  qPE.indirect <- -PECalculator$.summarizeData(res.sPE, mode="indirect")
  # calculate historical exposures ... 
  #
  res.hPE <- PECalculator$.peHistCalculation(QQ, PP)
  res.hPE$Hist.Exposure <- res.hPE$Hist.Exposure/1000000
  
  # plot simulation result 
    source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
    plot.OneCurve.backtest(unique(QQ$curve.name), FP.options, "pdf")
  #MPE plot and report of statistics
    if (file.access(FP.options$report.dir)!=0){mkdir(FP.options$report.dir)}
     filename = paste(FP.options$report.dir, run.name, ".MPE backtest results.pdf", sep = "") 
    pdf(filename, width=5.5, height =8, pointsize =8)
      
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
    graphics.off()
                                               
} 


