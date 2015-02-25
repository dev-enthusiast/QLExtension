# Move the ibt positions for PJM
#
# .format_positions_move
# .get_positions_gen
# .get_positions_moved
# get_positions_move_pjm
# .read_percentages_xlsx
# to_xlsx
# update_calculators

######################################################################
# Fill missing zonal power positions with 0's so that the calculator
# doesn't get upset.
# @param x a data.frame with colnames ("month", "bucket", "zone", "booked")
# @param posDate
# @region name
# @return a data.frame with rows extended
#
.fill_missing_zones <- function(x, posDate, region="PJM East" )
{

  if (toupper(region) == "PJM EAST" ) {
     locations <- c("AECO", "APS", "BGE", "EASTERN HUB", "JCPL", "KEYCON",
                    "METED", "PECO", "PENELEC", "PPL", "PSEG", "WESTERN HUB")
  }
  
  uLocations <- unique(x$zone)
  missingZones <- setdiff(locations, uLocations)
  if (length(missingZones) > 0) {
     extra <- expand.grid(bucket = c("2X16H", "5X16", "7X8"),
                          month  = .months_to_move(posDate, monthsFromCash=2),
                          zone   = missingZones, 
                          booked = 0)
  }
  y <- rbind(x, extra)
  y <- y[order(y$month, y$bucket, y$zone),]
  
  y
}


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
  percentPM <- .ratable_percent( posDate,
                                 limits=.read_percentages_xlsx("PJMEast_toPM"),
                                 method="business",
                                 monthsFromCash=2)
  toPM <- list()
  toPM$pwr <- merge(gen$pwr, percentPM, by="month")
  toPM$pwr$target <- toPM$pwr$booked * toPM$pwr$percent     
  toPM$pwr <- merge(toPM$pwr, moved$PM$pwr) # add the moved part
  toPM$pwr$toMove <-  toPM$pwr$target +  toPM$pwr$moved
  head(toPM$pwr)
  
  toPM$gas <- merge(gen$gas, percentPM, by="month")
  toPM$gas$target <- toPM$gas$booked * toPM$gas$percent     
  toPM$gas <- merge(toPM$gas, moved$PM$gas) # add the moved part
  toPM$gas$toMove <-  toPM$gas$target +  toPM$gas$moved
  head(toPM$gas)
  
 
  # what gets moved to WT the custom percentages
  percentWT <- .ratable_percent( posDate,
    limits=.read_percentages_xlsx("PJMEast_toWT"),
    method="business",
    monthsFromCash=2)
  toWT <- list()
  toWT$pwr <- merge(gen$pwr, percentWT, by="month")
  toWT$pwr$target <- toWT$pwr$booked * toWT$pwr$percent     
  toWT$pwr <- merge(toWT$pwr, moved$WT$pwr) # add the moved part
  toWT$pwr$toMove <-  toWT$pwr$target +  toWT$pwr$moved
  head(toWT$pwr)
  
  toWT$gas <- merge(gen$gas, percentWT, by="month")
  toWT$gas$target <- toWT$gas$booked * toWT$gas$percent     
  toWT$gas <- merge(toWT$gas, moved$WT$gas) # add the moved part
  toWT$gas$toMove <-  toWT$gas$target +  toWT$gas$moved
  head(toWT$gas)
 
  
  list(toPM=toPM, toWT=toWT)
}


######################################################################
# Get moved positions PJM, power and gas ...
# @param posDate the date you get the positions
# @param region=c("PJM East", "PJM West")
# 
.get_positions_gen <- function( posDate, region="PJM East" )
{
  months <- .months_to_move(posDate, monthsFromCash=2)
  
  qq <- get_positions_simcube(posDate, glt="Generation",
                              region=region)
  qq <- subset(qq, month %in% months & tradetype=="RATABLE GEN")
  #subset(qq, zone == "BGE" & month == as.Date("2015-02-01") & bucket == "5X16")
  
  # power ---------------------------------------------------------
  pwrBooked <- cast(qq, month + bucket + zone ~ ., sum, na.rm=TRUE,
                 fill=0, value="pds_delta", subset=!is.na(qq$pds_delta))
  colnames(pwrBooked)[ncol(pwrBooked)] <- "booked"
  pwrBooked <- .fill_missing_zones(pwrBooked, posDate, region=region)
  head(pwrBooked)
  #subset(pwrBooked, zone == "BGE" & month == as.Date("2015-02-01"))
  
  # fill with 0 missing bucket/delivery.point/month otherwise the merge with
  # qqMoved will not work well
  pwrBooked <- merge(pwrBooked,
                    expand.grid(bucket=c("2X16H", "5X16", "7X8"),
                                month=months,
                                zone=unique(pwrBooked$zone)),
                    all=TRUE)
  pwrBooked$booked[is.na(pwrBooked$booked)] <- 0

  # gas ------------------------------------------------------------
  aux <- subset(qq, grepl("^NG ", qq$deliverypoint))
  # for now map this to TRAZN6 Non
  aux$deliverypoint[ aux$deliverypoint == "NG TGPZ4 CG PHYSICAL"] <- "NG TRAZN6 NON GD MEAN"
  
  gasBooked <- cast(aux, month + deliverypoint ~ ., sum, na.rm=TRUE,
                 fill=0, value="expected_mmbtu", subset=!is.na(qq$expected_mmbtu))
  gasBooked <- subset(gasBooked, !is.na(month))
  colnames(gasBooked)[ncol(gasBooked)] <- "booked"
  head(gasBooked)
  # fill with 0 missing bucket/delivery.point/month otherwise the merge with
  # qqMoved will not work well
  gasBooked <- merge(gasBooked,
                    expand.grid(month=months,
                                bucket="7X24",
                                deliverypoint=unique(gasBooked$deliverypoint)),
                    all=TRUE)
  gasBooked$booked[is.na(gasBooked$booked)] <- 0
  
  
  list(pwr=pwrBooked, gas=gasBooked)
}


######################################################################
# Get moved positions PJM, power and gas ... 
#
.get_positions_moved <- function( posDate )
{
  ## books <- c("PJEGNRAP",    # Gen position moved to PM
  ##            "PJEBBRAP",    # PM position moved to WT
  ##            "PJEWTRAP")    # WT position
             
  ## qqMoved <- get_positions_simcube(posDate, books=books)

  ## if (nrow(qqMoved) == 0) {
  ##   # this only valid when nothing has been moved.  not needed Day2!
  ##   rLog("Found NO position moved!")
  ##   pwr <- expand.grid(bucket = c("2X16H", "5X16", "7X8"),
  ##                      month  = .months_to_move(posDate, monthsFromCash=2),
  ##                      moved  = 0)
  ##   gas <- expand.grid(bucket = "7X24",
  ##                      month  = .months_to_move(posDate, monthsFromCash=2),
  ##                      moved  = 0)
  ##   PM <- list(pwr=pwr, gas=gas)
  ##   WT <- list(pwr=pwr, gas=gas)
  ## } else {

    
  #----------------------------- PM moved ----
  qqMoved <- get_positions_simcube(posDate, books=c("PJEGNRAP", "PJEGNRP2"))
  pwrMoved <- cast(qqMoved, month + bucket + zone ~ ., sum, na.rm=TRUE,
                   fill=0, value="pds_delta")#, subset=glttype=="Underlying")
  colnames(pwrMoved)[ncol(pwrMoved)] <- "moved"
  #subset(pwrMoved, zone == "KEYCON" & month == as.Date("2014-10-01"))
  #subset(pwrMoved, bucket=="5X16" & month == as.Date("2014-10-01"))
  #subset(gen$pwr, zone == "BGE" & month == as.Date("2015-01-01"))
  #subset(qqMoved, )

  
  aux <- subset(qqMoved, grepl("^NG ", qqMoved$deliverypoint))
  # for now map this to TRAZN6 Non
  aux$deliverypoint[ aux$deliverypoint == "NG TGPZ4 CG PHYSICAL"] <- "NG TRAZN6 NON GD MEAN"
  
  gasMoved <- cast(aux, month + deliverypoint ~ ., sum, na.rm=TRUE,
                    fill=0, value="expected_mmbtu", subset=!is.na(qqMoved$expected_mmbtu))
  gasMoved <- subset(gasMoved, !is.na(month))
  colnames(gasMoved)[ncol(gasMoved)] <- "moved"
  head(gasMoved)
  PM <- list(pwr=pwrMoved, gas=gasMoved)
  
  #----------------------------- WT moved ----
  qqMoved <- get_positions_simcube(posDate, books=c("PJEWTRAP", "PJEWTRP2"))
  pwrMoved <- cast(qqMoved, month + bucket + zone ~ ., sum, na.rm=TRUE,
                   fill=0, value="pds_delta")#, subset=glttype=="Underlying")
  colnames(pwrMoved)[ncol(pwrMoved)] <- "moved"
  pwrMoved$moved <- -pwrMoved$moved  # the convention
  #subset(pwrMoved, zone == "BGE" & month == as.Date("2015-01-01"))

  # the gas
  aux <- subset(qqMoved, grepl("^NG ", qqMoved$deliverypoint))
  # for now map this to TRAZN6 Non
  aux$deliverypoint[ aux$deliverypoint == "NG TGPZ4 CG PHYSICAL"] <- "NG TRAZN6 NON GD MEAN"
  
  gasMoved <- cast(aux, month + deliverypoint ~ ., sum, na.rm=TRUE,
                    fill=0, value="expected_mmbtu", subset=!is.na(qqMoved$expected_mmbtu))
  gasMoved <- subset(gasMoved, !is.na(month))
  colnames(gasMoved)[ncol(gasMoved)] <- "moved"
  gasMoved$moved <- -gasMoved$moved   # the convention
  head(gasMoved)
  WT <- list(pwr=pwrMoved, gas=gasMoved)
  #}                                     # end of if branch
  
  list(PM = PM, WT = WT)
}



######################################################################
# Get the booked positions and the moved positions, and calculate
# what needs to get moved.
# @param posDate the date you want the positions, an R Date.
# @return list with toPM, toWT data.frames
#
get_positions_move_pjm <- function( posDate )
{
  rLog("Pulling positions for day", format(posDate))
  
  # get the gen position from SimCube, power and gas
  gen <- .get_positions_gen( posDate, region="PJM East" )

  # get positions moved from SimCube, power and gas
  moved <- .get_positions_moved( posDate )
  
  # prepare positions to move
  qq <- .format_positions_move(gen, moved, posDate)

  # new requirement 3/27/2014
  qq$toPM$pwr$zone <- paste(qq$toPM$pwr$zone, "RT")   
  qq$toWT$pwr$zone <- paste(qq$toWT$pwr$zone, "RT")   
  
  qq  
}


######################################################################
# Map commod gas locations to locations used in the calculator
# @param x is a data.frame with column "deliverypoint"
# 
..map_commod_gas <- function()
{
  data.frame(
    deliverypoint=c("NG TETZM3 GAS-DLY MEAN",  "NG TRAZN6 NON GD MEAN"),
    location =    c("TETZM3/GAS-DLY MEAN",     "TRAZN6 NON/GD MEAN"),
    tsdb     =    c("PRC_NG_TETZM3_GDM",       "PRC_NG_TRAZN6_NON_GD_MEAN")
  )
}


######################################################################
# Read custom percentages from a spreadsheet (for transfer between PM and WT)
# @return a list, see: .get_default_percentage
#
.read_percentages_xlsx <- function(sheetname, filename=NULL )
{
  if (is.null(filename))
    filename <- paste("//CEG/CERShare/All/Structured Risk/NEPOOL/Temporary/IBTs/PJM_IBTs/",
                      "template_percentages.xlsx", sep="")

  aux <- read.xlsx2(filename, sheetName=sheetname, startRow=2, endRow=6)

  res <- dlply(aux, c("year"), function(x) {
    if (any(x[,2:3] > 1.000001) )
      stop("Cannot have percentages higher than 1!")
    as.numeric(x[,2:3])
  })
  attr(res, "split_labels") <- NULL
  attr(res, "split_type") <- NULL

  res
}



######################################################################
# Write a record of what positions you move to a spreadsheet.
#
to_xlsx <- function( qq, posDate, prefix="" )
{
  require(xlsx)

  wb <- createWorkbook()
  sheet <- createSheet(wb, sheetName="toPM_pwr")
  addDataFrame(qq$toPM$pwr, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="toPM_gas")
  addDataFrame(qq$toPM$gas, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="toWT_pwr")
  addDataFrame(qq$toWT$pwr, sheet, row.names=FALSE)

  sheet <- createSheet(wb, sheetName="toWT_gas")
  addDataFrame(qq$toWT$gas, sheet, row.names=FALSE)
  
  fname <- paste("S:/All/Structured Risk/NEPOOL/Temporary/IBTs/PJM_IBTs/",
    "pgd_move_", format(posDate, "%Y%m%d"), "_",  
    format(Sys.Date(), "%Y%m%d"), prefix, ".xlsx", sep="")
  saveWorkbook(wb, fname)
  rLog("Wrote file", fname)
}


######################################################################
# Update the calculators
# for Gen to PM:  PJEGNRAP sells to PJEBBRAP
# for PM to WT:   PJEBBRAP sells to PJEWTRAP
# @param qq is a list with "pwr" and "gas" data.frames
# @pricingDate should be the same as the position date
#
update_calculators <- function( qq, pricingDate )
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  comments <- paste("Updated on", format(Sys.time()))

  startDt <- min(qq$toPM$pwr$month)
  endDt   <- max(qq$toPM$pwr$month)
  HRS <- PM:::ntplot.bucketHours(c("5X16", "2X16H", "7X8"),
    startDate=startDt, endDate=endDt)
  colnames(HRS)[2:3] <- c("month", "hrs")
  
  #calculator.list("CPS Trading Group", "E63126")  # Flannery
  
  #-----------------------------------------------------------------------
  # Power from Gen to PM
  delivPt <- data.frame(mkt="PWJ", location=sort(unique(qq$toPM$pwr$zone)))
  PP <- .pull_marks(pricingDate, delivPt, startDate=startDt, endDate=endDt)
  PPP <- cast(PP, month ~ location + bucket, I, fill=0, value="value")

  QQ <- merge(qq$toPM$pwr, HRS, by=c("month", "bucket"))
  QQ$mw <- QQ$toMove/QQ$hrs
  QQQ_toPM <- cast(QQ, month ~ zone + bucket, I, fill=0, value="mw")

  secdb.customizeCalculator("Calc ECFD PJME Power Gen to   0",
    quantity=QQQ_toPM, fixPrice=PPP, comments=comments)

  # Power from PM to WT
  QQ <- merge(qq$toWT$pwr, HRS, by=c("month", "bucket"))
  QQ$mw <- QQ$toMove/QQ$hrs
  QQQ_fromPMtoWT <- cast(QQ, month ~ zone + bucket, I, fill=0, value="mw")
  
  secdb.customizeCalculator("Calc ECFD PJME Power PM to    0",
     quantity=QQQ_fromPMtoWT, fixPrice=PPP, comments=comments)

  
  
  #-----------------------------------------------------------------------
  # Gas from Gen to PM
  map <- ..map_commod_gas()
  aux <- merge(qq$toPM$gas, map)
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
  QQQ_toPM <- cast(aux, month ~ location + bucket, I, fill=0, value="mmbtu.per.day" )

  secdb.customizeCalculator( "Calc FCFD PJME Gas Gen to P   0",
     quantity=QQQ_toPM, fixPrice=PPP, comments=comments)

  # Gas from PM to WT
  aux <- merge(qq$toWT$gas, map)
  
  aux <- merge(aux, daysInMonth)
  aux$mmbtu.per.day <- aux$toMove/aux$days
  QQQ_fromPMtoWT <- cast(aux, month ~ location + bucket, I, fill=0,
                         value="mmbtu.per.day" )
  secdb.customizeCalculator( "Calc FCFD PJME Gas PM to WT   0",
     quantity=QQQ_fromPMtoWT, fixPrice=PPP, comments=comments)
  
  0
}




######################################################################
######################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  require(RODBC)
  require(PM)
  require(xlsx)
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.simcube.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ibt.pgd.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/PJM/lib.PJM.ibt.R")

  posDate <- as.Date("2014-04-21")

  
  percentPM <- .ratable_percent( posDate,
                                 limits=.read_percentages_xlsx("PJMEast_toPM"),
                                 method="business",
                                 monthsFromCash=2)

  
  .calc_percent_yr(2015, posDate, limits=limits, method=method)

  
  
  #asOfDate <- as.Date("2013-12-13")
  qq <- get_positions_move_pjm(asOfDate)

  update_calculators(qq)

  to_xlsx(qq, asOfDate, prefix="")

  


}


  ## PP <- secdb.getCurrentMarks(unique(paste("COMMOD", aux$deliverypoint)),
  ##                             sort(unique(aux$month)), expand=TRUE)
  ## colnames(PP)[2] <- "month"
  ## PP$deliverypoint <- gsub("COMMOD ", "", PP$curveName)
  ## PP <- merge(PP, map, by=c("deliverypoint"))

