#----------------------------------------------------------------------------
source("H:/user/R/RMG/Utilities/lib.excel.functions.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.buy.sell.counterparty.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/sim.forward.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEInteractiveUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
require(reshape)
require(SecDb)

#-------------------------------------------------------------------------------
PECalculatorPortfolioCondor <- new.env(hash=TRUE)
#-------------------------------------------------------------------------------

################################################################################
# Independent MPE calculator for any given PF as of any time.
# It will simulate the curves in the portfolio (if they don't exist) and calculate
# MPE, seperating "buy" or "sell" contracts for CPs that have such features.
# For such CPs, PE from both "buy" and "sell" will be calculated and the bigger
# will represent overall PE.

PECalculatorPortfolioCondor$run <- function()
{
  #setLogLevel(RLoggerLevels$LOG_DEBUG)
  #asOfDate  <- as.Date(Sys.getenv("asOfDate"))
  asOfDate  <- as.Date("2008-05-20")
  rLog(as.character(asOfDate)) #see end of file for commented asOfDate setting
  isInteractive=TRUE  #set it to FALSE and run.name to "", if using the overnight simulation results
  run.name="Test"     #set it to the run.name under which simulation results are saved.
  FP.options <- ForwardPriceOptions$create( asOfDate=asOfDate,
                                              isInteractive,
                                              run.name=run.name )
  ## Obtain position information
  #Portfolio        <- as.character(Sys.getenv("PF"))
  Portfolio        <- "Freight Accrual Portfolio"#see end of file for commented PF setting
  Options          <- NULL
  Options$asOfDate <- asOfDate
  Options$books    <- .getPortfolioBooksFromSecDb(Portfolio)
  Positions        <- get.positions.buy.sell.counterparty(Options)
  colnames(Positions)[which(colnames(Positions)=="delta")] <- "position"

  ## Simulate curves by counterparties
  curvenames<-toupper(unique(Positions$curve.name))
  sim.all(curvenames, FP.options)

  ## Group buy-only, sell-only and buy-and-sell counterparties
  Positions.buy  <- subset(Positions, buy.sell.type == "B", drop=F) #all CPs that are of "buy" type
  Positions.sell <- subset(Positions, buy.sell.type == "S", drop=F) #all CPs that are of "sell" type
  cp.buy         <- unique(Positions.buy$counterparty)
  cp.sell        <- unique(Positions.sell$counterparty)
  cp.both        <- intersect(cp.buy, cp.sell)#names for CPs that have both "buy" and "sell" sides
  cp.buy.only    <- setdiff(cp.buy, cp.both)
  cp.sell.only   <- setdiff(cp.sell, cp.both)

  ## Run "buy" only counterparties
  rLog("Calculating PE for Buy only CPs")
  for(thiscp in cp.buy.only)
  {
    rLog("\n", thiscp)
    #get each curve for this run.name:
    CP          <- subset(Positions.buy, counterparty == thiscp, drop=F)
    res.sPE     <- NULL
    res.totalPE <- NULL
    try(res.sPE <- PEInteractiveUtils$getPE(thiscp, FP.options, QQ=CP[,-1]))
    if(!is.null(res.sPE))
    {
       qPE <- PEInteractiveUtils$qPE(res.sPE)
       ##Plot PE quantiles without initial PE
       PECalculatorPortfolioCondor$plotqPE(thiscp,qPE, FP.options)
       ##Calculate and plot quantiles with initial PE, which includes initial PE
       res.totalPE <- PECalculatorPortfolioCondor$getTotalPE(asOfDate,thiscp, res.sPE,type="Buy")
       qPE.total   <- PEInteractiveUtils$qPE(res.totalPE)
       PECalculatorPortfolioCondor$plotqPE(paste(thiscp,"_total", sep=""), qPE.total, FP.options)
       #Write to xls files
       PEInteractiveUtils$write.xls(qPE$direct, qPE$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=FALSE) 
       PEInteractiveUtils$write.xls(qPE.total$direct, qPE.total$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=TRUE) 
       rLog("\nDone for CP:", thiscp, "\n")
       res.sPE     <- NULL
       res.totalPE <- NULL
       gc()
    }else #Note: CPs with expired contracts only won't have results
    {
     rLog("No PE results for CP:", thiscp, "The contracts may have all expired.\n")
    }
  }
   
  ## Run "sell" only counterparties
  rLog("Calculating PE for Sell only CPs")
  for(thiscp in cp.sell.only)
  {
    rLog("\n",thiscp)
    CP          <- subset(Positions.sell, counterparty == thiscp, drop=F)
    try(res.sPE <- PEInteractiveUtils$getPE(thiscp, FP.options, QQ=CP[,-1]))
    if(!is.null(res.sPE))
    {  
       qPE <- PEInteractiveUtils$qPE(res.sPE)
       ##Plot PE quantiles without initial PE
       PECalculatorPortfolioCondor$plotqPE(thiscp,qPE, FP.options)
       ##Calculate and Plot PE quantiles with initial PE
       res.totalPE <- PECalculatorPortfolioCondor$getTotalPE(asOfDate,thiscp, res.sPE, type="Sell")
       qPE.total   <- PEInteractiveUtils$qPE(res.totalPE)
       PECalculatorPortfolioCondor$plotqPE(paste(thiscp,"_total", sep=""),qPE.total, FP.options)
       PEInteractiveUtils$write.xls(qPE$direct, qPE$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=FALSE) 
       PEInteractiveUtils$write.xls(qPE.total$direct, qPE.total$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=TRUE) 
       rLog("\nDone for CP:", thiscp, "\n")
       res.sPE     <- NULL
       res.totalPE <- NULL
       gc()
    }else
    {
     rLog("No PE results for CP:", thiscp,"The contracts may have all expired.\n")
    }
  }

  ## Run "Buy-and-Sell" counterparties
  rLog("Calculate PE for CPs that have both Buy and Sell types")
  for(thiscp in cp.both)
  {
    rLog("\n",thiscp)
    #Matach the positions in buy side and sell side first, so they have the same length of contract dates (not curves)
      CP.buy  <- subset(Positions.buy, counterparty == thiscp, drop=F)
      CP.sell <- subset(Positions.sell, counterparty == thiscp, drop=F)
      curvenames.buy  <- unique(CP.buy$curve.name)
      curvenames.sell <- unique(CP.sell$curve.name)
      all.dts         <- unique(c(as.character(CP.buy$contract.dt),as.character(CP.sell$contract.dt)))
      #Expand the contract dates of each curve in thiscp to make the dates all match,
      #with 0 positions added for expanded contract dates
      #Expand for the "buy" type, curve by curve
      app <- NULL
      for(curvename in curvenames.buy)
      {
        thiscurve.dts <- as.character(subset(CP.buy, curve.name==curvename)$contract.dt)                                           
        diff.dt       <- setdiff( all.dts, thiscurve.dts)
        if(length(diff.dt)>0) app <- rbind(app, data.frame(buy.sell.type="B", counterparty=thiscp, curve.name=curvename, contract.dt = diff.dt, position=0))
      }
      CP.buy <- rbind(CP.buy, app)
      CP.buy <- CP.buy[order(CP.buy$curve.name, CP.buy$contract.dt),]
      rownames(CP.buy)=c(1:dim(CP.buy)[1])
      #Expand for "sell" type curve by curve
      app <- NULL
      for(curvename in curvenames.sell)
      {
        thiscurve.dts <- as.character(subset(CP.sell, curve.name==curvename, drop=F)$contract.dt)
        diff.dt       <- setdiff( all.dts, thiscurve.dts)
        if(length(diff.dt)>0) app <- rbind(app, data.frame(buy.sell.type="S", counterparty=thiscp, curve.name=curvename, contract.dt = diff.dt, position=0))
      }
      CP.sell <- rbind(CP.sell, app)
      CP.sell <- CP.sell[order(CP.sell$curve.name, CP.sell$contract.dt),]
      rownames(CP.sell)=c(1:dim(CP.sell)[1])

    #Calculate PE using buy positions
    #clean up
    res.sPE.buy     <- NULL
    res.totalPE.buy <- NULL
    #get PE results
    try(res.sPE.buy <- PEInteractiveUtils$getPE(thiscp, FP.options, QQ=CP.buy[,-1]))
    #get quantiles
    if(!is.null(res.sPE.buy))
    {
      qPE.buy         <- PEInteractiveUtils$qPE(res.sPE.buy)
      res.totalPE.buy <- PECalculatorPortfolioCondor$getTotalPE(asOfDate,thiscp, res.sPE.buy,type="Buy")
      qPE.total.buy   <- PEInteractiveUtils$qPE(res.totalPE.buy)      
    }else{
      rLog("The buy type of this cp ", thiscp," may have expired!")
      qPE.buy         <- NULL
      res.totalPE.buy <- NULL
      qPE.total.buy   <- NULL
    }
    res.sPE.buy       <- NULL
    res.totalPE.buy   <- NULL
    gc()
    
    #Calculate the PE using sell positions
    res.sPE.sell      <- NULL
    res.totalPE.sell  <- NULL
    try(res.sPE.sell  <- PEInteractiveUtils$getPE(thiscp, FP.options, QQ=CP.sell[,-1]))
    if(!is.null(res.sPE.sell))
    {
      qPE.sell         <- PEInteractiveUtils$qPE(res.sPE.sell)
      res.totalPE.sell <- PECalculatorPortfolioCondor$getTotalPE(asOfDate,thiscp, res.sPE.sell,type="Sell")
      qPE.total.sell   <- PEInteractiveUtils$qPE(res.totalPE.sell)
    }else{
      rLog("The sell type of this cp ", thiscp," may have expired!")
      qPE.sell         <- NULL
      res.totalPE.sell <- NULL
      qPE.total.sell   <- NULL
    }
    res.sPE.sell       <- NULL
    res.totalPE.sell   <- NULL
    gc()
    
    #Match buy and sell positions and get the bigger of the two   
    if(!is.null(qPE.sell) & !is.null(qPE.buy)) #both sell and buy side have PE results
    {
      ##Sometimes qPE.total.sell and qPE.total.buy have different contract length because the initial PEs have different contract months
      ##In such cases, first match their contract months by expanding contract months (add 0 as values)
      sell.dts       <- colnames(qPE.total.sell$direct)
      buy.dts        <- colnames(qPE.total.buy$direct)
      sell.extra.dts <- setdiff(sell.dts, buy.dts)
      buy.extra.dts  <- setdiff(buy.dts, sell.dts)
      if(length(sell.extra.dts)>0)
      {
        extra <- matrix(0, nrow=3, ncol=length(sell.extra.dts))
        colnames(extra)        <- as.character(sell.extra.dts)
        qPE.total.buy$direct   <- cbind(qPE.total.buy$direct, extra)
        qPE.total.buy$indirect <- cbind(qPE.total.buy$indirect, extra) 
        qPE.total.buy$direct   <- qPE.total.buy$direct[, order(as.Date(colnames(qPE.total.buy$direct)))]
        qPE.total.buy$indirect <- qPE.total.buy$indirect[, order(as.Date(colnames(qPE.total.buy$indirect)))]
      }
      if(length(buy.extra.dts)>0)
      {
        extra <- matrix(0, nrow=3, ncol=length(buy.extra.dts))
        colnames(extra)         <- as.character(buy.extra.dts)
        qPE.total.sell$direct   <- cbind(qPE.total.sell$direct, extra)
        qPE.total.sell$indirect <- cbind(qPE.total.sell$indirect, extra) 
        qPE.total.sell$direct   <- qPE.total.sell$direct[, order(as.Date(colnames(qPE.total.sell$direct)))]
        qPE.total.sell$indirect <- qPE.total.sell$indirect[, order(as.Date(colnames(qPE.total.sell$indirect)))]
      }
      #Take the larger of the two for PE for each prctil
      qPE.max       <- PECalculatorPortfolioCondor$qPE.max(thiscp,qPE.buy, qPE.sell)
      qPE.total.max <- PECalculatorPortfolioCondor$qPE.max(thiscp,qPE.total.buy, qPE.total.sell)
    }else
    {  
      if(!is.null(qPE.buy))#if only buy side has PE results
      {
        qPE.max       <- qPE.buy
        qPE.total.max <- qPE.total.buy
      }else 
      {
        if(!is.null(qPE.sell))#if only sell side has PE results
        {
          qPE.max       <- qPE.sell
          qPE.total.max <- qPE.total.sell
        }else
        {
          if(is.null(qPE.sell) & is.null(qPE.buy))#both buy and sell side PEs are empty
          {
            rLog("There is no PE calculated for this CP: ", thiscp, "\n")
            next()
          }
        }
      }
    }
       
    ##make plots for the CPs of this category
    PECalculatorPortfolioCondor$plotqPE(thiscp,qPE.max, FP.options)
    PECalculatorPortfolioCondor$plotqPE(paste(thiscp,"_total", sep=""),qPE.total.max, FP.options)
    CP <- rbind(CP.buy, CP.sell)
    PEInteractiveUtils$write.xls(qPE.max$direct, qPE.max$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=FALSE) 
    PEInteractiveUtils$write.xls(qPE.total.max$direct, qPE.total.max$indirect,CP, outputDir=FP.options$report.dir, 
                                    cpName=thiscp, totalPE=TRUE)
    rLog("\nDone for CP:", thiscp, "\n")
  }#end of cp.both loop
}


###=======================================================================================================================
## Get total PE, which includes initial PE Exposure
##
PECalculatorPortfolioCondor$getTotalPE <- function(asOfDate,thiscp, res.sPE, type){
    asOfDate=as.Date(asOfDate)
    InitialExposures <- PEUtils$GetInitialExposures(AsOfDate=asOfDate)
    Initial.thiscp   <- InitialExposures[which(InitialExposures[,3]==thiscp & InitialExposures[,6]==type), c(3,6:7,11)]
  
    if(nrow(Initial.thiscp)>0)
    {
      ## Aggregate initial exposure, will add up values of the same ContractDate
      ini.thiscp.agg          <- aggregate(Initial.thiscp[,3],list(Initial.thiscp$ContractDate), sum)
      colnames(ini.thiscp.agg) <- c("contract.dt", "initial.exposure")
      
      ## Expand contract dates
      
      #If the initial exposure has more contract dates than in the positions files, expand the position file's contract dates
      #Note: if the position file has longer pricing dates than the initial exposure's contract dates, it will not matter because
      #the cumm.ini.exposure will then calculate ini exposure as 0 for pricing dates beyond those contract dates
      pricing.dts      <- as.matrix(res.sPE$pricing.dt)
      extra.dts        <- as.character(ini.thiscp.agg[which(as.Date(ini.thiscp.agg$contract.dt)>as.Date(pricing.dts[length(pricing.dts)])),1])
      #Sometimes extra.dts starts from a discontinuous month from the last date of pricing.dts, in such a case, populate all missing months 
      #by adding 0 positions
      dts.range        <- range(as.Date(c(secdb.dateRuleApply(pricing.dts[length(pricing.dts),], "+1m"),extra.dts)))
      extra.dts        <- as.character(seq(from=dts.range[1], to=dts.range[2], by="month"))
      if(!is.null(extra.dts))
      {
        pricing.dts      <- as.matrix(union(pricing.dts, extra.dts))
        res.sPE.expand   <- data.frame(pricing.dt = as.factor(extra.dts), matrix(0, nrow=length(extra.dts), ncol=1000))#use the extra contract dates to expand sPE
        colnames(res.sPE.expand)[-1] <- gsub('X', 'sim.',colnames(res.sPE.expand)[-1]) 
        res.sPE          <- rbind(res.sPE, res.sPE.expand)
      }
      ## Calculate initial exposure by adding all reported ini.exposures on contrract dates later than (or equal to) the current pricing date. 
      cumm.ini.exposure       <- apply(pricing.dts, 1 ,function(x){sum(ini.thiscp.agg[which(as.Date(ini.thiscp.agg$contract.dt)>=
                                          as.Date(x)),2])})
      res.iniPE               <- data.frame(pricing.dt=as.factor(pricing.dts), matrix(rep(cumm.ini.exposure, times=1000), ncol=1000))
                                
      colnames(res.iniPE)[-1] <- gsub('X', 'sim.',colnames(res.iniPE)[-1])
      res.totalPE             <- data.frame(pricing.dt=res.sPE[,1], res.sPE[,-1]+res.iniPE[,-1])  
    }else
    {
      res.totalPE <- res.sPE
    }
    return(res.totalPE)
}

###=====================================================================================================
PECalculatorPortfolioCondor$plotqPE <-function(thiscp,qPE, FP.options)
{
   mode="direct"
   PECalculator$.outputData(qPE$direct, FP.options$asOfDate, mode=mode, writeToFile=TRUE,showPlot=FALSE,
                              plotName=thiscp, outputDir=FP.options$report.dir)
   mode="indirect"
   PECalculator$.outputData(qPE$indirect, FP.options$asOfDate, mode=mode, writeToFile=TRUE,showPlot=FALSE,
                              plotName=thiscp, outputDir=FP.options$report.dir)
}

##=======================================================================================================
PECalculatorPortfolioCondor$qPE.max <-function(thiscp, qPE.buy=NULL, qPE.sell=NULL)
{    
  if( is.null(qPE.buy) & is.null(qPE.sell) ){
      qPE <- NULL
      rLog("NO PE for ", thiscp, " can be calculated! Contracts may be all expried")
  }
  if(!is.null(qPE.buy) & !is.null(qPE.sell)){
    qPE <- qPE.buy #to give qPE a structure only
    for (i in c(1:3))
    {
      qPE$direct[i,]   <- ifelse(qPE.buy$direct[i,]>= qPE.sell$direct[i,], qPE.buy$direct[i,], qPE.sell$direct[i,])
      qPE$indirect[i,] <- ifelse(qPE.buy$indirect[i,]<= qPE.sell$indirect[i,], qPE.buy$indirect[i,], qPE.sell$indirect[i,])
    }
  }else
  {
    if(is.null(qPE.buy))  qPE <- qPE.sell
    if(is.null(qPE.sell)) qPE <- qPE.buy
  }
  return(qPE)
}
       
##===========================================================================
.start = Sys.time()
#to test, uncomment below
#Sys.setenv(asOfDate="2008-04-04")
#Sys.setenv(PF = "Freight Accrual Portfolio")

res <- try(PECalculatorPortfolioCondor$run())

if (class(res)=="try-error"){
  cat("Failed Portfolio Calculation.\n")
}

Sys.time() - .start


#XLS$write.xls(Positions, paste('H:/_Work/Credit. MPE/20080407.FreightPortfolio.ICRs/FreightPositions.',format(AsofDate,'%Y%m%d'),'.xls',sep=''))



