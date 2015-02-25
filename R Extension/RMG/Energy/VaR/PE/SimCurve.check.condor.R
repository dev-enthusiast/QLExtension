source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages("RODBC"))
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")

#-----------------------------------------------------------------------
SimCurve.check.condor =new.env(hash =TRUE)
##--------------------------------------------------------------------
##
SimCurve.check.condor$run<-function(){
  #simCurve.asOfDate= as.Date(Sys.getenv("simAsOfDate"))
  simCurve.asOfDate =NULL
  if(is.null(simCurve.asOfDate)){
     simCurve.asOfDate <- as.Date("2008-03-14")
  }
  FP.options <- ForwardPriceOptions$create(asOfDate=simCurve.asOfDate,isInteractive=FALSE)
  #setLogLevel(RLogerLevels$LOG_DEBUG)
  curvenames <- SimCurve.check.condor$.getCurves()
  for (curvename in curvenames){
      plot.OneCurve(curvename, FP.options, type="pdf")
      rLog("From SimCurve Check: finished plotting:", curvename)
  }
}

SimCurve.check.condor$.getCurves <-function(){
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/sim.curves.check.xls")
  tables <- sqlTables(con)
  data   <- sqlFetch(con, "Curves")  #"Changes" is table name in the workbook
  odbcCloseAll()
  colnames(data) <- gsub("#", ".", colnames(data))
  ncurve =  which(is.na(data$Run.Name))[1]-1 #number of curves to test (before the blank rows in the regression.tests.xls file)
  idx  <- seq( 1:ncurve )
  #run.names <- as.character(data$Run.Name[idx]) #run names from Excel, indicates backtest type
  curvenames <- toupper(as.character(data$Curve.Long[idx]))
  return(curvenames)
}

#######################################################

.start = Sys.time()

res = try(SimCurve.check.condor$run() )

if (class(res)=="try-error"){
  cat("Failed Simulated curve checking.\n")
}
Sys.time() - .start



  
  