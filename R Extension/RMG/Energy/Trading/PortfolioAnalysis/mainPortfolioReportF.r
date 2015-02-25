# see demoPortfolioReport
# mainPortfolioReport(options)
# Anatoley Zheleznyak 11/16/2006

mainPortfolioReportF <- function(options) {
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")  
  source("H:/user/R/RMG/Utilities/extractDeltaVega.R")
  source("H:/user/R/RMG/Utilities/extractDeltaVegaA.R")
  source("H:/user/R/RMG/Utilities/mapCurveName2CommRegionType.R")
  source("H:/user/R/RMG/Utilities/bookMappingCPSPROD.R")  
  source("H:/user/R/RMG/Utilities/write.xls.R") 
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesCVSSAS.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesSAS.R")
  fileNameBooks <- "S:/Risk/Accrual Risk/AccrualBooksB.xls"
  for ( i in 1:length(options$portfolio) ) {
    for(j in 1:length(options$dates)) {
      options$asOfDate <- options$dates[j]
      options$dirOut <- paste("S:/Risk/2007,Q1/Risk.Analytics/Accrual/", as.character(options$asOfDate), "/", sep = "")
      dir.create(options$dirOut,  showWarnings = FALSE)
      fileName     <- paste(options$dirOut, "Positions ", options$portfolio[i], " ",
                                    as.character(options$asOfDate), "Gandhi.xls", sep = "")                                
      if(file.exists(fileName) == FALSE) {
#        browser() 
        # NE Cash
        options$books  <- get.portfolio.hierarchy("Nepool Cash Accrual Portfolio", options$asOfDate)
        options$books  <- c(options$books, "NYCASHDG", "NPCASHVL", "NPVLRHDG") 
        data <- extractDeltaVega(options$books, options$asOfDate)
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data1 <- merge(data, dataCommRegType)
        data1$ECONOMIC_OWNER <- "Navroz Gandhi NE Cash"
        # Old Settles
        options$books <-  get.portfolio.hierarchy("Nepool Congestion Portfolio", options$asOfDate)
        options$books <- c(options$books, get.portfolio.hierarchy("Nepool ISO Settlement Portfolio", options$asOfDate)) 
        options$books <- c(options$books, get.portfolio.hierarchy("Nepool Old Books Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("Nepool Other Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("NYPP Old Books Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("NYPP Other Accrual Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("SE Other Portfolio", options$asOfDate))
        options$books <- c(options$books, "GINNAOPS", "NMP1OPS" ,"NMP2OPS", "HQOPS", "VOLHEDGS", "NSBHEMTM", "NSCMPMTM")
        data <- extractDeltaVega(options$books, options$asOfDate)
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data2 <- merge(data, dataCommRegType)
        data2$ECONOMIC_OWNER <- "Navroz Gandhi Old Settles"
        # Fixed Price
        options$books <-  get.portfolio.hierarchy("Nepool FP Portfolio", options$asOfDate)
        options$books <- c(options$books, get.portfolio.hierarchy("NYPP NEV Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("NYPP Other Unit Portfolio", options$asOfDate))
        options$books <- c(options$books, get.portfolio.hierarchy("SE FP Portfolio", options$asOfDate))
        options$books <- c(options$books, "GINNAPPA", "WSGINNA", "NMILE1PA", "WSNMILE1", "NMILE2PA", "WSNMILE2",
                           "NMP2RSA", "IBTNYHG", "NMPNPNS", "NYONTHG", "NYUCPHDG", "NYZNAHDG", "NYZNBHDG", "NYZNCHDG",
                           "NYZNHHDG", "NEWHLMTM", "NPFTRMTM", "SENEPOOL")
        data <- extractDeltaVega(options$books, options$asOfDate)                           
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data3 <- merge(data, dataCommRegType)
        data3$ECONOMIC_OWNER <- "Navroz Gandhi FP"
        # Attrition
        options$books <-  get.portfolio.hierarchy("Nepool Mixed Attr Portfolio", options$asOfDate)
        options$books <- c(options$books, "NPGASVOL")
        data <- extractDeltaVega(options$books, options$asOfDate)                           
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data4 <- merge(data, dataCommRegType)
        data4$ECONOMIC_OWNER <- "Navroz Gandhi Attrition"
        # SE Cash
        options$books <-  get.portfolio.hierarchy("SE Cash Portfolio", options$asOfDate)
        options$books <- c(options$books, "SEMTM")
        data <- extractDeltaVega(options$books, options$asOfDate)                           
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data5 <- merge(data, dataCommRegType)
        data5$ECONOMIC_OWNER <- "Navroz Gandhi SE Cash"
        # Spec Congestion
        options$books <-  get.portfolio.hierarchy("Jaya Bajpai Trading", options$asOfDate)
        data <- extractDeltaVega(options$books, options$asOfDate)                           
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data6 <- merge(data, dataCommRegType)
        data6$ECONOMIC_OWNER <- "Navroz Gandhi Spec Congestion"
        # GTC Portfolio
        options$books <-  get.portfolio.hierarchy("GTC Accrual Portfolio", options$asOfDate)
        data <- extractDeltaVega(options$books, options$asOfDate)                           
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data7 <- merge(data, dataCommRegType)
        data7$ECONOMIC_OWNER <- "Navroz Gandhi GTC"
#        browser()
        dataAll <- rbind(data1,data2,data3,data4,data5,data6)
        dataAll$ECONOMIC_OWNER <- "Navroz Gandhi Total East Portfolio"
        data <- rbind(data1, data2, data3, data4, data5, data6, data7, dataAll)
        # rename Cap
        indCap <- which(data$COMMODITY == "ICAP" | data$COMMODITY == "UCAP")
        if (length(indCap) >0 ) { data$COMMODITY[indCap] <- "CAPACITY" } 
        # exlude past date and modify date format
        data$VALUATION_MONTH <- as.Date(paste(format(data$VALUATION_MONTH, "%Y-%m"), "-01", sep = ""))
        indVALMON <- which(data$VALUATION_MONTH < as.Date(paste(format(options$asOfDate, "%Y-%m"), "-01", sep = ""))) 
        if(length(indVALMON) > 0) { data <- data[-indVALMON,]} 
        # blank ECONOMIC_OWNER
        indBlank <- which(is.na(data$ECONOMIC_OWNER) == TRUE)
        if(length(indBlank) > 0) { 
          levels(data$ECONOMIC_OWNER) <- c(levels(data$ECONOMIC_OWNER), "Blank")
          data$ECONOMIC_OWNER[indBlank] <- "Blank"
        }
        # remove ANCILLARIES
        indANC <- which(data$COMMODITY == "ANCILLARIES")        
        if(length(indANC) > 0 ) {data <- data[-indANC,]}        
#        browser()
        # Delta & Vega aggregate       
        dataDelta <- localAggregate(data, "DELTA")               
        dataVega  <- localAggregate(data, "VEGA") 
        # get price changes
        dataChange <- readPriceVolChangesCVSSAS(options$portfolio[i], options$asOfDate) 
#        browser()      
        if(is.null(dataChange[[1]]) == TRUE) {
          options$VAR = FALSE
        } else {
          options$VAR = TRUE
        }        
        if(options$VAR == TRUE) {
#          dataChange <- readPriceVolChangesCVSSAS(options$portfolio[i], options$asOfDate)        
#          dataChange <- readPriceVolChangesSAS(options$portfolio[i], options$asOfDate) 
          # check if some curves are missing for Delta positions, retrieve more price changes 
          # need to make similar check with volatility  
          diffCurves <- setdiff(unique(data$CURVE_NAME), unique(dataChange[[1]]$CURVE_NAME))
#          browser()
          if(length(diffCurves) > 0 ) {
            dataDC <- subset(data, CURVE_NAME %in% diffCurves)
            indNull <- which(abs(dataDC$DELTA) < 0.1)
            if(length(indNull) >0 ) {dataDC <- dataDC[-indNull,]}
#            browser()
            if(dim(dataDC)[1] > 0) {
              maxDate <- aggregate(dataDC$VALUATION_MONTH, list(CURVE_NAME = dataDC$CURVE_NAME), max)          
              maxDate$x <- maxDate$x + as.Date("1970-01-01")                                               
              options$number.of.days <- 71                                                                 
              options$contract.dt.start <- as.Date(paste(format(options$asOfDate, "%Y-%m"), "-01", sep = ""))                                           
              options$source.dir <- "H:/user/R/RMG/"                                                       
              options$extraction.type <- 1    # 1 direct database extraction, 2 extraction from archive    
              options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"                                     
              options$file.dsn <- "CPSPROD.dsn"                                                            
              source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.price.differences.R")                   
              res <- NULL                                                                                                                       
              for(iYear in (as.numeric(format(min(maxDate$x), "%Y"))):(as.numeric(format(max(maxDate$x), "%Y")))) {                                                                     
                cYear = as.character(iYear)                                                      
                indY <- which(maxDate$x >=  as.Date(format(paste(cYear, "-01-01", sep = ""))) &   
                              maxDate$x <= as.Date(format(paste(cYear, "-12-01", sep = ""))))     
                if(length(indY) > 0) {                                                            
                  options$curve.names <- maxDate$CURVE_NAME[indY]                                 
                  options$contract.dt.end   <- as.Date(format(paste(cYear, "-12-01", sep = ""))) 
                  xxxx <- get.price.differences(options) 
                  if(length(colnames(res)) > length(colnames(xxxx))){
                     extraCols <- setdiff(colnames(res), colnames(xxxx))
                     xxxx1 <- array(NA, dim = c( dim(xxxx)[1], length(extraCols))) 
                     colnames(xxxx1) <- extraCols
                     xxxx <- cbind(xxxx, xxxx1)
                  }
                  if(length(colnames(res)) < length(colnames(xxxx)) & length(colnames(res)) > 0 ){
                     extraCols <- setdiff(colnames(xxxx), colnames(res))
                     res1 <- array(NA, dim = c( dim(res)[1], length(extraCols))) 
                     colnames(res1) <- extraCols
                     res <- cbind(res, res1)
                  }                  
                  res <- rbind(res, xxxx)                         
                }                                                                                 
              }   
              colnames(res) <-  colnames(dataChange[[1]])                                                                                       
              dataChange[[1]] <- rbind(dataChange[[1]], res)
            }
          }  
          dataChange[[1]]$VALUATION_MONTH <- as.character(dataChange[[1]]$VALUATION_MONTH)
          dataChange[[2]]$VALUATION_MONTH <- as.character(dataChange[[2]]$VALUATION_MONTH)
                    
          data <- rbind(merge(dataDelta, dataChange[[1]]), merge(dataVega, dataChange[[2]]))
          indCols <- c((length(colnames(data))-69):length(colnames(data)))
          for (i in 1:length(indCols)) {
            colnames(data)[indCols[i]] <- as.character(format(as.Date(strsplit(colnames(data)[indCols[i]],"_")[[1]][2], 
                                 "%d%b%y"), "%d-%b-%Y"))
            data[,indCols[i]] <- data[,indCols[i]]*data$value
          }
          idList <- c("VALUATION_MONTH","ECONOMIC_OWNER", "COMMODITY", "REGION", "FLOW_TYPE", "GREEK")
          measList <- colnames(data)[indCols]          
          data <- data[,-which(colnames(data) == "value" | colnames(data) == "variable" | colnames(data) == "CURVE_NAME")]
          data <- melt(data, id = idList, measured = measList)
          data <- rbind(data, dataDelta[,-which(colnames(dataDelta) == "CURVE_NAME")],  
                        dataVega[,-which(colnames(dataVega) == "CURVE_NAME")])                            
        } else {
          data <- rbind(dataDelta, dataVega)
          data <- data[,-which(colnames(data) == "CURVE_NAME")]
        } 
          
        res <- classComTime(data, options) 
#        browser()
        for (k in 1:(length(res)/2))                                               
          write.xls(RRR(res[[(2*k-1)]]), fileName, as.character(res[[(2*k)]]), visible=FALSE) 
      }
    }
  }    
}

classComTime <- function(data, options){ 
  # define time table of interest                 
  dates <- sort(as.Date(unique(data$VALUATION_MONTH)))
  indDates <- which(is.na(dates) == FALSE)  
  timeFrameList <- NULL  
  timeFrameName <- NULL           
  qEnd   <- c("Mar", "Jun", "Sep", "Dec")
  currentYear <- as.numeric(format(dates[indDates[1]], "%Y"))
  indCurrentYear <- which(as.numeric(format(dates, "%Y")) == (currentYear))
  for(i in 1:length(indCurrentYear)) {
    timeFrameList <- c(timeFrameList, list(indCurrentYear[i]))
    timeFrameName <- c(timeFrameName,  as.character(format(dates[indCurrentYear[i]], "%b.%y")))
    if(format(dates[indCurrentYear[i]], "%b") %in% qEnd) {
      indStart <- max(indCurrentYear[1], indDates[i] -2)
      timeFrameList <- c(timeFrameList, list(max(indCurrentYear[1], indDates[i] -2):indCurrentYear[i]))
      nameX <- paste("Q", as.character(which(format(dates[indCurrentYear[i]], "%b") == qEnd)), ".", 
                     as.character(format(dates[indDates[i]], "%y")), sep = "")
      timeFrameName <- c(timeFrameName, nameX)
    }  
  }
  for (i in 0:3) {
    indYear <- which(as.numeric(format(dates, "%Y")) == (currentYear+i))
    if(length(indYear) > 0) {
      timeFrameList <- c(timeFrameList, list(indYear))             
      timeFrameName <- c(timeFrameName, paste("FY.", as.character(currentYear+i), sep = ""))
    }  
  }
  
  timeFrameList <- c(timeFrameList, list(which(indDates <= max(indYear))))
  timeFrameName <- c(timeFrameName, paste("Sum", as.character(currentYear), as.character(currentYear+i)))  
  timeFrameList <- c(timeFrameList, list(which(indDates>max(indYear))))
  timeFrameName <- c(timeFrameName, paste("Beyond", as.character(currentYear+i))) 
  timeFrameList <- c(timeFrameList, list(indDates))
  timeFrameName <- c(timeFrameName, "Total")  
  # change time frame  
  formula <- paste(paste(colnames(data)[-which(colnames(data) %in% c("VALUATION_MONTH", "value"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM <- NULL
  for (i in 1:length(timeFrameName)) {
    timeFrame <- data.frame(VALUATION_MONTH = dates[timeFrameList[[i]]])
    if(dim(timeFrame)[1] > 0 ) { 
      MM <- merge(data, timeFrame)
      MMM <- rbind(MMM, data.frame(cast(MM, eval(formula), sum), TIME_FRAME = timeFrameName[i]))                              
    }  
  } 
#  browser()
  # add total
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("value", "FLOW_TYPE"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM1 <- cast(MMM, eval(formula), sum)
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("value", "REGION"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM2 <- cast(MMM, eval(formula), sum)                         
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("REGION", "value", "FLOW_TYPE"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM3 <- cast(MMM, eval(formula), sum)                   
  MMM <- rbind(MMM, data.frame(MMM1, FLOW_TYPE = "Total"))  
  MMM <- rbind(MMM, data.frame(MMM2, REGION = "Total"))    
  MMM <- rbind(MMM, data.frame(MMM3, REGION = "Total", FLOW_TYPE = "Total")) 
  # Sum over Economic Owners if more than 2)  
  EO <- unique(data$ECONOMIC_OWNER) 
  if(length(EO) > 1) {
    formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("ECONOMIC_OWNER", "value"))], 
                           collapse = " + ", sep = ""), " ~ .", sep = "")  
    MMM <- rbind(data.frame(ECONOMIC_OWNER = "ALL", cast(MMM, eval(formula), sum)), MMM)
  }
  # rearange order
  MMM <- rearangeOrder(MMM)
  res <- NULL
  indGreek <- which(MMM$variable == "DELTA" | MMM$variable == "VEGA")
  MMMGREEK <- MMM[ indGreek,]
  MMMPNL   <- MMM[-indGreek,]
  uGreek <- unique(MMMGREEK$GREEK) 
  for( iGreek in 1:length(uGreek)) {
    MMMGREEKx <- subset(MMMGREEK, MMMGREEK$GREEK == uGreek[iGreek])
    formula <- paste(paste(colnames(MMMGREEKx)[-which(colnames(MMMGREEKx) %in% 
                           c("variable", "value", "GREEK", "TIME_FRAME"))], 
                           collapse = " + ", sep = ""), " ~ TIME_FRAME", sep = "") 
    res <- c(res, list(cast(MMMGREEKx, eval(formula), I), uGreek[iGreek]))
    if (options$VAR == TRUE) {
      MMMPNLx <-  subset(MMMPNL, MMMPNL$GREEK == uGreek[iGreek])
      if(dim(MMMPNLx)[1] > 0)
        res <- c(res, list(VAR(MMMPNLx[,-which(colnames(MMMPNLx) == "GREEK")]), paste("VAR", uGreek[iGreek], sep = ""))) 
    }    
  }
  if (options$VAR == TRUE) 
    res <- c(res, list(VAR(MMMPNL[,-which(colnames(MMMPNL) == "GREEK")]), "VAR"))
  return(res)
}

rearangeOrder <- function(MMM) {
  # rearange order
#  browser()
  MMM$TIME_FRAME <- factor(MMM$TIME_FRAME, levels = unique(MMM$TIME_FRAME)) 
  # COM
  listCOM <- unique(MMM$COMMODITY)
  indx <- c(1:length(listCOM))
  indList <- c(which(listCOM == "POWER"), which(listCOM == "CAPACITY"), which(listCOM == "GAS"), 
               which(listCOM == "COAL")) 
  if(length(indList) > 0) {            
    listCOM <- listCOM[c(indList, indx[-c(indList)])]   
    MMM$COMMODITY <- factor(MMM$COMMODITY, levels = listCOM) 
  }  
  # REGION (total  at the end)
  listREG <- sort(unique(MMM$REGION))
  indREG <- c(1:length(listREG))
  indTotal <- which(listREG == "Total")
  listREG <- listREG[c(indREG[-indTotal], indTotal)]
  MMM$REGION <- factor(MMM$REGION, levels = listREG)
  return(MMM)
}

localAggregate <- function(data, GREEK) {
  nCol <- which(colnames(data) == GREEK)
  ind <- which( abs(data[,nCol]) < 0.01 )       
  if(length(ind) > 0 ) { data <- data[-ind,] }   
  colnames(data)[nCol] <- "value" 
  data <- data.frame(cast(data, CURVE_NAME+ECONOMIC_OWNER+COMMODITY+REGION+FLOW_TYPE+VALUATION_MONTH ~ ., sum),
                     GREEK = GREEK, variable = GREEK)
  return(data)                   
} 

VAR <- function(MMMPNL) {
#  browser()
  indTotal <- union(which(MMMPNL$FLOW_TYPE == "Total"), which(MMMPNL$REGION == "Total"))
  formula <- paste(paste(colnames(MMMPNL)[-which(colnames(MMMPNL) %in% 
        c("REGION", "value", "FLOW_TYPE", "COMMODITY"))], 
        collapse = " + ", sep = ""), " ~ .", sep = "")                                                               
  MMMPNL <- rbind(MMMPNL,  data.frame(cast(MMMPNL[-indTotal,], eval(formula), sum), 
                  COMMODITY = "Total", REGION = "Total", FLOW_TYPE = "Total"))                                                  
  # cast to expand over 70 days              
  formula <- paste(paste(colnames(MMMPNL)[-which(colnames(MMMPNL) %in% c("variable", "value"))],     
                       collapse = " + ", sep = ""), " ~ variable", sep = "")  
  nLength <- dim(MMMPNL)[2] - 2
  MMMPNL <- cast(MMMPNL, eval(formula), sum)
  MMMPNL <- data.frame(MMMPNL[,(1:nLength)], 
                       value = apply(MMMPNL[,-(1:nLength)], 1, function(x){4*sd(x, na.rm = TRUE)}))
  formula <- paste(paste(colnames(MMMPNL)[-which(colnames(MMMPNL) %in% c("TIME_FRAME", "value"))], 
                         collapse = " + ", sep = ""), " ~ TIME_FRAME", sep = "") 
  res <- cast(MMMPNL, eval(formula), sum) 
  return(res)  
}

# RRR - Reduce Redundant Rows 
RRR <- function(res) {     
  indFT <- which(res$FLOW_TYPE == "Total" & res$REGION != "Total") 
  indRM <- NULL 
  for (m in 1:length(indFT)) {
    if( length(which(res$ECONOMIC_OWNER == res$ECONOMIC_OWNER[indFT[m]] & 
                     res$COMMODITY == res$COMMODITY[indFT[m]] & res$REGION == res$REGION[indFT[m]])) == 2) {
      indRM <- c(indRM, indFT[m])
    }      
  }
  res <- res[-indRM,] 
#  browser()
  indR <- which(res$REGION == "Total" & res$FLOW_TYPE != "Total") 
  indRM <- NULL
  for(m in 1:length(indR)) {
    if (length(which(res$ECONOMIC_OWNER == res$ECONOMIC_OWNER[indR[m]] & 
                     res$COMMODITY == res$COMMODITY[indR[m]] & res$REGION == res$REGION[indR[m]])) == 2) {
      indRM <- c(indRM, indR[m])       
    }  
  }
  res <- res[-indRM,]
  return(res)
}            