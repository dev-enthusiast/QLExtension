# For Chelsea, Syam
#
# Writing code has much in common with engineering.  If you build it well
# it will last.  
#
# IMPORTANT:  Read and follow the R style guide
# http://google-styleguide.googlecode.com/svn/trunk/google-r-style.html
#
# help.start()  # to explore help
# ?merge        # help on command merge
#
# Go to www.rseek.org and search for stuff you don't know. 
#
# Go to http://cran.r-project.org/ for manuals and other docs.
#
# You should learn about packages: reshape, ply, zoo
# http://had.co.nz/reshape/
# http://plyr.had.co.nz/
# http://cran.r-project.org/web/packages/zoo/index.html

# This file shows how to estimate PnL after a change in marks
#
# On 12/27 I've remarked the ARRs curve 
# 


#################################################################
# get ccg ARR positions.  
#
get_arr_positions_ccg <- function(asOfDate)
{
  rLog("Pulling CCG positions for:", as.character(asOfDate), ", service:",
        "ARR")
  
  pName <- "Nepool Accrual Portfolio"              
  books <- .getPortfolioBooksFromSecDb(pName)

  # If I get all the books, it picks up attrition books too
  books <- "NSARR1"  
  
  QQ <- get.positions.from.blackbird(asOfDate=asOfDate, books=books,
    fix=FALSE, service="ARR")
  QQ$quantity <- QQ$notional
  
  return(QQ)
}


##########################################################################
# Read the current ARR marks from SecDb.  ARR marks are added to SecDb by
# Strats.  Can be found by using a Full Requirements calculator.
#
get_arr_prices <- function(delivPt=c("MAINE", "NH", "VT", "CT", "RI",
  "SEMA", "WMA", "NEMA"), startDate=Sys.Date(),
  endDate=as.Date("2015-12-01"), asOfDate=Sys.Date())
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- delivPt
  bucket      <- "FLAT"
  serviceType <- "ARR"
  
  rLog("Get the ARR marks from SecDb for", format(asOfDate))  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType)
  #head(curveData)
  rLog("Done")

  curveData
}


##########################################################################
# Calculate PnL given a set of ARR positions and two dates, calculate PnL
# date2 > date1
get_arr_PnL <- function(PP1, PP2, qq)
{
  PP <- merge(PP1[, c("location", "month", "price1")],
              PP2[, c("location", "month", "price2")], all=TRUE)
  names(PP)[which(names(PP)=="location")] <- "delivery.point"
  names(PP)[which(names(PP)=="month")] <- "contract.dt"
  
  PnL <- merge(PP, qq, all.y=TRUE, by=c("delivery.point", "contract.dt"))
  PnL$pnl <- (PnL$price2 - PnL$price1)*PnL$quantity


  return(PnL)  
}


#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)

  source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

  delivPt   <- c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA")
  startDate <- as.Date("2012-01-01")
  endDate   <- as.Date("2016-12-01")
  
  day1 <- as.Date("2011-12-23") 
  day2 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))

  QQ <- get_arr_positions_ccg(day1)
  qq <- cast(QQ, contract.dt + delivery.point ~ ., sum, value="quantity",
             subset=contract.dt >= startDate)
  colnames(qq)[3] <- "quantity"
  
  # get prices from SecDb
  PP1 <- get_arr_prices(delivPt=delivPt, endDate=endDate,
    asOfDate=day1, startDate=startDate)
  names(PP1)[which(names(PP1)=="value")] <- "price1"

  PP2 <- get_arr_prices(delivPt=delivPt, endDate=endDate,
    asOfDate=day2, startDate=startDate)
  names(PP2)[which(names(PP2)=="value")] <- "price2"

  PnL <- get_arr_PnL(PP1, PP2, qq)

  sum(PnL$pnl)  # total

  # break it up by years
  PnL$year <- format(PnL$contract.dt, "%Y")
  aux <- cast(PnL, year ~ ., sum, value="pnl")
  print(aux, row.names=FALSE)

  
  # checks with the Bal Report for NSARR1 ... 
  

  # how to debug when you have problems?
  

}

