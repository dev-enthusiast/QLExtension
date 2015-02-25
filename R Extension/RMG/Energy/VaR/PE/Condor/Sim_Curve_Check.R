source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages(c("RODBC","SecDb", "reshape", "filehash")))
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
soruce("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")

#-----------------------------------------------------------------------
Sim_Curve_Check =new.env(hash =TRUE)
##--------------------------------------------------------------------
##
Sim_Curve_Check$run<-function(){
  simCurve.asOfDate= overnightUtils.getAsOfDate()#as.Date(Sys.getenv("asOfDate"))
  #simCurve.asOfDate =NULL
  if(is.null(simCurve.asOfDate)){
     simCurve.asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
     rLog("Check curves simulated on", as.character(simCurve.asOfDate))
  }
  FP.options <- ForwardPriceOptions$create(asOfDate=simCurve.asOfDate,isInteractive=FALSE)
  curvenames <- Sim_Curve_Check$.getCurves()
  #curvenames<- c("commod fco fob indohsp physical","commod fco indobitmed physical",
  #               "commod frt pm indo-ara physical","commod frc avgtc bpi",
  #               "commod frt route4 bci","commod pwg ger peak physical")

  #get historical starting date according to allcurves.xls, 
  #so that only the history dates used in curve simulation is plotted
  allcurves <- get.curve.pedigree(curvenames, FP.options)
  for(curvename in curvenames){
      FP.options.thiscurve <- FP.options
      hist.dt <- as.Date(allcurves[which(allcurves[,1]== curvename), "HistStartDate"])
      if (!is.na(hist.dt) && !is.null(hist.dt) && length(hist.dt)>0){
            FP.options.thiscurve$start.dt.hist <- hist.dt
            FP.options.thiscurve$N.hist <- as.numeric(FP.options.thiscurve$asOfDate - FP.options.thiscurve$start.dt.hist)
            rLog("****From Sim_Curve_Check: The starting hist date for curve", curvename,
            "is changed to", as.character(hist.dt))
      }
      plot.OneCurve(curvename, FP.options.thiscurve, type="pdf")
      rLog("********From Sim_Curve_Check: finished plotting:", curvename)
  }
}

Sim_Curve_Check$.getCurves <-function(){
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/Sim_Curve_Check.xls")
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

res = try(Sim_Curve_Check$run() )

if (class(res)=="try-error"){
  cat("*********Failed checking of simulated curves.*********\n")
}
Sys.time() - .start



  
  