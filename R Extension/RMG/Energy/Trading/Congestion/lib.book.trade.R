# Utility to move the retail spreads from PM East Retail Portfolio to the
# congestion portfolio.
# An example of how to customize a calc with excel is at:
#   S:\All\Structured Risk\CNE\PMEast_Retail_Template.xls
#
# .customize_basis_swap
# .customize_calc_1x3          calc with 1 commod and 3 buckets
# .customize_calc_2x3          calc with 2 commod and 3 buckets
# .customize_calc_1x2          calc with 1 commod and 2 buckets
# .find_nodes_with_positions 
# .filter_positions_move 
# .get_prices
# .read_quantity_from_xlsx
#
#
# Written by Adrian Dragulescu


secdb.evaluateSlang('link("_lib ntplot addin");')

################################################################################
# 1 commodity only.  Good for a gas calculator. 
# fullCalculatorName <- "Calc SWAP gas deltas          0"
# Note that the gas price is in cents! 
#   QQ is a data.frame with columns ("month", "delta")
#   PP is a data.frame with columns ("month", "price")
#
.customize_basis_swap <- function(fullCalculatorName, QQ, PP, recalculate=TRUE,
   location="ALGCG")
{
  calculatorName <- paste(secdb.getValueType(fullCalculatorName, "Calc Prefix"),
                          secdb.getValueType(fullCalculatorName, "Description"))
  if (nchar(calculatorName)==0)
    stop("Check the fullCalculatorName.  It is probably wrong.")

  trader  <- secdb.getValueType(fullCalculatorName, "Trader")
  group   <- secdb.getValueType(fullCalculatorName, "Group")

  # always clear the custom details before writing
  lib  <- "_lib commod calc utils"
  func <- "CalcUtils::ClearCustomDetails"
  s <- secdb.getValueType(fullCalculatorName, "Calc Security")
  res  <- secdb.invoke(lib, func, s)  # works now, may work with fullCalculatorName too  
  rLog("Cleared custom details")  
  
  # always change start/end date of a calculator to match the inputs 
  func <- "CalcUtils::CustomizeStartEnd"
  startDt <- format(QQ$month[1], "%d%b%y")
  endDt   <- format(seq(QQ$month[nrow(QQ)], by="1 month",
                        length.out=2)[2]-1, "%d%b%y")
  res <- secdb.invoke(lib, func, fullCalculatorName, startDt, endDt)
  if (res != "Successful update"){
    stop("Cannot set calculator Start/End date.")
  } else {
    rLog("Successfuly changed start/end date.")
  }

  lib  <- "_LIB R Bridge"
  func <- "R Bridge::CustomizeCalc"

  if (!is.null(QQ)) {
    data <- NULL
    for (m in 1:nrow(QQ)) 
      data[[m]] <- list(QQ$month[m], QQ$delta[m])

    res <- secdb.invoke(lib, func, group, trader, calculatorName,
      "Energy", location, "7x24", "Quantity", data, recalculate)
  }

  if (!is.null(PP)) {
    data <- NULL
    for (m in 1:nrow(PP)) 
      data[[m]] <- list(PP$month[m], PP$price[m])

    res <- secdb.invoke(lib, func, group, trader, calculatorName,
      "Price", location, "7x24", "Fixed Price", data, recalculate)
  }

  rLog("Done")
}


################################################################################
# 1 commodity, 3 buckets
#
.customize_calc_1x3 <- function(QQ, PP,
   fullCalcName="Calc ECFD NEPOOL 1x3          0")
{
  secdb.customizeCalculator(fullCalcName, quantity=QQ, fixPrice=PP)
  cat("Done.\n\n\n")
}

################################################################################
# 2 commodity (for spreads), 3 buckets
#
.customize_calc_2x3 <- function(QQ, PP,
   fullCalcName="Calc ECFD NEPOOL 2x3          0")
{
  secdb.customizeCalculator(fullCalcName, quantity=QQ, fixPrice=PP)
  cat("Done.\n\n\n")
}

################################################################################
# 1 commodity, 2 buckets
#
.customize_calc_1x2 <- function(QQ, PP,
   fullCalcName="Calc ECFD NEPOOL 1x2          0")
{
  secdb.customizeCalculator(fullCalcName, quantity=QQ, fixPrice=PP)
  cat("Done.\n\n\n")
}

################################################################################
# 1 commodity, 1 bucket
#
.customize_calc_1x1 <- function(QQ, PP,
   fullCalcName="Calc ECFD NEPOOL 1x1          0")
{
  secdb.customizeCalculator(fullCalcName, quantity=QQ, fixPrice=PP)
  cat("Done.\n\n\n")
}


################################################################################
# get a dataframe with positions by node.  An auxiliary function. 
# 
.find_nodes_with_positions <- function(QQ)
{
  uQQ <- cast(QQ, delivery.point ~ ., sum, fill=NA, value="delta")
  colnames(uQQ)[2] <- "totalDelta"

  maxTerm <- cast(QQ, delivery.point ~., max, fill=NA, value="contract.dt")
  colnames(maxTerm)[2] <- "maxTerm"
  maxTerm$maxTerm <- as.Date(maxTerm$maxTerm, origin="1970-01-01")

  uQQ <- merge(uQQ, maxTerm)
  uQQ$totalDelta <- round(uQQ$totalDelta)

  uQQ <- subset(uQQ, abs(uQQ$totalDelta) > 1)
  
  uQQ <- uQQ[order(-abs(uQQ$totalDelta)),]

  uQQ
}

################################################################################
# Filter the big overnight positions for the things you actually need.
# You can specify either deliveryPt OR etis, not both.
# QQ is what you get from get.positions.blackbird
#
.filter_positions_move <- function(QQ, asOfDate, deliveryPt=NULL, etis=NULL,
  addHub=TRUE, portfolioName="NEPOOL FP PORTFOLIO", market="PWX", startDate=NULL)
{
  if ("portfolio" %in% names(QQ))
    QQ <- subset(QQ, portfolio==portfolioName)
  if ("mkt" %in% names(QQ))  
  QQ <- subset(QQ, mkt == market) 
 
  if (!is.null(deliveryPt)) {
    qq <- subset(QQ, delivery.point %in% deliveryPt)
    if (length(deliveryPt) > 1) {
      rLog("Aggregating all delivery points under ", deliveryPt[1])
      qq$delivery.point <- deliveryPt[1]
    }
  }
  if (!is.null(etis))
    qq <- subset(QQ, eti %in% etis)
  
  out <- ""
  if (any(qq$quantity.uom != "MWH"))
    out <- c(out, "STOP!  Some NEPOOL power positions have quantity not MWH.")

  qq <- cbind(qq, classifyCurveName(qq$curve, by=c("location", "bucket"))[,-1])
  aux <- cast(qq, contract.dt + delivery.point + bucket ~ ., sum, value="delta")
  names(aux)[ncol(aux)] <- "delta"

  # when you transfer positions, make all locations end in DA
  ind <- !(grepl(" DA$", aux$delivery.point) |
    aux$delivery.point %in% c("SC", "CL8"))
  aux$delivery.point[ind] <- paste(aux$delivery.point[ind], " DA", sep="")
  
  # add the hours
  HRS <- ntplot.bucketHours(c("2X16H", "7X8", "5X16"), region="NEPOOL",
      startDate=min(qq$contract.dt), endDate=max(qq$contract.dt))
  names(HRS)[2:3] <- c("contract.dt", "hours")
  aux <- merge(aux, HRS, by=c("bucket", "contract.dt"), all.x=TRUE)
  aux$mw <- aux$delta/aux$hours

  # make sure you have all buckets for each location/month ...
  uLocs <- expand.grid(delivery.point=unique(aux$delivery.point),
    contract.dt=unique(aux$contract.dt), bucket=c("2X16H", "5X16", "7X8"))
  aux <- merge(aux[,c("contract.dt", "delivery.point", "bucket", "mw")],
               uLocs, all=TRUE)
  aux[is.na(aux)] <- 0
  
  qq <- cast(aux, contract.dt ~ delivery.point + bucket, sum, fill=0, value="mw")
  qq <- as.data.frame(qq)
  names(qq)[1] <- "month"
  #names(qq) <- gsub("^BOS_",    "NEMA DA_",  names(qq))

  
  # calculate the offsetting hub positions
  ind <- grep("_2X16H$", names(qq))
  qq$`HUB_2X16H` <- -apply(qq[,ind, drop=FALSE], 1, sum)
  ind <- grep("_5X16$", names(qq))
  qq$`HUB_5X16`  <- -apply(qq[,ind, drop=FALSE], 1, sum)
  ind <- grep("_7X8$", names(qq))
  qq$`HUB_7X8`   <- -apply(qq[,ind, drop=FALSE], 1, sum)

  # go to the last Dec
  months <- seq(as.Date(qq$month[1]), as.Date(format(tail(qq$month,1),
    "%Y-12-01")), by="1 month")
  qq <- merge(qq, data.frame(month=months), all=TRUE)
  qq[is.na(qq)] <- 0

  # start from next month
  if (is.null(startDate))
    startDate <- as.Date(secdb.dateRuleApply(asOfDate, "+1e"))+1
  qq <- subset(qq, month >= startDate)
  rownames(qq) <- NULL
  qq[,c(2:ncol(qq))] <- round(qq[,c(2:ncol(qq))], 5)  
  
  return(qq) 
}



################################################################################
# colnames are in the form of: HUB_5X16
#
.get_prices <- function(asOfDate, QQ)
{
  startDate   <- QQ$month[1]
  endDate     <- QQ$month[nrow(QQ)]
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- unique(gsub("(.*)_(.*)$", "\\1", colnames(QQ)[-1]))
  bucket      <- unique(gsub(".*_(.*)$", "\\1", colnames(QQ)[-1]))
  serviceType <- "Energy"
  useFutDates <- FALSE
  
  # get the prices, and format them right 
  PP <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  PP <- cast(PP, month ~ location + bucket, I, fill=NA)

  PP <- PP[,colnames(QQ)]
  
  return(PP)
}


################################################################################
# colnames contain the location and the bucket, e.g. columns
#   c("month", "CT DA_5X16", "CT DA_7X8", "CT DA_2X16H")
# only one location (3 buckets) at a time.
# @param isQtyInMWh if true, need to divide by hours in bucket for the calculator
# @return a data.frame ready for the calculator
# 
.read_quantity_from_xlsx <- function(filename, sheet="position_calc",
   colIndex=NULL, rowIndex=NULL, addHUB=FALSE, isQtyInMWh=FALSE)
{
  require(xlsx)
  rLog("Reading positions from:", filename)
  rLog("for sheet:", sheet)
  QQ <- read.xlsx(filename, sheetName=sheet, rowIndex=rowIndex)
  colnames(QQ) <- toupper(colnames(QQ))

  if (class(QQ[,1])!="Date"){
    stop("First column you read should be the contract month!")
  }
  colnames(QQ)[1] <- "month"

  # when you read spaces "CT DA_PEAK" --> "CT.DA_PEAK"
  colnames(QQ) <- gsub("\\.DA_", " DA_", colnames(QQ))

  # only one location at a time (mostly 3 buckets)
  if (ncol(QQ) > 4)
    stop("You probably have too many locations/buckets!")  # should have better check
  
  # do some validation on the positions spreadsheet ... 
  buckets <- gsub(".*_(.*)$", "\\1", colnames(QQ)[-1])
  
  allowedBuckets <-  c("PEAK", "OFFPEAK", "FLAT", "7X8", "2X16H", "5X16")
  ind <- buckets %in% allowedBuckets
  if (any(!ind))
    rLog("Unkown bucket:", buckets[!ind])

  # remove empty rows
  ind <- which(apply(QQ, 1, function(x)all(is.na(x))))
  if (length(ind)>0)
    QQ <- QQ[-ind,]

  # convert to numeric
  QQ[,2:ncol(QQ)] <- apply(QQ[,2:ncol(QQ)], 2, as.numeric)
  
  # replace mising cells with zeros ...
  QQ[is.na(QQ)] <- 0
  
  # convert to MW
  if (isQtyInMWh) {
    hrs <- PM::ntplot.bucketHours(buckets, region="NEPOOL",
      startDate=min(QQ[,1]), endDate=max(QQ[,1]))
    colnames(hrs)[2:3] <- c("month", "hrs")
    QQL <- melt(QQ, id=1)
    QQL$bucket <- gsub(".*_(.*)$", "\\1", QQL$variable)
    QQL <- merge(QQL, hrs, by=c("month", "bucket"), all.x=TRUE)
    QQL$value <- QQL$value/QQL$hrs
    QQ <- cast(QQL, month ~ variable, I, value="value", fill=NA)
    QQ <- as.data.frame(QQ)  # this this for the apply below
  }  

  if (addHUB){
    # calculate the offsetting hub positions
    ind <- grep("_5X16$", names(QQ))
    QQ$`HUB_5X16`  <- -apply(QQ[,ind, drop=FALSE], 1, sum)
    ind <- grep("_2X16H$", names(QQ))
    QQ$`HUB_2X16H` <- -apply(QQ[,ind, drop=FALSE], 1, sum)
    ind <- grep("_7X8$", names(QQ))
    QQ$`HUB_7X8`   <- -apply(QQ[,ind, drop=FALSE], 1, sum)
  }
  
  return(QQ)

##        month NEMA DA_2X16H NEMA DA_5X16 NEMA DA_7X8  HUB_5X16  HUB_2X16H    HUB_7X8
## 1 2015-02-01    -3415.9559   -19341.276  -9987.6049 19341.276  3415.9559  9987.6049
## 2 2015-03-01   -13421.6120   -37400.632 -15539.3410 37400.632 13421.6120 15539.3410
## 3 2015-04-01     -295.4583    -4496.778  -1237.7615  4496.778   295.4583  1237.7615
## 4 2015-05-01     -183.4546    -2806.606   -228.1162  2806.606   183.4546   228.1162
## 5 2015-06-01     -711.0471    -6589.502  -1195.0291  6589.502   711.0471  1195.0291
}


################################################################################
################################################################################















##   # sort them in the order of the zone numbers ...
##   locs <- data.frame(loc=gsub("_.*$", "", colnames(PP)[-1]), ind.PP=1:(ncol(PP)-1))
##   rightOrder <- data.frame(loc=c(deliveryPt, "HUB"), ind=1:(length(deliveryPt)+1))
##   ind <- merge(rightOrder, locs, all.y=TRUE)
##   ind <- ind[order(ind$ind, ind$ind.PP), ]
##   PP <- PP[,c(1, ind$ind.PP+1)]






## print(qq, row.names=FALSE)

## # customize the hub calc now ... 
## qqHub <- QQQ[, 1:4]; qqHub[,2:4] <- -qq[,2:4]
## ppHub <- PP[, 1:4]
## secdb.customizeCalculator(fullCalcName, quantity=qqHub, fixPrice=ppHub)


## REMEMBER!  CONGIBT BUYS NECIBT SELLS!



## aux1 <- subset(QQ, delivery.point=="MAINE" & contract.dt==as.Date("2010-08-01"))
## aux2 <- subset(QQ, delivery.point=="MAINE DA" & contract.dt==as.Date("2010-08-01"))
## aux3 <- subset(QQ, delivery.point=="MAINE-SC" & contract.dt==as.Date("2010-08-01"))



## ################################################################################
## # Get what you want to move.  Adapted from Condor/daily_positions_AAD.R
## # 
## .fp_positions_move <- function(QQ, asOfDate, deliveryPt=NULL, etis=NULL)
## {
##   if (!is.null(deliveryPt))
##     qq <- subset(QQ, delivery.point==deliveryPt)
##   if (!is.null(etis))
##     qq <- subset(QQ, eti %in% etis)
  
  
##   out <- ""
##   if (any(qq$quantity.uom != "MWH"))
##     out <- c(out, "STOP!  Some NEPOOL power positions have quantity not MWH.")

##   qq <- cbind(qq, classifyCurveName(qq$curve, by=c("location", "bucket"))[,-1])
##   aux <- cast(qq, contract.dt + delivery.point + bucket ~ ., sum, value="delta")
##   names(aux)[ncol(aux)] <- "delta"

##   # when you transfer positions, make all locations end in DA
##   ind <- !(grepl(" DA$", aux$delivery.point) |
##     aux$delivery.point %in% c("SC", "CL8"))
##   aux$delivery.point[ind] <- paste(aux$delivery.point[ind], " DA", sep="")
  
##   # add the hours
##   HRS <- ntplot.bucketHours(c("2X16H", "7X8", "5X16"), region="NEPOOL",
##       startDate=min(qq$contract.dt), endDate=max(qq$contract.dt))
##   names(HRS)[2:3] <- c("contract.dt", "hours")
##   aux <- merge(aux, HRS, by=c("bucket", "contract.dt"), all.x=TRUE)
##   aux$mw <- aux$delta/aux$hours

##   # make sure you have all buckets for each location/month ...
##   uLocs <- expand.grid(delivery.point=unique(aux$delivery.point),
##     contract.dt=unique(aux$contract.dt), bucket=c("2X16H", "5X16", "7X8"))
##   aux <- merge(aux[,c("contract.dt", "delivery.point", "bucket", "mw")],
##                uLocs, all=TRUE)
##   aux[is.na(aux)] <- 0
  
##   qq <- cast(aux, contract.dt ~ delivery.point + bucket, sum, fill=0, value="mw")
##   qq <- as.data.frame(qq)
##   names(qq)[1] <- "month"
##   #names(qq) <- gsub("^BOS_",    "NEMA DA_",  names(qq))

  
##   # calculate the offsetting hub positions
##   ind <- grep("_2X16H$", names(qq))
##   qq$`HUB_2X16H` <- -apply(qq[,ind, drop=FALSE], 1, sum)
##   ind <- grep("_5X16$", names(qq))
##   qq$`HUB_5X16`  <- -apply(qq[,ind, drop=FALSE], 1, sum)
##   ind <- grep("_7X8$", names(qq))
##   qq$`HUB_7X8`   <- -apply(qq[,ind, drop=FALSE], 1, sum)

##   # go to the last Dec
##   months <- seq(as.Date(qq$month[1]), as.Date(format(tail(qq$month,1),
##     "%Y-12-01")), by="1 month")
##   qq <- merge(qq, data.frame(month=months), all=TRUE)
##   qq[is.na(qq)] <- 0

##   # start from next month
##   startDate <- as.Date(secdb.dateRuleApply(asOfDate, "+1e"))+1
##   qq <- subset(qq, month >= startDate)
##   rownames(qq) <- NULL
##   qq[,c(2:ncol(qq))] <- round(qq[,c(2:ncol(qq))], 5)  
  
##   return(qq) 
## }


## ################################################################################
## # Get what you want to move.  Adapted from Condor/daily_positions_AAD.R
## # 
## .find_nodes_with_positions <- function(QQ)
## {
##   uQQ <- cast(QQ, delivery.point ~ ., sum, fill=NA, value="delta")
##   colnames(uQQ)[2] <- "totalDelta"

##   maxTerm <- cast(QQ, delivery.point ~., max, fill=NA, value="contract.dt")
##   colnames(maxTerm)[2] <- "maxTerm"
##   maxTerm$maxTerm <- as.Date(maxTerm$maxTerm, origin="1970-01-01")

##   uQQ <- merge(uQQ, maxTerm)
##   uQQ$totalDelta <- round(uQQ$totalDelta)

##   uQQ <- subset(uQQ, abs(uQQ$totalDelta) > 1)
  
##   uQQ <- uQQ[order(-abs(uQQ$totalDelta)),]

##   uQQ
## }

