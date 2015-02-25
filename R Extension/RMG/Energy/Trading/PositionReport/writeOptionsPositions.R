# writes options positions to the file

writeOptionsPositions <- function(books, booksToVar, fileName, dateX) {
  con <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")  
  queryR <- paste("select td.VEGA, td.DELTA, td.VALUATION_MONTH, th.EXTERNALDEALID, td.LOCATION_NM, ",
    "th.TRADE_ENTRY_DATE, th.PROFIT_CENTER_1, th.TRADER ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and td.source_feed = 'R' and td.vega != 0 and th.SOURCE_NAME = 'SECDB'", sep="")
  if (length(books)>0){
    books <- paste(books, sep="", collapse="', '")
    queryR <- paste(queryR, " and th.PROFIT_CENTER_1 in ('", books, "')", sep="")
  }
  dataR <- sqlQuery(con, queryR)
  EDIDR <- unique(dataR$EXTERNALDEALID)
  queryG <- paste("select td.BUY_SELL_TYPE, td.PRICE_USD, td.QUANTITY, td.VALUATION_MONTH, th.PROFIT_CENTER_1, ",
    "td.CALL_PUT_TYPE, td.STRIKE, th.TRADER, th.EXTERNALDEALID, th.TRADE_ENTRY_DATE, td.LOCATION_NM ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and td.source_feed = 'G' and td.option_flag = 'Y' and td.QTY_UOM != 'USD' ",
    "and td.quantity != 0 and th.SOURCE_NAME = 'SECDB'", sep="")
  if (length(EDIDR)>0){
    EDIDR <- paste(EDIDR, sep="", collapse="', '")
    queryG <- paste(queryG, " and th.EXTERNALDEALID in ('", EDIDR, "')", sep="")
  }
  dataG <- sqlQuery(con, queryG)
  close(con)  
  dataG$QUANTITY <- ifelse(dataG$BUY_SELL_TYPE == "S", -abs(dataG$QUANTITY), abs(dataG$QUANTITY))

  # match dates 
  EDIDR <- unique(dataR$EXTERNALDEALID)  
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
  data <- merge(dataR, dataG)
  colnames(data)[which(colnames(data) == "PROFIT_CENTER_1")] <- "BOOK"
  data <- merge(data,booksToVar)
  write.table(data, file = fileName, append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  return(data)
}