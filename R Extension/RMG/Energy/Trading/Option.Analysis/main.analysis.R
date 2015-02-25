
main.analysis <- function(options){

  require(RODBC)
  source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")

  books <- paste("'",paste(options$books,sep="",collapse="', '"),"'",sep="")
  con <- odbcConnect("VCTRLP", uid="vcentral_read", pwd="vcentral_read")
  query <- paste("select th.EXTERNALDEALID, th.profit_center_1 book, ", 
    "td.source_feed, td.valuation_date, td.location_nm, ", 
    "td.buy_sell_type, td.call_put_type, td.transaction_type, ",
    "td.period_start_date, td.period_end_date, td.strike, ",
    "td.quantity, td.fx_rate, td.valuation_month, td.option_flag, ",
    "td.interest_rate, td.correlation, td.volatility, ", 
    "td.payment_date, td.expiration_date, td.delta, td.gamma, td.vega,", 
    "td.theta, td.price, td.mtm_value_usd ",
    "from vcentral.trade_detail td, vcentral.trade_header th ",
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date ", 
    "and td.valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ", 
    "and td.period_end_date > to_date('", options$asOfDate, "','yyyy-mm-dd') ",
#   "and td.option_flag = 'Y' ",
    "and td.source_feed = 'G' ",              
    "and td.TRADE_HEADER_ID in (",
    "   select trade_header_id from vcentral.trade_header",
    "   where valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd')",
    "   and PROFIT_CENTER_1 in (",books,") ",
#    "   and externaldealid = 'OPS0R2F'",              
    ")", sep="")

  cat("Getting positions from VCENTRAL ... "); flush.console()
  data <- sqlQuery(con, query)
  cat("Done. \n")
  close(con)
  colnames(data) <- tolower(gsub("_", ".", colnames(data)))
  data$location.nm <- as.character(data$location.nm)
  data$valuation.month <- as.Date(data$valuation.month)
  data$valuation.date  <- as.Date(data$valuation.date)
  
  uTradeID <- unique(data$externaldealid)
  cat(paste("There are", length(uTradeID), "trades.\n")); flush.console()

  #----------------------get the prices
  options$pricing.dt.start <- options$asOfDate
  options$curve.names <- as.character(na.omit(unique(data$location.nm)))
  cat("Read prices from CPSPROD ... "); flush.console()  
  res <- read.prices.from.CPSPROD(options)
  colnames(res) <- c("valuation.date","location.nm","valuation.month","forward")
  res$location.nm <- as.character(res$location.nm)
  cat("Done. \n")
  cat("Start calculations ... "); flush.console()
 
  data <- merge(data, res, all.x=T)
  #---------------- Look at the options ---------------
  MM <- subset(data, option.flag=='Y')
  MM <- subset(MM, !is.na(MM$forward))   # get rid of the payments
  MM <- subset(MM, MM$quantity != 0)     # get rid of the ?

  Tex <- as.numeric(as.Date(MM$expiration.date) - MM$valuation.date)/365
  DF  <- exp(-MM$interest.rate*Tex)   
  levels(MM$call.put.type) <- c("CALL","PUT")
  cols <- c("book", "location.nm", "valuation.month", "externaldealid",
     "call.put.type", "strike", "quantity", "interest.rate", "volatility",
     "expiration.date", "forward")
  MM <- cbind(uID=1:nrow(MM), MM[, which(colnames(MM) %in% cols)])

  SS <- data.frame(uID=rep(MM$uID, each=length(options$scenarios)),
              sim=rep(options$scenarios, nrow(MM)))
  
  DD  <- data.frame(uID=MM$uID, fwd=MM$forward, sigma=MM$volatility, Tex=Tex,
           K=MM$strike, r=0, type=MM$call.put.type, DF)
  ALL   <- merge(DD, SS)
  ALL$S <- ALL$fwd * ALL$sim                     # perturbed price
  ALL$RMG.unit <- blackscholes(ALL)$value * DF

  cols <- c("uID", "book", "location.nm", "valuation.month", "externaldealid",
      "quantity")  
  res <- merge(ALL[, c("uID", "sim", "RMG.unit")],
               MM[, which(colnames(MM)%in%cols)])
  res$RMG.value <- res$RMG.unit * res$quantity
  res <- res[, -which(colnames(res) %in% c("uID", "RMG.unit", "quantity"))]
  head(res)
  cat("Done. \n"); flush.console()
  
  aux <- aggregate(res$RMG.value, by=list(sim=res$sim), sum, na.rm=T)
  aux$x <- aux$x/1000000   # look in millions
  ind <- which(aux$sim==1)
  aux$sim <- as.numeric(as.character(aux$sim))
  plot(aux$sim-1, aux$x-aux$x[ind], col="blue",
       xlab="Price deviations", ylab="Value, $")
  abline(h=0, col="grey")
  
  return(list(res, MM))
}


#   table(data$externaldealid, as.character(data$valuation.month))

##   table(data$transaction.type)
##   subset(data, transaction.type=="ELEC PHYSICAL SERIES OPTION")
##   subset(data, transaction.type=="COMMOD SERIES STRIP OPT")
##   subset(data, transaction.type=="COMMOD SERIES OPTION AVG")  
##   uLocationNM <- as.character(na.omit(unique(data$location.nm)))
  
