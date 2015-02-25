mainOptionReport <- function(options) {
  require(RODBC)
  require(reshape)

  options$dir <- "H:/user/R/RMG/Energy/Trading/DailyOptionReport/Data/"
  options$portfolio <- "Power Trading Portfolio"
  options$fileAsOfDate     <- paste(options$dir, "OptionsPositions_", as.character(options$asOfDate), ".csv", sep = "")
  options$filePreviousDate <- paste(options$dir, "OptionsPositions_", as.character(options$previousDate), ".csv", sep = "")   
  if(file.exists(options$fileAsOfDate) == FALSE) {
    dataAsOfDate <- writeOptionPosition(options, options$fileAsOfDate, options$asOfDate)
  } else {
    dataAsOfDate <- read.csv(options$fileAsOfDate)
  }

  if(file.exists(options$filePreviousDate) == FALSE) {
    dataPreviousDate <- writeOptionPosition(options, options$filePreviousDate, options$previousDate)
  } else {
    dataPreviousDate <- read.csv(options$filePreviousDate)
  }
  # subset trader if defined
#  browser()
  if(is.na(options$trader) == FALSE ) {
    dataAsOfDate     <- subset(dataAsOfDate, TRADER == options$trader)
    dataPreviousDate <- subset(dataPreviousDate, TRADER == options$trader)   
  }
    
  # Delta & Vega report asOfDate
  resDelta     <- aggregateLocStrMon(dataAsOfDate, "DELTA")
  resVega      <- aggregateLocStrMon(dataAsOfDate, "VEGA")
  
  dataPreviousDateX <- dataPreviousDate
  dataPreviousDateX$DELTA <- -dataPreviousDateX$DELTA
  dataPreviousDateX$VEGA  <- -dataPreviousDateX$VEGA
  
  dataPreviousDateX$TRADE_ENTRY_DATE <- as.Date(dataPreviousDateX$TRADE_ENTRY_DATE)
  dataAsOfDate$TRADE_ENTRY_DATE <- as.Date(dataAsOfDate$TRADE_ENTRY_DATE)
  dataPreviousDateX$VALUATION_MONTH <- as.Date(dataPreviousDateX$VALUATION_MONTH)
  dataAsOfDate$VALUATION_MONTH <- as.Date(dataAsOfDate$VALUATION_MONTH)  
  dataDeltaDates <- rbind(data.frame(dataAsOfDate),data.frame(dataPreviousDateX))
  resDeltaDelta <- aggregateLocStrMon(dataDeltaDates, "DELTA")  
  resDeltaVega  <- aggregateLocStrMon(dataDeltaDates, "VEGA")  
  # notional     
  dataX <- subset(dataAsOfDate, as.Date(TRADE_ENTRY_DATE) == (as.Date(options$asOfDate)))
  EDIDX <- unique(dataX$EXTERNALDEALID)  
  con <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")    
  query1 <- paste("select td.BUY_SELL_TYPE, td.PRICE_USD, td.QUANTITY, td.LOCATION_NM, th.PROFIT_CENTER_1, th.PROFIT_CENTER_2, ",
    "td.CALL_PUT_TYPE, td.STRIKE, th.TRADER, th.EXTERNALDEALID, th.TRADE_ENTRY_DATE, td.VALUATION_MONTH ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = th.trade_entry_date ",
    "and th.trade_entry_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ",
    "and td.source_feed = 'G' and td.option_flag = 'Y' and td.QTY_UOM != 'USD' ", 
    "and td.quantity != 0 and th.SOURCE_NAME = 'SECDB'", sep="")
  if (length(EDIDX)>0){
    EDIDX <- paste(EDIDX, sep="", collapse="', '")
    query1 <- paste(query1, " and th.EXTERNALDEALID in ('", EDIDX, "')", sep="")
  }  
  data1 <- sqlQuery(con, query1)
  close(con)
  
  # match dates 
  EDID1 <- unique(data1$EXTERNALDEALID)  
  EDIDX <- unique(dataX$EXTERNALDEALID)
  data1$VALUATION_MONTH <- as.Date(data1$VALUATION_MONTH)
  dataX$VALUATION_MONTH <- as.Date(dataX$VALUATION_MONTH)
  for (i in 1: length(EDIDX)) {
    indX <- which(dataX$EXTERNALDEALID == EDIDX[i])
    indY <- which(as.character(data1$EXTERNALDEALID) == as.character(EDIDX[i]))
    if (length(indY) > 0 ) {
      dRange <- range(as.Date(dataX$VALUATION_MONTH[indX])) - range(as.Date(data1$VALUATION_MONTH[indY]))
      if( dRange[1] < 32 & dRange[1] > 27 & dRange[2] < 32 & dRange[2] > 27) 
         data1$VALUATION_MONTH[indY] <- as.Date(paste(format(data1$VALUATION_MONTH[indY] + 31, "%Y-%m"),"-01",sep = ""))       
    }
  }
  # correct quantity sign 
  data1$QUANTITY <- ifelse(data1$BUY_SELL_TYPE == "S", -abs(data1$QUANTITY), abs(data1$QUANTITY))
  
  MMnotional <- aggregate(data1$QUANTITY, list(call.put=data1$CALL_PUT_TYPE,  strike=data1$STRIKE, location=data1$LOCATION_NM,
      contract.month= as.character(as.Date(data1$VALUATION_MONTH))), sum)
  colnames(MMnotional)[length(colnames(MMnotional))] <- "value"
  ind0 <- which(MMnotional$value == 0)
  if(length(ind0) >0) {MMnotional <- MMnotional[-ind0,]} 
  resNotional <- cast(MMnotional, location + call.put + strike ~ contract.month, I)
  resNotional <- data.frame(resNotional)
  indX <- grep("X", colnames(resNotional))
  colnames(resNotional) <- gsub("X", "", colnames(resNotional))
  colnames(resNotional)[indX] <- format(as.Date(colnames(resNotional)[indX],"%Y.%m.%d"),"%b-%Y")
  if(length(indX) > 1) { resNotional <- cbind(resNotional, Total=apply(resNotional[,indX], 1, sum, na.rm=TRUE)) }
#  write.table(resNotional, file = paste("C:/temp/OptionBookNotionalPosition_", as.character(options$asOfDate), ".csv", sep = ""),
#              append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
    
  return(list(data.frame(resNotional), data.frame(resDelta), data.frame(resVega), 
              data.frame(resDeltaDelta), data.frame(resDeltaDelta))) 
}  

aggregateLocStrMon <- function(data, field) {
  n <- which(colnames(data) == field)
  MM <- aggregate(data[,n], list(location = data$LOCATION_NM, strike = data$STRIKE, 
                                        contractMonth = as.character(as.Date(data$VALUATION_MONTH))), sum)
  colnames(MM)[length(colnames(MM))] <- "value"
  res <- cast(MM, location + strike ~ contractMonth, I)
  res <- data.frame(res)
  indX <- grep("X", colnames(res))
  colnames(res) <- gsub("X", "", colnames(res))
  colnames(res)[indX] <- format(as.Date(colnames(res)[indX],"%Y.%m.%d"),"%b-%Y")  
  res <- cbind(res, TotalDelta = apply(res[,indX], 1, sum, na.rm=TRUE))
  return(res)
}

writeOptionPosition <- function(options, fileName, dateX) {
  books <- get.portfolio.hierarchy(options$portfolio, dateX)
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
  queryG <- paste("select td.BUY_SELL_TYPE, td.PRICE_USD, ",
    "td.CALL_PUT_TYPE, td.STRIKE, th.TRADER, th.EXTERNALDEALID, th.TRADE_ENTRY_DATE ",
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
  dup <- duplicated(dataG$EXTERNALDEALID)
  indUnique <- which(dup == FALSE)
  data <- merge(dataR, dataG[indUnique,])
  write.table(data, file = fileName, append = FALSE, sep = ",", row.names = FALSE, col.names = TRUE)
  return(data)
}
# copied Adrians function for convenience
get.portfolio.hierarchy <- function(parent, pricing.dt){
  if (nargs()==1){pricing.dt <- Sys.Date()-1}
  pricing.dt <- format(pricing.dt, "%m/%d/%Y")
  con   <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  query <- paste("SELECT * from foitsox.sdb_portfolio_hierarchy ",
    "WHERE PRICING_DATE=TO_DATE('", pricing.dt, "','MM/DD/YYYY')", sep="")
  MM    <- sqlQuery(con, query)
  odbcCloseAll()
  MM <- MM[,c("PARENT", "CHILD")]
  MM$PARENT <- toupper(as.character(MM$PARENT))
  MM$CHILD  <- toupper(as.character(MM$CHILD))
    
  child <- NULL
  get.children <- function(parent){
    child  <<- c(child, setdiff(parent, MM[,1]))
    parent <- MM[which(MM[,1] %in% parent),2]
    if (length(parent)>0){
      get.children(parent)
    } else {
      return(child)
    }
  }
  res <- get.children(toupper(parent))
  ind <- which(nchar(res)>8)      # remove strings longer than 8 char
  if (length(ind)>0){res <- res[-ind]}  
  return(sort(res))
}
