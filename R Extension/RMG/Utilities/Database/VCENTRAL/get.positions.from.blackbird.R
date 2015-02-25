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

source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
suppressWarnings(try(memory.limit(3*1024), silent=TRUE))

##########################################################################
# Fix issues with wrong delivery points.  It is recommended to call this
# function if you use the delivery point info.  
#
# When the prism positions are correct this function will do nothing!
#
# This function is expensive, and for large QQ's it may take long to execute.
#
fix.positions.from.blackbird <- function(QQ)
{
  # Noticed some issues with delivery points, see: PZP8E7A
  # some positions with curve COMMOD PWX 5X16 PHYSICAL have delivery point MCC SEMA DA
  # commented this out on 1/4/2013 -- why should this be a problem ?!  I should look at
  # delivery points not curve. 
  ## ind <- which(abs(QQ$delta) > 1 & abs(QQ$notional) < 10^-4)
  ## if (length(ind) > 0){
  ##   ind.PWX <- grep("^COMMOD PWX", QQ$curve[ind])
  ##   if (length(ind.PWX)>0){
  ##     ind <- ind[ind.PWX]
  ##     aux <- sapply(strsplit(QQ$curve[ind], " "), length)
  ##     ind.hub <- ind[which(aux == 4)] # missing location, e.g. COMMOD PWX 5X16 PHYSICAL 
  ##     ind.dp  <- ind[which(QQ$delivery.point[ind.hub] != "HUB")] # with wrong delivery points
  ##     # so set the delivery point to HUB
  ##     QQ$delivery.point[ind.dp] <- "HUB"
  ##   }
  ##   # maybe other regions have the same issues, need to check ...
  ## }

  # eliminate the cash positions
  QQ <- subset(QQ, !(curve %in% c("USD", "CAD")))

  # remove ISOFEE, VLRCST, UCAP, etc from attrition books.  That's the only
  # place that I'm aware that has service == "NULL"
  ind <- which(QQ$leg.type == "ELEC ATTRITION PRICER")
  if (length(ind)>0) {
     # strange as it seems, region seem to have the proper filter
     ## ind.rm <- !(QQ$region[ind] %in% c("PEAK", "OFFPEAK"))  <-- commented out on 6/12/2012
     ## QQ <- QQ[-ind[ind.rm],]
  }

  # BASKET OPTIONs have also service == "ENERGY"
  # try to fix the delivery.point for these curves,
  # the 4th string from curve is the delivery.point
  ind <- (QQ$service=="ENERGY" & QQ$delivery.point=="NULL" & grepl("BASKET OPTION",
     QQ$leg.type) & grepl("COMMOD PW.* PHYSICAL", QQ$curve) )
  if (any(ind)){
    aux <- sapply(strsplit(QQ$curve[ind], " "), "[", 4)
    QQ$delivery.point[ind] <- aux
  }

  # sometimes UCAP trades have service Energy if they're are in a leg.type=="BASKET OPTION"
  ind <- QQ$curve=="COMMOD PWX UCAP ROP PHYSICAL" & QQ$service=="ENERGY"
  QQ$service[ind] <- "UCAP"
  
  # some NEPOOL deals done pre SMD have delivery point = PTF
  ind <- which(QQ$delivery.point == "PTF" & QQ$mkt == "PWX")
  if (length(ind)>0)
    QQ$delivery.point[ind] <- "HUB"
  
  # remove positions w/o curves. 
  ind <- which(QQ$curve == "NO CURVE NAME")
  if (length(ind)>0) QQ = QQ[-ind,]
       
  # this occurs for some old deals, need to find the ETI ... 
  ind <- grep("COMMOD PWR NEPOOL", QQ$curve)
  if (length(ind)>0){
    # care only if service == ENERGY
    ind <- ind[which(QQ$service[ind]=="ENERGY")]
    QQ$curve[ind] <- "COMMOD PWX 5X16 BOS PHYSICAL"
  }

  # Noticed on 1/13/2012 that FTR trades show funny positions in PRISM
  # see for example PAP4ISD, Cal 12 offpeak path, Hub -> Nema
  # so, for FTRs, make the delta == the notional part, and get rid of the other
  # legs.  SecDb is trying to be cute, and I cannot make sense of those legs.
  #
  ind.ftrs <- QQ$deal.sub.type == "FTR"
  QQ$delta[ind.ftrs] <- QQ$notional[ind.ftrs]
  
  return(QQ)
}




###############################################################################
# NOTE: For most books, SERVICE == "ENERGY" usually brings back the correct
#   deltas.  Attrition books have SERVICE == "NULL", and they have VLRCST, ISOFEE, 
#   UCAP, etc. in them.  See the fix.positions.from.blackbird for a way to fix this.
#
get.positions.from.blackbird <- function(asOfDate=Sys.Date()-1, books=NULL,
  service=c("ENERGY", "NULL"), contract.dt=NULL, etis=NULL, fix=TRUE,
  portfolio=NULL) 
{
  if (!("RODBC" %in% search()))
    require(RODBC)
  
  query <- paste("select eti, trade_type, trading_book, load_name, ",
    "deal_sub_type, load_class, is_cleared, is_ibt, counterparty, ",
    "leg_type, region, buy_sell_cd, delta, trade_date, ",
    "trade_start_date, trade_end_date, curve, delivery_point, ", 
    "volume, notional, contract_date, contract_price, qty_bucket_type, ",
    "service, quantity_uom, mtm_usd ", 
    "from prism.trade_flat_vw t where t.valuation_date ='",
    format(asOfDate, "%d%b%Y"), "'", sep="") 


  if (!is.null(portfolio)) {
    books <- .getPortfolioBooksFromSecDb(portfolio)
  }
  
  if (length(books)!=0){
    aux   <- paste(toupper(books), sep="", collapse="', '")
    aux   <- paste(" and trading_book in ('", aux, "')", sep = "")
    query <- paste(query, aux, sep="")
  } 

  if (!is.null(service)){
    if (identical(toupper(service),"ENERGY"))
       rLog("NOTE: Attrition deals have sevice='NULL' so you will miss them!")
    svc   <- paste(service, sep="", collapse="', '")
    aux   <- paste(" and service in ('", svc, "')", sep = "")
    query <- paste(query, aux, sep="")
  }

  if (!is.null(contract.dt)){
    aux   <- paste(" and contract_date = '", format(contract.dt, "%d-%b-%y"),
                                                    "'", sep="")
    query <- paste(query, aux, sep="")
  }

  if (!is.null(etis)){
    aux   <- paste(toupper(etis), sep="", collapse="', '")
    aux   <- paste(" and eti in ('", aux, "')", sep = "")
    query <- paste(query, aux, sep="")
  }
  
  
  data <- NULL
  rLog("Querying PRISM...")
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/PRISMP.dsn"),
    ";UID=e47187;PWD=e47187;", sep="")

  #rLog("Sys.info is")
  #print(Sys.info())
  #rLog("con.string is", con.string)
  
  con <- odbcDriverConnect(con.string)
  if (con == -1) {
    rLog("Cannot connect to DB.  Exiting ...")
    q(save="no", 99)
  }
  
  data <- sqlQuery(con, query) 
  odbcCloseAll()
  
  rLog("Done querying db.")

  # do some cosmetics 
  names(data) <- gsub("_", ".", tolower(names(data)))
  names(data)[which(names(data)=="contract.date")]   <- "contract.dt"
  names(data)[which(names(data)=="qty.bucket.type")] <- "bucket"
  data$contract.dt <- as.Date(data$contract.dt)
  data$trade.date <- as.Date(data$trade.date)
  data$mkt <- sapply(strsplit(data$curve, " "), function(x){x[2]})
  
  if (fix & nrow(data)>0)
    data <- fix.positions.from.blackbird(data)
    
  return(data)
}


####################################################################
####################################################################
#
.demo.get.postions.from.blackbird <- function()
{

  require(CEGbase)
  require(reshape)
  require(SecDb)

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  
  asOfDate    <- as.Date("2012-09-04")
  portfolio   <- "PM Northeast Portfolio"
  qq <- get.positions.from.blackbird( asOfDate=asOfDate,
    fix=TRUE, service="TMOR", portfolio=portfolio )
  unique(qq[,c("trading.book", "eti", "counterparty")])
  
  asOfDate    <- as.Date("2012-06-11")
  portfolio   <- "Nepool Mixed Attr Portfolio"
  qq <- get.positions.from.blackbird( asOfDate=asOfDate,
    fix=FALSE, portfolio=portfolio, service="FWDRES" )    # <-- this is junk
  cast(qq, contract.dt + curve ~ ., sum, fill=NA,
      value="delta")

  
  asOfDate  <- as.Date("2012-10-30")
  books     <- "NEBKSAPP"
  qq <- get.positions.from.blackbird( asOfDate=asOfDate, books=books, fix=FALSE)
  cast(qq, contract.dt + curve + delivery.point ~., sum, value="delta")

  
  asOfDate  <- as.Date("2012-05-21")
  etis      <- "PAP4LMJ"
  qq <- get.positions.from.blackbird( asOfDate=asOfDate, etis=etis, fix=TRUE)
  q1 <- subset(qq, contract.dt == as.Date("2012-08-01"))

  asOfDate  <- as.Date("2012-07-26")
  books     <- "NSEUARI"
  qq <- get.positions.from.blackbird( asOfDate=asOfDate, books=books, fix=FALSE)
  cast(qq, contract.dt + curve + delivery.point ~., sum, value="delta")

  
}

