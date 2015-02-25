# Utility to move the WM positions from FP to the congestion portfolio.
# moved Oneida
# 
# 

################################################################################
# Get what you want to move.  Adapted from Condor/daily_positions_AAD.R
# 
.getPositionsMove <- function(asOfDate, books, locations=c("NPX", "ROSETON.4011"))
{
  QQ <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
                                       fix=TRUE)
  QQ <- subset(QQ, !(curve %in% c("USD", "CAD")))

  QQ <- subset(QQ, delivery.point %in% c("NPX", "ROSETON.4011 DA"))
  QQ <- QQ[order(QQ$delivery.point, QQ$bucket, QQ$contract.dt),]
  
  qq <- cast(QQ, delivery.point + bucket + contract.dt ~ ., sum, value="delta")
  names(qq)[4] <- "value"
    
  # add HRS
  HRS <- FTR:::FTR.bucketHours(start.dt=as.POSIXlt("2003-01-01 01:00:00"),
         end.dt=as.POSIXlt("2018-01-01 00:00:00"), buckets=c("5X16", "7X8", "2X16H"))
  HRS$bucket <- HRS$class.type; HRS$class.type <- NULL
  QQ.delivPt <- merge(qq, HRS, by=c("bucket", "contract.dt"), all.x=TRUE)
  QQ.delivPt$mw <- round(QQ.delivPt$value/QQ.delivPt$hours, 4)

  QQ.delivPt$value <- NULL
  qq <- cast(QQ.delivPt, contract.dt ~ delivery.point + bucket, I,
             fill=0, value="mw")
  names(qq)[1] <- "month"
  #names(qq) <- gsub("ROSETON.4011", "ROSETON.4011 DA", names(qq))
  names(qq) <- gsub("NPX", "DAM NPX", names(qq))
  
  return(qq)
  
}



################################################################################
#
.getPrices <- function(asOfDate, startDate, endDate)
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- c("ROSETON.4011 DA")
  bucket      <- c("2X16H", "7X8", "5X16")
  serviceType <- "Energy"
  useFutDates <- FALSE
  
  # get the prices, and format them right 
  PP <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  PP <- cast(PP, month ~ location + bucket, I, fill=NA)

  commodity   <- "COMMOD PWR NYPP PHYSICAL"
  location    <- c("DAM NPX")
  bucket      <- c("2X16H", "7X8", "5X16")
  serviceType <- "Energy"
  useFutDates <- FALSE
  
  # get the prices, and format them right 
  PP2 <- secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  PP2 <- cast(PP2, month ~ location + bucket, I, fill=NA)
  
  PP <- merge(PP2, PP)
  
  return(PP)
}

################################################################################
#
#
.get_PnL <- function(qq_filename, day1=as.Date("2010-12-23"),
                     day2=as.Date("2011-01-11"))
{
  qq_filename <- paste("H:/user/R/RMG/Energy/Trading/Congestion/NYPP/Deals/",
    "WasteManagement/positions_2010-01-11.csv", sep="")
  day1 <- as.Date("2010-12-23")
  day2 <- as.Date("2011-01-11")
  
  qq <- read.csv(qq_filename)
  qq$month <- as.Date(qq$month)
  names(qq) <- gsub("DAM\\.NPX", "DAM NPX", names(qq))
  names(qq) <- gsub("4011\\.DA", "4011 DA", names(qq))
  startDate <- qq$month[1]
  endDate   <- qq$month[nrow(qq)]
  QL <- melt(qq, id=1)
  names(QL)[ncol(QL)] <- "mw"
  QL$bucket <- gsub(".*_(.*)", "\\1", QL$variable)
  # add HRS
  HRS <- FTR:::FTR.bucketHours(start.dt=as.POSIXlt("2003-01-01 00:00:00"),
         end.dt=as.POSIXlt("2012-12-31 23:00:00"), buckets="7X24")
  QL <- merge(QQ.delivPt, HRS, all.x=TRUE)
  
  
}


################################################################################
# source("H:/user/R/RMG/Utilities/Interfaces/PM/R/secdb.customizeCalculator.R")
#
.customizeCalcs <- function(QQ, PP)
{
  cList <- calculator.list("CPS Trading Group", "E47187")

  rLog("Customize the NY side")
  ind <- c(1, grep("^DAM NPX", colnames(QQ)))
  qq <- QQ[,ind]
  pp <- PP[,ind]
  fullCalcName <- "Calc ECFD wm deals ny         0" 
  if (!identical(dimnames(qq), dimnames(pp)))
    stop("Prices and quantities data.frame don't have the same dimensions")
  secdb.customizeCalculator(fullCalcName, quantity=qq, fixPrice=pp)
  cat("Done.\n\n")

  rLog("Customize the NEPOOL side")
  ind <- c(1, grep("^ROSETON", colnames(QQ)))
  qq <- QQ[, ind]
  pp <- PP[, ind]
  fullCalcName <- "Calc ECFD wm deals ne         0" 
  if (!identical(dimnames(qq), dimnames(pp)))
    stop("Prices and quantities data.frame don't have the same dimensions")
  secdb.customizeCalculator(fullCalcName, quantity=qq, fixPrice=pp)
  cat("Done.\n\n")
  
}


################################################################################
#  
require(CEGbase); require(SecDb); require(PM); require(reshape)
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

#books <- .getPortfolioBooksFromSecDb("CNE NewYork Accr Portfolio")
books <- "NYONEIDA"
asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))  # from last night

locations <- c("NPX", "ROSETON.4011")  # all I care now
QQ <- .getPositionsMove(asOfDate, books, locations)


#write.csv(QQ, file="H:/user/R/RMG/Energy/Trading/Congestion/NYPP/Deals/WasteManagement/positions_2010-01-12.csv", row.names=FALSE)
QQ <- subset(QQ, month >= as.Date("2013-01-01"))
rownames(QQ) <- NULL

startDate <- QQ$month[1]
endDate   <- QQ$month[nrow(QQ)]
PP <- .getPrices(Sys.Date(), startDate, endDate)
PP <- PP[, colnames(QQ)]  # only where I have positions


## # skip the cash month
## QQ <- QQ[2:nrow(QQ),]
## PP <- PP[2:nrow(PP),]

# customize the calcs
.customizeCalcs(QQ, PP)

# this is what you paste in the xls file
print(QQ, row.names=FALSE)

# CONGIBT buys, IBTNYCSG sells
# two trades, one for NPX, one for ROSETON















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






write.csv(qq, file=paste("retailPositionsMoved_", as.character(asOfDate),
                ".csv", sep=""), row.names=FALSE)



###########################################################################
# Steps to take:
# - go to your area and modify the calcs so they start from next month
# - 




###########################################################################
# get the exact positions from PRISM


books <- c("NYMONROE", "NYHIACRS", "NYCHAFFE", "NYMILSET")
QQ <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
                                      fix=TRUE)
QQ <- subset(QQ, !(curve %in% c("USD", "CAD")))

QQ.delivPt <- getPositionsDeliveryPoint(QQ, subset=TRUE,
  buckets=c("2X16H", "5X16", "7X8"))

# the positions are 7x24
QQ.delivPt <- cast(QQ.delivPt, mkt + delivery.point + contract.dt ~ ., sum, fill=0)
names(QQ.delivPt)[ncol(QQ.delivPt)] <- "value"

# add HRS
require(FTR)
HRS <- FTR.bucketHours(start.dt=as.POSIXlt("2003-01-01 00:00:00"),
         end.dt=as.POSIXlt("2012-12-31 23:00:00"), buckets="7X24")
QQ.delivPt <- merge(QQ.delivPt, HRS, all.x=TRUE)
QQ.delivPt$mw <- QQ.delivPt$value/QQ.delivPt$hours

res <- cast(QQ.delivPt[,c("contract.dt", "delivery.point", "mw")],
  contract.dt ~ delivery.point, I, fill=0, value="mw")

apply(res[,-c(1,7)], 1, sum)







## .positionsMove.20091230 <- function()
## {
##   months <- seq(as.Date("2010-01-01"), as.Date("2010-12-01"), by="1 month")
    
##   qq <- data.frame(month=months,
##     `NEG G MONROE_7X24`  = 1.00,         # relative to DAM WEST, checked               
##     `CENTRAL HI A_7X24`  = 2.85,         # relative to DAM WEST, checked
##     `CHAFEE LFGE_7X24`   = 5.00,         # relative to DAM WEST, checked
##     `MILLSEA LFGE`       = 5.70,         # relative to DAM WEST, checked
##     `SITHE STERLI`       = 1.00,         # relative to DAM WEST, checked
##     `DAM WEST`           = -15.55,                
##      check.names=FALSE)
  
##   return(qq) 
## }
