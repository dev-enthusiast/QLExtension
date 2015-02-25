# Calculate the PnL from the cash month
#
# calc_pnl_FTRs              - for one month only
# calc_cash_PnL              - MAIN function         <-------
# .fix_sellers_choice
# get_cash_positions
# get_forward_prices
# get_settle_prices
# .report_cash_pnl
# spread_pnl   - calculate the pnl as a spread to hub, so you can
#                identify the bad locations


######################################################################
# Calculate the FTR pnl for the month
# @param month the month as an R Date, e.g. currentMonth()
# @param asOfDate the reference price date (gas expiry or last business day)
#
calc_pnl_FTRs <- function( month, asOfDate=.get_gas_expiration_date(month) )
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  require(FTR)
  FTR.load.ISO.env("NEPOOL")
  
  monthlyAuction <- FTR_Auction(month, nextMonth(month)-1, round=0)
  aux <- paste("F", format(month, "%y"), "-1Y", sep="")
  annualAuctions <- paste(aux, c("-R1", "-R2"), sep="")
  auctions <- c(monthlyAuction$auctionName, annualAuctions)
   
  startOnDt <- month
  endOnDt   <- Sys.Date()

  res <- make.FTR.PnL(auctions, startOnDt, endOnDt)  # from FTR package, libFTRpresentation
  res$PnL <- NULL  # needs to be calculated relative to secdb marks!
  colnames(res)[which(colnames(res)=="fix.price")] <- "auction.price"
  
  # need to get fix.price as of gas expiration not auction clearing price
  uNames <- unique(c(res$source.name, res$sink.name))
  aux <- merge(NEPOOL$MAP[c("name", "mcc.deliv.pt")], data.frame(name=uNames),
               all.y=TRUE)  
  PP <- get_forward_prices(asOfDate, month,
    data.frame(mkt="PWX", delivery.point=c(aux$mcc.deliv.pt, "MCC HUB DA")),
    buckets=c("PEAK", "OFFPEAK"))
  PP <- subset(PP, delivery.point != "HUB DA")
  colnames(PP)[2:3] <- c("mkt", "location")
  SS <- .marks_spread(PP)
  SS$class.type <- gsub("^PEAK", "ONPEAK", SS$bucket)
  colnames(SS)[4] <- "mcc.deliv.pt"
  SS <- merge(NEPOOL$MAP[c("name", "mcc.deliv.pt")], SS, by=c("mcc.deliv.pt"))
  SS$mcc.deliv.pt <- SS$bucket <- NULL  # not needed anymore
  
  SS$source.name <- SS$name
  SS$source.mcc <- SS$value
  res <- merge(res, SS[,c("class.type", "source.name", "source.mcc")],
    by=c("source.name", "class.type"), all.x=TRUE)
  SS$sink.name <- SS$name
  SS$sink.mcc <- SS$value
  res <- merge(res, SS[,c("class.type", "sink.name", "sink.mcc")],
    by=c("sink.name", "class.type"), all.x=TRUE)
  res$P1 <- res$sink.mcc - res$source.mcc
  res$sink.mcc <- res$source.mcc <- NULL

  # add the hours
  HRS <- data.frame(class.type=c("OFFPEAK", "ONPEAK"),
    hours=c(nrow(secdb.getHoursInBucket("NEPOOL", "OFFPEAK",
      as.POSIXct(paste(month, "01:00:00")),
      min(Sys.time(), as.POSIXct(paste(nextMonth(month), "00:00:00"))))),
            nrow(secdb.getHoursInBucket("NEPOOL", "PEAK",
      as.POSIXct(paste(month, "01:00:00")),
      min(Sys.time(), as.POSIXct(paste(nextMonth(month), "00:00:00")))))))
  res <- merge(res, HRS, all.x=TRUE)
  res$PnL <- res$hours * res$mw * (res$settle.price - res$P1)
  
  res
}


######################################################################
# Main function
#
calc_cash_pnl <- function(month)
{
  #asOfDate <- .get_gas_expiration_date(month)
  asOfDate <- as.Date(secdb.dateRuleApply(month, "-1b"))
  
  rLog("get the Spreads PnL")
  books <- c("NEPPWRBS", "CONGIBT", "CONGIBT2", "NPFTRMT1")
  QQ <- get_cash_positions(asOfDate, month, books)
  QQ <- QQ[!grepl("^MCC", QQ$delivery.point),]  # remove the ftrs
    
  PP1 <- get_forward_prices(asOfDate, month, QQ)
  PP2 <- get_settle_prices(month, QQ)
  PP2 <- .fix_sellers_choice(PP1, PP2)
  
  colnames(PP1)[which(colnames(PP1)=="delivery.point")] <- "location"
  colnames(PP2)[which(colnames(PP2)=="delivery.point")] <- "location"
  colnames(PP1)[which(colnames(PP1)=="market")] <- "mkt"
  colnames(PP2)[which(colnames(PP2)=="market")] <- "mkt"
  colnames(QQ)[which(colnames(QQ)=="delivery.point")] <- "location"
  QQ$curve <- NULL
   
  SP1 <- .marks_spread(PP1)
  SP2 <- .marks_spread(PP2)
  SQQ <- .positions_spread(QQ)
  spnl <- calc_pnl_pq(SP1, SP2, SQQ, SQQ)
  

  rLog("get the FTRs PnL")
  ftr <- calc_pnl_FTRs( month, asOfDate )


  rLog("get the Virtuals PnL")
  require(PM)
  lastDay <- min(Sys.Date()-1, nextMonth(month)-1)
  days <- seq(currentMonth(month), lastDay, by="1 day")
  virts <- get_bal_report(days, books="VIRTSTAT")$VIRTSTAT

  
  return( list(spreads=spnl$PnL, ftrs=ftr, virts=virts, positionsDate=asOfDate) )  
}


######################################################################
# Sellers choice doesn't settle properly.  Set the qty to zero for
# estimates.
#
.fix_sellers_choice <- function(PP1, PP2)
{
  if ("SC" %in% PP1$delivery.point) {
    aux <- subset(PP1, delivery.point %in% c("SC", "HUB DA"))
    sc  <- cast(aux, bucket ~ delivery.point, I, fill=NA)
    sc$spread <- sc$SC - sc$`HUB DA`
    sc$`HUB DA` <- sc$SC <- NULL
    
    aux <- subset(PP2, delivery.point == "HUB DA")
    aux <- merge(aux, sc)
    aux$delivery.point <- "SC"
    aux$value <- aux$value + aux$spread
    aux$spread <- NULL
    
    PP2 <- rbind(PP2, aux)
  }
  
  PP2
}


######################################################################
# Pull the positions from prism, low level
#
get_cash_positions <- function(asOfDate, contract.dt, book, agg=TRUE)
{
  QQ <- get.positions.from.blackbird(asOfDate, books=book,
    service=NULL, contract.dt=contract.dt, fix=TRUE)

  QQ <- QQ[, c("eti", "trade.type", "delta", "curve", "delivery.point",
    "volume", "notional", "quantity.uom", "bucket", "mkt", "trading.book")]
  QQ <- subset(QQ, curve != "USD")
  ind <- which(QQ$delivery.point == "HUB" & QQ$mkt == "PWX")
  if (length(ind)>0)
    QQ$delivery.point[ind] <- "HUB DA"

  if (agg){  # sum of all the undiscounted MWh for this month    
    QQ <- aggregate(QQ$delta, QQ[, c("trading.book", "curve", "delivery.point",
      "quantity.uom", "bucket", "mkt")], sum)
    names(QQ)[ncol(QQ)] <- "value"   
  }

  uMKT <- sort(unique(QQ$mkt))
  if (!all(uMKT %in% c("PWX", "PWY"))){
    rLog("Unique markets:", uMKT)
    rLog("---> You have some markets that I don't support!!!!!")
  }

  totalQQ <- sum(QQ$value)
  if (abs(totalQQ) > 7400){
    cat("ATTENTION!!!  Total 7x24 position is ", round(totalQQ/740,0),
        " MW!!!!\n\n\n")
  }
  QQ <- cbind(month=contract.dt, QQ)
  QQ$quantity.uom <- NULL
  colnames(QQ)[which(colnames(QQ)=="trading.book")] <- "book"

  QQ <- subset(QQ, value != 0)
  
  if (contract.dt == currentMonth()) {
    rLog("*Current month quantities capture partial intra-month flowed MWh only.")
    QQ$value <- QQ$value * as.numeric(Sys.Date()-contract.dt)/as.numeric(nextMonth()-currentMonth())
  }
  
  return(QQ)
}


######################################################################
# Pull the positions from prism, low level
# @param asOfDate
# @param contract.dt is the month you are intrested
# @param QQ a data.frame with columns mkt, delivery.point

get_forward_prices <- function(asOfDate, contract.dt, QQ,
  buckets=c("5X16", "2X16H", "7X8"))
{
  uMKT <- sort(unique(QQ$mkt))

  PP <- NULL
  if ("PWX" %in% uMKT){
    commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
    location    <- unique(c(subset(QQ, mkt=="PWX")$delivery.point, "HUB DA"))
    bucket      <- buckets
    serviceType <- "Energy"
    useFutDates <- FALSE

    curveData <- PM:::secdb.getElecPrices( asOfDate, contract.dt, contract.dt,
      commodity, location, bucket, serviceType, useFutDates)
    curveData$market <- "PWX"
    PP <- rbind(PP, curveData)
  }

  if ("PWY" %in% uMKT){
    commodity   <- "COMMOD PWR NYPP PHYSICAL"
    location    <- unique(c(subset(QQ, mkt=="PWY")$delivery.point, "DAM HUDV"))
    bucket      <- buckets
    serviceType <- "Energy"
    useFutDates <- FALSE

    curveData <- PM:::secdb.getElecPrices( asOfDate, contract.dt, contract.dt,
      commodity, location, bucket, serviceType, useFutDates)
    curveData$market <- "PWY"
    PP <- rbind(PP, curveData)
  }
  PP <- PP[, c("market", "location", "bucket", "value")]
  names(PP)[2] <- c("delivery.point")
  PP <- cbind(month=contract.dt, PP)
  
  return(PP)
}


######################################################################
# Get the settle prices from SecDb
#
get_settle_prices <- function(month, QQ)
{
  startDate <- currentMonth(month)
  endDate   <- nextMonth(month)-1

  uMKT <- sort(unique(QQ$mkt))

  PP <- NULL
  if ("PWX" %in% uMKT) {
    ind <- grep(" PWX ", QQ$curve)
    container <- "Bal Close PWR NEPOOL PM"
    location  <- unique(QQ[ind, "delivery.point"]) #c("Hub", "CL8")
    curveData <- PM:::secdb.getSettlePrices(startDate, endDate,
      location, container)
    names(curveData)[which(names(curveData)=="value")] <- "settlePrice"
    curveData$mkt <- "PWX"
    PP <- rbind(PP, curveData)
  }
    
  if ("PWY" %in% uMKT) {
    ind <- grep(" PWY ", QQ$curve)
    container <- "Bal Close PWR NYPP PM"
    location  <- unique(QQ[ind, "delivery.point"]) #c("Hub", "CL8")
    curveData <- PM:::secdb.getSettlePrices(startDate, endDate,
      location, container)
    names(curveData)[which(names(curveData)=="value")] <- "settlePrice"
    curveData$mkt <- "PWY"
    PP <- rbind(PP, curveData)
  }

  buckets <- c("5X16", "2X16H", "7X8")
  st <- as.POSIXct(paste(startDate, "01:00:00"))
  et <- as.POSIXct(paste(endDate, "23:00:00")) + 3600
  indB <- secdb.getBucketMask("NEPOOL", buckets[1], st, et)
  for (b in 2:length(buckets)){
   indB <- merge(indB, secdb.getBucketMask("NEPOOL", buckets[b], st, et), all=TRUE)
  }
  indB <- melt(indB, id=1)
  indB <- subset(indB, value)
  names(indB)[2] <- "bucket"
  indB$bucket <- as.character(indB$bucket)

  PP  <- merge(PP, indB[,c("time", "bucket")], all.x=TRUE)
  PPb <- cast(PP, mkt + location + bucket ~ ., mean, na.rm=TRUE, fill=NA,
              value="settlePrice")
  names(PPb)[c(2,4)] <- c("delivery.point", "value")
  PPb <- data.frame(PPb)
  PPb <- cbind(month=month, PPb)
  
  return(PPb)
}


######################################################################
# make the spreadsheet
#
.report_cash_pnl <- function( pnl )
{
  require(xlsx)
  month <- pnl$spreads$month[1]
  qqDate <- pnl$positionsDate
    
  DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/Cash/"
  fname <- paste(DIR, format(month, "%Y-%m"), ".xlsx", sep="")
  
  wb <- createWorkbook()
  cs <- CellStyle(wb) + Fill(foregroundColor="steelblue1",
    backgroundColor="steelblue1") + Border()
  ds <- CellStyle(wb) + DataFormat( "$#,##0.00" )  # was .00
  qs <- CellStyle(wb) + DataFormat( "#,##0" )
  ts <- CellStyle(wb) + DataFormat( "$#,##0" )  # for total PnL
  sheet <- createSheet(wb, sheetName="Summary")
  X <- data.frame(
   a=c("Month", "As of date", "Positions as of", "P1 are prices as of",
     "P2 are settled prices"),
   b=c(month, Sys.Date(), qqDate, qqDate, ""))
  addDataFrame(X, sheet, col.names=FALSE, row.names=FALSE)
  cb <- CellBlock(sheet, 7, 2, 10, 2)
  CB.setRowData(cb, c("", "PnL"), 1, rowStyle=cs)
  CB.setColData(cb, c("Spreads", "FTRs", "Virtuals", "Total"), 1, rowOffset=1)
  pnl.1 <- sum(pnl$spreads$PnL)
  pnl.2 <- sum(pnl$ftrs$PnL)
  pnl.3 <- pnl$virts$Today[nrow(pnl$virts)] - pnl$virts$Yesterday[1]
  CB.setColData(cb, c(pnl.1, pnl.2, pnl.3, pnl.1+pnl.2+pnl.3), 2, rowOffset=1, colStyle=ts)
  autoSizeColumn(sheet, 1:3)
  
  sheet <- createSheet(wb, sheetName="Spreads")
  colStyle <- list()
  addDataFrame(pnl$spreads, sheet, row.names=FALSE, colnamesStyle=cs,
    colStyle=list(`6`=ds, `7`=ds, `10`=qs, `11`=qs, `12`=qs, `13`=ds, `14`=ds, `15`=ds))
  autoSizeColumn(sheet, 1:ncol(pnl$spreads))
  
  sheet <- createSheet(wb, sheetName="FTRs")
  addDataFrame(pnl$ftrs, sheet, row.names=FALSE, colnamesStyle=cs,
    colStyle=list(`8`=ds, `9`=ds, `10`=ds, `12`=ds))
  autoSizeColumn(sheet, 1:ncol(pnl$ftrs))

  sheet <- createSheet(wb, sheetName="Virts")
  addDataFrame(pnl$virts, sheet, row.names=FALSE, colnamesStyle=cs)
  autoSizeColumn(sheet, 1:ncol(pnl$virts))

  
  saveWorkbook(wb, fname)
  rLog("Wrote file", fname)
}




######################################################################
######################################################################
.main <- function()
{
  require(CEGbase)
  require(CEGterm)
  require(reshape)
  require(SecDb)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.nepool.marks.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.FTR.auction.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/lib.PnL.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.bal.report.R")  
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/calc_realized_cash_pnl.R")

  month <- as.Date("2014-01-01")
  pnl <- calc_cash_pnl(month)
  .report_cash_pnl(pnl)
  
  
  # break-down of spread PnL by location
  aux <- cast(pnl$spreads, mkt + location ~ ., function(x){round(sum(x))},
       value="PnL")
  colnames(aux)[3] <- "PnL"
  print(aux[order(aux$PnL),], row.names=FALSE)
  

  
  
}







