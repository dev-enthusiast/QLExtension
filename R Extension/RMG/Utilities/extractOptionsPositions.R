
extractOptionsPositions <- function(books, dateX) {
  browser()
  con <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")  
  queryRCore <- paste("select td.VEGA, td.DELTA, td.VALUATION_MONTH, th.EXTERNALDEALID, td.LOCATION_NM, ",
    "th.TRADE_ENTRY_DATE, th.PROFIT_CENTER_1, th.TRADER ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and td.source_feed = 'R' and td.vega != 0 and th.SOURCE_NAME = 'SECDB'", sep="")
  dataR <- NULL
  iIn <- 0
  while(iIn*options$nFieldLimit < length(books)) {
    booksX <- paste(books[(iIn*options$nFieldLimit+1):min(length(books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
    queryR <- paste(queryRCore, " and th.PROFIT_CENTER_1 in ('", booksX, "')", sep="")
    dataR <- rbind(dataR, sqlQuery(con, queryR))
    iIn <- iIn + 1
  }
    
  EDIDR <- unique(dataR$EXTERNALDEALID)
  if (length(EDIDR)>0){
    queryGCore <- paste("select td.BUY_SELL_TYPE, td.PRICE_USD, td.QUANTITY, td.VALUATION_MONTH, th.PROFIT_CENTER_1, ",
      "td.CALL_PUT_TYPE, td.STRIKE, th.TRADER, th.EXTERNALDEALID, th.TRADE_ENTRY_DATE, td.LOCATION_NM ",
      "from vcentral.trade_detail td, vcentral.trade_header th ",
      "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
      "and td.valuation_date = th.valuation_date ",
      "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
      "and td.source_feed = 'G' and td.option_flag = 'Y' and td.QTY_UOM != 'USD' ",
      "and td.quantity != 0 and th.SOURCE_NAME = 'SECDB'", sep="")       
    dataG <- NULL
    iIn <- 0
    while(iIn*options$nFieldLimit < length(EDIDR)) {  
      EDIDRX <- paste(EDIDR[(iIn*options$nFieldLimit+1):min(length(EDIDR),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
      queryG <- paste(queryGCore, " and th.EXTERNALDEALID in ('", EDIDRX, "')", sep="")
      dataG <- rbind(dataG, sqlQuery(con, queryG))
      iIn <- iIn + 1
    }  
    dataG$QUANTITY <- ifelse(dataG$BUY_SELL_TYPE == "S", -abs(dataG$QUANTITY), abs(dataG$QUANTITY))
    # match dates 
    EDIDG <- unique(dataG$EXTERNALDEALID)
    dataR$VALUATION_MONTH <- as.Date(dataR$VALUATION_MONTH)
    dataG$VALUATION_MONTH <- as.Date(dataG$VALUATION_MONTH)
    for (i in 1: length(EDIDR)) {
      indR <- which(dataR$EXTERNALDEALID == EDIDR[i])
      indG <- which(as.character(dataG$EXTERNALDEALID) == as.character(EDIDR[i]))
      if (length(indG) > 0 ) {
        dRange <- range(as.Date(dataR$VALUATION_MONTH[indR])) - range(as.Date(dataG$VALUATION_MONTH[indG]))
        if( dRange[1] < 32 & dRange[1] > 27 & dRange[2] < 32 & dRange[2] > 27) 
          dataG$VALUATION_MONTH[indG] <- as.Date(paste(format(dataG$VALUATION_MONTH[indG] + 31, "%Y-%m"),"-01",sep = ""))       
      }
    } 
    # merge 
#    browser()
    if(dim(dataR)[1] == dim(dataG)[1]) {
      data <- merge(dataR, dataG, all.x=T) 
    } else {
      data <- NULL
      for(i in 1:length(EDIDR)) {
        indR1 <- which(dataR$EXTERNALDEALID == EDIDR[i]) 
        indG1 <- which(dataG$EXTERNALDEALID == EDIDR[i]) 
        if( (length(indR1) == length(indG1)) | (sum(dataG$QUANTITY[indG1]) == 0) ) {
          data <- rbind(data, merge(dataR[indR1,], dataG[indG1,]))
        } else {
          lengthR1 <- length(unique(dataR$VALUATION_MONTH[indR1]))
          lengthG1 <- length(unique(dataG$VALUATION_MONTH[indG1]))          
          if( min(lengthR1,lengthG1)/max(lengthR1,lengthG1) < 0.8 ) {
            indDrop <- which(colnames(dataG) == "VALUATION_MONTH") 
            if(is.na(dataG$LOCATION_NM[indG1[1]]) == TRUE) 
              indDrop <- c(indDrop, which(colnames(dataG) == "LOCATION_NM"))       
            dataGX <- dataG[indG1,-indDrop] 
            dataGX$QUANTITY <- dataGX$QUANTITY*lengthG1/lengthR1 
            dataRX <- dataR[indR1,]
            redFactor <- lengthG1*length(unique(dataGX$STRIKE))/length(unique(dataGX$LOCATION_NM))
            dataRX$DELTA <- dataRX$DELTA/redFactor
            dataRX$VEGA <- dataRX$VEGA/redFactor
            data <- rbind(data, merge(dataRX, dataGX))
          } else {
            data <- rbind(data, merge(dataR[indR1,], dataG[indG1,]))
          }
#          browser()
        }
      }  
    }
         
      
    colnames(data)[which(colnames(data) == "PROFIT_CENTER_1")] <- "BOOK"
  } else {
    data <- NA
  }
  close(con)  
  return(data)
}