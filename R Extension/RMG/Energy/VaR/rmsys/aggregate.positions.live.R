# Aggregate positions for a given run if run from archive. 
#
#
# Written by Adrian Dragulescu on 18-Jan-2006

get.trades.strategies <- function(run, options){
  TT <- SS <- NULL
  etis  <- run$PBT$trade[!is.na(run$PBT$trade)]
  if (length(etis)>0){  #------------------------------------- get trades
    qetis <- paste(etis, sep="", collapse="', '")
    query <- paste("select d.deal_reference_cd, d.deal_type_id, p.long_name, " ,
      "lp.contract_date, lp.delta, lp.vega from live_positions lp, deals d, ",
      "price_indexes p where lp.deal_id = d.deal_id and lp.price_index = p.index_num ",
      "and d.deal_reference_cd in ('", qetis, "')", sep="") 
    TT <- sqlQuery(options$connections$RMSYS, query)
    colnames(TT) <- c("trade", "transaction.type", "curve.name", "contract.dt", 
                    "delta", "vega")
    TT$trade <- as.character(TT$trade)
    TT <- merge(TT, run$PBT[, c("weight","trade")])
    TT$delta <- TT$delta*TT$weight; TT$vega <- TT$vega*TT$weight
    ind <- (etis %in% unique(as.character(TT$trade)))
    if (any(!ind)){
      msg <- paste("The following trades were not found in RMSYS:",
        paste(etis[!ind], collapse="\n"))
      tkmessageBox(message=msg)
    }
  }
  ind <- !is.na(run$PBT$strategy) & is.na(run$PBT$trade)
  if (any(ind)){   #----------------------------------- get stragegies
    strategies  <- run$PBT$strategy[ind]
    qstrategies <- paste(strategies, sep="", collapse="', '")
    query <- paste("select s.strat_name, d.deal_type_id, p.long_name, ",
      "lp.contract_date, sum(lp.delta), sum(lp.vega) from live_positions lp, ",
      "deals d, price_indexes p, strategies s where lp.deal_id = d.deal_id ",
      "and lp.price_index = p.index_num and d.strat_id = s.strat_id ",
      "and upper(s.strat_name) in ('", qstrategies, "') group by s.strat_name, ",
      "d.deal_type_id, p.long_name, lp.contract_date", sep="")
    SS <- sqlQuery(options$connections$RMSYS, query)
    colnames(SS) <- c("strategy", "transaction.type", "curve.name", "contract.dt",
       "delta", "vega")
    SS <- merge(SS, run$PBT[, c("weight","strategy")])
    SS$delta <- SS$delta*SS$weight; SS$vega <- SS$vega*SS$weight
    ind <- (strategies %in% unique(as.character(SS$strategy)))
    if (any(!ind)){
      msg <- paste("The following strategies were not found in RMSYS:",
        paste(strategies[!ind], collapse="\n"))
      tkmessageBox(message=msg)
    }
    colnames(SS)[1] <- "trade"
    TT <- rbind(TT,SS)                     # trades and strategies
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
  VV$vol.type <- ifelse(VV$transaction.type %in% c(8, 12), "D", "M")
  VV$curve.name <- toupper(VV$curve.name)
  VV <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)
  VV <- subset(VV, value != 0)
  return(list(DD,VV))
}

aggregate.positions.live <- function(run, options){
  DD <- VV <- NULL
  load(options$save$pHier.file)        # load portfolio hierarchy
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
  if (length(exPBT)>0 && length(na.omit(exPBT$book.id))>0){
    #---------------------------get deltas: portfolios/books----------------------
    qbooks <- paste(sort(unique(na.omit(exPBT$book.id))), sep="", collapse=", ")
    query <- paste("select b.book_id, p.long_name, lp.contract_date, sum(lp.delta) ",
      "from live_positions lp, deals d, price_indexes p, books b where lp.deal_id = ",
      "d.deal_id and lp.price_index = p.index_num and b.book_id = d.book_id and ", 
      "b.book_id in (",  qbooks, ") and lp.delta != 0 group by b.book_id, ",
      "p.long_name, lp.contract_date", sep="")
    DD <- sqlQuery(options$connections$RMSYS, query)
    colnames(DD) <- c("book.id", "curve.name", "contract.dt", "value")
    DD <- subset(DD, value !=0)
    ind <- grep("Dollars", DD$curve.name)  # remove currencies, US and Canadian
    if (length(ind)>0){DD  <- DD[-ind,]}
    if (any(run$PBT$weight)==-1){
      DD <- merge(DD, exPBT[,c("weight","book.id")])
      DD$value  <- DD$value * DD$weight
      DD$weight <- NULL
    }
    DD   <- cast(DD, curve.name + contract.dt ~ ., sum)                  

    #---------------------------get vegas: portfolios/books-----------------------
    query <- paste("select b.book_id, d.deal_type_id, p.long_name, lp.contract_date, ",
       "sum(lp.vega) from live_positions lp, deals d, price_indexes p, books b ", 
       "where lp.deal_id = d.deal_id and lp.price_index = p.index_num and vega <>0 ",
       "and d.book_id = b.book_id and b.book_id in (", qbooks, ") group by ",
       "b.book_id, d.deal_type_id, p.long_name, lp.contract_date", sep="")
    VV <- sqlQuery(options$connections$RMSYS, query)
    colnames(VV) <- c("book.id", "transaction.type", "curve.name", "contract.dt", "value")
    if (any(run$PBT$weight)==-1){
      VV <- merge(VV, exPBT[,c("weight","book.id")])
      VV$value  <- VV$value * VV$weight
      VV$weight <- NULL
    }
    VV$vol.type <- ifelse(VV$transaction.type %in% c(8, 12), "D", "M")
    VV   <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)
  } # finish with books/portfolios
  
  #---------------------------------get trades/strategies---------------------
  if (any(!is.na(run$PBT$trade)) | any(!is.na(run$PBT$strategy))){
    res <- get.trades.strategies(run, options)
    DD  <- rbind(DD, res[[1]])
    VV  <- rbind(VV, res[[2]])
  }
  DD$contract.dt <- as.Date(format(as.Date(DD$contract.dt), "%Y-%m-01"))
  VV$contract.dt <- as.Date(format(as.Date(VV$contract.dt), "%Y-%m-01"))
  DD$curve.name  <- toupper(as.character(DD$curve.name))
  VV$curve.name  <- toupper(as.character(VV$curve.name))
  # aggregate one more time  
  DD   <- cast(DD, curve.name + contract.dt ~ ., sum)       
  VV   <- cast(VV, curve.name + contract.dt + vol.type ~ ., sum)
  DD <- cbind(DD, vol.type=NA)
  VV$value <- 100*VV$value    
  DD <- subset(DD, value != 0)
  VV <- subset(VV, value != 0)
  
  return(list(DD,VV))
}
