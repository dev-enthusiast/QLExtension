source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
try(load.packages( c("SecDb", "RODBC", "reshape", "filehash")))
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/plots.sim.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")


PE_Unit_Backtest =new.env(hash =TRUE)

#######################################################
PE_Unit_Backtest$run <-function(){

 ##Load curves from PE_Unit_Backtest.xls and generate dataframe QQ
 QQ <- PE_Unit_Backtest$.getRuns()
 isInteractive = TRUE
 run.name="PEbacktest.unit_pricecap_3theta3gamma"#This is an overall run name to declare backtest. Run.names in xls are in subfolders.
                                    #set it to "", and set isInteractive=FALSE to use already simulated curves under /overnight

 ##Set up backtest starting date (must be some date in history)
 ##backtestAsOfDate= as.Date("2008-02-01")
 backtestAsOfDate= as.Date(overnightUtils.getAsOfDate())
 if(is.null(backtestAsOfDate)){
    backtestAsOfDate=as.Date("2007-10-01")
 }
 rLog("Backtest start from:", as.character(backtestAsOfDate))
 end.dt <-as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
 rLog("Backtest ends on: ",as.character(end.dt))
 
 if(end.dt<=backtestAsOfDate){
    rLog("Invalid backtesting dates!") 
    backtestAsOfDate <-as.Date(secdb.dateRuleApply(Sys.Date(), "-150b"))
    rLog("Reset backtest starting date to:", as.character(backtestAsOfDate))
 }
 
 ##Initiate options for backtesting
 options <- PE_Unit_Backtest$.setOptions(run.name, start.dt=as.Date(backtestAsOfDate), end.dt=as.Date(end.dt),
                                         isInteractive=isInteractive)

 ##Get historical prices. This can also be encapsulated in a sub-function.
  hPP.env <- .getFileHashEnv("//nas-msw-07/rmgapp/Prices/Historical")#from get.marks.from.R.R()
  PP <- get.prices.from.R(QQ[, c("curve.name", "contract.dt")], options, hPP.env)
  options <- PE_Unit_Backtest$.resetOptions(PP, options)

 ##Set FP.options and give it to PE$options$fp
 PE_Unit_Backtest$.setFPoptions(QQ[,-1], options)
 
 ##Simulate the curves in QQ
 PE_Unit_Backtest$.simulate(QQ[,-1], PE$options$fp) #simulate price
 
 ##Calculate PE and make plots
 PE_Unit_Backtest$.PE(options,QQ, PP)

}



#################################################################################
# set options
#
PE_Unit_Backtest$.setOptions <- function(run.name, start.dt=NULL, end.dt=NULL, isInteractive=TRUE) {
  options <- NULL
  if(is.null(end.dt)) end.dt<-Sys.Date()
  options$run.name <- run.name
  options$asOfDate <-  end.dt  #backtest ending date

  if(is.null(start.dt)) start.dt <- as.Date("2007-04-01")
  options$backtest.start.dt <- start.dt  # backtest starting date
  options$calc$hdays <- seq(options$backtest.start.dt, options$asOfDate, by="day")
  options$isInteractive <- isInteractive
  return(options)
 }

#################################################################################
# Get run jobs from the Excel file PE.unit.tests.xls
#
PE_Unit_Backtest$.getRuns <- function(){
  con    <- odbcConnectExcel("H:/user/R/RMG/Energy/VaR/PE/PE_Unit_Backtest.xls")
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
PE_Unit_Backtest$.resetOptions <- function(PP, options){
  options$calc$hdays <- as.Date(names(PP)[grep("-", names(PP))])
  options$backtest.start.dt <- options$calc$hdays[1]
  return(options)
}

##########################################################################\
#set simulation options
PE_Unit_Backtest$.setFPoptions<-function(QQ, options=NULL){
  FP.options <- ForwardPriceOptions$create(asOfDate = options$backtest.start.dt,
    isInteractive = options$isInteractive, run.name = options$run.name)

  # assign pre-defined backtest contract dates (by month), only used in backtesting case
   FP.options$backtest.contract.dts = seq(min(QQ$contract.dt),max(QQ$contract.dt), by="month")
  # extend forecast month to the max month of the backtested months (# of simulated month may be > 48 months)
  corrected.contract.dts = seq(FP.options$contract.dts[1], max(QQ$contract.dt), by = "month")
  if ((mon<-length(corrected.contract.dts)) > FP.options$D){
    FP.options$D = mon
    FP.options$contract.dts = corrected.contract.dts
  }
  PE$options$fp <- FP.options
}

############################################################################################
#
PE_Unit_Backtest$.simulate <- function(QQ, FP.options=NULL){
  #start date re-adjusted, used as folder name in "R:\SimPrices\Network\"
  # simulate prices for the curves in the deal.
  rLog("************From PE backtest:Simulating selected curves ...************")
  sim.all(unique(QQ$curve.name), FP.options)
  rLog("************From PE backtest:Curve simulations done!**********")

}

#############################################################################################
## will calculate PE and plot PE results curve by curve
## options specifies PE options. QQ is the position data frame. PP is historical prices
 
 PE_Unit_Backtest$.PE <-function(options, QQ, PP){
  #PE is an object already loaded
  PE$options$asOfDate <- options$backtest.start.dt
  PE$BASE_SIM_DIR   <- "//nas-msw-07/rmgapp/SimPrices/Network/"  #or put in "Potential Exposure"?
  if(!options$isInteractive){
      PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
                              "overnight/", options$run.name, "/result/", sep="")
  }else{
      PE$CURVE_DATA_DIR <- paste( PE$BASE_SIM_DIR, PE$options$asOfDate, "/",
                              "interactive/", options$run.name, "/result/", sep="")
  }

  run.names=unique(QQ$run.name)
  #Note: this test can be done for a group of curves under the same run.name
  #To run each individual curve, just use a different name for each curve.
  
  for(i in 1:length(run.names)){
    QQ.sub <- subset(QQ, run.name==run.names[i])
    curve.names=unique(QQ.sub$curve.name)
    PP.sub=NULL
    for(j in 1:length(curve.names)){
      PP.sub= rbind(PP.sub, subset(PP, curve.name ==curve.names[j]))
    }
    PP.sub.F0 <- subset(PP.sub, select = c("curve.name","contract.dt", as.character(options$backtest.start.dt)))
    rownames(PP.sub.F0)=c(1:nrow(PP.sub.F0))
    positionList <- merge(QQ.sub[,-1], PP.sub.F0)
    colnames(positionList)[4]="F0"
    #NOTE: need to add handler for NA F0 values later. merge will automaticly remove rows with NA F0 values
    res.sPE <- PECalculator$.aggregateSimulations(positionList)
    
    ### PE by quantiles
    qPE=NULL
    qPE$direct <- PECalculator$.summarizeData(res.sPE, mode="direct")
    qPE$indirect <- -PECalculator$.summarizeData(res.sPE, mode="indirect")

    ### calculate historical exposures ...
    #
    res.hPE <- PECalculator$.peHistCalculation(positionList, PP.sub)
    res.hPE$Hist.Exposure <- res.hPE$Hist.Exposure/1000000
    rLog("*********From PE backtest: PE calculated for both simuation and historical**********")


    ### plot simulation result for each curve
    # reset the run.name in options to put each curve in an individual folder
    if(is.null(PE$options$fp)){
        rLog("***********From PE backtest: PE options not set yet! Check code and re-run***********")
        return()
    }else
    {   #append run.name to report directory to make backtest report folders
        PE$options$fp$backtest.report.dir <-  paste(PE$options$fp$report.dir, run.names[i],"_backtest/", sep="")
    }
    #Get historical starting date from allcurves.xls
    allcurves <- get.curve.pedigree(curve.names, PE$options$fp)
    for (k in 1:length(curve.names)){
        PE$options$fp.thiscurve <- PE$options$fp
        hist.dt <- as.Date(allcurves[which(allcurves[,1]== curve.names[k]), "HistStartDate"])
        if (!is.na(hist.dt) && !is.null(hist.dt) && length(hist.dt)>0){
            PE$options$fp.thiscurve$start.dt.hist <- hist.dt
            PE$options$fp.thiscurve$N.hist <- as.numeric(PE$options$fp.thiscurve$asOfDate - PE$options$fp.thiscurve$start.dt.hist)
            rLog("****From PE backtest: The starting hist date for curve", curve.names[k],
            "is changed to", as.character(hist.dt))
        }
        res <- try(plot.OneCurve.backtest(curve.names[k], PE$options$fp.thiscurve, "pdf"))
        if(class(res)=="try-error") rLog("*********From PE backtest: failed plotting of",curve.names[k],"********")
    }

    ### plot PE of the run.name    
    res<-try(PE_Unit_Backtest$.plotPE(run.names[i], res.hPE, qPE))
    if(class(res)=="try-error") {
        rLog("*********From PE backtest: PE CANNOT be plotted for run name:", run.names[i],"***********")
    }else{
        rLog("*********From PE backtest: PE plotted for run name:", run.names[i],"**********")
    }
    
    ### plot max data:
    max.direct <- PE_Unit_Backtest$.findMaxData(res.sPE, positionList, mode="direct")
    rLog("Direct PE have max on:", unique(as.character(max.direct$pricing.dt)))
    max.indirect <- PE_Unit_Backtest$.findMaxData(res.sPE, positionList, mode="indirect")
    rLog("Indirect PE have max on:", unique(as.character(max.indirect$pricing.dt)))
    plotMax(max.direct,positionList,run.names[i], mode="direct")
    plotMax(max.indirect,positionList, run.names[i], mode="indirect")     
         
 }#end of loops through all run.names 
 
}

#########################################################################################
#MPE plot and report of statistics

PE_Unit_Backtest$.plotPE <- function(runname, res.hPE, qPE) {

    if (file.access(PE$options$fp$backtest.report.dir)!=0){mkdir(PE$options$fp$backtest.report.dir)}
    filename = paste(PE$options$fp$backtest.report.dir, ".MPE backtest results.pdf", sep = "")
    pdf(filename, width=5.5, height =8, pointsize =8)

    # plot the PE results ...
    rangePlot <- range(as.numeric(qPE$direct),  as.numeric(qPE$indirect),
                     as.numeric(res.hPE$Hist.Exposure))
    pricing.dts <- as.Date(colnames(qPE$direct))
    
    xlabel=paste("Pricing date. Backtest start on ", as.character(PE$options$asOfDate),sep="")
    plot(pricing.dts, y=NULL, xlim=range(pricing.dts),
     ylim=rangePlot, type="l", main=runname,
     ylab="Potential Exposure, M$", xlab=xlabel)
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

#######################################################
## Find the max of the nominated quantiles, and find the pricing date and simulation # of
## these max values. Pull out the simulated prices for the curves in the position file  
## on these dates and simulations

## Call this function after obtaining dataToPlot (in PE_Unit_Backtest$.PE, it's res.sPE for each run.name
PE_Unit_Backtest$.findMaxData <-function(dataToPlot, positionList,mode){

  ind <- grep("sim", names(dataToPlot))
  prctil <- c(0.50, 0.95, 0.99)
  if( mode=="direct" )
  {
    dataToPlot[,ind] = apply(dataToPlot[,ind], c(1,2), max, 0)
  } else if( mode=="indirect" )
  {
    dataToPlot[,ind] = apply(dataToPlot[,ind]*-1, c(1,2), max, 0)
  } else
  {
    rWarn( "Incorrect mode setting.  Defaulting to 'direct'." )
    mode="direct"
    dataToPlot[,ind] = apply(dataToPlot[,ind], c(1,2), max, 0)
  }
  
  all.quantiles <- apply(dataToPlot[,ind], 1, quantile, prctil)#quantiles with pricing date
  max.quantiles <- apply(all.quantiles, 1, max)
  #find the pricing dates that gives max for each quantile
  dt.ind <- apply(all.quantiles, 1, which.max)
  pricing.dt = as.character(dataToPlot$pricing.dt)[dt.ind]
  max.PEdata <- dataToPlot[dt.ind,-1]
  
  #find the simulations that is closeset to the max of each quantile
  #Note: function quantile() returns estimates of quantiles of given data, not order statistics.
  #So the quantile data may not be found in the simulation results. Find the closest data instead.
  sPP.env <<- .getFileHashEnv(PE$CURVE_DATA_DIR)
  i=as.matrix(c(1:length(prctil)), nrow=length(prctil))
  sim.ind <- apply(i,1, function(x)which.min(abs(max.PEdata[x,]- max.quantiles[x])))
  price.res <-data.frame(prctil=prctil, pricing.dt=pricing.dt,
                          sim.number = sim.ind, max.PE = max.quantiles/1E+6)
  curve.names<-tolower(unique(positionList$curve.name))
  curve.names<-curve.names[which(curve.names %in% ls(sPP.env))]
  
  icurve=curve.names[1]
  idx<-which(as.character(positionList$contract.dt) %in% colnames(sPP.env[[icurve]]))
  #colnames(sPP.env$"commod fco api4 weekly")
  if(length(idx)==0) {
      rWarn("*********From PE backtest: The contract dates in the position file are not simulated!********")
      return()
      }                                                          
  contract.dt=unique(positionList[idx,]$contract.dt)
  contract.dt = contract.dt[order(contract.dt)]
  
  maxprices <- NULL
  for (j in  c(1:length(prctil))){
    prices=NULL
    for(i in c(1:length(curve.names))) { #take out the simulated prices on the given pricing dates and sim #
            curve.prices <- sPP.env[[curve.names[i]]][dt.ind[j], as.character(contract.dt), sim.ind[j]]
            prices<- rbind(prices,curve.prices)
    }
    prices=data.frame(curve.name=curve.names, prices)
    maxprices=rbind(maxprices, data.frame(t(replicate(nrow(prices),price.res[j,])),prices))
  }
  colnames(maxprices)[-c(1:5)]=as.character(contract.dt)    
  return(maxprices)
   
}

#################################################################################
## Plot the simulated results on the Max PE pricing date and simulation
## And compare with historical if possible
plotMax <- function(maxData, positionList, runname, mode){
    library(lattice)
    if(is.null(mode)) mode="direct"
                    
    #open output file
    if (file.access(PE$options$fp$backtest.report.dir)!=0){mkdir(PE$options$fp$backtest.report.dir)}
    filename = paste(PE$options$fp$backtest.report.dir, runname, mode, ".max.pdf", sep = "")
    pdf(filename, width=5.5, height =8, pointsize =8)

    curve.names<-unique(positionList$curve.name)

    index<- apply(maxData, 1, function(x){x[1]=paste(x[1],x[2],x[3],sep=", ")})# a little trick to simplify the creation of legend
    max.plot=cbind(index, maxData[,-(1:4)])
    maxplot.melt= melt(max.plot,id=c(1:2))
    maxplot.melt$index=as.factor(as.character(maxplot.melt$index))
    head(maxplot.melt)
    colnames(maxplot.melt)[3]="contract.dt"
    if(length(curve.names)> 1){
      rLog("length>0")
      print(xyplot(value~contract.dt|curve.name, data =maxplot.melt,groups=index,type="l", layout=c(1:length(curve.names)), 
            scales=list(x=list(rot=90)),auto.key = list(points = FALSE, lines = TRUE, space = "top", rows=3),
            ylab="Quantiles on pricing dates of max PE", title=paste(mode,"MPE")))
    }else{
      print(xyplot(value~contract.dt|curve.name, data =maxplot.melt,groups=index,type="l", 
            scales=list(x=list(rot=90)),auto.key = list(points = FALSE, lines = TRUE, space = "top", rows=3),
            ylab="Quantiles on pricing dates of max PE", title=paste(mode,"MPE")))

    }
    rLog("**********From PE Backtest: Plotted max for", runname,mode)
    graphics.off()
}
 


#######################################################

.start = Sys.time()

res = try( PE_Unit_Backtest$run() )

if (class(res)=="try-error"){
  cat("Failed PE backtest.\n")
}

Sys.time() - .start

  
 