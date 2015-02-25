FTR.loadPositions <- function(from="PRISM", books=NULL, asOfDate=NULL,
                              region=NULL, incdec=FALSE){
  if (is.null(asOfDate)) asOfDate <- Sys.Date()-1
  
  if (toupper(from)=="SECDB") {
    res <- .FTR.loadPositions.SecDb(books, asOfDate, region, incdec)
  } else if (toupper(from)=="PRISM") {
    res <- .FTR.loadPositions.PRISM(books, asOfDate, region, incdec)
  } else if (toupper(from) == "XLSX") {
    currentMon <- as.Date(format(asOfDate, "%Y-%m-01"))
    fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
      "FTR.paths.xlsx", sep="")

    require(xlsx)
    aux <- read.xlsx(fname, sheetName="Paths", colIndex=1:13)
    aux <- aux[which(!is.na(aux[,1])),]
    
    
    uAuctions <- na.omit(unique(aux$auction))
    auctions  <- auctions_in_date( asOfDate, uAuctions )
    if (length(auctions)==0){
      rLog("No FTR positions for today!")
      return(NULL)
    }

    res <- subset(aux, auction %in% auctions)
    if (!is.null(books))
      res <- subset(res, book %in% books)
    names(res)[which(names(res)=="fix.price")] <- "CP"
  }
  
  # TCC's should be FLAT only!  Check for that.  Sometimes PRISM
  # screws up the buckets for a few days at the end of the month!
  if (nrow(res)>0){
    aux <- subset(res, region=="NYPP" & class.type != "FLAT")
    if (nrow(aux)>0){
      rLog("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
      rLog("Found some TCC trades that are not class.type == FLAT!")
      rLog("I will coerce them.  You need to check that's correct ...")
      ind <- which(res$region=="NYPP" & res$class.type != "FLAT")
      res[ind, "class.type"] <- "FLAT"
      rLog("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    }
  } else {
    rLog("No FTR positions found!")
  }

  return(res)
}

########################################################################
#
.FTR.loadPositions.PRISM <- function(books, asOfDate, region, incdec)
{
  con.string <- paste("FileDSN=S:/All/Risk/Software/R/prod/Utilities/",
    "DSN/PRISMP.dsn;UID=e47187;PWD=e47187;", sep="")  
  con <- odbcDriverConnect(con.string)

  qbooks <- paste("'", paste(books, sep="", collapse="', '"), "'", sep="")
  query <- paste("select eti, trade_type, trading_book, delta, ", 
    "counterparty, trade_start_date, trade_end_date, curve, delivery_point, ", 
    "volume, notional, contract_date, qty_bucket_type ", 
    "from prism.trade_flat_vw t where t.valuation_date ='",
    format(asOfDate, "%d%b%Y"),
    "' and trading_book in (", qbooks, ")",
    " and upper(deal_type) = 'ELEC PHYSICAL SERIES'", sep="") 

  QQ <- sqlQuery(con, query)
  odbcCloseAll()

  # remove the payments
  QQ <- subset(QQ, CURVE != "USD")

##   if (nrow(QQ)==0){
##     rLog("No positions for the books selected!")
##     return(QQ)
##   }

  # select inc/decs only if required 
  ind <- QQ$TRADE_START_DATE == QQ$TRADE_END_DATE
  if (incdec){QQ <- QQ[ind, ] } else { QQ <- QQ[!ind, ] }

  if (nrow(QQ)==0){
    rLog("No positions for the books selected!")
    return(QQ)
  }
  
  # pick only the MCC's for now...
  ind <- grep("MCC ", QQ$DELIVERY_POINT)
  if (length(ind)>0 && !incdec)
    QQ <- QQ[ind, ]
  
  QQ$ETI <- as.character(QQ$ETI)
  
  # add the class.type
  if (!incdec){
    aux <- split(QQ$QTY_BUCKET_TYPE, QQ$ETI)
    aux <- lapply(aux, function(x)as.character(unique(x)))
    aux <- unlist(sapply(aux, function(x){
      switch(length(x), "ONPEAK", "OFFPEAK", "FLAT")}))
    aux <- data.frame(ETI=names(aux), class.type=unlist(aux))
    QQ <- merge(QQ, aux)
  }
  
  colnames(QQ) <- tolower(gsub("_", ".", colnames(QQ)))
  colnames(QQ)[c(2,3,6,7,10)] <- c("buy.sell", "book", 
    "start.dt", "end.dt", "mw")

#  QQ <- subset(QQ, counterparty=="NYISO-TCC")
##   # see DAM MCC POLETTI!  # not neccessary anymore - using mccLocations
##   QQ$deliv.pt <- gsub("^DAM MCC ", "DAM ", toupper(QQ$deliv.pt))

  aux <- strsplit(as.character(QQ$curve), " ")
  mkt <- sapply(aux, function(x){x[2]})
  QQ$region   <- ifelse(mkt == "PWX", "NEPOOL", ifelse(mkt=="PWY", "NYPP", NA))

  if (!is.null(region))
    QQ <- QQ[QQ$region %in% region, ]
  
  # add the ptid's
  QQ$ptid <- NA
  if ("NEPOOL" %in% unique(QQ$region)){
    if (!exists("NEPOOL") || !is.environment(NEPOOL))
      FTR.load.ISO.env("NEPOOL")
    
    aux <- subset(QQ, region=="NEPOOL"); aux$ptid <- NULL
    aux$deliv.pt <- gsub("^MCC ", "", toupper(aux$delivery.point))
    aux$deliv.pt <- gsub(" DA$", "", aux$deliv.pt)
    aux$deliv.pt <- gsub(" RT$", "", aux$deliv.pt)
    aux <- merge(aux, NEPOOL$MAP[, c("deliv.pt", "ptid")], by="deliv.pt", all.x=T)
    aux$deliv.pt <- NULL
    
    QQ  <- rbind(subset(QQ, region!="NEPOOL"), aux)   # put it back 
  }
  if ("NYPP" %in% unique(QQ$region)){
    if (!exists("NYPP") || !is.environment(NYPP))
      FTR.load.ISO.env("NYPP")
    aux <- subset(QQ, region=="NYPP"); aux$ptid <- NULL
    aux$mw <- -aux$mw   # reverse the sign because of SecDb booking

    map <- NYPP$MAP[, c("mccLocations", "ptid")]
    names(map)[1] <- "delivery.point"
    aux <- merge(aux, map, by="delivery.point", all.x=T)
    QQ  <- rbind(subset(QQ, region!="NYPP"), aux)
  }

  QQ$variable <- ifelse(QQ$mw > 0, "sink.ptid", "source.ptid") 
  
##   # try to fix DeliveryPt, Name inconsitency if there is one
##   if (any(is.na(QQ$ptid)))  QQ <- .fix.name.map(QQ)
  if (incdec){
    QQ$schedule  <- gsub(".* ", "", QQ$delivery.point)
    QQ$variable  <- gsub("ptid", "schedule", QQ$variable)
    QQ <- unique(QQ[,c("eti", "region", "book", "counterparty", "start.dt",
      "end.dt", "buy.sell", "qty.bucket.type", "mw", "variable", "ptid",
                       "schedule")])
    names(QQ)[8] <- "bucket"
  } else {
    QQ <- unique(QQ[,c("eti", "region", "book", "counterparty", "start.dt",
      "end.dt", "buy.sell", "class.type", "mw", "variable", "ptid")])
  }
  colnames(QQ)[ncol(QQ)] <- "value"
  QQ$mw <- abs(QQ$mw)     # sources and sinks are distinguished
  QQ <- cast(QQ, ... ~ variable, I)
  # order the source sink columns properly ... 
  QQ <- QQ[,c(1:(ncol(QQ)-2), ncol(QQ), ncol(QQ)-1)]  
  
  return(data.frame(QQ))
}



########################################################################
#
`.FTR.loadPositions.SecDb` <-
function(books=NULL, asOfDate=NULL, region=NULL, filter=NULL)
{
  cat("Getting positions from SecDb ... ")
  QQ <- NULL
  for (book in books){
    res <- secdb.invoke("AAD: _lib SecDB to R", "read_deltas_leaf",
                      list(book),
                      format(asOfDate, "%Y%m%d"))
    if (length(res)==0){next}
    aux <- matrix(unlist(res), nrow=length(res), byrow=T)
    aux <- cbind(book, aux)
    colnames(aux) <- c("book", tolower(gsub(" ", ".", names(res[[1]]))))
    QQ  <- rbind(QQ, as.data.frame(aux))
  }
  
  ind <- grep("CASH", toupper(QQ$security.type))
  if (length(ind)>0){QQ <- QQ[-ind,]}

  # select the region of interest
  if (!is.null(region)){
    ind <- grep(toupper(region), toupper(QQ$market.location))
    QQ  <- QQ[ind,]
  }
  
  QQ$end.date   <- as.Date(QQ$end.date,   format="%d%b%y")
  QQ$start.date <- as.Date(QQ$start.date, format="%d%b%y")
 
  QQ$location <- gsub("MCC ", "", toupper(QQ$delivery.point))
  QQ$location <- gsub(" DA$", "", QQ$location)
  QQ$location <- gsub(" RT$", "", QQ$location)

  # remove the one day only positions
  QQ <- subset(QQ, end.date-start.date>0)  
  rownames(QQ) <- NULL
  
  cat("Done.\n")
  return(QQ)
}

.fix.name.map <- function(QQ)
{
  ind <- which(is.na(QQ$ptid))
  aux <- sapply(strsplit(QQ$location[ind], "\\."), function(x){x[length(x)]})
  QQ$ptid[ind] <- aux
  return(QQ)
}




## .add.class.type <- function(QQ){

##   aux <- strsplit(as.character(QQ$curve), " ")
##   mkt <- t(sapply(aux, function(x){x[2:3]}))
##   mkt[,2] <- gsub("H", "", mkt[,2])

##   .map.class.type <- function(x){
##     res <- "ONPEAK"
##     if (x[1]=="PWX"){
##       if (any(regexpr(x[2], c("7X8", "2X16"))))
##         res <- "OFFPEAK"
##     }
##     if (x[1]=="PWY"){
  
##     }
    
##   }
## }
##   aux <- aggregate(rep(1, nrow(QQ)), list(QQ$ETI, QQ$CONTRACT_DATE), sum)
##   aux <- unique(aux[,c(1,3)])
##   aux[,2] <- aux[,2]/2         # sink and source for all of them ...
##   aux <- aux[aux$x >= 1, ]
##   # this may occasionally not work.  Should test...
##   aux$class.type <- ifelse(aux$x == 1, "ONPEAK", ifelse(aux$x==2, "OFFPEAK",
##                              "FLAT"))
##   colnames(aux)[1] <- "ETI"
