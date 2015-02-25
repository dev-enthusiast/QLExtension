#
#  options <- NULL
#  options$Locations <- list( list(loc="COMMOD WTI EXCHANGE", name="WTI"),
#                     list(loc="COMMOD NG Exchange",  name="NG") )
#  options$asOfDate <- as.Date("2007-07-13")
#  options$dateTill <- as.Date("2010-12-01")
#  options$returnType <- "all"   ("all", "asofdate" or "pricediff")
#  options$curve.names <-  c("COMMOD PWX 5X16 PHYSICAL", "COMMOD PWJ 5X16 WEST PHYSICAL", 
#  "COMMOD PWY 5X16 NYC PHYSICAL",
#  "COMMOD PWO 5X16 SC PHYSICAL", "COMMOD PWM 6X16 PHYSICAL", "COMMOD PWC 6X16 NP15 PHYSICAL", 
#  "COMMOD PWP 6X16 SP15 PHYSICAL", "COMMOD PWP 6X16 PHYSICAL")
#  source("H:/user/R/RMG/Utilities/SecDb/R/get.prices.from.SecDB.r")
#  res <- get.prices.from.SecDB(options)

get.prices.from.SecDB <- function(options) {
  source("H:/user/R/RMG/TimeFunctions/dateGS.R")
                           
  library(SecDb)
  require(reshape)
  secdb.setDatabase("!Bal Prod 1;ReadOnly{!Bal Prod 2}")
  
  if (length(options$dateTill) == 0) {options$dateTill <- as.Date("2050-12-01")}
  if (length(options$returnType) == 0) {options$returnType <- "asofdate"}
  if (length(options$asOfDate) == 0) {options$asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))}
  #if (length(options$Locations) == 0) { list(list(loc="COMMOD NG Exchange",  name="NG") ) }
  
  last.mark <- Sys.Date()
  useClose  <- TRUE
  
  evaltext <- NULL
  for (i in 1:length(options$curve.names)) {
      evaltext <- paste(evaltext, 'list(loc="', options$curve.names[i], '") ', sep='') 
      if (i < length(options$curve.names)) {
        evaltext <- paste(evaltext, ", ", sep="")
      }
  }
  evaltext <- paste("list(", evaltext, ")", sep="") 
  options$Locations <- eval(parse(text=evaltext))
  
  res <- secdb.invoke("_lib elec dly fut changes", "DlyFutChanges::GetFutStrip", 
  options$Locations, last.mark, options$asOfDate, options$dateTill, useClose, "CPS")
  
  clean.res <- function(x){
    TT  <- unlist(x[["FutToday"]])
    TT  <- data.frame(date="today", contract=names(TT), price=as.numeric(TT))
    YY  <- unlist(x[["FutYesterday"]])
    YY  <- data.frame(date="asOfDate", contract=names(YY), price=as.numeric(YY))
    res <- cbind(curve.name=x$loc, rbind(TT, YY))
  }
  res <- lapply(res, clean.res)
  res <- do.call("rbind", res)
  names(res)[4] <- "value"
  
  res <- cast(res, curve.name + contract  ~ date, I)
  res$contract <- as.Date(dateGS(res$contract, 1, options))
  res$priceDiff <- res$today - res$asOfDate
  res <- cbind(rep(options$asOfDate, nrow(res)), res)
  colnames(res) <- c("PRICING_DATE", "COMMOD_CURVE", "START_DATE", "TODAY_PRICE", "ASOFDATE_PRICE", "PRICE_DIFF")
  
  if (options$returnType == "today") {
    res <- res[c("PRICING_DATE", "COMMOD_CURVE", "START_DATE", "TODAY_PRICE")]
  } else if (options$returnType == "asofdate") {
    res <- res[c("PRICING_DATE", "COMMOD_CURVE", "START_DATE", "ASOFDATE_PRICE")]
  } else if (options$returnType == "pricediff") {
    res <- res[c("PRICING_DATE", "COMMOD_CURVE", "START_DATE", "PRICE_DIFF")]
  } 
  
  colnames(res)[4] <- "PRICE"
  
  return(res)
}