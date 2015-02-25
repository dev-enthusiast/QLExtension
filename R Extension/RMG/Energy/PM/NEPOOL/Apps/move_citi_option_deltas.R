# Daily move the positions for the Citi option deltas
#
# .customize_calculators_citi
# .get_deltas_cog
# .get_prices_tsdb
#

###########################################################
# fill the calcs  
#
.customize_calculators_citi <- function(pp)
{
  rLog("customizing gas calc")  # e49248

  fullCalculatorName <- "Calc SWAP gas deltas          0"
  QQ <- pp$NG[,c("month", "delta")]
  PP <- pp$NG[,c("month", "price")]
  PP$price <- PP$price*100                  # NG price in cents in the calc!
  .customize_basis_swap(fullCalculatorName, QQ, PP)
  rLog("\n\n")

  
  fullCalculatorName <- "Calc ECFD power deltas        0"
  QQ <- pp$PWX[,c("month", "mw_HUB_5X16")]
  names(QQ)[2] <- "HUB_PEAK"
  PP <- pp$PWX[,c("month", "price")]
  names(PP)[2] <- "HUB_PEAK"
  PM:::secdb.customizeCalculator(fullCalculatorName, quantity=QQ, fixPrice=PP)
  
}


###########################################################
# returns a list with deltas for power and gas
#
.get_deltas_cog <- function(positionsDate=NULL)
{ 
  if (is.null(positionsDate))
    positionsDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

  books <- "KLNHGMTM"

  rLog("Reading positions for date", format(positionsDate))

  aux <- get.positions.from.blackbird( asOfDate=positionsDate, books=books,
    fix=TRUE, service=NULL)

  res <- cast(aux, contract.dt + bucket + curve + mkt ~ ., sum, value="delta")
  names(res)[ncol(res)] <- "delta"
  res <- subset(res, contract.dt >= positionsDate)

  res <- split(res, res$mkt)
  res <- lapply(res, function(x){x[order(x$contract.dt), ]})

  aux <- res[["PWX"]]
  hrs <- FTR:::FTR.bucketHours(start.dt=as.POSIXct(positionsDate),
    end.dt=as.POSIXct("2014-01-01 00:00:00"), buckets=c("5X16"))
  names(hrs) <- c("contract.dt", "bucket", "hours")
  aux <- merge(aux, hrs, all.x=TRUE)
  aux$`HUB_5X16` <- aux$delta/aux$hours
  aux <- data.frame(month=aux$contract.dt, `mw_HUB_5X16` = aux$`HUB_5X16`)
  res[["PWX"]] <- aux

  names(res$NG)[1] <- "month"
  
  res
}


###########################################################
# current SecDb prices
#
.get_prices_live <- function(qq)
{

  rLog("Pulling live prices from secdb")
  startDate <- qq$PWX$month[1]
  endDate   <- as.Date("2013-12-31")
  contracts <- seq(startDate, endDate, by="1 month")
  pp1 <- rep(NA, length(contracts))
  for (m in seq_len(length(contracts))) {
    pp1[m] <- secdb.getValueType(paste("NG",
      format.dateMYY(contracts[m], -1),
      " ALGCG GAS-DLY MEAN", sep=""), "Spot")
  }
  pp1 <- data.frame(month=contracts, price=pp1)
  
  pp2 <- PM:::secdb.getElecPrices( Sys.Date(), startDate, endDate,
    "COMMOD PWR NEPOOL PHYSICAL", "HUB", "PEAK", "Energy", FALSE)
  names(pp2)[6] <- "price"
  pp2 <- pp2[,c("month", "price")]
  
  qq$NG  <- merge(qq$NG, pp1)
  qq$PWX <- merge(qq$PWX, pp2)

  qq
}


###########################################################
# 
.get_prices_tsdb <- function(qq, asOfDate=NULL)
{
  if (is.null(asOfDate))
    asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

  rLog("Pulling prices from tsdb for date", format(asOfDate))
  startDate <- qq$PWX$month[1]
  endDate   <- qq$PWX$month[nrow(qq$PWX)]
  endDate   <- as.Date("2013-12-31")
  pp1 <- tsdb.futCurve("prc_ng_algcg_gdm", asOfDate, startDate, endDate)
  names(pp1) <- c("month", "price")
  
  pp2 <- tsdb.futCurve("prc_pwx_5x16_phys", asOfDate, startDate, endDate)
  names(pp2) <- c("month", "price")
  
  qq$NG  <- merge(qq$NG, pp1)
  qq$PWX <- merge(qq$PWX, pp2)

  qq
}


###########################################################
###########################################################
#
require(CEGbase)
require(SecDb)
require(reshape)
require(methods)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.book.trade.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

asOfDate <- Sys.Date()

qq <- .get_deltas_cog()

#pp <- .get_prices_tsdb(qq)
pp <- .get_prices_live(qq)

.customize_calculators_citi(pp)

rLog("Done")









