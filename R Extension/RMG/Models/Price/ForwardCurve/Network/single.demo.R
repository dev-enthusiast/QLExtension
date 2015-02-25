# ------------------------------------------------------------------------------
# Small code snippets that shows you how to run the simulations.

rm(list=ls())
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R") 
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
require(reshape)
require(RODBC)

single.demo <- new.env(hash=TRUE)

# ------------------------------------------------------------------------------
# This is to plot curve simulations and MPE by single curves only(no portfolios)
single.demo$run <- function()
{
    asOfDate   <- "2008-04-24"
    run.name   <- "Test"       
    FP.options <- ForwardPriceOptions$create( asOfDate=asOfDate,
                                              isInteractive=FALSE, 
                                              run.name=run.name )
    # Read curve from file 
    QQ         <- single.demo$getRuns()
    curvenames <- toupper(unique(QQ$curve.name))
    sim.all(curvenames, FP.options)
    rLog("*From DEMO: Finished simulation.")

    # Check historical start date, plot single curve simulations
    allcurves <- get.curve.pedigree(curvenames, FP.options)
    for( curvename in curvenames )
    {
        FP.options.thiscurve <- FP.options
        hist.dt              <- as.Date(allcurves[which(allcurves[,1]== curvename), "HistStartDate"])
        if( !is.na(hist.dt) && !is.null(hist.dt) && length(hist.dt)>0 )
        {
            FP.options.thiscurve$start.dt.hist <- hist.dt
            FP.options.thiscurve$N.hist        <- as.numeric(FP.options.thiscurve$asOfDate - FP.options.thiscurve$start.dt.hist)
            rLog("*From DEMO: Starting Hist Date for curve", curvename, " changed to", as.character(hist.dt))
        }
        plot.OneCurve(curvename, FP.options.thiscurve, type="pdf")
        rLog("*From DEMO: finished plotting:", curvename)
    }

    # Make MPE plots
    loptions            <- NULL
    loptions$asOfDate   <- FP.options$asOfDate
    loptions$calc$hdays <- seq( FP.options$asOfDate-FP.options$N.hist, FP.options$asOfDate, by="day")
    PE$options$asOfDate <- as.Date(FP.options$asOfDate)
    PE$BASE_SIM_DIR     <- "//nas-msw-07/rmgapp/SimPrices/Network/"  #or put in "Potential Exposure"?
    PE$options$fp       <- FP.options
    if( !FP.options$isInteractive )
    {
        PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
                                    "overnight/", run.name, "/result/", sep="" )
    }else{
        PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
                                    "interactive/", run.name, "/result/", sep="" )
    }
    hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")#from get.marks.from.R.R()  
    PP      <- get.prices.from.R(QQ, loptions, hPP.env)
    for( curvename in curvenames )
    {
        rLog(curvename)
        QQ.sub    <- subset(QQ, curve.name==curvename)
        PP.sub    <- NULL
        PP.sub    <- subset(PP, curve.name ==curvename)
        PP.sub.F0 <- subset(PP.sub, select = c("curve.name","contract.dt", as.character(asOfDate)))
        rownames(PP.sub.F0) <- c(1:nrow(PP.sub.F0))
        positionList        <- merge(QQ.sub[,-1], PP.sub.F0)
        Idx                 <- which( positionList$contract.dt<=PE$options$asOfDate
                                     |is.na(positionList[[dim(positionList)[2]]]) )
        if( length(Idx)>0 ) 
        {
            positionList <- positionList[-Idx,]
        }
        colnames(positionList)[4] <- "F0"
        
        PE$options$asOfDate <- as.Date(asOfDate)
        res.sPE             <- PECalculator$.aggregateSimulations(positionList)
        # PE by quantiles
        qPE                 <- NULL
        qPE$direct          <- PECalculator$.summarizeData(res.sPE, mode="direct")*1E+6
        qPE$indirect        <- -PECalculator$.summarizeData(res.sPE, mode="indirect")*1E+6
        single.demo$plotPE(curvename, res.sPE,qPE)
    }
}

# ------------------------------------------------------------------------------
single.demo$plotPE <- function(curvename, res.sPE, qPE) 
{
    filename <- paste(PE$options$fp$report.dir, curvename, ".MPE.pdf", sep = "")
    pdf(filename, width=5.5, height =8, pointsize =8)

    # plot the PE results ...
    rangePlot   <- range(as.numeric(qPE$direct),  as.numeric(qPE$indirect))
    pricing.dts <- as.Date(colnames(qPE$direct))
    
    xlabel <- paste("Pricing date is ", as.character(PE$options$asOfDate),sep="")
    plot( pricing.dts, y=NULL, xlim=range(pricing.dts),
          ylim=rangePlot, type="l", main=curvename,
          ylab="Potential Exposure, M$", xlab=xlabel )
    colorList <- c("black", "blue", "red", "black", "blue", "red")
    for( index in 1:nrow(qPE$direct) )
    {
        lines(pricing.dts, qPE$direct[index,], col=colorList[index])
        points(pricing.dts, qPE$direct[index,], pch=index, col=colorList[index])
        lines(pricing.dts, qPE$indirect[index,], col=colorList[index])
        points(pricing.dts, qPE$indirect[index,], pch=index, col=colorList[index])
    }
    legend( "topright", legend=c(rownames(qPE$direct)),
            col=c(colorList[1:nrow(qPE$direct)]),
            text.col=c(colorList[1:nrow(qPE$direct)]),
            pch=c(1:nrow(qPE$direct)) )
    graphics.off()
}

# ------------------------------------------------------------------------------
single.demo$getRuns <- function()
{
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
      }else 
      {
          rWarn(paste("the job: ", run.names[i], " has no symmbol in the Excel workbook", sep="") )
      }
  }
  rownames(QQ) <- c(1:nrow(QQ))
  return(QQ)
}
  
# ------------------------------------------------------------------------------
res = try( single.demo$run() )
if( class(res)=="try-error" )
{
    cat("Failed PE backtest.\n")
}