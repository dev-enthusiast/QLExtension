# Given curvename, starting of contract dates, and range of historical pricing dates, 
# this code will fill the missing curves(taken out of tsdb) in "hPrices.asOfDate.RData" files
# If the curve exists, but data for some contract dates are missing , it will alse add those contract dates
# Note: this code CANNOT check the consistency of data. If the tsdb data and the data in "hPrices.asOfDate.Rdata" files are 
# different, this code CANNOT detect it or make changes.
# source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/update.hPrices.R")
# to run: try 
#            update.hPrices()
## 2008-01-29, by JBai            

 update.hPrices <-function(curvename = NULL) {
 #source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/update.hPrices.R")
 require(filehash)
 require(SecDb)
 source("H:/user/R/RMG/Utilities/RLogger.R")
 setLogLevel(RLoggerLevels$LOG_DEBUG)
 
 if(is.null(curvename)) curvename <- "COMMOD FCO KILROOT COL PHYSICAL"
 histStart.dt <- as.Date("2005-01-01")
 asOfDate <- as.Date(Sys.Date())
 histEnd.dt <- asOfDate                                         

 contractStart.dt <- as.Date("2005-01-01")
 contract.dts <- seq(contractStart.dt, by="month", length.out=360) #assume the max length of contracts might be only 30 years
 tsdb.curveData <- tsdb.futStrip( curvename, histStart.dt, histEnd.dt, contract.dts ) tsdb.curveData <- subset(tsdb.curveData, !is.na(value))
 
 if(nrow(tsdb.curveData)!=0){
  tsdb.pricing.dts <- unique(tsdb.curveData$pricing.dt) #obtain historical pricing dates
  tsdb.pricing.dts <- tsdb.pricing.dts[which(tsdb.pricing.dts >= as.Date("2007-05-30"))]
 
  #cycle through pricing dates
  tsdb.n.Pdays <- length(tsdb.pricing.dts)  #number of pricing dates
  rLog(paste("Filling data of curve ", curvename, " to hPrices files...", sep =""))
  for (i in 1:tsdb.n.Pdays){
    hPrices.file=paste("S:/All/Risk/Data/VaR/prod/Prices/hPrices.",tsdb.pricing.dts[i],".RData", sep ="")
    results<- try(load(hPrices.file))
    if (class(results)== "try-error"){
      print(cat("No such hPrices file exist on pricing date: ",as.Date(tsdb.pricing.dts[i]), sep=""))
      next
    }

    hP.thiscurve.data <- subset(hP, curve.name == curvename)
    if(nrow(hP.thiscurve.data) == 0) {#if there is no such curve
      rLog(paste("The curve is missing in this file: hPrices.", tsdb.pricing.dts[i],".RData", sep =""))
      rLog(paste( "Now add this curve: ",curvename,sep=""))
      hP <- .hPrices.add.data(subset(tsdb.curveData, pricing.dt == tsdb.pricing.dts[i]), hP)
      save(hP, file=hPrices.file)
  
    }else { #if the curve exist, check if there are less contract dates recorded
       tsdb.subset <- subset(tsdb.curveData, pricing.dt == tsdb.pricing.dts[i])
       tsdb.contract.dts <- unique(tsdb.subset$contract.dt) #obtain contract dates
       extra.contract.dts <- tsdb.contract.dts[-which(tsdb.contract.dts %in% hP.thiscurve.data$contract.dt)]
       
       if(length(extra.contract.dts) >0){
         rLog(paste("Data for some contract dates are missing from file hPrices.",tsdb.pricing.dts[i],
                      ".RData. Now add those contract dates", sep =""))
        
         hP <- .hPrices.add.data(subset(tsdb.subset, contract.dt == extra.contract.dts),hP)
         save(hP, file = hPrices.file)    
       }else{ rLog(paste("All data exist for the file: hPrices.", tsdb.pricing.dts[i], ".RData", sep =""))}
    }#end of else
                                    
  }#end of pricing date loop
  
  } else {print("there are no TSDB data for this curve, or only NA for this curve")}
   
}#end of function

#
#####################################################################################
# This function will add the new tsdb data to the hPrices data, adjusting the format
# to be the same as the format of the "hPrices.asofdate.RData" file
# Note: The "S:/all/Risk/data/VaR/Prod/Prices/hPrices.asofdate.RDate" files have various formats
#  with different asofdates.
#
 .hPrices.add.data <- function(tsdb.newdata, hP.old) {
  newdata <- tsdb.newdata[,-3]
  colnames(newdata)[3]= as.character(tsdb.newdata$pricing.dt[1])
  colname <- colnames(hP.old)
  tsdb.data = data.frame(newcol = newdata[,"curve.name"])
  colnames(tsdb.data)[ncol(tsdb.data)] = "curve.name"
  
  #adjust the columns in newdata so that newdata has the same columns (in the same order) as hP.old
  for(i in 2: length(colname)){  
        if(colname[i] %in% colnames(newdata)){ 
           tsdb.data= data.frame(tsdb.data, newcol =newdata[,colname[i]])
           colnames(tsdb.data)[ncol(tsdb.data)] = as.character(colname[i])
        }
        else{ tsdb.data = data.frame(tsdb.data, newcol =NA)   #append an empty column
           colnames(tsdb.data)[ncol(tsdb.data)] = as.character(colname[i])
        }
    }  
  colnames(tsdb.data)<- colnames(hP.old)
  hP.old <- rbind(hP.old, tsdb.data)
  hP.old <- hP.old[order(hP.old[,"curve.name"],hP.old[,"contract.dt"]),]
  rownames(hP.old) <- 1:nrow(hP.old)
  hP.old
  
 }
 
 ##
 ##
 ##############################################################################
 # Use this code to update the prices in filehash using tsdb data
 # to use: type 
 #              update.filehash("COMMOD FCO BAFA INDEX")
 #
 update.filehash <- function(curvename = NULL) {
   require(filehash); require(SecDb); require(reshape)
   filehashOption(defaultType = "RDS")
   source("H:/user/R/RMG/Utilities/RLogger.R")
   setLogLevel(RLoggerLevels$LOG_DEBUG)
 
 if(is.null(curvename)) {curvename <- "COMMOD FCO TABONEO SBA PHYSICAL"}

 #open filehash
 filehashpath = paste("//nas-msw-07/rmgapp/Prices/Historical")
 db <- dbInit(filehashpath)
 hPP.env <- db2env(db)
 if (!is.environment(hPP.env))
  {
      rError("Could not load the price filehash environment.\n")
      stop("Exiting.\n")
  }

 hPP <- NULL
 hPP$curve.data <- hPP.env[[tolower(curvename)]]
 if(!is.null(hPP$curve.data)){
    rLog("The filehash exists for curve: ", curvename)
    hPP$contract.dts <- sort(hPP$curve.data$contract.dt)
    columnNames = colnames(hPP$curve.data)
    hPP$pricing.dts = columnNames[grep( "(\\d{4}-\\d{2}-\\d{2})", columnNames, perl=TRUE )]
    hPP$pricing.dts = sort(hPP$pricing.dts)
    startDate = as.Date(hPP$pricing.dts[1])
    endDate = as.Date(hPP$pricing.dts[length(hPP$pricing.dts)])
    
    rLog("Reading TSDB Data for:", curvename)
    tsdbData = tsdb.futStrip(curvename, startDate, endDate, hPP$contract.dts)
    tsdbData <- subset(tsdbData, !is.na(value))

    #Correct for the case where we don't get back all the pricing dates
    tsdbData$pricing.dt = factor(as.character(tsdbData$pricing.dt), levels=hPP$pricing.dts)
    
    rLog("Reshaping TSDB Data...")
    filledData = cast(tsdbData, curve.name+contract.dt~pricing.dt, add.missing=TRUE)
    filled.colnames <- colnames(filledData)
    filledData <- data.frame(filledData)
    colnames(filledData) <- filled.colnames
    filledData$curve.name <- as.character(filledData$curve.name)
    filledData$contract.dt <- as.Date(filledData$contract.dt)
    res <- filledData
   
    #save the new data to filehash
    res <- try(dbInsert(db, tolower(curvename),res ))
    if (class(res)=="try-error"){
      rLog(paste("Could not insert curve:",cname," into sim filehash.\n",sep=""))
     return()
    }
     rLog("Data in filehash refilled by TSDB data")
  }else{ 
     rLog("The curve does not exist in the filehash")
  }
    
}