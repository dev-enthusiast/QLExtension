  source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
  try(load.packages( c("SecDb", "RODBC", "reshape", "filehash")))
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
  source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
  source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
  #source("H:/user/R/RMG/Energy/VaR/PE/backtest.PE.R")
  #source("H:/user/R/RMG/Energy/VaR/PE/backtest.PE_regress.R")

PE.backtest.unit = new.env(hash=TRUE)
#################################################################################
#
#
PE.backtest.unit$setOptions <- function(run.name, start.dt=NULL, end.dt=NULL) {
  options <- NULL
  if(is.null(end.dt)) end.dt<-Sys.Date()
  options$run.name <- run.name
  options$asOfDate <-  end.dt  #backtest ending date

  if(is.null(start.dt)) start.dt <- as.Date("2007-04-01")
  options$backtest.start.dt <- start.dt  # backtest starting date
  options$calc$hdays <- seq(options$backtest.start.dt, options$asOfDate, by="day")
  return(options)
 }

#################################################################################
# Get run jobs from the Excel file PE.unit.tests.xls
#
PE.backtest.unit$getRuns <- function(){
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/PE.backetst.unit.xls")
  tables <- sqlTables(con)
  data   <- sqlFetch(con, "Specification")  #"Changes" is table name in the workbook
  odbcCloseAll()

  #replace the # sign in the column name to "."
  colnames(data) <- gsub("#", ".", colnames(data))
  ncurve =  which(is.na(data$Run.Name))[1]-1 #number of curves to test (before the blank rows in the regression.tests.xls file)
  idx  <- seq( 1:ncurve )
  run.names <- as.character(data$Run.Name[idx]) #run names from Excel, indicates backtest type
  curve.names <- toupper(as.character(data$Curve.Long[idx]))
  contract.start.dts <- as.Date(data$Start.Dt[idx])
  contract.end.dts   <- as.Date(data$End.Dt[idx])
  positions <- as.integer(data$Quantity)
  QQ <- NULL
  for (i in 1:ncurve){
    if(!is.na(curve.names[i])){
      contract.dates <-  seq(contract.start.dts[i], contract.end.dts[i], by="month")
      QQ <- rbind(QQ,data.frame(run.name= run.names[i],curve.name = curve.names[i], contract.dt = contract.dates, position = positions[i], stringsAsFactors=FALSE))
      }
    else {
      rWarn(paste("the job: ", run.names[i], " has no symmbol in the Excel workbook", sep="") )
      }
  }#end of curve loops
  rownames(QQ)=c(1:nrow(QQ))
  return(QQ)
}
  

##
#########################################################################
# reset the first historical (i.e.staring date) date of backtesting
PE.backtest.unit$resetOptions <- function(PP, options){
  options$calc$hdays <- as.Date(names(PP)[grep("-", names(PP))])
  options$backtest.start.dt <- options$calc$hdays[1]
  return(options)
}

############################################################################################
#
PE.backtest.unit$simulate <- function(QQ, options=NULL){
  #start date re-adjusted, used as folder name in "R:\SimPrices\Network\"
  # simulate prices for the curves in the deal.

  #set simulation options
  FP.options <- ForwardPriceOptions$create(asOfDate = options$backtest.start.dt,
    isInteractive = FALSE, run.name = options$run.name)

  # assign pre-defined backtest contract dates (by month), only used in backtesting case
   FP.options$backtest.contract.dts = seq(min(QQ$contract.dt),max(QQ$contract.dt), by="month")
  # extend forecast month to the max month of the backtested months (# of simulated month may be > 48 months)
  corrected.contract.dts = seq(FP.options$contract.dts[1], max(QQ$contract.dt), by = "month")
  if ((mon<-length(corrected.contract.dts)) > FP.options$D){
    FP.options$D = mon
    FP.options$contract.dts = corrected.contract.dts
  }
  PE$options$fp <- FP.options
  rLog("From PE backtest:Simulating selected curves ...")
  sim.all(unique(QQ$curve.name), FP.options)
  rLog("From PE backtest:Curve simulations done!")

}

#############################################################################################
## will calculate PE and plot PE results curve by curve
## options specifies PE options. QQ is the position data frame. PP is historical prices
 
 PE.backtest.unit$PE <-function(options, QQ, PP){
  #PE is an object already loaded
  PE$options$asOfDate <- options$backtest.start.dt
  PE$BASE_SIM_DIR   <- "//nas-msw-07/rmgapp/SimPrices/Network/"  #or put in "Potential Exposure"?
  PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
                              "overnight/", options$run.name, "/result/", sep="")

  run.names=unique(QQ$run.name)
  #Note: this test can be done for a group of curves under the same run.name
  #To run each individual curve, just use a different name for each curve.
  
  for(i in 1:length(run.names)){
    QQ.sub <- subset(QQ, run.name==run.names[i])
    curve.names=unique(QQ.sub$curve.name)
    PP.sub= subset(PP, curve.name==curve.names)
    PP.sub.F0 <- subset(PP.sub, select = c("curve.name","contract.dt", as.character(options$backtest.start.dt)))
    rownames(PP.sub.F0)=c(1:nrow(PP.sub.F0))
    positionList <- merge(QQ.sub[,-1], PP.sub.F0)
    colnames(positionList)[4]="F0"
    #NOTE: need to add handler for NA F0 values later. merge will automaticly remove rows with NA F0 values
    res.sPE <- PECalculator$.aggregateSimulations(positionList)
    
    # PE by quantiles
    qPE=NULL
    qPE$direct <- PECalculator$.summarizeData(res.sPE, mode="direct")
    qPE$indirect <- -PECalculator$.summarizeData(res.sPE, mode="indirect")

    # calculate historical exposures ...
    #
    res.hPE <- PECalculator$.peHistCalculation(positionList, PP.sub)
    res.hPE$Hist.Exposure <- res.hPE$Hist.Exposure/1000000
    rLog("From PE backtest: PE calculated for both simuation and historical")


    # plot simulation result for each curve
    #reset the run.name in options to put each curve in an individual folder
    if(is.null(PE$options$fp)){
        rLog("From PE backtest: PE options not set yet! Check code and re-run")
        return()
    }else
    {   #append run.name to report directory to make backtest report folders
        PE$options$fp$backtest.report.dir <-  paste(PE$options$fp$report.dir, run.names[i],"/", sep="")
    }
    
    for (k in 1:length(curve.names)){
        res <- try(plot.OneCurve.backtest(curve.names[k], PE$options$fp, "pdf"))
        if(class(res)=="try-error") rLog("From PE backtest: failed plotting of",curve.names[k])
    }

    #plot PE of the run.name    
    res<-try(PE.backtest.unit$plotPE(run.names[i], res.hPE, qPE))
    if(class(res)=="try-error") {
        rLog("From PE backtest: PE CANNOT be plotted for run name:", run.names[i])
    }else{
        rLog("From PE backtest: PE plotted for run name:", run.names[i])
    }
         
 }#end of loops through all run.names 
}

#########################################################################################
#MPE plot and report of statistics

PE.backtest.unit$plotPE <- function(runname, res.hPE, qPE) {

    if (file.access(PE$options$fp$backtest.report.dir)!=0){mkdir(PE$options$fp$backtest.report.dir)}
    filename = paste(PE$options$fp$backtest.report.dir, ".MPE backtest results.pdf", sep = "")
    pdf(filename, width=5.5, height =8, pointsize =8)

    # plot the PE results ...
    rangePlot <- range(as.numeric(qPE$direct),  as.numeric(qPE$indirect),
                     as.numeric(res.hPE$Hist.Exposure))
    pricing.dts <- as.Date(colnames(qPE$direct))
    plot(pricing.dts, y=NULL, xlim=range(pricing.dts),
     ylim=rangePlot, type="l", main=runname,
     ylab="Potential Exposure, M$")
    colorList = c("black", "blue", "red", "black", "blue", "red")
    for( index in 1:nrow(qPE$direct)){
      lines(pricing.dts, qPE$direct[index,], col=colorList[index])
      points(pricing.dts, qPE$direct[index,], pch=index, col=colorList[index])
      lines(pricing.dts, qPE$indirect[index,], col=colorList[index])
      points(pricing.dts, qPE$indirect[index,], pch=index, col=colorList[index])
    }
    lines(res.hPE$pricing.dt, res.hPE$Hist.Exposure, col="gray", lwd=2)

    legend( "topright", legend=c(rownames(qPE$direct), "hist"),
          col=c(colorList[1:nrow(qPE$direct)], "gray"),
          text.col=c(colorList[1:nrow(qPE$direct)], "gray"),
          pch=c(1:nrow(qPE$direct),-1) )
    graphics.off()
}

