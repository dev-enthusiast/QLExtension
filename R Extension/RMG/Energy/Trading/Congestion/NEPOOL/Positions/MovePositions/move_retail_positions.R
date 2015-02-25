# Utility to move the retail spreads from PM East Retail Portfolio to the
# congestion portfolio.
#
# 
# S:\All\Structured Risk\CNE\PMEast_Retail_Template.xls
#
#
# Written by AAD on 17-Jul-2009

################################################################################
# Get what you want to move.  Adapted from Condor/daily_positions_AAD.R
# zone <- "CT", only one zone
# @param accounting is NOT used anywhere!
#
.retailPositionsMove <- function(QQ, asOfDate, zone=NULL, accounting="ACCRUAL")
{
  RR <- subset(QQ, portfolio=="CNE NEWENGLAND PORTFOLIO")

  ## if (accounting=="ACCRUAL"){
  ##   RR <- subset(QQ, portfolio=="PM EAST RETAIL PORTFOLIO")
  ## } else if (accounting=="MTM"){   
  ##   RR <- subset(QQ, trading.book=="NEBKNES") # from MTM Other Hedges, CNE block trades!
  ## } else {
  ##   rLog("Unknown accounting", accounting)
  ## }
  RR <- subset(RR, mkt == "PWX")  # only NEPOOL 
  RR <- RR[,c("curve", "contract.dt", "delta", "bucket", "quantity.uom")]

  
  out <- ""
  if (any(RR$quantity.uom != "MWH"))
    out <- c(out, "STOP!  Some NEPOOL power positions have quantity not MWH.")

  RR <- cbind(RR, classifyCurveName(RR$curve, by=c("location", "peak/offpeak"))[,-1])
  aux <- cast(RR, contract.dt + location + bucket ~ ., sum, value="delta")
  names(aux)[ncol(aux)] <- "delta"

  # add the hours
  HRS <- ntplot.bucketHours(c("2X16H", "7X8", "5X16"), region="NEPOOL",
      startDate=min(RR$contract.dt), endDate=max(RR$contract.dt))
  names(HRS)[2:3] <- c("contract.dt", "hours")
  aux <- merge(aux, HRS, by=c("bucket", "contract.dt"), all.x=TRUE)
  aux$mw <- aux$delta/aux$hours

  # pick only the positions above the 25 MW threshold
  #  aux <- subset(aux, abs(mw) >= threshold & location != "MAHUB")
  #  if (nrow(aux) == 0)
  #    stop("No positions to move!")
  
  aux <- subset(aux, mw < 0)               # take only the shorts ... 
  aux$roundMW <- -round(aux$mw)            # flip the sign; why did I do this?!
  aux <- subset(aux, location != "MAHUB")  # maybe Steve has some stuff ...

  # make sure you have all buckets for each location/month ...
  uLocs <- expand.grid(location=unique(aux$location),
    contract.dt=unique(aux$contract.dt), bucket=c("2X16H", "5X16", "7X8"))
  aux <- merge(aux[,c("contract.dt", "location", "bucket", "roundMW")],
               uLocs, all=TRUE)
  aux[is.na(aux)] <- 0
  
  qq <- cast(aux, contract.dt ~ location + bucket, I, fill=0, value="roundMW")
  qq <- as.data.frame(qq)
  names(qq)[1] <- "month"
  names(qq) <- gsub("^BOS_",    "NEMA DA_",  names(qq))
  names(qq) <- gsub("^BOSTON_",    "NEMA DA_",  names(qq))
  names(qq) <- gsub("^CT_",     "CT DA_",    names(qq))
  names(qq) <- gsub("^MAINE_",  "MAINE DA_", names(qq))
  names(qq) <- gsub("^NH_",     "NH DA_",    names(qq))
  names(qq) <- gsub("^RI_",     "RI DA_",    names(qq))
  names(qq) <- gsub("^SEMA_",   "SEMA DA_",  names(qq))
  names(qq) <- gsub("^WMA_",    "WMA DA_",   names(qq))
  names(qq) <- gsub("^VT_",     "VT DA_",    names(qq))

  if (!is.null(zone)) {
    ind <- which(sapply(strsplit(colnames(qq), " "), "[[", 1) %in% zone)
    qq <- qq[, c(1, ind)]
 }
   
  # calculate the offsetting hub positions
  ind <- grep("_2X16H$", names(qq))
  qq$`HUB_2X16H` <- -apply(qq[,ind, drop=FALSE], 1, sum)
  ind <- grep("_5X16$", names(qq))
  qq$`HUB_5X16`  <- -apply(qq[,ind, drop=FALSE], 1, sum)
  ind <- grep("_7X8$", names(qq))
  qq$`HUB_7X8`   <- -apply(qq[,ind, drop=FALSE], 1, sum)

  # sort them in the order of the zone numbers ...
  locs <- data.frame(loc=gsub("_.*$", "", colnames(qq)[-1]), ind.qq=1:(ncol(qq)-1))
  rightOrder <- data.frame(loc=c("HUB", "MAINE DA", "NH DA", "VT DA",
    "CT DA", "RI DA", "SEMA DA", "WMA DA", "NEMA DA"), ind=1:9)
  ind <- merge(rightOrder, locs, all.y=TRUE)
  ind <- ind[order(ind$ind, ind$ind.qq), ]
  qq <- qq[,c(1, ind$ind.qq+1)]

  # go to the last Dec
  months <- seq(as.Date(qq$month[1]), as.Date(format(tail(qq$month,1),
    "%Y-12-01")), by="1 month")
  if (tail(months,1) < as.Date("2013-12-01"))
    months <- c(months, seq(tail(months,1), as.Date("2013-12-01"),
      by="1 month")[-1])
  qq <- merge(qq, data.frame(month=months), all=TRUE)
  qq[is.na(qq)] <- 0

  # start from next month
  startDate <- as.Date(secdb.dateRuleApply(asOfDate, "+1e"))+1
  qq <- subset(qq, month >= startDate)
  rownames(qq) <- NULL
  
  return(qq) 
}


################################################################################
#
.getPrices <- function(asOfDate, startDate, endDate)
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c("HUB", "MAINE DA", "NH DA", "VT DA",
    "CT DA", "RI DA", "SEMA DA", "WMA DA", "NEMA DA")
  bucket      <- c("5X16", "2X16H", "7X8")
  serviceType <- "Energy"
  useFutDates <- FALSE
  
  # get the prices, and format them right 
  PP <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  PP <- cast(PP, month ~ location + bucket, I, fill=NA)

  # sort them in the order of the zone numbers ...
  locs <- data.frame(loc=gsub("_.*$", "", colnames(PP)[-1]), ind.PP=1:(ncol(PP)-1))
  rightOrder <- data.frame(loc=c("HUB", "MAINE DA", "NH DA", "VT DA",
    "CT DA", "RI DA", "SEMA DA", "WMA DA", "NEMA DA"), ind=1:9)
  ind <- merge(rightOrder, locs, all.y=TRUE)
  ind <- ind[order(ind$ind, ind$ind.PP), ]
  PP <- PP[,c(1, ind$ind.PP+1)]
  
  return(PP)
}

################################################################################
#
.customizeCalcs <- function(QQQ, PP)
{
  cList <- calculator.list("CPS Trading Group", "E47187")

  cNameMap <- matrix(c(
    "Calc ECFD Spreads Retail CT   0", "CT DA", 
    "Calc ECFD Spreads Retail Hu   0", "HUB", 
    "Calc ECFD Spreads Retail ME   0", "MAINE DA", 
    "Calc ECFD Spreads Retail NE   0", "NEMA DA", 
    "Calc ECFD Spreads Retail NH   0", "NH DA",
    "Calc ECFD Spreads Retail RI   0", "RI DA", 
    "Calc ECFD Spreads Retail SE   0", "SEMA DA", 
    "Calc ECFD Spreads Retail WM   0", "WMA DA"), ncol=2, byrow=TRUE)
  rownames(cNameMap) <- cNameMap[,2]
  cNameMap <- cNameMap[,-2]   # transform to a vector, drop=TRUE

  if (!identical(dim(QQQ), dim(PP)))
    stop("Prices and quantities data.frame don't have the same dimensions")

  uZones <- unique(gsub("(.*)_.*", "\\1", colnames(QQQ)[-1]))
  
  for (z in seq_along(uZones)){
    cat("Working on zone:", uZones[z], "...")
    qq <- QQQ[,c(1, grep(uZones[z], colnames(QQQ)))]
    pp <- PP[,c(1, grep(uZones[z], colnames(PP)))]
    fullCalcName <- as.character(cNameMap[uZones[z]])
    #source("H:/user/R/RMG/Utilities/Interfaces/PM/R/secdb.customizeCalculator.R")
    secdb.customizeCalculator(fullCalcName, quantity=qq, fixPrice=pp)
    
    cat("Done.\n\n\n")
  }

  
}


################################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(PM)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Positions/MovePositions/move_retail_positions.R")
  
  ZZ <<- data.frame(
    zone=c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
    deliv.pt=c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),                
    ptid=4001:4008)

  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  dirIn <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
  setwd(dirIn)
  fileName <- paste(dirIn, "positionsAAD_", as.character(asOfDate), ".RData", sep="")
  load(fileName)
  rLog("Loaded", fileName)  # loads variable QQ

  zone <- NULL
  zone <- c("CT", "NEMA", "MAINE")

  #QQQ <- .retailPositionsMove(QQ, asOfDate, zone=zone, accounting="MTM") 
  QQQ <- .retailPositionsMove(QQ, asOfDate, zone=zone, accounting="ACCRUAL")
  QQQ <- subset(QQQ, month <= as.Date("2017-12-01"))
  
  #QQQ <- QQQ[,-which(grepl("HUB DA_", colnames(QQQ)))]  # not sure why this is here?!
  print(QQQ, row.names=FALSE)    # paste in the xls file
  
  
  startDate <- QQQ$month[1]
  endDate   <- QQQ$month[nrow(QQQ)]
  PP <- .getPrices(Sys.Date(), startDate, endDate)
  PP <- PP[, colnames(QQQ)]  # only where I have positions
 
  # customize the calcs
  .customizeCalcs(QQQ, PP)


  # positions QQQ are what Steve needs to buy to get flat.
}






## # just to CT to start
## ind <- grep("^CT DA", colnames(QQQ))
## qq  <- QQQ[, c(1,ind)]
## pp  <- PP[, c(1,ind)]

## source("H:/user/R/RMG/Utilities/Interfaces/PM/R/secdb.customizeCalculator.R")


## print(qq, row.names=FALSE)

## # customize the hub calc now ... 
## qqHub <- QQQ[, 1:4]; qqHub[,2:4] <- -qq[,2:4]
## ppHub <- PP[, 1:4]
## secdb.customizeCalculator(fullCalcName, quantity=qqHub, fixPrice=ppHub)






## write.csv(qq, file=paste("retailPositionsMoved_", as.character(asOfDate),
##                 ".csv", sep=""), row.names=FALSE)



###########################################################################
# Steps to take:
# - go to your area and modify the calcs so they start from next month
# - 
