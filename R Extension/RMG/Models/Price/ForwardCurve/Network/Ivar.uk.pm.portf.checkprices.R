
#
#check the xls file and take out the curve(s) that has flat prices.
#User can check the historical prices in the all.mkt.Prices.RData, and check prices the hPrices.asOfDate.RData file
#given a certain asOfDate.
# to run, try:

#source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/Ivar.uk.pm.portf.checkprices.R")
#Ivar.uk.pm.portf.checkprices()
#
################################################################################
  Ivar.uk.pm.portf.checkprices <- function(asOfDate =NULL ,curvename =NULL,
                                            filepath =NULL,last.flat.date=NULL){

  require(RODBC)
  require(SecDb)
  source("H:/user/R/RMG/Utilities/RLogger.R")
  setLogLevel(RLoggerLevels$LOG_DEBUG)

  #user need to check the Excel sheets for existence of 0 changes and find the last date such changes happen
  if(is.null(asOfDate)) {
      asOfDate = as.Date(secdb.dateRuleApply(Sys.Date(),"-1b"))
      rLog(paste("Takeing files from folders on ",asOfDate, sep ="")) 
      filepath = paste( "S:/All/Risk/Reports/VaR/prod/", asOfDate,
      "/rmg.risk.uk.pm.portfolio/IVaR.rmg.risk.uk.pm.portfolio.xls",sep="")
  }
  if(is.null(curvename)) curvename = "COMMOD FCO TABONEO SBA PHYSICAL"
  if(is.null(last.flat.date )) last.flat.date = "2007-11-21"
  mkt =strsplit(curvename, " ")[[1]][2]
  prices.con   <- odbcConnectExcel(filepath)
  prices.table <- sqlTables(prices.con)
  prices.data  <- sqlFetch(prices.con, "Prices") #"allcurves" is table name in the workbook
  odbcCloseAll()
  colnames(prices.data) <- gsub("#",".", colnames(prices.data))
  ncols =dim(prices.data)[2]
  idx <- which(prices.data[,1] == curvename & is.na(prices.data$vol.type))
  #vol.types = unique(prices.data[idx,3])
  contract.dts = unique(prices.data[idx, 2])
  asofdates = colnames(prices.data[idx,5:(ncols-1)])
  flat.asofdates = asofdates[which(asofdates<=last.flat.date)]
  xls.prices = prices.data[idx,c(1:2, 5:(ncols-1))]
  xls.prices <- xls.prices[,c(1:2, order(flat.asofdates)+2)]  #order by asofdate
  xls.prices <- xls.prices[order(xls.prices$contract.dt),] #order by contract dates
  rownames(xls.prices) =1:nrow(xls.prices)

  
  load("S:\\All\\Risk\\Data\\VaR\\prod\\Prices\\Market\\all.FCO.Prices.RData")
  hP.columns <- which((colnames(hP.mkt)%in% flat.asofdates))
  hP.rows <- which((hP.mkt$curve.name == curvename)& hP.mkt$contract.dt %in% as.Date(contract.dts))
  hP.prices <- hP.mkt[hP.rows, c(1:2, hP.columns)]
  hP.asofdates <- colnames(hP.prices)[-(1:2)]
  hP.prices <- hP.prices[,c(1:2, order(hP.asofdates)+2)]
  hP.prices <- hP.prices[order(hP.prices$contract.dt),]
  rownames(hP.prices) = 1:nrow(hP.prices)
  contract.dts <- hP.prices$contract.dt  
  asOfDates <- as.Date(colnames(hP.prices)[-(1:2)])
  
  #plot xls prices vs historical prices
  layout(1:2)
  matplot(t(xls.prices[,-(1:2)]), type = "l", xlab=paste("as of dates"), ylab ="prices", main=paste("prices registered in Excel")) 
  matplot( t(hP.prices[,-(1:2)]), type = "l", xlab=paste("as of dates"), ylab ="prices", main=paste("prices from hP.mkt data file")) 
  if(any(is.na(hP.prices[-(1:2)]))) rLog("there are NA values in the historical market price data file")
  matrix <- abs(hP.prices[,-(1:2)] - xls.prices[,-(1:2)])<= 1E-5
  
  if(all(matrix, na.rm =TRUE))  {
  rLog("There are no inconsistent prices between Excel and historical market except for NA values")
  }
 
  rLog("Now check prices on one of the asOfDates from filehash")
  i = 1 #or a random number
  asOfDate.one <- asOfDates[i]
  load(paste("S:/All/Risk/Data/VaR/prod/Prices/hPrices.",asOfDate.one, ".RData", sep =""))
  curve.idx <-  which(hP$curve.name == curvename)
  if(length(curve.idx)==0){
    rLog(paste("NO curve ", curvename, " in historical prices filehash",sep=""))
  }else
  {
    contract.idx <- which(hP[curve.idx,2] %in% contract.dts)
    if(is.null(contract.idx)){
        rLog("There are no such contract dates for this curve")
    }else{
        rLog(paste("The prices in the historical filehash for curve: ", curvename, " do exist", sep=""))
        hP.filehash.prices <- hP[curve.idx[contract.idx],4]
        names(hP.filehash.prices) <-hP[curve.idx[contract.idx],2]
        hP.mkt.prices <- hP.prices[, i+2]
        names(hP.mkt.prices)<- hP.prices$contract.dt
        layout =(1:2)
        plot(contract.dts, hp.mkt.prices, type = "l", xlab=paste("contract dates"), ylab ="prices", main=paste("prices in market file on ",asOfDate.one, sep ="") ) 
        plot(contract.dts, hP.filehash.prices, type = "l", xlab=paste("contract dates"), ylab ="prices", main=paste("prices from historical file hash on ", asOfDate.one, sep="")) 

    }
  }
    
}#end of code
  
  


  
  
  
