extractDeltaVegaA <- function(books, dateX) {
#  browser()
  #con <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read") 
  file.dsn.path = "H:/user/R/RMG/Utilities/DSN/"
  file.dsn = "VCTRLP.dsn"
  connection.string <- paste("FileDSN=", file.dsn.path, file.dsn,
     ";UID=vcentral_read;PWD=vcentral_read;", sep="")
  con <- odbcDriverConnect(connection.string) 
  
# SRA 
  queryRCore <- paste("select sum(td.VEGA), sum(td.DELTA), td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1 ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and th.source_name = 'SRA' ",
    "and th.source_eid = 12 ", 
    "and th.source_eid = th.SOR_INDICATOR ", 
    "and (td.valuation_type='E' OR td.valuation_type IS NULL) ",
    "AND (td.price_type='INDEX' OR ", 
    "(NOT (td.price_type='PHYSICAL' AND th.deal_type ='Inhouse Deal' AND UPPER(td.product) LIKE '%SWAP%') AND ", 
    "NOT (td.price_type='PHYSICAL' AND th.deal_type ='Swap Deal'))) ",               
    "and td.source_feed = 'R' ", sep="")
  dataRA <- NULL
  iIn <- 0
  while(iIn*options$nFieldLimit < length(books)) {
    booksX <- paste(books[(iIn*options$nFieldLimit+1):min(length(books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
    queryR <- paste(queryRCore, " and th.PROFIT_CENTER_1 in ('", booksX, "')", 
                    " group by td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1", sep="")
    dataRA <- rbind(dataRA, sqlQuery(con, queryR))
    iIn <- iIn + 1
  }
## SECDB  
  querySCore <- paste("select sum(td.VEGA), sum(td.DELTA), td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1 ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and th.source_name = 'SECDB' ",
    "and th.source_eid = 2 ",
    "and th.source_eid = th.SOR_INDICATOR ",        
    "and td.source_feed = 'R' ", sep="")
  dataS <- NULL
  iIn <- 0
  while(iIn*options$nFieldLimit < length(books)) {
    booksX <- paste(books[(iIn*options$nFieldLimit+1):min(length(books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
    queryS <- paste(querySCore, " and th.PROFIT_CENTER_1 in ('", booksX, "')", 
                    " group by td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1", sep="")
    dataS <- rbind(dataS, sqlQuery(con, queryS))
    iIn <- iIn + 1
  } 
## RMSYS  COMMENT FOR NOW
  queryRMCore <- paste("select sum(td.VEGA), sum(td.DELTA), td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1 ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date = to_date('", dateX, "','yyyy-mm-dd') ",
    "and th.source_name = 'RMSYS' ",
    "and th.source_eid = th.SOR_INDICATOR ",    
    "and td.source_feed = 'R' ", sep="")
  dataRM <- NULL
  iIn <- 0
  while(iIn*options$nFieldLimit < length(books)) {
    booksX <- paste(books[(iIn*options$nFieldLimit+1):min(length(books),(iIn+1)*options$nFieldLimit)], 
                    sep="", collapse="', '")
    queryRM <- paste(queryRMCore, " and th.PROFIT_CENTER_1 in ('", booksX, "')", 
                    " group by td.VALUATION_MONTH, td.LOCATION_NM, th.PROFIT_CENTER_1", sep="")
    dataRM <- rbind(dataRM, sqlQuery(con, queryRM))
    iIn <- iIn + 1
  }
     
  close(con) 
  if(dim(dataRA)[1] > 0) {
    dataRA$VALUATION_MONTH <- as.numeric(dataRA$VALUATION_MONTH)/(24*60*60) + as.Date("1970-01-01")
    dataS <- rbind(dataS,dataRA)
  }
  if(dim(dataRM)[1] >0) {
  } 
  colnames(dataS) <- c("VEGA", "DELTA", "VALUATION_MONTH", "CURVE_NAME", "BOOK")
  dataS$CURVE_NAME <- toupper(dataS$CURVE_NAME)  
  return(dataS)
}
