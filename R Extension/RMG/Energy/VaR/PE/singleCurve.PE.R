# -------------------------------------------------------------------------------------------------
# Run price simulations, plot simulated prices, price changes and plot PE results for single curves
# This code can be used to check price simulation and price move based on MPE
#--------------------------------------------------------------------------------------------------

source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R") 
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEInteractiveUtils.R")

require(reshape)
require(RODBC)

singleCurve.PE <- new.env(hash=TRUE)

# ------------------------------------------------------------------------------
# This is to plot curve simulations and MPE by single curves only(no CP or portfolios)
# Curves to be simulated are specified in PE_Unit_Backtest.xls

singleCurve.PE$run <- function(){
    asOfDate   <- "2008-05-13"
    run.name   <- "" 
    isInteractive  <- TRUE  
    FP.options <- ForwardPriceOptions$create( asOfDate=asOfDate,
                                              isInteractive, 
                                              run.name=run.name )
    # Read curve from file 
    QQ         <- singleCurve.PE$getRuns()
    curvenames <- toupper(unique(QQ$curve.name))
    sim.all(curvenames, FP.options)
    rLog("Finished simulation.")

    # Check historical start date, plot single curve simulations
    allcurves <- get.curve.pedigree(curvenames, FP.options)
    for( curvename in curvenames )
    {
        FP.options.thiscurve <- FP.options
        hist.dt              <- as.Date(allcurves[which(allcurves[,1]== curvename), "HistStartDate"])
        if( !is.na(hist.dt) && !is.null(hist.dt) && length(hist.dt)>0 ){
            FP.options.thiscurve$start.dt.hist <- hist.dt
            FP.options.thiscurve$N.hist        <- as.numeric(FP.options.thiscurve$asOfDate - FP.options.thiscurve$start.dt.hist)
            rLog("*From DEMO: Starting Hist Date for curve", curvename, " changed to", as.character(hist.dt))
        }
        plot.OneCurve(curvename, FP.options.thiscurve, type="pdf")
        QQ.sub    <- subset(QQ, curve.name==curvename)
        res.sPE   <- PEInteractiveUtils$getPE(curvename, FP.options, QQ.sub)
        qPE       <- PEInteractiveUtils$qPE(res.sPE)
        PEInteractiveUtils$plotPE(curvename, FP.options, qPE)
        rLog("From singleCurve.PE: finished plotting:", curvename)
    }
}



# ------------------------------------------------------------------------------
singleCurve.PE$getRuns <- function(){
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/PE_Unit_Backtest.xls")
  tables <- sqlTables(con)
  data   <- sqlFetch(con, "Specification")  #"Changes" is table name in the workbook
  odbcCloseAll()

  #replace the # sign in the column name to "."
  colnames(data)     <- gsub("#", ".", colnames(data))
  ncurve             <-  which(is.na(data$Run.Name))[1]-1 #number of curves to test (before the blank rows in the regression.tests.xls file)
  idx                <- seq( 1:ncurve )
  run.names          <- as.character(data$Run.Name[idx]) #run names from Excel, indicates backtest type
  curve.names        <- toupper(as.character(data$Curve.Long[idx]))
  contract.start.dts <- as.Date(data$Start.Dt[idx])
  contract.end.dts   <- as.Date(data$End.Dt[idx])
  positions          <- as.integer(data$Quantity)
  QQ                 <- NULL
  for( i in 1:ncurve )
  {
      if(!is.na(curve.names[i]))
      {
          contract.dates <- seq(contract.start.dts[i], contract.end.dts[i], by="month")
          QQ             <- rbind(QQ,data.frame(run.name= run.names[i],curve.name = curve.names[i], contract.dt = contract.dates, position = positions[i], stringsAsFactors=FALSE))
      }else{
          rWarn(paste("the job: ", run.names[i], " has no symmbol in the Excel workbook", sep="") )
      }
  }
  rownames(QQ) <- c(1:nrow(QQ))
  return(QQ)
}
  
# ------------------------------------------------------------------------------
res = try( singleCurve.PE$run() )
if( class(res)=="try-error" ) 
{
    cat("Failed PE backtest.\n")
}