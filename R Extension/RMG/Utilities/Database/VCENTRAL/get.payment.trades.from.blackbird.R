###############################################################################
# Get positions from PRISM.  The function name is misleading, sorry -- AAD.
#
# get.positions.from.blackbird.R
# 
# asOfDate    <- Sys.Date()-1
# books       <- c("NSCMPPPA")  
# service     <- "ENERGY"
# contract.dt <- as.Date("2010-01-01")
#
# source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
# QQ <- get.positions.from.blackbird(options)
# QQ <- fix.positions.from.blackbird(QQ)


###############################################################################
# NOTE: For most books, SERVICE == "ENERGY" usually brings back the correct
#   deltas.  Attrition books have SERVICE == "NULL", and they have VLRCST, ISOFEE, 
#   UCAP, etc. in them.  See the fix.positions.from.blackbird for a way to fix this.
#
get.payment.trades.from.blackbird <- function(asOfDate=Sys.Date()-1,
  books=NULL) 
{
  if (!("RODBC" %in% search()))
    require(RODBC)
  
  query <- paste("select eti, trade_type, trading_book, load_name, ",
    "load_class, is_cleared, is_ibt, counterparty, ",
    "leg_type, region, buy_sell_cd, delta, trade_date, ",
    "trade_start_date, trade_end_date, curve, delivery_point, ", 
    "volume, notional, contract_date, contract_price, qty_bucket_type, ",
    "service, quantity_uom, employee_id ", 
    "from prism.trade_flat_vw t where t.valuation_date ='",
    format(asOfDate, "%d%b%Y"), "'", sep="") 

  if (length(books)!=0){
    aux   <- paste(toupper(books), sep="", collapse="', '")
    aux   <- paste(" and trading_book in ('", aux, "')", sep = "")
    query <- paste(query, aux, sep="")
  } 

  aux   <- " and upper(deal_type)='ELEC PAYMENT SERIES'"
  query <- paste(query, aux, sep="")

  data <- NULL
  rLog("Querying PRISM...")
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/PRISMP.dsn"),
    ";UID=e47187;PWD=e47187;", sep="")  
  con <- odbcDriverConnect(con.string)
  data <- sqlQuery(con, query) 
  odbcCloseAll()
  
  rLog("Done querying db.")

  # do some cosmetics 
  names(data) <- gsub("_", ".", tolower(names(data)))
  data$trade.date <- as.Date(data$trade.date)
    
  return(data)
}



##   names(data)[which(names(data)=="contract.date")]   <- "contract.dt"
##   names(data)[which(names(data)=="qty.bucket.type")] <- "bucket"
##   data$contract.dt <- as.Date(data$contract.dt)
#    data$CURVE_NAME = toupper(data$CURVE_NAME)
##   data$mkt <- sapply(strsplit(data$curve, " "), function(x){x[2]})
  

