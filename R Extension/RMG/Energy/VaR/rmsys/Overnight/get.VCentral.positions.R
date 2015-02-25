# Get VCentral deltas and vegas for rmsys by book. 
#
#
# Written by Adrian Dragulescu on 22-Nov-2006


get.VCentral.positions <- function(pHier, options){

  cat("Reading in all VCentral positions ... ")
  file <- paste(options$save$datastore.dir,"all.positions.",
                  options$asOfDate,".RData", sep="")
  if (file.exists(file)){
    load(file); cat("Done!\n");
    return(list(all.positions$deltas, all.positions$vegas))
  }
  #----- Get rmsys deltas by book, curve, month ---------------------------- 
  query <- paste("select rl.book_id, td.LOCATION_NM, ",
    "td.VALUATION_MONTH, sum(td.skew_delta) from vcentral.trade_detail td, ",
    "vcentral.trade_header th, rmsys.deals@rmsys_read_link rl ",
    "where td.TRADE_HEADER_ID=th.TRADE_HEADER_ID ", 
    "and td.valuation_date = th.valuation_date and td.valuation_date = ",
    "to_date('", options$asOfDate, "','yyyy-mm-dd') and td.SOURCE_FEED='R' ",
    "and rl.deal_reference_cd = th.externaldealid ",
    "and td.location_nm is not null and td.TRADE_HEADER_ID in ( ",
    "select trade_header_id from vcentral.trade_header ", 
    "where valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ", 
    "and source_name='RMSYS') group by rl.book_id, td.LOCATION_NM, ",
    "td.VALUATION_MONTH", sep="")
  DD <- sqlQuery(options$connections$VCTRLP, query)# about 17000 rows,22-Nov-2006
  colnames(DD) <- c("book.id", "curve.name", "contract.dt", "delta")

  ind <- grep("Dollars", DD$curve.name)  # remove currencies, US and Canadian
  if (length(ind)>0){DD  <- DD[-ind,]}
  DD <- DD[order(DD$book), ]
  DD$contract.dt <- as.Date(DD$contract.dt) # bring all dates to first of month
  DD$contract.dt <- as.Date(format(DD$contract.dt, "%Y-%m-01"))
  DD$curve.name  <- toupper(DD$curve.name)
  DD <- subset(DD, delta !=0)               # drop out the zero positions
##   bookmap <- pHier[,-1]
##   colnames(bookmap)[which(colnames(bookmap)=="child")] <- "book"
##   DD <- merge(DD, bookmap, all.x=T); DD$book.id <- NULL

  
  #----- Get rmsys vegas by book, curve, month ---------------------------- 
  query <- paste("select rl.book_id, td.location_nm, ",
    "td.transaction_type, td.valuation_month, td.vega from ",
    "vcentral.trade_detail td, vcentral.trade_header th, ",
    "rmsys.deals@rmsys_read_link rl  where td.TRADE_HEADER_ID=th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ",
    "and td.valuation_date=to_date('", options$asOfDate, "','yyyy-mm-dd') ", 
    "and td.SOURCE_FEED = 'R' and td.vega != 0 ",
    "and rl.deal_reference_cd = th.externaldealid ",             
    "and td.TRADE_HEADER_ID in ( ",
    "select trade_header_id from vcentral.trade_header ", 
    "where valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ",
    "and source_name='RMSYS')", sep="")
  VV <- sqlQuery(options$connections$VCTRLP, query) # about 5200 rows,22-Nov-2006
  colnames(VV) <- c("book.id", "curve.name", "transaction.type",
                    "contract.dt", "value")
  table(VV$transaction.type)
  VV$contract.dt <- as.Date(VV$contract.dt) # bring all dates to first of month
  VV$contract.dt <- as.Date(format(VV$contract.dt, "%Y-%m-01"))
  #-------------------------------------NEED TO CHECK THIS MAPPING!!! --------
  VV$vol.type <- ifelse(VV$transaction.type %in% c("FSOPT", "TRNSPRT"), "D",
                "M")
  VV$vol.type <- as.factor(VV$vol.type)
  VV$curve.name <- toupper(VV$curve.name)
  VV <- cast(VV, book.id + curve.name + contract.dt + vol.type ~ ., sum)
  VV <- subset(VV, value != 0)
  colnames(VV)[5] <- "vega"
##   VV <- merge(VV, bookmap, all.x=T); VV$book.id <- NULL
  VV$vega <- 100*VV$vega    # rmsys vegas use normal convention...
  
  if (options$run$is.overnight){
    all.positions <- NULL
    all.positions$asOfDate <- options$asOfDate
    all.positions$deltas <- DD
    all.positions$vegas  <- VV
    filename <- paste(options$save$datastore.dir, "all.positions.",
                      options$asOfDate, ".RData", sep="")
    save(all.positions, file=filename)
  }
  cat("Done!\n")
  return(list(DD, VV))
}

##   query <- paste("select th.PROFIT_CENTER_1, td.LOCATION_NM, ",
##     "td.VALUATION_MONTH, sum(td.DELTA) from vcentral.trade_detail td, ",
##     "vcentral.trade_header th where td.TRADE_HEADER_ID=th.TRADE_HEADER_ID ", 
##     "and td.valuation_date = th.valuation_date and td.valuation_date = ",
##     "to_date('", options$asOfDate, "','yyyy-mm-dd') and td.SOURCE_FEED='R' ",  
##     "and td.location_nm is not null and td.TRADE_HEADER_ID in ( ",
##     "select trade_header_id from vcentral.trade_header ", 
##     "where valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ", 
##     "and source_name='RMSYS') group by th.PROFIT_CENTER_1, td.LOCATION_NM, ",
##     "td.VALUATION_MONTH", sep="")
