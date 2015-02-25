# Deal with the generation IBTs.
# My CONGIBT trade: PAPA3CT
#
#
# .calc_percent_yr
# .does_move_make_sense
# .get_default_percentage - the default waterfall percentages
# .get_positions_move
# ..get_positions_prism
# .months_to_move
# .ratable_percent        - return a data.frame with columns month, percent
# .skip_day               - if you need to skip a day
# .to_xlsx
# .update_calculators
# .update_calculator_gas
#




######################################################################
# Get the percent hedged for a given year
# @param year an integer, e.g. 2014
# @param asOfDate an R Date
# @param limits a list with the percentages.   
#
.calc_percent_yr <- function( year, asOfDate=Sys.Date(),
                             limits=.get_default_percentage(),
                             method="calendar") 
{
  cashYr <- year - as.numeric(format(asOfDate, "%Y")) 
  lim <- {if (cashYr == 0)
    limits[["cash"]]
  else if (cashYr == 1) {
    limits[["cash+1"]]
  } else if (cashYr == 2) {
    limits[["cash+2"]]
  } else if (cashYr == 3) {
    limits[["cash+3"]]
  } else {
    c(0, 0)
  }}

  if (method == "calendar") {
    noDays <- as.numeric(asOfDate - as.Date(format(asOfDate, "%Y-01-01"))) + 1
    daysYear <- as.numeric(as.Date(format(asOfDate, "%Y-12-31")) -
                         as.Date(format(asOfDate, "%Y-01-01"))) + 1
    percent <- noDays*(lim[2]-lim[1])/daysYear + lim[1]
  } else if (method == "business") {
    boy <- as.Date(format(asOfDate, "%Y-01-01"))
    noDays <- length(which(!secdb.isHoliday(seq(boy, asOfDate, by="1 day"), calendar="NYM"))) 
    daysYear <- max(252,noDays)
    percent <- noDays*(lim[2]-lim[1])/daysYear + lim[1]
    #days <- seq(boy, asOfDate, by="1 day"); data.frame(bdays=days[!secdb.isHoliday(days)])
  } else {
    stop("Unknown method ",  method)
  }

  
  percent
}


######################################################################
# Do some tests to see if the move makes sense ...
# @param qq the list with data.frames to move
# @return a boolean, TRUE if you should proceed and update the calculators,
#    FALSE if you need to stop.
#
.does_move_make_sense <- function( qq ) 
{
  # look at the first month peak hub position to move.
  # if greater than 200 MW or 0 MW, something is wrong!
  firstMonth <- min(qq$hub$month)
  value <- subset(qq$hub, month == firstMonth & bucket == "5X16")$toMove/720

  res <- TRUE
  if (abs(value) > 200) {     
    res <- FALSE
  }

  if (abs(sum(qq$hub$moved)) < 10) {  # if you don't find any positions 
    res <- FALSE
  }

  
  return(res)
}


######################################################################
# Get the default percentages from the waterfall for the next 3 years
# @return a list()
#
.get_default_percentage <- function() 
{
  list("cash"   = c(1.0, 1.0),
       "cash+1" = c(2/3, 1.0),
       "cash+2" = c(1/3, 2/3),
       "cash+3" = c(0,   1/3)
       )
}


######################################################################
# Get the booked positions and the moved positions, and calculate
# what needs to get moved.
# @param asOfDate current date
# @return data.frame with columns 
#
.get_positions_move <- function( asOfDate=Sys.Date() )
{
  books <- c("MYSTC89M", "FORERIVM", "NPKLEEN", "MYSTIC7M",
             "NSWHITE2", "NSECOME", "XWYMAN4", "NSCMPPPA", 
             "NEPGDMOD", "NEPGDMD2")

  qq <- ..get_positions_prism(books, asOfDate)
  if (nrow(qq) <= 1) {
    rLog("Did not return any positions ... trying again one day earlier")
    qq <- ..get_positions_prism(books, as.Date(secdb.dateRuleApply(asOfDate, "-1b")))
  }
  #qq=subset(qq, month==as.Date("2013-06-01") & mkt == "PWX")
  #percent = data.frame(month=as.Date("2013-06-01"), percent=1.0)
  
  percent <- .ratable_percent( asOfDate )
  qq <- subset(qq, month <= max(percent$month) & month >= min(percent$month))
  
  hub   <- .get_positions_move_Hub(qq, percent)
  nodes <- .get_positions_move_Nodes(qq, percent)
  gas   <- .get_positions_move_Gas(qq, percent)

  
  list(hub=hub, nodes=nodes, gas=gas)
}


######################################################################
# 
#
..get_positions_prism <- function(books, posDate)
{
  rLog("Getting positions for ", format(posDate))

  try(QQ <- get.positions.from.blackbird( asOfDate=posDate, books=books,
    fix=TRUE, service=NULL))  # service=ENERGY does not pick the AGT gas in NEPGDMOD
  if ("try-error" %in% class(QQ) || nrow(QQ) == 0) {
    rLog("Failed to get positions for ",  format(posDate))
    posDate <- secdb.dateRuleApply(posDate, "-1b")
    rLog("Try to get positions for ", format(posDate))
    try(QQ <- get.positions.from.blackbird( asOfDate=posDate, books=books,
      fix=TRUE, service=NULL))    
  }
  unique(QQ[,c("trading.book", "curve", "mkt", "service")])
  
  QQQ <- subset(QQ, mkt %in% c("PWX", "NG") &
               contract.dt >= nextMonth(asOfDate))
  QQQ <- QQQ[!grepl(" UCAP ", QQQ$curve),]
  aux <- cast(QQQ, mkt + trading.book + contract.dt + bucket + delivery.point + service ~ .,
                sum, value="delta")
  names(aux)[ncol(aux)] <- "value"
  colnames(aux)[which(colnames(aux)=="contract.dt")] <- "month"

  aux
}


######################################################################
# Get the booked positions and the moved positions, and calculate
# what needs to get moved.
# @param qq, a data.frame with positions by month, bucket, location
#   see .get_positions_move
# @return data.frame with columns 
#
.get_positions_move_Gas <- function( qq, percent )
{
  qq <- subset(qq, mkt=="NG")
  qqBooked <- subset(qq, !(trading.book %in% c("NEPGDMOD", "NEPGDMD2") ))
  ngBooked <- cast(qqBooked, month + bucket ~ ., sum, value="value")
  colnames(ngBooked)[3] <- "booked"
  ngBooked <- merge(ngBooked, percent, by="month")
  ngBooked$target <- ngBooked$booked * ngBooked$percent

  ngMoved  <- subset(qq, trading.book %in% c("NEPGDMOD", "NEPGDMD2"))
  ngMoved <- cast(ngMoved, month + bucket ~ ., sum, value="value")
  colnames(ngMoved)[3] <- "moved"
  
  res <- merge(ngBooked, ngMoved, by=c("month", "bucket"), all.x=TRUE)
  res$moved[is.na(res$moved)] <- 0
  res$toMove <- res$target + res$moved

  
  res
}


######################################################################
# Get the booked positions and the moved positions, and calculate
# what needs to get moved.
# @param qq, a data.frame with positions by month, bucket, location
#   see .get_positions_move
# @return data.frame with columns 
#
.get_positions_move_Hub <- function( qq, percent )
{
  qq <- subset(qq, mkt=="PWX" & service == "ENERGY")
  qqBooked <- subset(qq, !(trading.book  %in% c("NEPGDMOD", "NEPGDMD2")))
  hubBooked <- cast(qqBooked, month + bucket ~ ., sum, value="value")
  colnames(hubBooked)[3] <- "booked"
  hubBooked <- merge(hubBooked, percent, by="month")
  hubBooked$target <- hubBooked$booked * hubBooked$percent

  qqMoved  <- subset(qq, trading.book  %in% c("NEPGDMOD", "NEPGDMD2") )
  hubMoved <- cast(qqMoved, month + bucket ~ ., sum, value="value")
  colnames(hubMoved)[3] <- "moved"
  
  res <- merge(hubBooked, hubMoved, by=c("month", "bucket"), all.x=TRUE)
  res$moved[is.na(res$moved)] <- 0
  res$toMove <- res$target + res$moved

  
  res
}



######################################################################
# Get the booked positions and the moved positions, and calculate
# what needs to get moved.
# @param qq, a data.frame with positions by month, bucket, location
#   see .get_positions_move
# @return data.frame with columns 
#
.get_positions_move_Nodes <- function( qq, percent )
{
  qq <- subset(qq, mkt=="PWX" & service == "ENERGY")
  qq$mkt <- NULL
  qqBooked <- subset(qq, !(trading.book %in% c("NEPGDMOD", "NEPGDMD2")) )
  colnames(qqBooked)[ncol(qqBooked)] <- "booked"
  qqBooked$trading.book <- NULL

  # fill with 0 missing bucket/delivery.point/month otherwise the merge with
  # qqMoved will not work well
  qqBooked <- merge(qqBooked,
                    expand.grid(bucket=c("2X16H", "5X16", "7X8"),
                                month=percent$month,
                                delivery.point=unique(qqBooked$delivery.point)), all=TRUE)
  qqBooked$booked[is.na(qqBooked$booked)] <- 0  
  qqBooked <- merge(qqBooked, percent, by=c("month"), all=TRUE)
  qqBooked$target <- qqBooked$booked * qqBooked$percent
  qqBooked <- qqBooked[order(qqBooked$delivery.point, qqBooked$bucket, qqBooked$month),]
  qqBooked$service <- NULL

  qqMoved  <- subset(qq, trading.book  %in% c("NEPGDMOD", "NEPGDMD2") )
  qqMoved$trading.book <- NULL
  colnames(qqMoved)[ncol(qqMoved)] <- "moved"
  qqMoved <- cast(qqMoved, delivery.point + month + bucket ~ ., sum,
                  fill=0, value="moved")
  colnames(qqMoved)[ncol(qqMoved)] <- "moved"
  qqMoved <- subset(qqMoved, !(delivery.point %in% c("HUB", "HUB DA")))
  
  res <- merge(qqBooked, qqMoved, by=c("month", "bucket",
    "delivery.point"), all=TRUE)
  res$moved[is.na(res$moved)] <- 0
  res$toMove <- res$target + res$moved
  res <- cbind(mkt="PWX", res)  # put this back

  res <- subset(res, delivery.point %in%
     c("CT DA", "EDGAR.40327 DA", "MYSTIC.MYS7 DA", "MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA"))
  
  res
}


######################################################################
# Which months to move
# For Nepool, monthsFromCash=3, for PJM East monthsFromCash=2 
#
.months_to_move <- function(asOfDate=Sys.Date(), monthsFromCash=3)
{  
  promptYear <- as.numeric(format(asOfDate, "%Y")) + 1
  prompt2 <- seq(currentMonth(asOfDate), by="1 month", length.out=4)[monthsFromCash+1]
  months  <- seq(prompt2,
    as.Date(paste(promptYear+2, "-12-01", sep="")), by="1 month")

  
  months
}



######################################################################
# Get the ratable table percentages of a given asOfDate for the next
# 3 years.
# @param limits a list with the percentages.
# @param method a string, either "calendar" or "business".  How to do the
#    calculation of percentage increases.
# @param monthsFromCash the argument from .months_to_move
# @return data.frame with columns ("month", "percent") 
#
.ratable_percent <- function( asOfDate=Sys.Date(),
                              limits=.get_default_percentage(),
                              method="calendar",
                              monthsFromCash=3 )
{
  currentYear <- as.numeric(format(asOfDate, "%Y")) 
  promptYear <- as.numeric(format(asOfDate, "%Y")) + 1
  boy <- as.Date(paste(promptYear, "-01-01", sep=""))
  
  years <- currentYear:(promptYear+2) 
  per <- ldply(years, function(year, asOfDate) {
    data.frame(year = year,
               percent = .calc_percent_yr(year, asOfDate, limits=limits, method=method))
  }, asOfDate)
  
  
  res <- data.frame(month = .months_to_move(asOfDate, monthsFromCash=monthsFromCash))
  res$year <- as.numeric(format(res$month, "%Y"))
  res <- merge(res, per, by="year")
  res$year <- NULL
  
  res
}


######################################################################
# Check if a day needs to be skipped.  
# @param asOfDate
# @param filename the path to the file that keeps track of days to skip
# @return a boolean, TRUE if you need to skip the day.
#
.skip_day <- function(asOfDate,
   filename="S:/All/Structured Risk/NEPOOL/Temporary/IBTs/skip_days_nepool.txt")
{
  aux <- read.csv(filename, comment.char="#")
  aux$dayToSkip <- as.Date(gsub(" ", "", aux$dayToSkip))
  
  asOfDate %in% aux$dayToSkip
}


######################################################################
# save to xlsx for archiving
#
.to_xlsx <- function( qq, asOfDate, prefix="")
{
  require(xlsx)

  fname <- paste("S:/All/Structured Risk/NEPOOL/Temporary/IBTs/",
                 "pgd_move_", format(asOfDate, "%Y%m%d"), prefix, ".xlsx", sep="")
  wb <- createWorkbook()
  sheet <- createSheet(wb, sheetName="Hub")
  aux <- qq$hub
  aux$month <- format(aux$month, "%Y%m%d")  # for jenny
  addDataFrame(aux, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="Nodes")
  aux <- qq$nodes
  aux$month <- format(aux$month, "%Y%m%d")
  addDataFrame(aux, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="Gas")
  aux <- qq$gas
  aux$month <- format(aux$month, "%Y%m%d")
  addDataFrame(aux, sheet, row.names=FALSE)
  
  saveWorkbook(wb, fname)
  rLog("Wrote file", fname)
  
  return(0)
}


######################################################################
# update the calculators
#
.update_calculators <- function( qq )
{
  comments <- paste("Updated on", format(Sys.Date(), "%Y%m%d"))
  
  #-----------------------------------------------------------------------
  # the BASIS calculator
  # a trade from NEPGDIB2 -> CONGIBT3
  #
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  delivPt <- c("EDGAR.40327 DA", "MYSTIC.MYS7 DA", "MYSTIC.MYS8 DA",
               "MYSTIC.MYS9 DA", "CT DA", "HUB DA")
  PP <- .pull_marks(asOfDate, delivPt, startDate=min(qq$nodes$month),
                    endDate=max(qq$nodes$month))
  PPP <- cast(PP, month ~ location + bucket, I, fill=0, value="value")

  # calculate the total Hub position
  qHub <- cast(qq$nodes, month + bucket ~ ., function(x){-sum(x)},
               fill=0, value="toMove")
  colnames(qHub)[3] <- "toMove"
  qHub$delivery.point <- "HUB DA"
  # add the hours and calculate the qty in MW
  QQ  <- rbind(qq$nodes[,c("month", "delivery.point", "bucket", "toMove")], qHub)
  # you may not have all the 3 buckets, for example on My7 7x8 ...
  aux <- cast(QQ, month + delivery.point ~ bucket, I, fill=0, value="toMove")
  QQ <- melt(aux, id=1:2)
  rownames(QQ) <- NULL
  colnames(QQ)[which(colnames(QQ)=="value")] <- "toMove"
  
  HRS <- PM:::ntplot.bucketHours(c("5X16", "2X16H", "7X8"), startDate=min(qq$nodes$month),
                            endDate=max(qq$nodes$month))
  colnames(HRS)[2:3] <- c("month", "hrs")
  QQ <- merge(QQ, HRS, by=c("month", "bucket"))
  QQ$mw <- QQ$toMove/QQ$hrs
  QQQ <- cast(QQ, month ~ delivery.point + bucket, I, fill=0, value="mw")

  # update the calc
  PM:::secdb.customizeCalculator("Calc ECFD pgd basis move      0", quantity=QQQ, fixPrice=PPP)
  lib <- "_lib elec load parse addin fns"
  func <- "LoadParse::CalcComment"
  secdb.invoke(lib, func, "Calc ECFD pgd basis move      0", comments)

  
  #-----------------------------------------------------------------------
  # the HUB calculator
  # a trade from NEPGDMD2 -> BGMOD2
  #
  PPhub <- subset(PP, location=="HUB DA")
  PPP <- cast(PPhub, month ~ location + bucket, I, fill=0, value="value")

  qHub <- cast(qq$hub, month + bucket ~ ., I, fill=0, value="toMove")
  colnames(qHub)[3] <- "toMove"
  QQ <- merge(qHub, HRS)
  QQ$mw <- QQ$toMove/QQ$hrs
  QQ$delivery.point <- "HUB DA"
  QQQ <- cast(QQ, month ~ delivery.point + bucket, I, fill=0, value="mw")

  # update the calc
  PM:::secdb.customizeCalculator("Calc ECFD pgd hub move        0",
                                 quantity=QQQ, fixPrice=PPP)
  lib <- "_lib elec load parse addin fns"
  func <- "LoadParse::CalcComment"
  secdb.invoke(lib, func, "Calc ECFD pgd hub move        0", comments)
  
  
  #-----------------------------------------------------------------------
  # the Gas calculator
  # a trade from NEPGDMD2 -> BGMOD2
  #
  .update_calculator_gas(qq)
  lib <- "_lib elec load parse addin fns"
  func <- "LoadParse::CalcComment"
  secdb.invoke(lib, func, "Calc FCFD pgd gas move        0", comments)

  
}

######################################################################
# update the calculators
#
.update_calculator_gas <- function(qq)
{
  calcs <- calculator.list("CPS Trading Group", "E47187")
  cname <- "Calc FCFD pgd gas move        0"

  rLog("\n\nUpdate the gas calculator ...")
  
  # always clear the custom details before writing
  lib  <- "_lib commod calc utils"
  func <- "CalcUtils::ClearCustomDetails"
  s <- secdb.getValueType(cname, "Calc Security")
  res  <- secdb.invoke(lib, func, s)  # works now, may work with fullCalculatorName too  
  rLog("Cleared custom details")

  
  lib  <- "_LIB R Bridge"
  # always change start/end date of the calculator to match the inputs 
  func <- "CalcUtils::CustomizeStartEnd"
  startDt <- format(min(qq$gas$month), "%d%b%y")
  endDt   <- format(nextMonth(max(qq$gas$month))-1, "%d%b%y")
  res <- secdb.invoke(lib, func, cname, startDt, endDt)
  if (res != "Successful update"){
    stop("Cannot set calculator Start/End date.")
  } else {
    rLog("Successfuly changed start/end date.")
  }
  
  func <- "R Bridge::CustomizeCalc"
  # set the qty
  data <- NULL
  for (r in 1:nrow(qq$gas)) {
    noDays <- as.numeric(nextMonth(qq$gas$month[r]) - qq$gas$month[r])
    data[[r]] <- list(qq$gas$month[r], qq$gas$toMove[r]/noDays)
  }
  res <- secdb.invoke(lib, func, "CPS Trading Group", "E47187", cname,
        "Fuel", "ALGCG/GAS-DLY MEAN", "7x24", "Quantity", data, TRUE)

  months <- seq(min(qq$gas$month), max(qq$gas$month), by="1 month")
  pp <- PM:::secdb.getCurrentMarks("Commod NG ALGCG GAS-DLY MEAN", months)
  data <- NULL
  for (r in 1:nrow(qq$gas))
    data[[r]] <- list(qq$gas$month[r], pp$value[r])
  res <- secdb.invoke(lib, func, "CPS Trading Group", "E47187", cname,
        "Price", "ALGCG/GAS-DLY MEAN", 1, "Fixed Price", data, TRUE)
  
}




######################################################################
######################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")

  asOfDate <- Sys.Date()
  #.calc_percent_yr(2014)
  # percent <- .ratable_percent( asOfDate )


  qq <- .get_positions_move( asOfDate )
  .update_calculators(qq)
  
  
  
  .to_xlsx(qq, asOfDate, prefix="_test")

  # the basis trade:
  # 1) profitCenter: BGMOD customer: NEPGDMOD
  # 2) profitCenter: CONGIBT customer: NEPGDIBT


  q2 <- subset(qq, trading.book %in% c("NEPGDMOD", "NEPGDMD2") &
             delivery.point %in% c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA", "MYSTIC.MYS7 DA"))
  
  sum(subset(qqMoved,
    delivery.point %in% c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA", "MYSTIC.MYS7 DA"))$moved)
  sum(subset(res,
    delivery.point %in% c("MYSTIC.MYS8 DA", "MYSTIC.MYS9 DA", "MYSTIC.MYS7 DA"))$booked)



  aggregate(res[,c("booked", "target", "moved", "toMove")],
            list(delivery.point=res$delivery.point), sum)
  
  
  
}


######################################################################
# Some tests for the calculation of the ratable percent to move
#
.test.ratable_percent <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")
  asOfDate <- as.Date("2013-12-11")
  .ratable_percent( asOfDate )

  
  asOfDate <- as.Date("2014-06-30")
  .ratable_percent( asOfDate )
  
  
  asOfDate <- as.Date("2015-01-01")
  #.ratable_percent( asOfDate )
  .ratable_percent( asOfDate, method="business")


  days <- c(as.Date("2012-12-31"),
            seq(as.Date("2013-01-01"), by="1 month", length.out=12),
            as.Date("2013-12-31"))
  
  

  
}
