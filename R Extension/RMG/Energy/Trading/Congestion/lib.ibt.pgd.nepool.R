# Deal with the generation IBTs for NEPOOL. 
# Use the simcube positions.  Keep general library functions in lib.ibt.pgd.R
#
# .format_positions_move
# .get_positions_move
# ..get_positions_prism
# ..map_commod_gas
# .skip_day               - if you need to skip a day
# .to_xlsx
# .update_calculators
# .update_calculator_gas
#


######################################################################
# Calculate what needs to get moved.
# @param gen are the positions in the Gen portfolio, a list with
#   2 data.frames one for pwr, one for gas.
# @param moved are the positions already moved 
# @posDate the day you pull 
# @return a list with positions to move
#
.format_positions_move <- function( gen, moved, posDate )
{
  # what gets moved to PM.  PM always gets the default percentages
  ## percentPM <- .ratable_percent( posDate,
  ##                                limits=.get_default_percentage(),
  ##                                method="calendar",
  ##                                monthsFromCash=2)
  ## toPM <- list()
  ## toPM$pwr <- merge(gen$pwr, percentPM, by="month")
  ## toPM$pwr$target <- toPM$pwr$booked * toPM$pwr$percent     
  ## toPM$pwr <- merge(toPM$pwr, moved$PM$pwr) # add the moved part
  ## toPM$pwr$toMove <-  toPM$pwr$target +  toPM$pwr$moved
  ## head(toPM$pwr)
  
  ## toPM$gas <- merge(gen$gas, percentPM, by="month")
  ## toPM$gas$target <- toPM$gas$booked * toPM$gas$percent     
  ## toPM$gas <- merge(toPM$gas, moved$PM$gas) # add the moved part
  ## toPM$gas$toMove <-  toPM$gas$target +  toPM$gas$moved
  ## head(toPM$gas)
  
 
  # what gets moved to WT the custom percentages
  percentWT <- .ratable_percent( posDate,
    limits=.get_default_percentage(),
    method="calendar",
    monthsFromCash=2)
  toWT <- list()
  toWT$hub <- merge(gen$hub, percentWT, by="month")
  toWT$hub$target <- toWT$hub$booked * toWT$hub$percent     
  toWT$hub <- merge(toWT$hub, moved$WT$hub) # add the moved part
  toWT$hub$toMove <-  toWT$hub$target +  toWT$hub$moved
  head(toWT$hub)
  
  toWT$gas <- merge(gen$gas, percentWT, by="month")
  toWT$gas$target <- toWT$gas$booked * toWT$gas$percent     
  toWT$gas <- merge(toWT$gas, moved$WT$gas) # add the moved part
  toWT$gas$toMove <-  toWT$gas$target +  toWT$gas$moved
  head(toWT$gas)
 
  toWT$basis <- merge(gen$basis, moved$WT$basis, all=TRUE) # add the moved part
  toWT$basis <- merge(toWT$basis, percentWT, by="month")
  toWT$basis$booked[is.na(toWT$basis$booked)] <- 0 
  toWT$basis$target <- toWT$basis$booked * toWT$basis$percent     
  toWT$basis$toMove <-  toWT$basis$target +  toWT$basis$moved
  toWT$basis <- toWT$basis[,c("month", "bucket", "deliverypoint", "booked",
                              "percent", "target", "moved", "toMove")]
  toWT$basis$toMove[which(toWT$basis$deliverypoint == "EDGAR.40327")] <- 0  # enforce it
  head(toWT$basis)
  
  list(toPM=NULL, toWT=toWT)
}


######################################################################
# Get the power locations moved for basis 
#
.get_moved_basis_locations <- function() 
{
   c("EDGAR.40327",
     "CT",               # Kleen
     "MYSTIC.MYS7", "MYSTIC.MYS8", "MYSTIC.MYS9")
}


######################################################################
# only move this gas location
.get_moved_gas_locations <- function() 
{
   c("NG ALGCG GAS-DLY MEAN")
}


######################################################################
# Calculate the positions to move
# @param posDate a date for the positions.
# @param monthsFromCash how many months to skip from cash month when moving 
# @return data.frame with columns 
#
.get_positions_move_nepool <- function( posDate, monthsFromCash=3 )
{
  months <- .months_to_move(posDate, monthsFromCash=monthsFromCash)

  # get the gen position from SimCube, power and gas
  gen <- .get_positions_gen( posDate, months )

  # get positions moved from SimCube, power and gas
  moved <- .get_positions_moved( posDate, months )
  
  # prepare positions to move
  qq <- .format_positions_move(gen, moved, posDate)

  
  qq
}
  


######################################################################
# Get the booked positions
# @param posDate a date for the positions.
# @param months a vector of months 
# @return a list of data.frames
#
.get_positions_gen <- function( posDate, months)
{
  movedBasisLocations <- .get_moved_basis_locations()
  movedGasLocations   <- .get_moved_gas_locations()
  
  books <- c("MYSTC89M", "NPKLEEN", "MYSTIC7M",
             "NSWHITE2", 
             "XWYMAN4",  "XFRMGHM1", "XFRMGHM2", "XFRMGHM3",
             "XLSTREET", "XMEDWAY1", "XMEDWAY2", "XMEDWAY3") 
  
  qq <- get_positions_simcube(posDate, books=books)

  qq <- subset(qq, month %in% months)
  # Kleen is not mapped at CT!
  qq$deliverypoint[qq$deliverypoint == "KLEEN.14614"] <- "CT" 
  
  #---------------------------------------------------------
  # for hub you move all the power in the books, including the marginal loss!
  hub <- cast(qq, month + bucket ~ ., sum, na.rm=TRUE,
              fill=0, value="pds_delta", subset=!is.na(qq$pds_delta))
  colnames(hub)[ncol(hub)] <- "booked"
  head(hub)
  # fill with 0's the missing bucket/months
  hub <- merge(hub,
               expand.grid(bucket=c("2X16H", "5X16", "7X8"),
                           month=months),
                    all=TRUE)
  hub$booked[is.na(hub$booked)] <- 0
  head(hub)
  
  #---------------------------------------------------------
  basis <- cast(qq, month + bucket + deliverypoint ~ ., sum, na.rm=TRUE,
              fill=0, value="pds_delta", subset=!is.na(qq$pds_delta))
  colnames(basis)[ncol(basis)] <- "booked"
  basis <- subset(basis, deliverypoint %in% movedBasisLocations )
  head(basis)
  
  #---------------------------------------------------------
  aux <- subset(qq, grepl("^NG ", qq$deliverypoint))
  aux$deliverypoint <- "NG ALGCG GAS-DLY MEAN"
  gas <- cast(aux, month + deliverypoint ~ ., sum, na.rm=TRUE,
                 fill=0, value="delta_mmbtu", subset=!is.na(qq$delta_mmbtu))
  gas <- subset(gas, !is.na(month))  #& deliverypoint %in% movedGasLocations)
  colnames(gas)[ncol(gas)] <- "booked"
  head(gas)
 
  
  list(hub=hub, gas=gas, basis=basis)  
}


######################################################################
# Get the moved positions
# @param posDate a date for the positions.
# @param months a vector of months 
# @return a list of data.frames
#
.get_positions_moved <- function( posDate, months )
{  
  books <- c("NEPGDMOD", "NEPGDMD2", "NEPGDMD3") # added nepgdmd3 on 7/10
  
  qq <- get_positions_simcube(posDate, books=books)
  qq <- subset(qq, month %in% months)
  ## cast(qq, deliverypoint ~ bucket, function(x)round(sum(x)),
  ##   fill=0, subset=month == as.Date("2014-09-01"), value="pds_delta")
 
  #---------------------------------------------------------
  hub <- cast(qq, month + bucket ~ ., sum, na.rm=TRUE,
              fill=0, value="pds_delta", subset=!is.na(qq$pds_delta))
  colnames(hub)[ncol(hub)] <- "moved"
  head(hub)
  # fill with 0's the missing bucket/months
  hub <- merge(hub,
               expand.grid(bucket=c("2X16H", "5X16", "7X8"),
                           month=months),
                    all=TRUE)
  hub$moved[is.na(hub$moved)] <- 0

  #---------------------------------------------------------
  basis <- cast(qq, month + bucket + deliverypoint ~ ., sum, na.rm=TRUE,
              fill=0, value="pds_delta",
              subset=!is.na(qq$pds_delta) & deliverypoint != "HUB")
  colnames(basis)[ncol(basis)] <- "moved"
  head(basis)
  # fill with 0's the missing bucket/months/deliverypoint
  basis <- merge(basis,
               expand.grid(bucket = c("2X16H", "5X16", "7X8"),
                           month = months,
                           deliverypoint = unique(basis$deliverypoint)),
                    all=TRUE)
  basis$moved[is.na(basis$moved)] <- 0
  #cast(basis, deliverypoint ~ bucket, I, fill=0, subset=month == as.Date("2014-09-01"))
 

  
  #---------------------------------------------------------
  # Gas positions.  There are some residual positions at NG Exchange, TETZM3, FERC,
  # etc.  Ignore them.  
  aux <- subset(qq, grepl("^NG ", qq$deliverypoint)
                & deliverypoint == "NG ALGCG GAS-DLY MEAN")
  gas <- cast(aux, month + deliverypoint ~ ., sum, na.rm=TRUE,
                 fill=0, value="delta_mmbtu", subset=!is.na(qq$delta_mmbtu))
  gas <- subset(gas, !is.na(month))
  colnames(gas)[ncol(gas)] <- "moved"
  head(gas)
 
  
  list(WT = list(hub=hub, gas=gas, basis=basis))
}


######################################################################
# Map commod gas locations to locations used in the calculator
# @param x is a data.frame with column "deliverypoint"
# 
..map_commod_gas <- function()
{
  data.frame(
    deliverypoint=c("NG ALGCG GAS-DLY MEAN"),
    location =    c("ALGCG/GAS-DLY MEAN"),
    tsdb     =    c("PRC_NG_ALGCG_GDM")
  )
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
  aux <- qq$toWT$hub
  aux$month <- format(aux$month, "%Y%m%d")  # for jenny
  addDataFrame(aux, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="Nodes")
  aux <- qq$toWT$basis
  aux$month <- format(aux$month, "%Y%m%d")
  addDataFrame(aux, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="Gas")
  aux <- qq$toWT$gas
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
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  delivPt <- c("EDGAR.40327 DA", "MYSTIC.MYS7 DA", "MYSTIC.MYS8 DA",
               "MYSTIC.MYS9 DA", "CT DA", "HUB DA")
  PP <- .pull_marks(asOfDate, delivPt, startDate=min(qq$toWT$basis$month),
                    endDate=max(qq$toWT$basis$month))
  PPP <- cast(PP, month ~ location + bucket, I, fill=0, value="value")

  # calculate the total Hub position
  qHub <- cast(qq$toWT$basis, month + bucket ~ ., function(x){-sum(x)},
               fill=0, value="toMove")
  colnames(qHub)[3] <- "toMove"
  qHub$deliverypoint <- "HUB DA"
  QQ <- qq$toWT$basis[,c("month", "deliverypoint", "bucket", "toMove")]
  QQ$deliverypoint <- paste(QQ$deliverypoint, "DA")
  # add the hours and calculate the qty in MW
  QQ  <- rbind(QQ, qHub)
  
  aux <- cast(QQ, month + deliverypoint ~ bucket, I, fill=0, value="toMove")
  QQ <- melt(aux, id=1:2)
  rownames(QQ) <- NULL
  colnames(QQ)[which(colnames(QQ)=="value")] <- "toMove"
  
  HRS <- PM:::ntplot.bucketHours(c("5X16", "2X16H", "7X8"), startDate=min(qq$toWT$basis$month),
                            endDate=max(qq$toWT$basis$month))
  colnames(HRS)[2:3] <- c("month", "hrs")
  QQ <- merge(QQ, HRS, by=c("month", "bucket"))
  QQ$mw <- QQ$toMove/QQ$hrs
  QQQ <- cast(QQ, month ~ deliverypoint + bucket, I, fill=0, value="mw")

  # update the calc
  PM:::secdb.customizeCalculator("Calc ECFD pgd basis move      0", quantity=QQQ, fixPrice=PPP)
  lib <- "_lib elec load parse addin fns"
  func <- "LoadParse::CalcComment"
  secdb.invoke(lib, func, "Calc ECFD pgd basis move      0", comments)

  
  #-----------------------------------------------------------------------
  # the HUB calculator 
  PPhub <- subset(PP, location=="HUB DA")
  PPP <- cast(PPhub, month ~ location + bucket, I, fill=0, value="value")

  qHub <- cast(qq$toWT$hub, month + bucket ~ ., I, fill=0, value="toMove")
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
  map <- ..map_commod_gas()
  aux <- merge(qq$toWT$gas, map)
  daysInMonth <- ldply(sort(unique(aux$month)), function(month){
    data.frame(month=month, days=as.numeric(nextMonth(month) - month))
  })

  PP <- ldply(unique(aux$tsdb), function(curve) {
    cbind(tsdb=curve, tsdb.futCurve( curveName=curve, asOfDate=posDate, 
         startDate=min(aux$month), endDate=nextMonth(max(aux$month)) ))
  })
  colnames(PP)[2] <- "month"
  PP <- merge(PP, map, by="tsdb")  
  PP$bucket <- "7X24"
  PPP <- cast(PP, month ~ location + bucket, I, fill=0)

  aux <- merge(aux, daysInMonth)
  aux$mmbtu.per.day <- aux$toMove/aux$days
  aux$bucket <- "7X24"
  QQQ_toWT <- cast(aux, month ~ location + bucket, I, fill=0, value="mmbtu.per.day" )

  secdb.customizeCalculator( "Calc FCFD pgd gas move        0",
     quantity=QQQ_toWT, fixPrice=PPP, comments=comments)
  
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

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.nepool.R")

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
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.nepool.R")
  
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
