#check the prices registered in the Excel files against data in all.mkt.Prices.RData files and hPrices files 
#user can either check one curve only (by giving the curvename),
#or check a market (by giving the market type)
# User need to specify the Excel filepath, market,etc. the default is to check
# the IVaR.rmg.risk.uk.pm.portfolio.xls
# E.g. try:
# source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/check.excel.prices.R")
# check.excel.prices()
# asOfDate =NULL;curvename =NULL;filepath =NULL;last.Pdate=NULL;first.Pdate =NULL; mkt =NULL;
# default market is FCO 
#                                            
################################################################################
  check.excel.prices <- function(curvename =NULL,  mkt=NULL, asOfDate =NULL,
                                            filepath =NULL, first.Pdate =NULL, last.Pdate=NULL){

  require(RODBC)
  require(SecDb)
  require(usingR)
   source("H:/user/R/RMG/Utilities/RLogger.R")
  setLogLevel(RLoggerLevels$LOG_DEBUG)                                                               
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/check.excel.prices.R")
  source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/util.R")
 
  if(is.null(asOfDate)) {
      asOfDate = as.Date(secdb.dateRuleApply(Sys.Date(),"-1b"))
      rLog(paste("Taking files from folders on ",asOfDate, sep ="")) 
            
  }
  if(is.null(first.Pdate)) first.Pdate = as.Date("2007-10-01")
  if(is.null(last.Pdate )) last.Pdate = asOfDate 
  if(is.null(mkt)) mkt = "FCO"
  if(is.null(filepath)) {filepath = paste( "S:/All/Risk/Reports/VaR/prod/", asOfDate,
                        "/rmg.risk.uk.pm.portfolio/IVaR.rmg.risk.uk.pm.portfolio.xls",sep="")}
  
  prices.con   <- odbcConnectExcel(filepath)
  prices.table <- sqlTables(prices.con)
  prices.data  <- sqlFetch(prices.con, "Prices") #"allcurves" is table name in the workbook
  odbcCloseAll()
  colnames(prices.data) <- gsub("#",".", colnames(prices.data))

  
  if(is.null(curvename)) {#loop trough all curves of the given market
  
    prices.data <- prices.data[grep(mkt, prices.data$curve.name), ]
    allnames <- as.character(levels(prices.data$curve.name))
    idx<- grep(mkt, allnames)
    all.mkt.names <-allnames[idx]
    for(i in 1:length(all.mkt.names)){                              
      curvename <- all.mkt.names[i]
      compare.curves(prices.data, curvename,mkt, first.Pdate, last.Pdate)
    }
  }else{
    mkt =strsplit(curvename, " ")[[1]][2]
    compare.curves(prices.data,curvename, mkt, first.Pdate, last.Pdate)
  } 
  
}
      

 #
 ################################################################################ 
 # 
  compare.curves <- function(prices.data, curvename, mkt, first.Pdate, last.Pdate){  
  rLog("curve: ", curvename)
  idx <- which(prices.data[,1] == curvename  & is.na(prices.data$vol.type))   
  ncols =dim(prices.data)[2]  
  #vol.types = unique(prices.data[idx,3])
  
  #load the prices recorded in the Excel file
  contract.dts = unique(prices.data[idx, 2])
  asofdates = colnames(prices.data[idx,5:(ncols-1)])
  pricing.dts = asofdates[which(asofdates>= first.Pdate & asofdates <= last.Pdate)]
  xls.prices = prices.data[idx,c(1:2, 5:(ncols-1))]
  xls.prices <- xls.prices[,c(1:2, order(pricing.dts)+2)]  #order by asofdate
  xls.prices <- xls.prices[order(xls.prices$contract.dt),] #order by contract dates
  rownames(xls.prices) =1:nrow(xls.prices)

  
  #load the prices from the all.mkt.Prices.RData file
  load(paste("S:\\All\\Risk\\Data\\VaR\\prod\\Prices\\Market\\all.",mkt,".Prices.RData", sep =""))
  #find market data according to the pricing and contract dates in the Excel file
  mkt.columns <- which((colnames(hP.mkt)%in% pricing.dts))
  mkt.rows <- which((hP.mkt$curve.name == curvename)& hP.mkt$contract.dt %in% as.Date(contract.dts))
  mkt.prices <- hP.mkt[mkt.rows, c(1:2, mkt.columns)]
  mkt.asofdates <- colnames(mkt.prices)[-(1:2)]
  mkt.prices <- mkt.prices[,c(1:2, order(mkt.asofdates)+2)]  #order by pricing dates
  mkt.prices <- mkt.prices[order(mkt.prices$contract.dt),]   #order by contract dates
  rownames(mkt.prices) = 1:nrow(mkt.prices)

  
  #plot xls prices vs historical prices in the "all market" file
  filepath = paste( "H:/user/R/RMG/Models/Price/ForwardCurve/Network/",mkt," prices/", sep="")
  filename= paste(filepath, curvename,".pdf",sep="")
  if(file.access(filepath) !=0) { mkdir(filepath) }
  pdf(filename, width=5.5, height=6, pointsize=8)
  layout(matrix(1:4,2,2))
  matplot(t(xls.prices[,-(1:2)]), type = "l", xlab=paste("as of dates"), ylab ="prices", main=paste("prices registered in Excel")) 
  matplot( t(mkt.prices[,-(1:2)]), type = "l", xlab=paste("as of dates"), ylab ="prices", main=paste("prices from all.mkt data file")) 
  if(any(is.na(mkt.prices[-(1:2)]))) rLog("there are NA values in the market data")
  matrix <- abs(mkt.prices[,-(1:2)] - xls.prices[,-(1:2)])<= 1E-5
  
  if(all(matrix, na.rm =TRUE))  {
  rLog("There are no inconsistent prices between Excel and market except for NA values")
  }else{ rLog("********* There are inconsistent prices between Excel and the market data ***********") 
  }

  #compare prices in the "all market" file with the "hPrices" file
  #reshape the mkt prices, removing all na's
  mkt.melt = melt(mkt.prices, id= c("curve.name", "contract.dt"), na.rm=TRUE)
  mkt.prices =cast(mkt.melt, curve.name + contract.dt~variable)
  contract.dts <- mkt.prices$contract.dt  
  asOfDates <- as.Date(colnames(mkt.prices)[-(1:2)])

  rLog("Now check prices on one of the asOfDates from hPrices")
  i = 1 #or a random number less than the length of the asOfDates
  asOfDate.one <- asOfDates[i]
  load(paste("S:/All/Risk/Data/VaR/prod/Prices/hPrices.",asOfDate.one, ".RData", sep =""))
  curve.idx <-  which(hP$curve.name == curvename)
  if(length(curve.idx)==0){
    rLog(paste("NO curve ", curvename, " in hPrices on", asOfDate.one,sep=""))
  }else
  {
    contract.idx <- which(hP[curve.idx,"contract.dt"] %in% contract.dts)
    if(is.null(contract.idx)){
        rLog("There are no such contract dates for this curve")
    }else{
        rLog(paste("The hPrices for this curve and contract dates exist"))
        one.hP.prices <- hP[curve.idx[contract.idx],as.character(asOfDate.one)]
        names(one.hP.prices) <-hP[curve.idx[contract.idx],"contract.dt"]
        one.mkt.prices <- mkt.prices[, i+2]
        names(one.mkt.prices)<- mkt.prices$contract.dt
        layout =(1:2)
        plot(contract.dts,one.mkt.prices, type = "l", xlab=paste("contract dates"), ylab ="prices", main=paste("prices from all market file on ",asOfDate.one, sep ="") ) 
        plot(contract.dts, one.hP.prices, type = "l", xlab=paste("contract dates"), ylab ="prices", main=paste("prices from hPrices file on", asOfDate.one, sep="")) 

    }
  }
  graphics.off()

}#end of code

  
 