# Info in PRISM does not keep hourly info, only at the bucket level.
#
# So any inc/dec that is done houly, gets messed up... 
#
INC.loadPositions <- function(from="PRISM", books=NULL, asOfDate, 
  region=NULL){
#  if (is.null(start.dt)) start.dt <- Sys.Date()-1
  
  if (toupper(from)=="SECDB"){
    stop("from=SecDb method not yet implemented.")
#    res <- .FTR.loadPositions.SecDb(books, asOfDate, region, incdec)
  }
  if (toupper(from)=="PRISM"){
    res <- .INC.loadPositions.PRISM(books, asOfDate, region)
  }
  
  return(res)
}

########################################################################
#
.INC.loadPositions.PRISM <- function(books, asOfDate, region)
{
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/PRISMP.dsn;UID=e47187;PWD=e47187;", sep="")  
  con <- odbcDriverConnect(con.string)

  qbooks <- paste("'", paste(books, sep="", collapse="', '"), "'", sep="")
  query <- paste("select eti, trade_type, trading_book, ", 
    "counterparty, trade_start_date, trade_end_date, curve, delivery_point, ", 
    "volume, notional, qty_bucket_type ",
    "from prism.trade_flat_vw t ",
    " where t.trade_start_date = t.trade_end_date",                 
    " and t.valuation_date = '", format(asOfDate, "%d%b%Y"), "'",  
    " and t.trade_start_date <= t.valuation_date",
#    " and counterparty in ('CPISONE')",  # there can be intrabook trades
    " and curve != 'USD' ",                  
    " and trading_book in (", qbooks, ")",
    " and upper(deal_type) = 'ELEC PHYSICAL SERIES'", sep="") 

  cat("Pulling data from BlackBird... \n")
  cat("  Please be patient as this may take a while...\n")
  flush.console()
  QQ <- sqlQuery(con, query)
  odbcCloseAll()

  if (nrow(QQ)==0){
    cat("No inc/dec positions for the books selected!\n")
    return(NULL)
  }
  cat("Done.\n")

  colnames(QQ) <- tolower(gsub("_", ".", colnames(QQ)))
  QQ$eti <- as.character(QQ$eti)
  colnames(QQ)[c(2,3,5,6,9,11)] <- c("buy.sell", "book", 
    "start.dt", "end.dt", "mw", "bucket")

  aux <- strsplit(as.character(QQ$curve), " ")
  mkt <- sapply(aux, function(x){x[2]})
  QQ$region   <- ifelse(mkt == "PWX", "NEPOOL", ifelse(mkt=="PWY", "NYPP", NA))
  if (!is.null(region))
    QQ <- QQ[QQ$region %in% region, ]
  

  # add the ptid's
  if ("NEPOOL" %in% unique(QQ$region)){
    load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
    QQ$deliv.pt <- gsub(" DA$", "", QQ$delivery.point)
    QQ$deliv.pt <- gsub(" RT$", "", QQ$deliv.pt)
    QQ <- merge(QQ, MAP[, c("deliv.pt", "ptid")], by="deliv.pt", all.x=T)
  }
  if ("NYPP" %in% unique(QQ$region)){
    load("//ceg/cershare/All/Risk/Data/FTRs/NYPP/ISO/DB/RData/map.RData")
    aux <- subset(QQ, region=="NYPP"); aux$ptid <- NULL
    QQ  <- subset(QQ, region!="NYPP")
    aux$deliv.pt <- gsub("^DAM ", "", aux$deliv.pt)
    aux$deliv.pt <- gsub("^ART ", "", aux$deliv.pt)
    aux <- merge(aux, NYPP$MAP[, c("deliv.pt", "ptid")], by="deliv.pt", all.x=T)
    QQ  <- rbind(QQ, aux)
  }

  QQ$variable <- ifelse(QQ$mw > 0, "sink.schedule", "source.schedule") 
  QQ$schedule  <- gsub(".* ", "", QQ$delivery.point)
  QQ <- unique(QQ[,c("eti", "region", "book", "counterparty", "start.dt",
    "end.dt", "buy.sell", "bucket", "mw", "variable", "ptid",
                       "schedule")])

  colnames(QQ)[ncol(QQ)] <- "value"
  QQ$mw <- abs(QQ$mw)     # sources and sinks are distinguished
  QQ <- cast(QQ, ... ~ variable, I)
  # order the source sink columns properly ... 
  QQ <- QQ[,c(1:(ncol(QQ)-2), ncol(QQ), ncol(QQ)-1)]  
  
  return(data.frame(QQ))
}



#    " and t.valuation_date >= to_date('", format(asOfDate-7, "%d%b%Y"),  "', 'DDMONYYYY')",
#    " and t.valuation_date < to_date('", format(asOfDate+31, "%d%b%Y"), "', 'DDMONYYYY')",
#    " and t.trade_start_date = to_date('", format(tradeDate, "%d-%b-%y"), "', 'DD-MON-YY')",
