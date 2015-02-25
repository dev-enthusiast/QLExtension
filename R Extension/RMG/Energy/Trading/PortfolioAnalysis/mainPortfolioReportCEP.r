# see demoPortfolioReport
# mainPortfolioReport(options)
# Anatoley Zheleznyak 11/16/2006

mainPortfolioReportCEP <- function(options) {
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchy.R")  
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.portfolio.hierarchyCCG.R")  
  source("H:/user/R/RMG/Utilities/extractDeltaVega.R")
  source("H:/user/R/RMG/Utilities/extractDeltaVegaA.R")
  source("H:/user/R/RMG/Utilities/mapCurveName2CommRegionType.R")
  source("H:/user/R/RMG/Utilities/bookMappingCPSPROD.R")  
  source("H:/user/R/RMG/Utilities/write.xls.R") 
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesCVSSAS.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesSAS.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/readPriceVolChangesRData.R")
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")     
  source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.prices.R")
  source("H:/user/R/RMG/Energy/Trading/PortfolioAnalysis/make.pos.rpt.by.region.R")
  source("H:/user/R/RMG/Utilities/get.pm.books.R")
      
  fileNameBooks <- "//ceg/cershare/Risk/Accrual Risk/AccrualBooksB.xls"
  
  cat("Starting mainPortfolioReportE \n")
  for ( i in 1:length(options$portfolio) ) {
    for(j in 1:length(options$dates)) {
      cat(paste("portfolio : ", options$portfolio[i], "date : ", options$dates[j], " \n", sep = ""))
      options$asOfDate <- options$dates[j]
      options$dirOut <- paste("//ceg/cershare/Risk/Portfolio Management/Reports/PositionVaRReports/", as.character(options$asOfDate), "/", sep = "")
      dir.create(options$dirOut,  showWarnings = FALSE)

      fileName     <- paste(options$dirOut, "Positions ", options$portfolio[i], " ",
                                    as.character(options$asOfDate), "_CEP.xls", sep = "")  
      fileNamePos  <- paste(options$dirOut, "Positions ", options$portfolio[i], " ",
                                    as.character(options$asOfDate), "POS_CEP.xls", sep = "")                                                                              
          
        #books <- get.pm.books()
        options$books <- get.portfolio.hierarchy("CEP Overall Portfolio", options$asOfDate)
        
        # Debug
        cat("calling  extractDeltaVegaA \n")  
        data <- extractDeltaVegaA(options$books, options$asOfDate)
        indNA <- which(is.na(data$CURVE_NAME))
        if(length(indNA) == 1) { data$CURVE_NAME[indNA] <- "COMMOD FRT ROUTE4 BCI" }
#        browser()
        # Debug
        cat("calling  bookMappingCPSPROD \n")
        dataBookMapping <- bookMappingCPSPROD(options) # adds ECONOMIC_OWNER
        data <- merge(data,dataBookMapping)
        # Debug
        cat("calling  mapCurveName2CommRegionType \n")
        dataCommRegType <- mapCurveName2CommRegionType(unique(data$CURVE_NAME))        
        data <- merge(data, dataCommRegType)
      
        
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
#        browser()
        # remove ANCILLARIES
        indANC <- which(data$COMMODITY == "ANCILLARIES")        
        if(length(indANC) > 0 ) {data <- data[-indANC,]}
        # Delta & Vega aggregate       
        dataDelta <- localAggregate(data, "DELTA")               
        dataVega  <- localAggregate(data, "VEGA") 
#        browser()
        
        cat("calling  readPriceVolChangesRData \n")
        dataChange <- readPriceVolChangesRData(options$portfolio[i], options$asOfDate)        
#        browser()
        if(is.null(dataChange[[1]]) == TRUE) {        
          options$VAR = FALSE
          data <- rbind(dataDelta, dataVega)
          data <- data[,-which(colnames(data) == "CURVE_NAME")]          
        } else {
          options$VAR = TRUE
# This line may need to be commented.
          # Debug
          cat("calling  extractAdditionalPriceVolDiff \n")
          dataChange[[1]] <- extractAdditionalPriceVolDiff(dataDelta, dataChange[[1]], options$asOfDate, "NA")
#          browser()
          if (!is.null(dataChange[[1]])) {
            dataChange[[1]]$VALUATION_MONTH <- as.character(dataChange[[1]]$VALUATION_MONTH)
            dataChange[[2]]$VALUATION_MONTH <- as.character(dataChange[[2]]$VALUATION_MONTH)          
            data <- rbind(merge(dataDelta, dataChange[[1]]), merge(dataVega, dataChange[[2]]))  
          }
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
#        browser()
        
        dataChange <- NULL
        dataDelta <- NULL
        dataVega <- NULL
        gc()
        res <- classComTime(data, options) 
        
        for (k in 1:(length(res)/2))                                               
          write.xls(RRR(res[[(2*k-1)]]), fileName, as.character(res[[(2*k)]]), visible=FALSE)     
      #}
    }
#    browser()
    # Debug
    cat("Deal with multiple days \n")
    nListFields <- length(options$listFields)
    if(length(options$dates) > 1) {
      AA <- NULL
      for(j in 1:length(options$dates)) {
        options$dirOut <- paste("//ceg/cershare/Risk/2007,Q1/Risk.Analytics/Accrual/", as.character(options$dates[j]), "/", sep = "")
        fileName[j]     <- paste(options$dirOut, "Positions ", options$portfolio[i], " ",
                                    as.character(options$dates[j]), ".xls", sep = "") 
        con    <- odbcConnectExcel(fileName[j])
        for (k in 1:nListFields)
          AA     <- c(AA, list(sqlFetch(con, options$listFields[k])))                                    
      }                                                                     
      odbcCloseAll() 
      for (i in 1:length(AA)) {          
        AA[[i]] <- melt(AA[[i]])
        indNA <- which(is.na(AA[[i]]$value))
        if(length(indNA) >0) {AA[[i]] <- AA[[i]][-indNA,]}
        if (i <= nListFields)
          AA[[i]]$value <- -1*AA[[i]]$value
      }
      for (i in 1:nListFields) {
        AA[[i]] <- rbind(AA[[i]], AA[[i + nListFields]])
        colnames(AA[[i]])[which(colnames(AA[[i]]) == "variable")] <- "TIME_FRAME" 
        AA[[i]] <- rearangeOrder(AA[[i]])
        AA[[i]] <- cast(AA[[i]], ECONOMIC_OWNER + COMMODITY + REGION + FLOW_TYPE ~ TIME_FRAME, sum)
        sheetName <- paste("Change", options$listFields[i], 
          as.character(format(options$dates[(length(options$dates)-1)], "%d%b%y")),sep ="")        
        write.xls(AA[[i]], fileName[length(options$dates)], sheetName, visible=FALSE)
      }
    }
  }    
  # Debug
  #make.pos.rpt.by.region(dataByRegion, fileNamePos)
  dataPower = subset(dataByRegion, COMMODITY=="POWER")
  colnames(dataPower)[4] <- "value"
  powerDelta = cast(dataPower, COMMODITY + VALUATION_MONTH + BOOK + ECONOMIC_OWNER + EXECUTIVE_OWNER + REGION + SUBREGION + OFFICE + FLOW_TYPE ~ ., sum)
  
  colnames(powerDelta)[10] <- "DELTA"
  
  colnames(dataPower)[4] <- "Delta"
  colnames(dataPower)[3] <- "value"
  powerVega <- cast(dataPower, COMMODITY + VALUATION_MONTH + BOOK + ECONOMIC_OWNER + EXECUTIVE_OWNER + REGION + SUBREGION + OFFICE + FLOW_TYPE ~ ., sum)
  
  colnames(powerVega)[10] <- "VEGA"
  powerRes <- merge(powerDelta, powerVega)
  
  dataOther = subset(dataByRegion, COMMODITY!="POWER")
  colnames(dataOther)[4] <- "value"
  otherDelta = cast(dataOther, COMMODITY + VALUATION_MONTH + BOOK + ECONOMIC_OWNER + EXECUTIVE_OWNER + REGION + SUBREGION + OFFICE + FLOW_TYPE ~ ., sum)
  
  colnames(otherDelta)[10] <- "DELTA"
  
  colnames(dataOther)[4] <- "Delta"
  colnames(dataOther)[3] <- "value"
  otherVega <- cast(dataOther, COMMODITY + VALUATION_MONTH + BOOK + ECONOMIC_OWNER + EXECUTIVE_OWNER + REGION + SUBREGION + OFFICE + FLOW_TYPE ~ ., sum)
  
  colnames(otherVega)[10] <- "VEGA"
  
  otherRes <- merge(otherDelta, otherVega)
  otherRes$VALUATION_MONTH <- as.character(otherRes$VALUATION_MONTH)
  powerRes$VALUATION_MONTH <- as.character(powerRes$VALUATION_MONTH)
  #write.xls(powerRes, fileNamePos, "Power", visible=FALSE)
  #write.xls(otherRes, fileNamePos, "Others", visible=FALSE)
  cat("Ending mainPortfolioReportE \n")
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
  if(length(EO) > 1) {
    formula <- paste(paste(colnames(MMM)[-which(colnames(MMM) %in% c("ECONOMIC_OWNER", "value"))], 
                           collapse = " + ", sep = ""), " ~ .", sep = "")  
    dfM <-  data.frame(ECONOMIC_OWNER = "ALL", cast(MMM, eval(formula), sum))
    colnames(dfM) <- c("ECONOMIC_OWNER", "COMMODITY", "REGION", "FLOW_TYPE", "GREEK", "variable", "TIME_FRAME", "value")
    MMM <- rbind(dfM, MMM)
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
  # Debug
  cat("Starting rearangeOrder \n")
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
  # Debug
  cat("Ending rearangeOrder \n")
  return(MMM)
}

localAggregate <- function(data, GREEK) {
#  browser()
  # Debug
  cat("Starting localAggregate \n")
  nCol <- which(colnames(data) == GREEK)
  indZero <- which( abs(data[,nCol]) < 0.01 )       
  if(length(indZero) > 0 ) { data <- data[-indZero,] }
  indNA <- which( is.na(data[,nCol]) )       
  if(length(indNA) > 0 ) { data <- data[-indNA,] }     
  colnames(data)[nCol] <- "value" 
  data <- data.frame(cast(data, CURVE_NAME+ECONOMIC_OWNER+COMMODITY+REGION+FLOW_TYPE+VALUATION_MONTH ~ ., sum),
                     GREEK = GREEK, variable = GREEK)
  colnames(data) <- c("CURVE_NAME", "ECONOMIC_OWNER", "COMMODITY",   "REGION","FLOW_TYPE", "VALUATION_MONTH", "value", "GREEK", "variable")
  ind0 <- which(abs(data$value) < 0.01)
  if (length(ind0) > 0) {data <- data[-ind0,]}  
  # Debug

  cat("Ending localAggregate \n")                 
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

RRR <- function(res) {    
  # Debug
  cat("Startinging RRR \n") 
  indFT <- which(res$FLOW_TYPE == "Total" & res$REGION != "Total") 
  indRM <- NULL 
  for (m in 1:length(indFT)) {
    if( length(which(res$ECONOMIC_OWNER == res$ECONOMIC_OWNER[indFT[m]] & 
                     res$COMMODITY == res$COMMODITY[indFT[m]] & res$REGION == res$REGION[indFT[m]])) == 2) {
      indRM <- c(indRM, indFT[m])
    }      
  }
  if(length(indRM) > 0) {res <- res[-indRM,] }
#  browser()
  indR <- which(res$REGION == "Total" & res$FLOW_TYPE != "Total") 
  indRM <- NULL
  for(m in 1:length(indR)) {
    if (length(which(res$ECONOMIC_OWNER == res$ECONOMIC_OWNER[indR[m]] & 
                     res$COMMODITY == res$COMMODITY[indR[m]] & res$REGION == res$REGION[indR[m]])) == 2) {
      indRM <- c(indRM, indR[m])       
    }  
  }
  if(length(indRM) > 0 ) { res <- res[-indRM,] }
  # Debug
  cat("Ending RRR \n")
  return(res)
}     

extractAdditionalPriceVolDiff <- function(data1, data2, asOfDate, typeChar) {
#  browser()
  # Debug
  cat("Starting extractAdditionalPriceVolDiff \n")
  #data2 <- data2[,1:72]
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
#    hPP <- aggregate.run.vols.mkt(QQp, options)  
    load(options$save$curve.info)
    load(options$save$fx.file)
    hPP <- convert.prices.to.USD(hPP, den, hFX)  
    indDates <- grep("-",names(hPP))
    aux <- as.matrix(hPP[, indDates])
    aux <- aux[,2:ncol(aux)] - aux[,1:(ncol(aux)-1)]          
    hPPX <- cbind(hPP[,1:2], aux) 
    colnames(hPPX) <- colnames(data2)
    data2$VALUATION_MONTH <- as.character(data2$VALUATION_MONTH)
    hPPX$VALUATION_MONTH <- as.character(hPPX$VALUATION_MONTH)
  
    data2 <- rbind(data2, hPPX)                      
  }
  
  # Debug
  cat("Ending extractAdditionalPriceVolDiff \n")
  return(data2)
}
       