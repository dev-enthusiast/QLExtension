# Utilities for reporting PnL for incdecs, FTRs and TCCs
# For an implementation see
#    H:\user\R\RMG\Energy\Trading\Congestion\Condor\daily_PnL_AAD.R
#
# Contains several functions:
#   - .classify_trade
#   - .construct_hourly_qty  (from monthly buckets)
#   - .get_cash_positions
#   - .is_virtual_trade
#   - .is_ftr_trade
#   - .is_spread_trade
#   - .load_map_ptid
#   - .map_location_to_tsdbsymbol
#   - calculate_bal_PnL                    <-- main function
#   - calculate_realized_PnL               <-- main function   
#   - cashMonth_ftr_PnL
#   - cashMonth_hub_PnL, for some residual position
#   - cashMonth_spread_PnL, for your spread to hub position
#   - daily.change.PQ
#   - daily.FTR.PnL 
#   - daily.incdec.PnL
#   - get_fix_price = price in SecDb at day 1
#   - get_floating_price = settled price from bom. (need tsdb symbol!)
#   - main.PnL <-- puts info together for the daily congestion PnL report,
#        used by procmon, not that great, I should retire it!
#   - nodal_realized_PnL = compare the realized nodal spreads to Hub relative to
#                          what was booked
#   - summary.PnL used for archiving
#


#######################################################################
# determine the type of the trade: FTR, VIRTUAL, SPREAD, OUTRIGHT
# given a set of positions
#

.classify_trade <- function(QQ)
{
  if (!all(c("eti", "bucket", "delta", "delivery.point",
             "trade.start.date", "trade.end.date") %in% colnames(QQ)))
    stop("Missing one of required columns: eti, ")

  QQ <- ddply(QQ, .(eti), function(x){
    x$trade.type <- ifelse(.is_virtual_trade(x),    "VIRTUAL",
                      ifelse(.is_ftr_trade(x),      "FTR",
                        ifelse(.is_spread_trade(x), "SPREAD",
                                                    "OTHER")))
    x
  }) 

  QQ
}


#######################################################################
# given a set of positions, expand to hourly curve
#
.construct_hourly_qty <- function(x)
{
  if (!all(c("mkt", "bucket", "month", "delta") %in% names(x)))
    stop("Missing a required column")

  ddply(x, .(month, mkt, bucket), function(y) {
    month <- y$month[1]
    region <- if (y$mkt[1] == "PWX") {
      "NEPOOL"
    } else if (y$mkt[1] == "PWY") {
      "NYPP"
    } else {
      "GOOGOOGAGA" }
    startDt <- as.POSIXct(paste(format(month), "01:00:00"))
    endDt <- as.POSIXct(paste(format(seq(month, by = "1 month", 
        length.out = 2)[2]), "00:00:00"))
    #hrs <- secdb.getBucketMask(region, y$bucket[1], startDt, endDt)
    hrs <- secdb.getHoursInBucket(region, y$bucket[1], startDt, endDt)
    names(hrs)[1] <- "hour"
    
    y$mw <- y$delta/nrow(hrs)
    expand.grid.df(y, hrs) 
  })

}


#######################################################################
# Pull the positions in the form that you want
#
.get_positions <- function(asOfDate, months=NULL,
   books=c("CONGIBT", "NPFTRMT1", "NESTAT", "VIRTS"), classifyTrade=TRUE)
{
  # load the positions from the archive 
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  fname <- paste(DIR, "positionsAAD_", format(asOfDate), ".RData", sep="") 
  load(fname)
  rLog("Loaded", fname)
  
  QQ$value <- QQ$delta
  QQ <- subset(QQ, service=="ENERGY" &
                   mkt %in% c("PWX", "PWY") )
  
  if (!is.null(months)) 
    QQ <- subset(QQ, contract.dt %in% months)
  
  pb <- unique(QQ[,c("portfolio", "trading.book")])
  bb <- merge(pb, data.frame(trading.book=books, books=books))
  bb <- ddply(bb, .(trading.book), function(x){x[1,]})
  bb$books <- NULL
  
  QQ <- merge(QQ, bb) # select only the books you are interested in only ONCE
  QQ$buy.sell <- QQ$trade.type
  QQ$trade.type <- NULL
  QQ <- QQ[order(QQ$eti), ]

  if (classifyTrade)
    QQ <- .classify_trade(QQ)  # trade.type %in% c(SPREAD, VIRTUAL, FTR, OTHER)

  cbind(asOfDate=asOfDate, QQ)
}


#######################################################################
# determine if this trade is a virtual.  Check that it's a one day
# trade, between DA and RT.   
# 
.is_virtual_trade <- function(qq)
{
  score <- 0
  
  startDate <- unique(qq$trade.start.date)
  endDate   <- unique(qq$trade.end.date)

  if (length(startDate)==1 & length(endDate)==1 & startDate==endDate){
    score <- score + 0.75  # very likely an INC/DEC  
  }

  da.legs <- length(grep(" DA$", qq$delivery.point))
  rt.legs <- length(grep(" RT$", qq$delivery.point))
  
  if ( da.legs == rt.legs & da.legs+rt.legs == nrow(qq) )
    score <- score + 0.75  # even more likely an INC/DEC  
  
  if (score > 1) TRUE else FALSE      
}


#######################################################################
# determine if this trade is an ftr.  Check that you have MCC at the
# beginning of delivery.point (only NEPOOL) and that you're a spread
#
.is_ftr_trade <- function(qq)
{
  if (toupper(qq$deal.sub.type[1]) == "FTR"){TRUE} else {FALSE}
  ## ind.mcc <- grep("^MCC ", qq$delivery.point)
  ## # offpeak trades are represented as a 2X16H and 7X8 leg!
  ## if (length(ind.mcc)==2 & qq[ind.mcc[1], "notional"]==-qq[ind.mcc[2], "notional"]) {
  ##   TRUE & .is_spread_trade(qq)
  ## } else {
  ##   FALSE
  ## }
}

#######################################################################
# determine if this trade is a spread (two locations)
# and deltas net to zero
#
.is_spread_trade <- function(qq)
{
  netQtyEq0 <- daply(qq, .(bucket), function(x){abs(sum(x$delta))<0.001})
  
  if ((nrow(qq) %% 2 == 0) & (all(netQtyEq0))) TRUE else FALSE
}


#######################################################################
# load the mapping between SecDb delivery.points and ptids for PWX, PWY
#
.load_map_ptid <- function()
{
  rLog("Load PWX, PWY node mappings")
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
  pwx <- cbind(mkt="PWX", MAP[,c("location", "ptid", "name")])
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/map.RData")
  pwy <- cbind(mkt="PWY", MAP[,c("location", "ptid", "name")])
  MAP <- NULL
  
  map_ptid <- rbind(pwx, pwy)
  #colnames(map_ptid)[1] <- "delivery.point"
  
  map_ptid
}


######################################################################
# Map a location to a REALIZED tsdb symbol
#
# map_ptid[, c("mkt", "location", "ptid")]
#
.map_location_to_tsdbsymbol <- function(x, map_ptid=NULL)
{
  if (!all(c("curve", "delivery.point", "mkt") %in% names(x)))
    stop("Missing a required column: curve, delivery.point, mkt")

  if (is.null(map_ptid))
    map_ptid <- .load_map_ptid()
  names(map_ptid)[which(names(map_ptid)=="mkt")] <- "mkt"

  x$is.da <- !grepl(" RT$", x$delivery.point)   # better look at RT!
#  x$is.da[grepl("^HUB|SC|CL8$", x$delivery.point)] <- TRUE
#  x$is.da[grepl("^DAM ", x$delivery.point) & x$mkt == "PWY"] <- TRUE

  # add the price component 
  x <- ddply(x, .(mkt, delivery.point, curve), function(y) {
    y$component <- if (grepl("^MCC ", y$delivery.point) & (y$mkt == "PWX")) {
        "MCC"
      } else {
        "LMP"  
      }
    y
  })

  # extract the location in order to get the ptid
  loc <- strsplit(x$delivery.point, " ")
  x$location <- sapply(loc, function(x){
    if (length(x)==1){                     # <-- one chunk
      x 
    } else if (length(x)==2) {             # <-- two chunks 
      if (x[2] %in% c("DA", "RT")){
        x[1] 
      } else if (x[1] %in% c("DAM")) {
        x[2]
      } else {
        "Failed from two chunks!"
      }
    } else if (length(x)==3) {             # <-- three chunks
      if ((x[1] == "MCC") & (x[3] %in% c("DA", "RT"))) {
        x[2]
      } else {
        "Failed from three chunks!"
      }
    } else {
      "More than 3 chunks in delivery.point!"
    }
  })

  x <- merge(x, map_ptid[,c("mkt", "location", "ptid")],
    by=c("mkt", "location"), all.x=TRUE)    
  x$ptid[x$mkt=="PWX" & x$location=="KLN"]  <- 14614  #  Kleen is special!
  x$ptid[x$mkt=="PWX" & x$location=="NEMA"] <- 4008   #  UUGH
  
  #x$ptid[is.na(x$ptid)] <- "Unknown ptid!"

  # construct the tsdb symbol
  x <- ddply(x, .(mkt, delivery.point, curve), function(y){
    if (y$mkt == "PWX") {
      symb <- "NEPOOL_SMD_"
      if (y$is.da) {
        symb <- paste(symb, "DA_", sep="")
      } else {
        symb <- paste(symb, "RT_", sep="")
      }
      if (is.na(y$ptid)){
        symb <- NA
      } else {
        symb <- paste(symb, y$ptid, "_", sep="")      
        symb <- switch(y$component,
          `LMP` = paste(symb, "LMP", sep=""),
          `MCC` = paste(symb, "congcomp", sep=""),
          "Unknown component!")
      }
    } else if (y$mkt == "PWY") {
        if (y$is.da) {
         symb <- "NYPP_DAM_"
       } else {
         symb <- "NYPP_RT_"
       }
       if ((y$ptid > 60000) & (y$ptid < 700000)) {
         token <- "ZONE"
       } else {
         token <- "GEN"
       }
       if (is.na(y$ptid)){
         symb <- NA
       } else {
        symb <- switch(y$component,
          `LMP` = paste(symb, token, "_LBM_", y$ptid, sep=""),
          `MCC` = paste(symb, "_MCC_", y$ptid, sep=""),
          "Unknown component!")
       } 
    } else {
       symb <- NA
    }
    y$tsdb.symbol <- symb
    y
  })
  
  x
}


######################################################################
# calculate a BAL style PnL between two dates for some positions
# PP is a list of data.frames with forward prices. Use it as a price cache
#
# Test if it works for service != ENERGY
#
calculate_bal_PnL <- function(QQ1, PP1, QQ2, PP2)
{
  #source("H:/user/R/RMG/Energy/Trading/Congestion/lib.daily.PnL.R")
  colnames(PP1)[which(colnames(PP1)=="month")] <- "contract.dt"
  colnames(PP2)[which(colnames(PP2)=="month")] <- "contract.dt"
  colnames(PP1)[which(colnames(PP1)=="serviceType")] <- "service"
  colnames(PP2)[which(colnames(PP2)=="serviceType")] <- "service"
  colnames(PP1)[which(colnames(PP1)=="value")] <- "P1"
  colnames(PP2)[which(colnames(PP2)=="value")] <- "P2"
  PP1 <- subset(PP1, P1 != Inf)  # for expired markets
  PP2 <- subset(PP2, P2 != Inf)

  # ignore the ETI, doesn't help me
  Q1 <- cast(QQ1, trading.book + mkt + curve + delivery.point +
   bucket + service + contract.dt ~ ., sum, value="delta")
  colnames(Q1)[ncol(Q1)] <- "Q1"
  Q2 <- cast(QQ2, trading.book + mkt + curve + delivery.point +
   bucket + service + contract.dt ~ ., sum, value="delta")
  colnames(Q2)[ncol(Q2)] <- "Q2"
  
  PQ1 <- merge(Q1[,c("trading.book", "mkt", "curve", "delivery.point",
    "bucket", "service", "contract.dt", "Q1")],
    PP1[,c("service", "mkt", "delivery.point", "bucket", "contract.dt", "P1")])

  PQ2 <- merge(Q2[,c("trading.book", "mkt", "curve", "delivery.point",
    "bucket", "service", "contract.dt", "Q2")],
    PP2[,c("service", "mkt", "delivery.point", "bucket", "contract.dt", "P2")])

  total1 <- sum(PQ1$P1*PQ1$Q1, na.rm=TRUE)
  total2 <- sum(PQ2$P2*PQ2$Q2, na.rm=TRUE)
  rLog("Total : ", dollar(total2-total1))
  

  PQ12 <- merge(PQ1, PQ2, all=TRUE)
    
  PQ12$PnL <- PQ12$Q2*PQ12$P2 - PQ12$Q1*PQ12$P1
  subset(PQ12, is.na(PQ12$PnL))
  
  pnl_DP <- cast(PQ12, delivery.point ~ ., sum, na.rm=TRUE, value="PnL")
  colnames(pnl_DP)[2] <- "PnL"
  
  #  PnL = Q2*(P2-P1) + P1*(Q2-Q1)
  pnl_DP <- ddply(PQ12, .(delivery.point), function(x){
    PnL <- sum(x$PnL, na.rm=TRUE)
    total_Q <- sum(x$Q2, na.rm=TRUE)
    PnL_dQ <- sum(x$P1*(x$Q2-x$Q1), na.rm=TRUE)
    PnL_dP <- sum((x$P2 - x$P1)*x$Q2, na.rm=TRUE)
    weighted_dP <- PnL_dP/total_Q
    
    data.frame(total_Q, weighted_dP, PnL_dP, PnL_dQ, PnL)
  })
  
  pnl_DP <- pnl_DP[order(-pnl_DP$PnL),]
  pnl_DP <- cbind(day1=PP1$asOfDate[1], day2=PP2$asOfDate[2], pnl_DP)

  pnl_DP
}



######################################################################
# calculate the FTR PnL 
#
cashMonth_ftr_PnL <- function(focusMonth)
{
  require(FTR)
  require(xlsx)
  FTR.load.ISO.env("NEPOOL")

  lastDay <- min(seq(focusMonth, by="1 month", length.out=2)[2] - 1, Sys.Date()-1)

  auctions <- c(FTR.AuctionTerm(focusMonth, focusMonth),
                paste("F", format(focusMonth, "%y"), "-1Y", sep=""))
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
    format(focusMonth, "%Y%m"), "/FTR.paths.xlsx", sep="")

  aux <- read.xlsx(fname, sheetName="Paths", colIndex=1:12)
  aux <- aux[which(!is.na(aux[,1])),]

  aux$auction <- FTR.AuctionTerm(aux$start.dt, aux$end.dt)
  aux <- subset(aux, auction %in% auctions)
  names(aux)[which(names(aux)=="fix.price")] <- "fixedPrice"
  
  res <- ddply(aux, .(auction), function(x){
    x$path.no <- 1:nrow(x)
    startDt <- as.POSIXct(paste(format(x$start.dt), "01:00:00"))[1]
    endDt   <- as.POSIXct(paste(format(x$end.dt+1), "00:00:00"))[1]
    paths <- x[,c("path.no", "source.ptid", "sink.ptid", "class.type")]

    y <- FTR.getPathReturns(paths, startDt, endDt, ISO.env=NEPOOL, melt=FALSE,
      hourly=TRUE)   # assumes CP = 0, so it's just the hourly SP for the path.
    y <- window(y, start=as.POSIXct(paste(format(focusMonth), "01:00:00")),
                end=as.POSIXct(paste(format(lastDay), "00:00:00")))
    yy <- as.data.frame(melt(as.matrix(y)))
    names(yy) <- c("hour", "path.no", "floatingPrice")
    yy$hour <- as.POSIXct(format(yy$hour))
    yy <- subset(yy, !is.na(floatingPrice))
    
    x <- merge(x, yy, by="path.no", all.x=TRUE)  # add the PnL column
    x$path.no <- NULL
    x
  })
  res$PnL <- res$mw * (res$floatingPrice - res$fixedPrice)
  
  res
}


######################################################################
# How you've done on the residual hub position
#
cashMonth_hub_PnL <- function(focusMonth, QQ, hubInfo)
{
  lastDay <- min(seq(focusMonth, by="1 month", length.out=2)[2] - 1, Sys.Date()-1)
  
  # all the days in the month, or until yesterday 
  days <- seq(focusMonth, lastDay, by="1 day")
  
  qq <- cast(QQ, mkt + bucket ~ ., sum,             # calculate residual position
    value="delta", subset=trade.type %in% c("SPREAD", "OTHER"))
  names(qq)[ncol(qq)] <- "delta"

  qq <- merge(qq, data.frame(mkt=names(hubInfo),    # add hub delivery.point 
    delivery.point=sapply(hubInfo, "[[", "delivery.point")))
  
  qq <- merge(qq, cbind(mkt=names(hubInfo),         # add hub curve
    do.call(rbind, lapply(hubInfo, "[[", "commod"))))
  qq$month <- focusMonth
  
  hQQ <- .construct_hourly_qty(qq)   # expand it hourly
  hQQ$delta <- hQQ$month <- hQQ$curve <- NULL
  
  # get the "fix price" as of the last day before beginning of month
  fixedPrice <- get_fix_price(fixPriceDate, focusMonth, focusMonth, qq)
  names(fixedPrice)[4] <- "fixedPrice"
      
  # get the floating & settled price from focusMonth to lastDay, hourly
  floatingPrice <- get_floating_price(qq, days)
  floatingPrice$curve <- NULL
  
  PnL <- merge(hQQ, fixedPrice)
  PnL <- merge(PnL, floatingPrice, all=TRUE)

  PnL <- PnL[order(PnL$mkt, PnL$delivery.point, PnL$bucket, PnL$hour), ]
  
  PnL$PnL <- PnL$mw * (PnL$floatingPrice - PnL$fixedPrice)    

  PnL
}


######################################################################
# Only for the cash month!!!  Some function will work with several months
# but focus is on cash month only.
#
cashMonth_spread_PnL <- function(focusMonth, QQ, hubInfo)
{ 
  lastDay <- min(seq(focusMonth, by="1 month", length.out=2)[2] - 1, Sys.Date()-1)
  
  # all the days in the month, or until yesterday 
  days <- seq(focusMonth, lastDay, by="1 day")

  rLog("Price SPREADs and OTHER swaps")
  qSS <- cast(QQ, mkt + bucket + delivery.point + curve ~ ., sum,
    value="delta", subset=trade.type %in% c("SPREAD", "OTHER"))
  names(qSS)[ncol(qSS)] <- "delta"
  qSS <- subset(qSS, abs(delta) > 0.01)
  qSS$month <- focusMonth

  hQQ <- .construct_hourly_qty(qSS)
  
  # get the "fix price" as of the last day before beginning of month
  fixedPrice <- get_fix_price(fixPriceDate, focusMonth, focusMonth, qSS)
  names(fixedPrice)[4] <- "fixedPrice"
      
  # get the floating & settled price from focusMonth to lastDay, hourly
  floatingPrice <- get_floating_price(qSS, days)
  floatingPrice$curve <- NULL

  PP <- merge(floatingPrice, fixedPrice)     # put the prices together
  PP <- merge(hQQ, PP, all.x=TRUE)           # add the positions

  # calculate the spread view ...
  x <- PP[,c("mkt", "delivery.point", "month", "bucket", "hour",
             "floatingPrice", "fixedPrice", "mw")]
  y1 <- view_as_spread(x[,c(1:5,6)], hubInfo, ind=6)  # floatingPrice

  y2 <- view_as_spread(x[,c(1:5,7)], hubInfo, ind=6)  # fixedPrice

  y3 <- view_as_spread(x[,c(1:5,8)], hubInfo, ind=6)  # deltas

  PnL <- merge(y1,  y2, all=TRUE)
  PnL <- merge(PnL, y3, all=TRUE) 
  
  PnL <- PnL[order(PnL$mkt, PnL$spread, PnL$bucket, PnL$hour), ]
  
  PnL$PnL <- PnL$spread.mw * (PnL$spread.floatingPrice - PnL$spread.fixedPrice)    

  PnL
}



######################################################################
# Calculate cash PnL with various levels of detail.
# Incorporate DA results.
#
# focusMonth - a beginning of the month Date 
#
# QQ - are the positions from prism as returned by the morning pull
#   from prism
#
# PnL is calculated relative to the marks 1b before the cash month
#
calculate_realized_PnL <- function(focusMonth, QQ)
{
  lastDay <- min(seq(focusMonth, by="1 month", length.out=2)[2] - 1, Sys.Date()-1)
  
  # all the days in the month, or until yesterday 
  days <- seq(focusMonth, lastDay, by="1 day")

  res <- NULL
  
  rLog("Price VIRTUALs")
  #qVV <- subset(QQ, trade.type=="VIRTUAL")
  #if (nrow(qVV)>0)
  {
    trd <- get_booked_virtuals(days=days, books="VIRTSTAT")
    aux <- ldply(trd, function(x) { data.frame(trade.date=x$trade.start.date)})
    pnlVV <- price_pfolio_incdec(trd)
    pnlVV <- data.frame(eti=names(pnlVV), trade.type="VIRTUAL", PnL=as.numeric(pnlVV))
    res$VIRTUAL <- pnlVV
  }

  rLog("Price FTRs")
  qFTR <- subset(QQ, trade.type=="FTR")
  if (nrow(qFTR)>0){
    # do something!
  }
  
  
  rLog("Price SPREADs and OTHER swaps")
  qSS <- subset(QQ, trade.type %in% c("SPREAD", "OTHER"))
  if (nrow(qSS)>0) {

    # get the "fix price" as of the last day before beginning of month
    fixedPrice <- get_fix_price(fixPriceDate, qSS)
    names(fixedPrice)[4] <- "fixedPrice"
    qSS <- merge(qSS, fixedPrice, by=c("delivery.point", "bucket", "mkt"),
                 all.x=TRUE)
    
    # get the floating/settled price from focusMonth to lastDay, daily
    FP <- get_floating_price(qSS, days)
      
    # add the daily floating price, calculate PnL
    X <- merge(qSS, FP, all.x=TRUE)
    X <- X[order(X$eti, X$delivery.point, X$day), ]
    X$PnL <- X$delta * (X$floatingPrice - X$fixedPrice)    
    X <- X[,c("eti", "trade.type", "mkt", "delivery.point", "day", "bucket",
              "fixedPrice", "floatingPrice", "delta", "PnL")]
    X <- X[order(X$eti, X$delivery.point, X$day), ]
    res$SPREAD <- X
  }
  
  res
}


#################################################################
# QQ - a data.frame as returned by FTR.loadPositions
# days - for which days to calculate the PnL
# hasFixPrice - a boolean if the QQ already contains the FixPrice
#
daily.FTR.PnL <- function(asOfDate, QQ, ISO.env, days=NULL,
  hasFixPrice=TRUE)
{
  if (nrow(QQ)==0){return(NULL)}
  QQ$path.no <- 1:nrow(QQ)

  if (!hasFixPrice)  # add the clearing prices
    QQ <- merge(QQ, FTR.get.CP.for.paths(QQ, ISO.env))

  # add the source/sink
  QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, ISO.env$MAP, to="name"))
  
  # get the hourly returns from the beginning of the month ...
  if (is.null(days)) {
    start.dt <- as.POSIXct(paste(format(asOfDate, "%Y-%m-01"),
                                 "01:00:00"))
    end.dt <- as.POSIXct(paste(asOfDate+1, "00:00:00"))
  }

  RR <- FTR.getPathReturns(QQ, start.dt, end.dt=end.dt, ISO.env=ISO.env,
                           hourly=TRUE)

  RR[is.na(RR)] <- 0
  RR <- data.frame(dt=as.POSIXct(index(RR)), RR)
  rownames(RR) <- NULL
  RR$day <- as.Date(format(RR$dt - 1, "%Y-%m-%d"))

  # aggregate daily
  DD <- melt(RR[,-1], id=ncol(RR)-1)
  DD <- cast(DD, day + variable ~ ., sum)
  names(DD)[2:3] <- c("path.no", "PnL")
  DD$path.no <- as.numeric(gsub("X", "", DD$path.no))

  res <- merge(QQ, DD, by="path.no", all.x=TRUE)

  return(res)  
}

#################################################################
# This function works only with NEPOOL inc/decs
#
daily.incdec.PnL <- function(asOfDate, books)
{
  QQ <- try(INC.loadPositions(from="PRISM", books=books, asOfDate))
  
  if (class(QQ)=="try-error" | is.null(QQ)) return(NULL)
  res <- INC.PnL(QQ, "NEPOOL")
  res <- res[order(res$start.dt, res$eti, res$source.schedule),]
  
  return(res)
}


#################################################################
# get prices before start of month
# QQ[, c("mkt", "delivery.point")]
#
get_fix_price <- function(asOfDate, QQ)
{
  uMKT <- sort(unique(QQ$mkt))

  PP <- NULL
  if ("PWX" %in% uMKT){
    qq <- subset(QQ, mkt=="PWX")
    fromMonth   <- min(QQ$contract.dt)
    toMonth     <- max(QQ$contract.dt)
    commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
    location    <- unique(qq$delivery.point)
    bucket      <- c("5X16", "2X16H", "7X8")
    serviceType <- unique(qq$service)
    useFutDates <- FALSE

    curveData <- PM:::secdb.getElecPrices( asOfDate, fromMonth, toMonth,
      commodity, location, bucket, serviceType, useFutDates)
    curveData$mkt <- "PWX"
    PP <- rbind(PP, curveData)
  }

  if ("PWY" %in% uMKT){
    qq <- subset(QQ, mkt=="PWY")
    fromMonth   <- min(QQ$contract.dt)
    toMonth     <- max(QQ$contract.dt)
    commodity   <- "COMMOD PWR NYPP PHYSICAL"
    location    <- unique(subset(QQ, mkt=="PWY")$delivery.point)
    bucket      <- c("5X16", "2X16H", "7X8")
    serviceType <- unique(qq$service)
    useFutDates <- FALSE

    curveData <- PM:::secdb.getElecPrices( asOfDate, fromMonth, toMonth,
      commodity, location, bucket, serviceType, useFutDates)
    curveData$mkt <- "PWY"
    PP <- rbind(PP, curveData)
  }
  # PP <- PP[, c("mkt", "location", "bucket", "value")]
  names(PP)[2] <- c("delivery.point")
  
  PP
}

#################################################################
# get prices inside the month, from the beginning of the month
# symbs[,c("bucket", "tsdb.symbol")]  -- not uniques OK
# given a set of positions.
#
# Returns the floatingPrice by day
get_floating_price <- function(x, days)
{
  if (!all(c("mkt", "bucket", "curve", "delivery.point") %in% names(x)))
    stop("Missing column mkt, delivery.point, curve, bucket")

  # get the map to tsdb symbols
  aux   <- unique(x[,c("mkt", "delivery.point", "curve")])
  symbs <- .map_location_to_tsdbsymbol(aux)

  # add bucket info
  symbs <- merge(symbs, unique(x[,c("mkt", "delivery.point",
                                      "curve", "bucket")]))
  uSymbs <- na.exclude(unique(symbs[,c("tsdb.symbol", "bucket")]))

  startDt <- as.POSIXct(paste(format(days[1]), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(days[length(days)]+1), "00:00:00"))
  PP <- FTR:::FTR.load.tsdb.symbols(unique(uSymbs$tsdb.symbol), startDt, endDt)

  uBkt <- unique(symbs$bucket)
  res  <- vector("list", length=length(uBkt))
  for (i in 1:length(uBkt)) {
    ind <- secdb.getBucketMask("NEPOOL", uBkt[i], startDt, endDt)
    ind <- ind[ind[,2], ]  # pick only this bucket
    aux <- PP[ind$time]    # subset prices
    #bux <- aggregate(aux, format(index(aux)-1, "%Y-%m-%d"), mean)  # by day
    cux <- melt(as.matrix(aux))
    colnames(cux) <- c("hour", "tsdb.symbol", "floatingPrice")
    res[[i]] <- cbind(bucket=uBkt[i], cux)
  }
  res <- do.call(rbind, res)

  symbs <- merge(symbs, res, all.x=TRUE)
  symbs <- symbs[order(symbs$location, symbs$bucket, symbs$hour), ]

  y <- merge(unique(x[,c("mkt", "delivery.point", "curve", "bucket")]),
    symbs[,c("bucket", "curve", "delivery.point", "mkt", "hour", "floatingPrice")])

  y$hour <- as.POSIXct(y$hour)
  y
}


#################################################################
#
main.PnL <- function(asOfDate, books, reportName, pnlFileArchive,
  QQ, runFTRs=TRUE, runTCCs=TRUE, runINCDECs=TRUE)
{

  if (missing(QQ)){    # the user can pass his own QQ if needed ...
    goBack <- 5
    last.bday <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
    while (goBack > 0){
      rLog(paste("Getting the positions from PRISM as of", last.bday))
      QQ <- try(FTR.loadPositions(books=books, asOfDate=last.bday))
      if ((class(QQ)!="data.frame") | (nrow(QQ)==0)) {
        last.bday <- last.bday-1
        goBack    <- goBack-1
        print(goBack)
      } else {
        goBack <- -1
      }
      if (goBack==0){
        rLog("Cannot get positions from BlackBird for the past 5 days ... Exiting")
        returnCode <- 1
        return("No positions")
      }
    }
  }
  
  # check if you have some NA's in the source.ptid or sink.ptid
  if (any(is.na(c(QQ$source.ptid, QQ$sink.ptid)))){
    ind <- unique(c(which(is.na(QQ$source.ptid)), which(is.na(QQ$sink.ptid))))
    msg <- "You have a NA source.ptid or sink.ptid.  Please FIX!"
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      "adrian.dragulescu@constellation.com", msg, capture.output(QQ[ind,]))
  }
  
  # for long term auctions the ETI info needs a Round !
  QQ <- .add.tcc.cp.round(QQ)   
  
  if (runFTRs){
    rLog("Get NEPOOL PnL")
    PnL.NEPOOL.FTR <- try(daily.FTR.PnL(asOfDate, QQ, NEPOOL))
    if (class(PnL.NEPOOL.FTR)=="try-error"){
      returnCode     <- 1
      PnL.NEPOOL.FTR <- NULL
    }
  }

  if (runTCCs){
    rLog("Get NYPP PnL")
    PnL.NYPP.TCC <- try(daily.FTR.PnL(asOfDate, QQ, NYPP))
    if (class(PnL.NYPP.TCC)=="try-error"){
      returnCode   <- 1
      PnL.NYPP.TCC <- NULL
    }
  }

  if (runINCDECs){
    rLog("Get Inc/Decs PnL")
    PnL.NEPOOL.incdec <- try(daily.incdec.PnL(last.bday, books))
    if (class(PnL.NEPOOL.incdec)=="try-error"){
      returnCode        <- 1
      PnL.NEPOOL.incdec <- NULL
    }
  }
     
  rLog("Make summary output for email, save PnL archive.")
  out <- summary.PnL(asOfDate, PnL.NEPOOL.FTR, PnL.NEPOOL.incdec,
    PnL.NYPP.TCC, pnlFileArchive, reportName)

  return(out)  
}


#################################################################
# qq - a set of (cash) positions, as in calculate_cash_PnL
# P1 - price before beginning of month
# P2 - realized price inside the month 
#
nodal_realized_PnL <- function(asOfDate, qq, P1, P2)
{

}

#################################################################
#
price_one_spread_trade <- function(x)
{

}

#################################################################
#
price_pfolio_spread_trades <- function(trd)
{

}

#################################################################
#
summary.PnL <- function(asOfDate, PnL.NEPOOL.FTR, PnL.NEPOOL.incdec,
  PnL.NYPP.TCC,  filename, reportName)
{
  if (file.exists(filename))
    load(filename)
  else PnL <- NULL
  
  out <- c("FTRs:", capture.output(print(PnL.NEPOOL.FTR)),
           "", "", "TCCs:", 
           capture.output(print(PnL.NYPP.TCC)),
           "", "", "IncDecs:",
           capture.output(print(PnL.NEPOOL.incdec)))
  
  PnL[[as.character(asOfDate)]] <- list(PnL.NEPOOL.FTR=PnL.NEPOOL.FTR,
    PnL.NEPOOL.incdec=PnL.NEPOOL.incdec, PnL.NYPP.TCC=PnL.NYPP.TCC)

  if (!exists("PnL.Day")) PnL.Day <- NULL

  # save daily PnL
  PnL.Day[[as.character(asOfDate)]] <- list(day=asOfDate,
    NEPOOL.FTR     = sum(PnL.NEPOOL.FTR$dPnL),
    NEPOOL.incdec  = NA,   # I do it below
    NYPP.TCC       = sum(PnL.NYPP.TCC$dPnL))
  if (!is.null(PnL.NEPOOL.incdec)){
    aux <- tapply(PnL.NEPOOL.incdec$PnL, PnL.NEPOOL.incdec$start.dt, sum)
    print(aux)
    for (day in names(aux))
      PnL.Day[[day]]$NEPOOL.incdec <- as.numeric(aux[day])
  }
  
  PnL     <- PnL[order(names(PnL))]
  PnL.Day <- PnL.Day[order(names(PnL.Day))]

  print(as.character(asOfDate))
  # make the email top summary YTD and MTD colums by product:
  ind <- which(as.Date(names(PnL.Day))>=as.Date(format(asOfDate, "%Y-01-01")) &
               as.Date(names(PnL.Day))<=asOfDate)
  YTD <- c(sum(unlist(sapply(PnL.Day[ind], "[", "NEPOOL.FTR")), na.rm=T),
           sum(unlist(sapply(PnL.Day[ind], "[", "NEPOOL.incdec")), na.rm=T),
           sum(unlist(sapply(PnL.Day[ind], "[", "NYPP.TCC")), na.rm=T))
  rLog("YTD:", YTD)
  ind <- which(as.Date(names(PnL.Day))>=as.Date(format(asOfDate, "%Y-%m-01")) &
               as.Date(names(PnL.Day))<=asOfDate)
  MTD <- c(sum(unlist(sapply(PnL.Day[ind], "[", "NEPOOL.FTR")), na.rm=T),
           sum(unlist(sapply(PnL.Day[ind], "[", "NEPOOL.incdec")), na.rm=T),
           sum(unlist(sapply(PnL.Day[ind], "[", "NYPP.TCC")), na.rm=T))
  rLog("MTD:", MTD)
  prevIncDec <- PnL.Day[[as.character(asOfDate-1)]]$NEPOOL.incdec
  if (is.null(prevIncDec)) prevIncDec <- NA
  DAY <- c(PnL.Day[[as.character(asOfDate)]]$NEPOOL.FTR,
           prevIncDec,  # one day back
           PnL.Day[[as.character(asOfDate)]]$NYPP.TCC)
  rLog("DAY:", DAY)  
  aux <- data.frame(YTD=YTD, MTD=MTD, PnL=DAY)
  rownames(aux) <- c("NEPOOL.FTR", "NEPOOL.incdec", "NYPP.TCC")
  aux["Total",] <- c(apply(aux, 2, sum, na.rm=TRUE))
  out <- c(capture.output(print(aux)), "\n\n", out)
  
  out <- c(paste(reportName, "for", asOfDate), "", out)

  save(list=c("PnL", "PnL.Day"), file=filename)
  
  return(out)
}

#################################################################
# Given a data.frame with variables calculated by delivery.point
# return a view with the spread PnL, delivery.point/hub
#
# x a data frame with c("mkt", "bucket", "delivery.point")
# ind an index of the columns that need to be viewed as a spread, e.g. price
#   hubInfo <- list(PWX=list(delivery.point="HUB"),
#                   PWY=list(delivery.point="DAM NPX"))
#
view_as_spread <- function(x, hubInfo, ind=ncol(x))
{
  label = names(x)[ind]
  
  if (!all(c("mkt", "bucket", "delivery.point") %in% names(x)))
    stop("Missing column mkt, delivery.point, curve, bucket")  

  .do_one_zone <- function(x, hubInfo, thisMKT="PWX")
  {
    hub <- subset(x, (delivery.point %in% hubInfo[[thisMKT]]$delivery.point &
        bucket %in% c("2X16H", "5X16", "7X8")) )
    
    pw  <- subset(x, mkt==thisMKT)
    colnames(hub)[which(names(hub)=="delivery.point")] <- "hub.delivery.point"
    colnames(hub)[which(names(hub)==label)] <- paste("hub.", label, sep="")
    hub$curve <- NULL

    pw <- merge(pw, hub, all.x=TRUE)
    pw$spread <- paste(pw$delivery.point,"/",pw$hub.delivery.point, sep="")
    pw[,paste("spread.", label, sep="")] <- (pw[,label] -
                                             pw[,paste("hub.", label, sep="")])

    aux <- pw[,c(names(x)[-ind], "spread", paste("spread.", label, sep=""))]
    aux <- subset(aux, delivery.point != "HUB")
    aux$delivery.point <- NULL          # you won't need this anymore
    aux
  }
  
  res <- NULL
  uMKT <- unique(x$mkt)
  for (thisMKT in uMKT) {
    res <- rbind(res, .do_one_zone(x, hubInfo, thisMKT))
  }

  res
}


#################################################################
# Deal with multi round auctions
#
.add.tcc.cp.round <- function(QQ)
{
  QQ$round <- ""   # only for long term TCCs now
  #QQ$round[which(QQ$eti == "PZP9ZD7")] <- "-R6" 
  
  QQ$round[which(QQ$eti == "PZPC1M6")] <- "-R5"   
  QQ$round[which(QQ$eti == "PZPC440")] <- "-R6"   
  QQ$round[which(QQ$eti == "PZPCA4V")] <- "-R7"

  return(QQ)
  
}


#################################################################
#################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

  hubInfo <- list(
    PWX=list(delivery.point="HUB",
      commod=data.frame(bucket=c("2X16H", "5X16", "7X8"),
        curve=c("COMMOD PWX 2X16 PHYSICAL",
          "COMMOD PWX 2X16 PHYSICAL", "COMMOD PWX 7X8 PHYSICAL"))),
    PWY=list(delivery.point="DAM NPX",
      commod=data.frame(bucket=c("2X16H", "5X16", "7X8"),
        curve=c("COMMOD PWY 2X16H NPX PHYSICAL",
          "COMMOD PWX 5X16 NPX PHYSICAL", "COMMOD PWX 7X8 NPX PHYSICAL"))))
  
  asOfDate     <<- as.Date("2011-12-01")  # a beginning of the month date
  focusMonth   <<- as.Date(format(asOfDate, "%Y-%m-01"))
  lastDay      <<- min(seq(focusMonth, by="1 month", length.out=2)[2]-1,
                       Sys.Date()-1)
  fixPriceDate <<- as.Date(secdb.dateRuleApply(focusMonth, "-1b"))
  
  books <- c("CONGIBT", "NPFTRMT1", "NESTAT", "VIRTSTAT")

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.daily.PnL.R")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")

  ######################################################################
  # Bal PnL estimation
  day1 <- as.Date("2011-12-12")
  QQ1 <- .get_positions(day1, months=NULL, books="CONGIBT", classifyTrade=FALSE)
  QQ2 <- .get_positions(day1+1, months=NULL, books="CONGIBT", classifyTrade=FALSE)
  QQ3 <- .get_positions(day1+2, months=NULL, books="CONGIBT", classifyTrade=FALSE)
  QQ4 <- .get_positions(day1+3, months=NULL, books="CONGIBT", classifyTrade=FALSE)
  PP1 <-  get_fix_price(day1,   QQ1)
  PP2 <-  get_fix_price(day1+1, QQ2)
  PP3 <-  get_fix_price(day1+2, QQ3)
  PP4 <-  get_fix_price(day1+3, QQ4)

  load("C:/Temp/PQ_1213-1212.RData")

  save(QQ1, QQ2, QQ3, QQ4, PP1, PP2, PP3, PP4, file="C:/Temp/PQ_1212-1215.RData")
  
  dPnL <- calculate_bal_PnL(QQ1, PP1, QQ2, PP2)
  dPnL <- rbind(dPnL, calculate_bal_PnL(QQ2, PP2, QQ3, PP3))
  dPnL <- rbind(dPnL, calculate_bal_PnL(QQ3, PP3, QQ4, PP4))

  
  


  
  if (!(format(day1) %in% names(fwdPP))) {
    fwdPP[[format(day1)]] <- get_fix_price(day1, QQ1)
  }
  if (!(format(day2) %in% names(fwdPP))) {
    fwdPP[[format(day2)]] <- get_fix_price(day2, QQ2)
  }

  

  
  

  ######################################################################
  # Cash PnL estimation
  QQ <- .get_positions(fixPriceDate, months=focusMonth, books=books)

  sPnL <- cashMonth_spread_PnL(focusMonth, QQ, hubInfo)
  
  hPnL <- cashMonth_hub_PnL(focusMonth, QQ, hubInfo)
  
  fPnL <- cashMonth_ftr_PnL(focusMonth)


  sum(hPnL$PnL)
  sum(sPnL$PnL, na.rm=TRUE)  # don't count SC contract


  
  
  cast(sPnL, mkt + spread ~ ., function(x){round(sum(x))}, value="PnL")

  
  # by delivery.point and bucket
  cast(sPnL, mkt + spread + bucket ~ .,
    function(x){round(sum(x))}, value="PnL")

  
  
  
  



  

  # add the hub position
  qHubs <- cast(x[,c(1:5,8)], mkt + month + bucket + hour ~ ., sum, value="delta")
  names(qHubs)[ncol(qHubs)] <- "spread.delta"
  qHubs$spread <- "HUB"
  y3 <- rbind(y3, qHubs)


  
  rDay <- cast(X, day ~ ., sum, value="PnL")
  sum(rDay, na.rm=TRUE)



  rDeliveryPoint <- cast(X, delivery)
  

  
  
  cast(X, delivery.point + bucket ~ ., function(x){round(sum(x))}, value="PnL")
  
  rDayEti <- cast(XX, eti ~ day, function(x){round(sum(x))}, value="PnL")
  rDayEti[,1:8]

  
  #rDayEti <- cast(XX, eti + day ~ ., function(x){round(sum(x))}, value="PnL")

  
  d <- subset(rDayEti, day=="2011-09-01")


  

  
  
}



