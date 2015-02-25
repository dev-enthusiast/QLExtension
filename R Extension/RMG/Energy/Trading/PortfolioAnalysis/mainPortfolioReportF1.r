# see demoPortfolioReport
# mainPortfolioReport(options)
# Anatoley Zheleznyak 11/16/2006

mainPortfolioReportF1 <- function(options) {
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")  
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchyCCG.R")  
  source("H:/user/R/RMG/Utilities/extractDeltaVega.R")
  source("H:/user/R/RMG/Utilities/extractDeltaVegaA.R")
  source("H:/user/R/RMG/Utilities/mapCurveName2CommRegionType.R")
  source("H:/user/R/RMG/Utilities/bookMapping.R")  
  source("H:/user/R/RMG/Utilities/write.xls.R") 
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesCVSSAS.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesSAS.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesRData.R") 
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R") 
  source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.prices.R")   
  fileNameBooks <- "S:/Risk/Accrual Risk/AccrualBooksB.xls"
  for ( i in 1:length(options$portfolio) ) {
    for(j in 1:length(options$dates)) {                            
      options$asOfDate <- options$dates[j]
      options$dirOut <- paste("S:/Risk/Portfolio Management/Reports/PositionVaRReports/", 
                              as.character(options$asOfDate), "/", sep = "")
      dir.create(options$dirOut,  showWarnings = FALSE)
      fileName     <- paste(options$dirOut, "Positions ", options$portfolio[i], " ",
                                    as.character(options$asOfDate), "Gandhi_X.xls", sep = "")                                
      #if(file.exists(fileName) == FALSE) {
        # Attrition
        bP <- c(get.portfolio.hierarchy("Nepool Mixed Attr Portfolio", options$asOfDate), "NPGASVOL", "NPVLRVOL")        
        bM1 <- get.portfolio.hierarchy("Gandhi Attrition Extra", options$asOfDate)
        bM2 <- get.portfolio.hierarchy("Gandhi Congestion Extra", options$asOfDate)        
        bM3 <- get.portfolio.hierarchy("Gandhi Cash", options$asOfDate)
        bM4 <- get.portfolio.hierarchy("Gandhi Cash Extra", options$asOfDate)        
        bAttr <- setdiff(bP, unique(c(bM1,bM2,bM3,bM4))) 
        data1 <- extractAllData(bAttr, options)
        if (!is.null(data1))  {
          data1$ECONOMIC_OWNER <- "Gandhi Attrition"
        }
#        browser()
        # Gandhi East Cash 
        bEastCash <- unique(c(get.portfolio.hierarchy("Gandhi GTC Cash", options$asOfDate), 
          get.portfolio.hierarchy("Gandhi NE Cash", options$asOfDate), 
          get.portfolio.hierarchy("Gandhi SE Cash", options$asOfDate)))
        data2 <- extractAllData(bEastCash, options)   
        if (!is.null(data2))  {                        
          data2$ECONOMIC_OWNER <- "Gandhi East Cash"  
        }
        # Gandhi East Vol
        bEastVol <- unique(c(get.portfolio.hierarchy("Gandhi Attrition", options$asOfDate), 
          get.portfolio.hierarchy("Gandhi GTC Vol", options$asOfDate)))  
        data3 <- extractAllData(bEastVol, options)  
        if (!is.null(data3))  {                         
          data3$ECONOMIC_OWNER <- "Gandhi East Vol"
        }
        # Fixed Price
        bP <- unique(c(get.portfolio.hierarchy("GTC Fixed Price Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("Nepool FP Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("Ginna Unit Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("NMP1 Unit Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("NMP2 Unit Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("NYPP Hedge Accrual Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("NYPP NEV Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("NYPP Other Unit Portfolio", options$asOfDate),
          get.portfolio.hierarchy("SE FP Portfolio", options$asOfDate),
          get.portfolio.hierarchy("Nepool MTM Other Hedges", options$asOfDate), "MCPJM2"))                              
       # bM <- unique(c(get.portfolio.hierarchy("Gandhi Fixed Price Extra", options$asOfDate), 
       bM <- c("GINNAOPS", "NMP1OPS", "NMP2OPS", "HQOPS", "NYCASHDG", 
          "NPCASHVL", "NPGASVOL", "NPVLRHDG", 
          "SEMTM", "NSBHEMTM", "NSCMPMTM", "VOLHEDGS")
        bFP <- setdiff(bP, bM)
        bExclude <- 'NPVLRVOL'
        bFP <- setdiff(bFP, bExclude)
        indx <- which(bFP == "NPGASOPT")
        indx1 <- indx - 1
        indx2 <- indx + 1
        indx3 <- length(bFP)
        bFP <- c(bFP[1:indx1], bFP[indx2:indx3])
        
          data4 <- extractAllData(bFP, options)    
      
        if (!is.null(data4))  {                      
          data4$ECONOMIC_OWNER <- "Gandhi FP"
        }
        # Gandhi GTC Cash
        bGTC_Cash  <- setdiff(unique(c(get.portfolio.hierarchy("GTC Cash Portfolio", options$asOfDate), "SEMTM")), 
          get.portfolio.hierarchy("Gandhi GTC Portfolio", options$asOfDate)) 
        data5 <- extractAllData(bGTC_Cash, options) 
        if (!is.null(data5))  {                          
          data5$ECONOMIC_OWNER <- "Gandhi GTC Cash"   
        }       
        # Gandhi GTC Vol 
        bGTC_Vol  <- get.portfolio.hierarchy("GTC Accrual Vol Portfolio", options$asOfDate)
        
          data6 <- extractAllData(bGTC_Vol, options)   
        if (!is.null(data6))  {                       
          data6$ECONOMIC_OWNER <- "Gandhi GTC Vol"  
        }                   
        # NE Cash
        bNE_Cash  <- setdiff(unique(c(get.portfolio.hierarchy("Nepool Cash Accrual Portfolio", options$asOfDate), 
          "NYCASHDG", "NPCASHVL", "NPVLRHDG")), 
          get.portfolio.hierarchy("Gandhi Old Settlements", options$asOfDate)) 
        data7 <- extractAllData(bNE_Cash, options)
        if (!is.null(data7))  {
          data7$ECONOMIC_OWNER <- "Gandhi NE Cash"  
        }      
        # Gandhi Other
        bOther  <- unique(c(get.portfolio.hierarchy("GTC Gas Portfolio", options$asOfDate),
          get.portfolio.hierarchy("GTC Other Portfolio", options$asOfDate),
          get.portfolio.hierarchy("Nepool Congestion Portfolio", options$asOfDate),
          get.portfolio.hierarchy("Nepool ISO Settlement Portfolio", options$asOfDate), 
          get.portfolio.hierarchy("Nepool Old Books Portfolio", options$asOfDate),
          get.portfolio.hierarchy("Nepool Other Portfolio", options$asOfDate),
          get.portfolio.hierarchy("NYPP Old Books Portfolio", options$asOfDate),
          get.portfolio.hierarchy("NYPP Other Accrual Portfolio", options$asOfDate),
          get.portfolio.hierarchy("SE Other Portfolio", options$asOfDate),
          "NSBHEMTM", "NSCMPMTM", "VOLHEDGS", "GINNAOPS", "NMP1OPS", "NMP2OPS", "HQOPS", "NPGASOPT",
          "MDLGATOL", "MDLGECEN", "MDLGECLD", "MDLGECRS", "MDLGTCCP", "MDLGTCNG", "MDLGTCPW", 
          "MDLGTCTR", "MDLJCKEN", "MDLJCKLD", "MDLJCKRS", "MDLSONTR" ))
        data8 <- extractAllData(bOther, options)
        if (!is.null(data8))  {
          data8$ECONOMIC_OWNER <- "Gandhi Other"
        }
        # SE Cash
        bSE_Cash <-  get.portfolio.hierarchy("SE Cash Portfolio", options$asOfDate)
        data9 <- extractAllData(bSE_Cash, options)  
        if (!is.null(data9))  {                         
          data9$ECONOMIC_OWNER <- "Gandhi SE Cash"
        }
        # Spec Congestion
        #data10 <- NULL
        #bSpecCong <-  get.portfolio.hierarchy("Jaya Bajpai Trading", options$asOfDate)
        #data10 <- extractAllData(bSpecCong, options)   
        #if (!is.null(data10))  {                        
        #  data10$ECONOMIC_OWNER <- "Gandhi Spec Congestion"
        #}
        # Gandhi GTC Vol
        #bGTC_vol <-  get.portfolio.hierarchy("GTC Accrual Vol Portfolio", options$asOfDate)
        #data11 <- extractAllData(bGTC_Vol, options)
        #data11$ECONOMIC_OWNER <- "Gandhi GTC Vol"
        # Gandhi PAC
        bGandhi_FAC <-  get.portfolio.hierarchy("Gandhi FAC Portfolio", options$asOfDate)
        data11 <- extractAllData(bGandhi_FAC, options)
        if (!is.null(data11))  {
          data11$ECONOMIC_OWNER <- "Gandhi FAC"
        }
        # Gandhi Total East       
        bTotalEast <- unique(c(bEastCash, bEastVol, bFP,  bGandhi_FAC, bOther))
        data12 <- extractAllData(bTotalEast, options)
        if (!is.null(data12))  {
          data12$ECONOMIC_OWNER <- "Gandhi Total East Portfolio"
        }
        data <- rbind(data1,data2,data3,data4,data5,data6,data7,data8,data9,data11,data12)
#        browser()
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
        dataChange <- readPriceVolChangesRData(options$portfolio[i], options$asOfDate) 
        if(is.null(dataChange[[1]]) == TRUE) {        
          options$VAR = FALSE
          data <- rbind(dataDelta, dataVega)
          data <- data[,-which(colnames(data) == "CURVE_NAME")]          
        } else {
          options$VAR = TRUE
          dataChange[[1]] <- extractAdditionalPriceVolDiff(dataDelta, dataChange[[1]], options$asOfDate, "NA")
#          browser()
#          dataChange[[1]]$VALUATION_MONTH <- as.character(dataChange[[1]]$VALUATION_MONTH)
          dataChange[[2]]$VALUATION_MONTH <- as.character(dataChange[[2]]$VALUATION_MONTH) 
          colnames(dataVega) <- c("CURVE_NAME","ECONOMIC_OWNER","COMMODITY","REGION","FLOW_TYPE", "VALUATION_MONTH","value","GREEK","variable")  
          colnames(dataDelta) <- c("CURVE_NAME","ECONOMIC_OWNER","COMMODITY","REGION","FLOW_TYPE", "VALUATION_MONTH","value","GREEK","variable")       
          data <- rbind(merge(dataDelta, dataChange[[1]]), merge(dataVega, dataChange[[2]]))
          indCols <- c((length(colnames(data))-69):length(colnames(data)))
          for (i in 1:length(indCols)) 
            data[,indCols[i]] <- data[,indCols[i]]*data$value
          idList <- c("VALUATION_MONTH","ECONOMIC_OWNER", "COMMODITY", "REGION", "FLOW_TYPE", "GREEK")
          measList <- colnames(data)[indCols] 

          data <- data[,-which(colnames(data) == "value" | colnames(data) == "variable" | colnames(data) == "CURVE_NAME")]
          data <- melt(data, id = idList, measured = measList)
          data <- rbind(data, dataDelta[,-which(colnames(dataDelta) == "CURVE_NAME")],  
                         dataVega[,-which(colnames(dataVega) == "CURVE_NAME")])           
        }
        data$VALUATION_MONTH <- as.Date(data$VALUATION_MONTH)
        res <- classComTime(data, options) 
#        browser()
        for (k in 1:(length(res)/2))                                               
          write.xls(RRR(res[[(2*k-1)]]), fileName, as.character(res[[(2*k)]]), visible=FALSE) 
      #}
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
  for (i in 0:4) {
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
      #MMMpre <- data.frame(cast(MM, eval(formula), sum) )
      #colnames(MMMpre) <- c("ECONOMIC_OWNER","COMMODITY","REGION","FLOW_TYPE","GREEK","variable","value")
      MMM <- rbind(MMM, data.frame(cast(MM, eval(formula), sum), TIME_FRAME = timeFrameName[i])) 
                                
    }  
  } 
#  browser()
  # add total
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("X.all.", "FLOW_TYPE"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM$X.all. <- as.numeric(MMM$X.all.)
  colnames(MMM) <- c("ECONOMIC_OWNER","COMMODITY","REGION","FLOW_TYPE","GREEK","variable","value", "TIME_FRAME")
  MMM1 <- cast(MMM, eval(formula), sum)
  dfMMM1 <- data.frame(MMM1, FLOW_TYPE = "Total")
  colnames(dfMMM1) <- c("ECONOMIC_OWNER","COMMODITY","REGION","GREEK", "variable", "TIME_FRAME", "value", "FLOW_TYPE")
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("value", "REGION"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM2 <- cast(MMM, eval(formula), sum)  
  dfMMM2 <- data.frame(MMM2, REGION = "Total")
  colnames(dfMMM2) <- c("ECONOMIC_OWNER","COMMODITY","FLOW_TYPE","GREEK", "variable", "TIME_FRAME", "value", "REGION")                       
  formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("REGION", "value", "FLOW_TYPE"))], 
                         collapse = " + ", sep = ""), " ~ .", sep = "")
  MMM3 <- cast(MMM, eval(formula), sum) 
  dfMMM3 <- data.frame(MMM3, REGION = "Total", FLOW_TYPE = "Total")
  colnames(dfMMM3) <- c("ECONOMIC_OWNER","COMMODITY","GREEK", "variable", "TIME_FRAME", "value","REGION", "FLOW_TYPE")                  
  MMM <- rbind(MMM, dfMMM1)  
  MMM <- rbind(MMM, dfMMM2)   

  MMM <- rbind(MMM, dfMMM3) 
  # Sum over Economic Owners if more than 2)  
  EO <- unique(data$ECONOMIC_OWNER) 
  if(length(EO) > 100) {
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
  dfMPNL <-  data.frame(cast(MMMPNL[-indTotal,], eval(formula), sum), 
                  COMMODITY = "Total", REGION = "Total", FLOW_TYPE = "Total")
  colnames(dfMPNL) <- c("ECONOMIC_OWNER","variable","TIME_FRAME", "value","COMMODITY","REGION","FLOW_TYPE" )                                                            
  MMMPNL <- rbind(MMMPNL,  dfMPNL)                                                  
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

extractAdditionalPriceVolDiff <- function(data1, data2, asOfDate, typeChar) {
#  browser()
  QQ1 <- unique(paste(data1$CURVE_NAME, data1$VALUATION_MONTH, sep = "__"))
  QQ2 <- unique(paste(data2$CURVE_NAME, data2$VALUATION_MONTH, sep = "__"))
  dQQ <- setdiff(QQ1, QQ2)
  if(length(dQQ) > 0) { 
    dQQX <- unlist(strsplit(dQQ, "__"))
    ind1 <- seq(1, length(dQQX), 2)
    ind2 <- seq(2, length(dQQX), 2) 
    QQp <- data.frame(curve.name = dQQX[ind1], contract.dt = as.Date(dQQX[ind2]), vol.type = typeChar, position = 0)
    QQp$curve.name <- as.character(QQp$curve.name)
    options <- overnightUtils.loadOvernightParameters(asOfDate, connectToDB=FALSE)
    hPP <- aggregate.run.prices.mkt(QQp, options)  
    load(options$save$curve.info)
    load(options$save$fx.file)
    hPP <- convert.prices.to.USD(hPP, den, hFX)  
    indDates <- grep("-",names(hPP))
    aux <- as.matrix(hPP[, indDates])
    aux <- aux[,2:ncol(aux)] - aux[,1:(ncol(aux)-1)]          
    hPPX <- cbind(hPP[,1:2], aux)                       
  }
  colnames(hPPX) <- colnames(data2)
  data2$VALUATION_MONTH <- as.character(data2$VALUATION_MONTH)
  hPPX$VALUATION_MONTH <- as.character(hPPX$VALUATION_MONTH)
  
  data2 <- rbind(data2, hPPX)
  return(data2)
}    

extractAllData <- function(books, options) {
  options$books <- books 
  data <- NULL
  data <- extractDeltaVegaA(options$books, options$asOfDate)  
  if (!is.null(data)) {                         
    dataBookMapping <- bookMapping(options) # adds ECONOMIC_OWNER
    data <- merge(data,dataBookMapping)
    dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
    data <- merge(data, dataCommRegType) 
  } else {
    data <- NULL
  }
  return(data)
}         