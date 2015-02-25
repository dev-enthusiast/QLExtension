# Pick two dates and investigate
#
# .find_hedge_quantity_hub
# .get_gas_expiration_date
# .pnl_for_cocco                  <--  daily by year
# .read_bal_report_xlsx
#

####################################################################
# figure out how much Hub you need to buy to stay flat
#
.find_hedge_quantity_hub <- function()
{
  require(PM)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.bal.report.R")  


  startDt <- Sys.Date() - 30
  endDt   <- Sys.Date() - 1
  days <- seq(startDt, endDt, by="1 day")
  
  bal  <- get_bal_report(days, books=c("CONGIBT"))

  PnL <- bal$CONGIBT[,c("Period", "Commod", "Locations")]
  PnL$Commod <- round(PnL$Commod)
  PnL$Locations <- round(PnL$Locations)
  PnL$Subtotal  <- PnL$Commod + PnL$Locations
  plot(PnL$Period, PnL$Locations/100000, col="blue")
  

  startDt <- as.Date("2014-01-01"); endDt <- as.Date("2014-12-01")
  pwr <- PM:::ntplot.estStrip("prc_pwx_5x16_phys", startDt, endDt)
  #plot(pwr$date, pwr$value)
  colnames(pwr)[c(2,3)] <- c("Period", "pwr.12mth.5x16.strip")
  pwr$dPrice <- NA
  pwr$dPrice[2:nrow(pwr)] <- pwr[2:nrow(pwr),3] - pwr[1:(nrow(pwr)-1),3]
  pwr$symbol <- NULL
  head(pwr)
  
  out <- merge(PnL, pwr, by="Period", all.x=TRUE)
  
  xyplot(Subtotal/100000 ~ dPrice, data=out,
         type=c("g", "p"),
         xlab="Change in Cal14 Peak, $/MWh",
         ylab="Bal Report subtotals, k$",
         main="CONGIBT",
         cex=1.3)

  

  
}


####################################################################
# month is a beginning of the month R Date
#
.get_gas_expiration_date <- function(month)
{
  bom <- format(month-1, "%Y-%m-01")  
  symb <- paste("prc_ng_exchange_", format(month, "%y%m"), sep="")
  res  <- tsdb.readCurve(symb, as.POSIXct(bom), as.POSIXct(month))
  
  tail(res,1)$date   # get the last observation  
}


####################################################################
# calculate the daily PnL for Cocco
#
.pnl_for_cocco <- function(day1, day2, save=TRUE)
{
  books <- c("NPFTRMT1", "CONGIBT", "NEPPWRBS", "NESTAT")

  # fixed price view
  QQ1 <- .get_positions_archive(asOfDate=day1, books, agg.book=FALSE)
  QQ2 <- .get_positions_archive(asOfDate=day2, books, agg.book=FALSE)
  # if you want to focus on something
  # QQ1 <- subset(QQ1, month==as.Date("2013-08-01") & book=="NEPPWRBS")
  # QQ2 <- subset(QQ2, month==as.Date("2013-08-01") & book=="NEPPWRBS")
   
  PP1 <- .pull_marks(day1, unique(QQ1[,c("mkt", "location")]))
  PP2 <- .pull_marks(day2, unique(QQ2[,c("mkt", "location")]))  
  pnl  <- calc_pnl_pq(PP1, PP2, QQ1, QQ2)

  # spread view  
  SP1 <- .marks_spread(PP1)
  SP2 <- .marks_spread(PP2)
  SQ1 <- .positions_spread(QQ1)
  SQ2 <- .positions_spread(QQ2)
  
  spnl <- calc_pnl_pq(SP1, SP2, SQ1, SQ2)

  res <- list(fp=pnl$PnL, spreads=spnl$PnL)
  if (save) {
    fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/Analysis/PnL/PxQ_",
      format(day1), "_to_", format(day2), ".RData", sep="")
    save(res, file=fname)
    rLog("Saved results to", fname)
  }
  
  res
}




########################################################################
#
.read_bal_report_xlsx <- function(book="CONGIBT")
{
  bal <- read.xlsx("H:/myTrades.xlsx", sheetName="CONGIBT", rowIndex=5:400)
  suppressWarnings(bal$period <- as.Date(as.numeric(bal$period)-25569,
                                         origin="1970-01-01"))
  bal <- bal[!is.na(bal$period),]

  startDt <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), length.out=2,
                 by="1 month")[2]
  endDt <- seq(startDt, length.out=2, by="11 month")[2]
  startDt <- as.Date("2012-01-01"); endDt <- as.Date("2012-12-01")
  pwr <- PM:::ntplot.estStrip("prc_pwx_5x16_phys", startDt, endDt)
  #plot(pwr$date, pwr$value)
  colnames(pwr)[c(2,3)] <- c("period", "pwr.12mth.5x16.strip")
  pwr$dPrice <- NA
  pwr$dPrice[2:nrow(pwr)] <- pwr[2:nrow(pwr),3] - pwr[1:(nrow(pwr)-1),3]
  pwr$symbol <- NULL
  
  out <- merge(bal, pwr, by="period", all.x=TRUE)
  
  xyplot(subtotals/1000000 ~ dPrice, data=out,
         type=c("g", "p"),
         xlab="Change in Cal12 strip, $",
         ylab="Bal Report subtotals, M$",
         main="CONGIBT",
         cex=1.3)

  ## plot(out$dPrice, out$subtotals/1000000, col="blue",
  ##      ylab="Subtotals PnL, M$")
  ## identify(out$dPrice, out$subtotals/1000000, labels=out$period)
}


########################################################################
# Aggregate a data.frame with columns (month, value) into bal-year,
# and years.  This is what Cocco wants reported. 
# 
.report_balyear_years <- function(X, asOfDate=Sys.Date())
{
  currentYear  <- as.numeric(format(asOfDate, "%Y"))
  X$timeBucket <- format(X$month)
  ind <- which(X$month >= as.Date(paste(currentYear+1, "-01-01", sep=""),
                 origin="1970-01-01"))
  if (length(ind) > 0)
    X$timeBucket[ind] <- format(X$month[ind], "%Y")
  
  res <-  cast(X, . ~ timeBucket, function(x){round(sum(x))}, value="PnL.dP")

  res
}


########################################################################
#
.main <- function()
{
  require(reshape)
  require(CEGbase)
  require(xlsx)
  require(SecDb) 
  require(lattice)

  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/lib.PnL.R")
  
  asOfDate <- Sys.Date()
  
  day1 <- as.Date("2013-05-03")
  day2 <- as.Date("2013-05-09")
  aux  <- .pnl_for_cocco(day1, day2)
  pnl  <- aux[["fp"]]
  spnl <- aux[["spreads"]]   

  print(.report_balyear_years( spnl )[,-1], row.names=FALSE)

  print(cast(pnl, . ~ year, function(x){round(sum(x))}, value="PnL.dP")[,-1],
        row.names=FALSE)
  cast(spnl, mkt ~ year, function(x){round(sum(x))}, value="PnL.dP") 
  cast(spnl, location ~ year, function(x){round(sum(x))}, value="PnL.dP")
  cast(spnl, book ~ year, function(x){round(sum(x))}, value="PnL.dP")


  cast(spnl, location ~ ., function(x){round(sum(x))}, value="PnL.dP")
  
  write.csv(spnl, file="C:/temp/junk.csv", row.names=FALSE)

  


  

  
  subset(spnl, month <= as.Date("2013-09-01"))
  


  # pick only the change by delivery points
}






## ########################################################################
## #
## .by_delivPt <- function(QQ)
## {
##   aux <- subset(QQ, variable=="mtm.usd")
##   aux <- cast(aux, delivery.point + year ~ asOfDate, sum, fill=0)
##   aux$diff <- aux[,4] - aux[,3]
##   aux$diff <- round(aux$diff)
  
  
  
## }




## ########################################################################
## #
## .get_positions <- function(day)
## {
##   DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
##   load(paste(DIR, "positionsAAD_", format(day), ".RData", sep=""))
##   aux <- subset(QQ, 
##     portfolio %in% c("NEPOOL CONGESTION PORTFOLIO", "NPFTRMT1",
##                      "NESTAT", "PM EAST RETAIL PORTFOLIO") &
##     mkt == "PWX"
##   )
##   aux <- aux[,c("trading.book", "eti", "trade.type", "trade.date",
##     "contract.dt", "curve", "delivery.point", "bucket", "service",
##     "delta", "mtm.usd")]
##   aux <- melt(aux, id=1:9)
##   aux$year <- format(aux$contract.dt, "%Y")

##   # aggregate by year
##   bux <- cast(aux, trading.book+eti+trade.type+trade.date+year +
##     curve+delivery.point+bucket+service + variable ~ ., sum, fill=0 )
##   names(bux)[ncol(bux)] <- "value"
##   bux <- cbind(asOfDate=day, data.frame(bux))

##   bux   
## }

