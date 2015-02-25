source("H:/user/R/RMG/Energy/VaR/PE/PE.backtest.unit.R")

backtest.PE.condor =new.env(hash =TRUE)

#######################################################
backtest.PE.condor$run <-function(){

 #read curves from Excel and generate dataframe QQ
 QQ <- PE.backtest.unit$getRuns()

 run.name="PEbacktest.unit"
 #initiate options for backtesting
 options <- PE.backtest.unit$setOptions(run.name, start.dt=as.Date("2007-08-01"), end.dt=as.Date("2008-03-11"))
 #get historical prices. This can also be encapsulated in one function.
  hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")#from get.marks.from.R.R()
  PP <- get.prices.from.R(QQ[, c("curve.name", "contract.dt")], options, hPP.env)
 options <- PE.backtest.unit$resetOptions(PP, options)

 #simulate the curves in QQ
 PE.backtest.unit$simulate(QQ[,-1], options) #simulate price, and create FP.options to assign to PE$options$fp
 #calculate PE and make plots
 PE.backtest.unit$PE(options,QQ, PP)

}


#######################################################

.start = Sys.time()

res = try( backtest.PE.condor$run() )

if (class(res)=="try-error"){
  cat("Failed PE backtest.\n")
}

Sys.time() - .start
