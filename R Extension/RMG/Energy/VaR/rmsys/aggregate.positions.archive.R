# Aggregate positions for a given run if run from archive. 
#
#
# Written by Adrian Dragulescu on 18-Jan-2006

get.trades <- function(run, options){
  ETIs  <- paste(run$PBT$trade[!is.na(run$PBT$trade)], sep="", collapse="', '")
  query <- paste("select th.EXTERNALDEALID, td.location_nm, ", 
    "td.valuation_month, td.transaction_type, td.delta, td.vega ", 
    "from vcentral.trade_detail td, vcentral.trade_header th ", 
    "where td.TRADE_HEADER_ID = th.TRADE_HEADER_ID ",
    "and td.valuation_date = th.valuation_date and ",
    "td.valuation_date = to_date('", options$asOfDate, "','yyyy-mm-dd') ",
    "and td.SOURCE_FEED = 'R' and td.TRADE_HEADER_ID in (select ",
    "trade_header_id from vcentral.trade_header where valuation_date = ",
    "to_date('", options$asOfDate, "','yyyy-mm-dd') and source_name='RMSYS' ",
    "and EXTERNALDEALID in ('", ETIs, "'))", sep="")
  TT <- sqlQuery(options$connections$VCTRLP, query)
  colnames(TT) <- c("trade", "curve.name", "contract.dt", "transaction.type",
                  "delta", "vega")
  TT <- merge(TT, run$PBT[, c("weight","trade")])
  TT$delta <- TT$delta*TT$weight; TT$vega <- TT$vega*TT$weight
  ind <- (run$PBT$trade[!is.na(run$PBT$trade)] %in% unique(TT$trade))
  if (any(run$PBT$trade[!is.na(run$PBT$trade)][!ind])){
    msg <- paste("The following trades were not found in VCENTRAL:",
      paste(run$PBT$trade[!is.na(run$PBT$trade)][!ind], collapse="\n"))
    tkmessageBox(message=msg)
  }  
  ind <- grep("Dollars", TT$curve.name)  # remove currencies, US and Canadian
  if (length(ind)>0){TT  <- TT[-ind,]}
  TT$contract.dt <- as.Date(TT$contract.dt) # bring all dates to first of month
  TT$contract.dt <- as.Date(format(TT$contract.dt, "%Y-%m-01"))
  TT$curve.name  <- toupper(TT$curve.name)
  TT <- melt(TT, id = 1:4)
  DD <- cast(TT, curve.name + contract.dt ~ ., sum, subset=variable=="delta")
  DD <- subset(DD, value != 0)

  VV <- subset(TT, variable=="vega")
  VV$vol.type <- ifelse(VV$transaction.type %in% c("FSOPT", "TRNSPRT"), "D",
                "M")
  VV$vol.type <- as.factor(VV$vol.type)
  VV$curve.name <- toupper(VV$curve.name)
  VV <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)
  VV <- subset(VV, value != 0)
  return(list(DD,VV))
}

aggregate.positions.archive <- function(run, options){
  cat(paste("Loading portfolio hierarchy ... "))
  load(options$save$pHier.file)  
  cat("Done!\n")
  cat(paste("Loading positions ... "))
  load(options$save$all.positions.file)
  cat("Done!\n")
  exPBT <- NULL  
  #----------------------------------------------expand portfolios into books
  if (any(!is.na(run$PBT$portfolio) & !is.na(run$PBT$book))){ # portfolios and books
    colnames(pHier)[1:2] <- c("portfolio", "book")
    exPBT <- rbind(exPBT, merge(subset(run$PBT, !is.na(portfolio) & !is.na(book)),
                                pHier, all.x=T))
  }
  ind <- !is.na(run$PBT$portfolio) & is.na(run$PBT$book)      # porfolios alone
  if (any(ind)){  
    colnames(pHier)[1:2] <- c("portfolio", "hier.book")
    aux <- merge(subset(run$PBT, ind), pHier, all.x=T)
    aux$book <- aux$hier.book; aux$hier.book <- NULL
    exPBT <- rbind(exPBT, aux)
  }
  if (any(is.na(run$PBT$portfolio) & !is.na(run$PBT$book))){  # books alone 
    colnames(pHier)[2] <- "book"
    aux <- merge(subset(run$PBT, select=-portfolio), pHier, all.x=T)
    exPBT <- rbind(exPBT, aux[!is.na(aux$book),])
  }
  #----------------------------------------------------------------------------------
  if (length(na.omit(exPBT$book.id))>0){
    books <- na.omit(exPBT$book.id)
    if (any(duplicated(books))){
      cat(paste("These books are not unique: ", exPBT$books[which(duplicated(books))],
              "!\n", sep=""))}
    DD   <- subset(all.positions$deltas, book.id %in% exPBT$book.id)
    colnames(DD)[which(colnames(DD) == "delta")] <- "value"
    if (any(run$PBT$weight)==-1){
      DD <- merge(DD, run$PBT[,c("weight","book")])
      DD$value  <- DD$value * DD$weight
      DD$weight <- NULL
    }
    DD   <- cast(DD, curve.name + contract.dt ~ ., sum)                  # Deltas
    VV   <- subset(all.positions$vegas, book.id %in% exPBT$book.id)      # Vegas
    colnames(VV)[which(colnames(VV) == "vega")] <- "value"
    if (nrow(VV)>0){
      VV   <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)}
  }
  #---------------------------------- get deltas/vegas by trade ID----------
  if (any(!is.na(run$PBT$trade))){
    res <- get.trades(run, options)
    DD  <- rbind(DD, res[[1]])
    VV  <- rbind(VV, res[[2]])
    # aggregate one more time  
    DD   <- cast(DD, curve.name + contract.dt ~ ., sum)       
    VV   <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)
  }
  DD <- cbind(DD, vol.type=NA)
  
  return(list(DD,VV))
}
