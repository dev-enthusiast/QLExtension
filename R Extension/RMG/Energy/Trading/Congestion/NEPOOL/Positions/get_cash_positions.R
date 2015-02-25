# Get the cash positions from Prism 
# Used by Condor/daily_positions_cash.R
#
#



#################################################################
#
.get_cash_accrual <- function()
{
  rLog("Get cash accrual length ...")
  books <- c(
    .getPortfolioBooksFromSecDb("NEPOOL FP PORTFOLIO"), 
    .getPortfolioBooksFromSecDb("NEPOOL STRUCTURED HEDGE PORTFOL"),
    "NEPGDIBT", "NEGASRS") 
             
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="ENERGY")
  qq <- subset(aux, contract.dt <= focusMonth &
                    contract.dt >= focusMonth &
                    mkt == "PWX"
                )
  
  res <- cast(qq, mkt + bucket ~ ., sum, value="delta")
  print(res)
   
  res
}


#################################################################
#
.get_intramonth_trades <- function(asOfDate, focusMonth,
                                   books=c("CPDAYHD1"))
{
  rLog("Get cash intramonth trades ...")

  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
    fix=TRUE, service="ENERGY")
  
  qq <- subset(aux, contract.dt <= focusMonth &
                    contract.dt >= focusMonth &
                    as.Date(trade.end.date) > asOfDate #&
                    #mkt == "PWX"
              )
  qq$trade.start.date <- as.Date(qq$trade.start.date)
  qq$trade.end.date <- as.Date(qq$trade.end.date)
  
  if (nrow(qq)==0)   # if there are no positions in Prism!
    return(list(NULL, NULL))
  
  # expand trades to daily
  res <- ddply(qq, .(eti), function(x){
    #if (x$eti[1] == "PAS31PG") {browser()}
    uDP <- unique(x$delivery.point)
    if (length(uDP) > 1) {                  # DART trades or something else
      rLog("A spread trade eti: ", x$eti[1], " Skipping it for now!")
      return(data.frame())
    }
    sign <- ifelse(x$trade.type == "SELL", -1, 1)
    qty <- structure(as.numeric(x$volume)*sign, names=x$bucket)
    PM:::.expandMonthlyBucketsDaily(focusMonth, qty, agg=FALSE)
  })
  names(res)[which(names(res)=="date")] <- "fwd.date"

  rr <- merge(res, unique(qq[,c("mkt", "eti", "trade.date", "trade.start.date",
    "trade.end.date")]), by="eti", all.x=TRUE)
  
  # now, trades that were done in the middle of the month should only
  # show positions from trade.start to trade.end
  rr <- ddply(rr, .(eti), function(x){
    subset(x, fwd.date >= trade.start.date & fwd.date <= trade.end.date)
  })
  
  
  list(qq, rr)
}


#################################################################
#
.get_mtm_hedges <- function()
{
  rLog("Get mtm hedges ...")
  books <- c("BGMTM", "KLNHGMTM", "NPCASHVL")
  aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
   fix=TRUE, service="ENERGY")
  qq <- subset(aux, contract.dt <= focusMonth &
                    contract.dt >= focusMonth &
                    mkt == "PWX"
                )
  
  res <- cast(qq, mkt + bucket ~ ., sum, value="delta")
  print(res)
  
  res
}


#################################################################
#
.get_units <- function(QQ)
{
  rLog("Get unit length...")
  qq <- subset(QQ, trading.book %in% c("NPKLEEN", "FORERIVM",
                                       "MYSTC89M", "MYSTIC7M"))
  res <- cast(qq, mkt + bucket ~ ., sum, value="delta")
  print(res)
  
  res
}


#################################################################
#
.main <- function()
{
  require(CEGbase); require(PM); require(reshape)
  source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Positions/get.cash.positions.R")
  
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"


  
  asOfDate   <- as.Date("2011-05-25")
  focusMonth <- as.Date("2011-05-01")

  load(paste(dir, "positionsAAD_", format(asOfDate),".RData", sep=""))

  QQ <- subset(QQ, contract.dt <= focusMonth &
                    contract.dt >= focusMonth &
                    portfolio %in% "NEPOOL ACCRUAL PORTFOLIO" &
                    mkt == "PWX"
                )

   # get positions for different parts of the business
  .get_units()
  .get_mtm_hedges()
  .get_cash_accrual()
  


  
}


