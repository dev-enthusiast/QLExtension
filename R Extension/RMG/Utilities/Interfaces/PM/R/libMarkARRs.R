#
# 2009-10-21: Look at the ARR marks and how they are currently calculated
#   by Kevin Telford in 
#   S:\All\Structured Risk\NEPOOL\Ancillary Services\ARR Valuation
#
# Document the "ARR Curve ddmmyy.xls" spreadsheet (old method) -- talk to
# Kevin Telford.  Estimate pool ARR dollars by year.  Split the pool ARR
# dollars by zones according to History.  Shape the yearly dollars using a
# historical shape.
#
# NEPOOL peak load estimates by zone come from Dinkar/Ling
#
# from ntplot: Nepool Region::Nepool Data::NEPOOL SD ARRAWDSUM Reports
# for info on this report go to
# http://www.iso-ne.com/support/tech/file_formats/mis_frmts/ 
#
#
# Functions:
# - .adjustCongestionMarks for the ARR regression. 
# - .calc.fwdARR.dollarMWh: use a load forecast to get an ARR number in $/MWh
# - get.hist.ARR: read tsdb symbols with historical arr info
# - get.SecDb.ARR: read the ARR marks from SecDb $/peak-MWmonth
# - get.SecDb.ARRdollars in $ (directly comparable with NEPOOL ARR dollars amounts)
# - get.ARR.PnL: given a set of position calculate the PnL
# - .make.PMview.ARR: make the pm view for forward ARRs 
# - .plot.histfwd.ARR.marks: plot the data.frame with arr marks
# - .read.peak.load.forecast: read the most current peak load forecast
# - realized_ARR_prices_CNE: calculate where ARR prices have been coming in
# - realized_ARR_prices_CCG:
# - realized_ARR_prices_pool:
# - .study.ARR.regression: check the relationship, do the loess, etc.
#
#
# Started on 2009-10-24 by Adrian Dragulescu
# 
# This library is used with main script:
#   Energy/Trading/Congestion/NEPOOL/Marks/update_ARR_marks.R
#

##########################################################################
# Write down how you'll adjust the forward view on congestion
# Where CC is a data.frame returned by secdb.getImpliedCongestionMarks
#
.adjustCongestionMarks <- function(CC, focusZone=focusZone)
{
  CC <- subset(CC, zone == focusZone)
  
  CC$adjCongMark <- CC$congMark

  if (focusZone=="RI"){   
    rLog("Have the RI peak/offpeak congestion set ot $0!")
    ind <- which(CC$zone=="RI")
    if (length(ind) > 0)
      CC$adjCongMark[ind] <- 0  
  }
  
  if (focusZone=="SEMA"){
    rLog("Have the SEMA offpeak congestion not be higher than $0.10")
    ind <- which(CC$zone=="SEMA" & CC$bucket == "OFFPEAK" & CC$congMark > 0.10)
    if (length(ind) > 0)
      CC$adjCongMark[ind] <- 0.10
    rLog("Have the SEMA onpeak congestion not be higher than $0.20")
    ind <- which(CC$zone=="SEMA" & CC$bucket == "ONPEAK" & CC$congMark > 0.20)
    if (length(ind) > 0)
      CC$adjCongMark[ind] <- 0.20
  }
  
  if (focusZone=="WMA"){
    rLog("Have the WMA offpeak congestion not be higher than $0.20")
    ind <- which(CC$zone=="WMA" & CC$bucket == "OFFPEAK" & CC$congMark > 0.20)
    if (length(ind) > 0)
      CC$adjCongMark[ind] <- 0.20
    rLog("Take down WMA onpeak from 2014 onwards after NEEWS, no higher",
         "than $0.20")
    ind <- which(CC$zone=="WMA" & CC$month >= as.Date("2014-01-01") &
                 CC$bucket == "ONPEAK")
    if (length(ind) > 0)
      CC$adjCongMark[ind] <-  pmax(CC$congMark[ind]-0.75, 0.20)
  }

  
  return(CC)
}




##########################################################################
# Translate the bucket of fwd ARR into $/MWh using a load forecast
#
.calc.fwdARR.dollarMWh <- function(fwdARR.pm, delivPt=c("ME", "NH",
  "VT", "CT", "RI", "SEMA", "WMA", "NEMA"), endDate=as.Date("2014-12-31"))
{
  
  # grab the forecasted TLP loads
  symbols <- paste("NEPOOL_", delivPt, "_tsdb", sep="")
  fLL <- FTR.load.tsdb.symbols(symbols, startDate, endDate)

  # calc the energy
  EE <- aggregate(fLL, format(index(fLL), "%Y-%m-01"), sum)
  colnames(EE) <- colnames(fLL)
  colnames(EE) <- gsub(".*_(.*)_.*", "\\1", colnames(EE))
  EE <- melt(as.matrix(EE))
  colnames(EE) <- c("month", "delivPt", "energy")

  aux <- merge(fwdARR.pm[,c("month","delivPt", "arr")], EE, all.x=TRUE)
  aux <- na.omit(aux)
  aux$arr.dollarMWh <- aux$arr/aux$energy

  (round(tapply(aux$arr.dollarMWh, list(format(aux$month, "%Y")),
               mean), 2))  
}



##########################################################################
# Customizing zone regressions.
# Where zone is the zone name, e.g. "RI"
#
.customize.zone <- function(zone)
{  
  if (zone %in% c("NEMA", "SEMA")){
    rLog("For", zone, "filter historical data from 2008-01 to present.")
    custom <- NULL
    custom$FILTER <- data.frame(month=seq(as.Date("2008-01-01"),
      Sys.Date(), by="1 month"))
  } else if (zone %in% "CT") {
    rLog("For CT don't filter historical data.")
    custom <- list(FILTER = NULL)
  } else {
    rLog("No customization necessary for zone:", zone)
    custom <- list(FILTER = NULL)
  }

  return(custom)
}


##########################################################################
# get the payments, dollars
#
get.hist.ARR <- function(zones=4001:4008,
  startDate=as.Date("2004-01-01"))
{
  HH <- vector("list", length=length(zones))
  for (z in 1:length(zones)){  
    rLog(paste("Working on zone", zones[z]))
    zone <- as.character(zones[z])
    # zonal load share dollars for month onpeak from the yearly auction
    zlsd.yon <- paste("NE_ARRAWD_YR_ON_", zone, "_ZLSD", sep="")
    # zonal load share dollars for month offpeak from the yearly auction
    zlsd.yoff <- paste("NE_ARRAWD_YR_OFF_", zone, "_ZLSD", sep="")
    # zonal load share dollars for month onpeak from the monthly auction
    zlsd.mon <- paste("NE_ARRAWD_MTH_ON_", zone, "_ZLSD", sep="")
    # zonal load share dollars for month offpeak from the monthly auction
    zlsd.moff <- paste("NE_ARRAWD_MTH_OFF_", zone, "_ZLSD", sep="")
    # zonal peak hour load for month
    zpl.mon <- paste("NE_ARRAWD_MTH_ON_", zone, "_ZPHL", sep="")
  
    symbols <- c(zlsd.yon, zlsd.yoff, zlsd.mon, zlsd.moff, zpl.mon)
  
    hh <- FTR:::FTR.load.tsdb.symbols(symbols, startDate, Sys.Date())

    arrname <- paste("arr_", zone, "_dollars_peak-month", sep="")
    hh$arrname <- -(hh[,zlsd.yon] + hh[,zlsd.mon] + hh[,zlsd.mon] + 
                    hh[,zlsd.moff])/hh[,zpl.mon]
    colnames(hh)[ncol(hh)] <- arrname

    HH[[z]] <- hh
  }
  HH <- do.call(cbind, HH)

  return(HH)
}


##########################################################################
# Calculate PnL given a set of ARR positions and two dates, calculate PnL
# date2 > date1
get.ARR.PnL <- function(date1, date2, QQ, delivPt=c("MAINE", "NH",
  "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
{
  if (any(!c("delivery.point", "month", "quantity") %in%
        names(QQ))){
    stop(paste("Missing one of delivery.point, month,",
               ,"quantity in names(QQ)!"))
  }
  QQ <- subset(QQ, month >= date1)
  
  startDate <- min(QQ$month)
  endDate   <- max(QQ$month)

  # get prices from SecDb
  PP1 <- get.SecDb.ARR(delivPt=delivPt, endDate=endDate,
    asOfDate=date1, startDate=startDate)
  names(PP1)[which(names(PP1)=="value")] <- "price1"

  PP2 <- get.SecDb.ARR(delivPt=delivPt, endDate=endDate,
    asOfDate=date2, startDate=startDate)
  names(PP2)[which(names(PP2)=="value")] <- "price2"

  PP <- merge(PP1[, c("location", "month", "price1")],
              PP2[, c("location", "month", "price2")], all=TRUE)
  names(PP)[which(names(PP)=="location")] <- "delivery.point"

  PnL <- merge(PP, QQ[,c("delivery.point", "month", "quantity")],
    all=TRUE)
  PnL$pnl <- (PnL$price2 - PnL$price1)*PnL$quantity


  return(PnL)  
}


##########################################################################
# Read the current ARR marks from SecDb.  ARR marks are added to SecDb by
# Strats.  Can be found by using a Full Requirements calculator.
#
get.SecDb.ARR <- function(delivPt=c("MAINE", "NH", "VT", "CT", "RI",
  "SEMA", "WMA", "NEMA"), startDate=Sys.Date(),
  endDate=as.Date("2015-12-01"), asOfDate=Sys.Date())
{
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  location    <- delivPt
  bucket      <- "FLAT"
  serviceType <- "ARR"
  
  rLog("Get the ARR marks from SecDb ... ")  
  curveData <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType)
  #head(curveData)
  rLog("Done")

  curveData
}


##########################################################################
# Make histfwd.ARR.marks
# HH - the zoo object from get.hist.ARR
# CC - a view on forward congestion, can be the implied congestion from
#      marks
#
.make.PMview.ARR <- function(MM, CC, HH, focusZone=focusZone)  
{
  # if you adjusted CC use the adjusted (only here!)
  if ("adjCongMark" %in% colnames(CC))
    CC$congMark <- CC$adjCongMark

  CC <- subset(CC, zone == focusZone)
  
  # get the relationship between the dollar payment and CP
  loessFun <- function(X){
    model <- loess(value ~ CP, data=X, degree=1, family="symmetric")
    x  <- seq(min(X$CP), max(X$CP), length.out=250)
    y  <- predict(model, data.frame(CP=x), se=TRUE)

    return(list(model=model, y=y, x=x))
  }
  reg <- daply(MM, c("zone", "class.type"), loessFun, .progress="text")


  # estimate forward arr dollars using the loess regression above
  # and the forward congestion marks
  estARRFun <- function(oneCC, reg){
    #browser()
    zname  <- as.character(oneCC$zone[1])
    bucket <- as.character(oneCC$bucket[1])
    rLog(paste("Working on", zname, bucket))
    model   <- reg[[bucket]]$model
    # for prediction purposes truncate the forward congestion range.
    # this will avoid NA's in the forecastDollars
    minCong <- min(model$x)  # minimum historical congestion
    maxCong <- max(model$x)  # max historical congestion
    oneCC$truncCongMark <- oneCC$congMark
    oneCC$truncCongMark[oneCC$congMark < minCong] <- minCong
    oneCC$truncCongMark[oneCC$congMark > maxCong] <- maxCong
    forecastDollars <- predict(model, data.frame(CP=oneCC$truncCongMark), se=TRUE)

    se.predict <- sqrt(forecastDollars$residual.scale^2 + forecastDollars$se.fit^2)

    period <- "month"   # for annuals you have different column names ...
    if ("year" %in% colnames(oneCC)) period <- "year"
    forecastDollars <- cbind(oneCC[, c(period, "bucket", "delivPt")],
      arr=forecastDollars$fit, arr.se=se.predict)

    forecastDollars
  }

  # forecast ARRs from monthly auctions
  fwdARRm <- daply(CC, c("bucket", "zone"), estARRFun, reg)
  fwdARRm <- do.call(rbind, fwdARRm)
  fwdARRm$auction <- "monthly"
  fwdARRm$year <- format(fwdARRm$month, "%Y")
  
  # do the forecast for the annual ones too
  aCC <- CC
  aCC$year <- format(CC$month, "%Y"); aCC$month <- NULL
  aCC <- cast(aCC, year + bucket + delivPt + zone ~ ., mean, fill=NA,
              value="congMark")
  names(aCC)[ncol(aCC)] <- "congMark"

  fwdARRa <- daply(aCC, c("bucket", "zone"), estARRFun, reg)
  fwdARRa <- do.call(rbind, fwdARRa)
  fwdARRa$auction <- "annual"
  fwdARRa <- merge(unique(fwdARRm[,c("month","year")]), fwdARRa, all=TRUE)


  # replace the current year annual arr's with the actual value!
  # this is where I need HH
  currentYear <- format(asOfDate, "%Y")
  ind <- grep("NE_ARRAWD_YR_", colnames(HH))
  aux <- aggregate(HH[,ind], format(index(HH), "%Y"), mean)

  if (currentYear %in% index(aux)){  # if not at begining of the year
    aux <- aux[currentYear,]
    aux <- melt(as.matrix(aux))
    aux$ptid <- as.numeric(gsub(".*_([[:digit:]]{4})_.*", "\\1", aux[,"X2"]))
    aux <- merge(aux, ZZ, all.x=TRUE)
    aux$bucket <- paste(gsub("NE_ARRAWD_YR_(.*)_.*_.*", "\\1", aux[,"X2"]),
                        "PEAK", sep="")
    aux$year <- aux[,"X1"]
    aux <- aux[,c("bucket", "delivPt", "value", "year")]
    fwdARRa <- merge(fwdARRa, aux, all.x=TRUE)
    ind <- which(!is.na(fwdARRa$value))
    fwdARRa$arr[ind]    <- fwdARRa$value[ind]
    fwdARRa$arr.se[ind] <- 0
    fwdARRa$value <- NULL        # don't need it anymore!
  }
  
  # put the monthly and annual auctions together, to make the PM view
  fwdARR <- rbind(fwdARRm, fwdARRa)
  rownames(fwdARR) <- NULL
  fwdARR <- fwdARR[order(fwdARR$month, fwdARR$bucket), ]
  
  fwdARR
}

##########################################################################
# NOT WORKING YET!
#  
.plot.histfwd.ARR.dollars <- function(HH, fwdARR.pm, fwdARR.SecDb, delivPt="CT")
{
  if (length(delivPt)>1)
    stop("One one delivPt at a time buddy!")
  
  thisDelivPt <- delivPt # thisDelivPt <- "CT"

  # the SecDb mark
  zfwdARR.SecDb <- subset(fwdARR.SecDb, location==thisDelivPt)
  aux <- cbind(zfwdARR.SecDb[,c("month", "location", "value")], source="SecDb")
  names(aux)[2:3] <- c("delivPt", "arr")
  DD <- aux

  # the PM view
  zfwdARR.pm <- subset(fwdARR.pm, delivPt==thisDelivPt)
  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM")
  names(aux)[3] <- "arr"
  DD <- rbind(DD, aux)

  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM+std")
  names(aux)[3] <- "arr"
  aux$arr <- aux$arr + zfwdARR.pm[,"arr.mark.se"]
  DD <- rbind(DD, aux)
  
  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM-std")
  names(aux)[3] <- "arr"
  aux$arr <- pmax(aux$arr - zfwdARR.pm[,"arr.mark.se"],0)
  DD <- rbind(DD, aux)

  # the historicals
  allZones <- structure(as.character(4001:4008), names=c("ME", "NH",
     "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
  zone <- allZones[thisDelivPt]
  ind <- grep(paste("arr_",zone,"_dollars_peak-month", sep=""), colnames(HH))
  aux <- melt(as.matrix(HH[,ind]))
  aux <- aux[,c(1,3)]
  aux[,1] <- as.Date(aux[,1])
  colnames(aux) <- c("month", "arr")
  aux$delivPt <- thisDelivPt
  aux$source  <- "Hist"
  DD <- rbind(DD, aux)

  windows()
  plot(DD$month, DD$arr, type="n", ylab="ARR $/peak MW-month",
       main=paste("Zone", thisDelivPt))
  aux <- subset(DD, source=="Hist")
  lines(aux$month, aux$arr, col="blue", type="o")
  aux <- subset(DD, source=="PM")
  lines(aux$month, aux$arr, col="red", type="o")
  aux <- subset(DD, source=="PM-std")
  lines(aux$month, aux$arr, col="gray", type="l")
  aux <- subset(DD, source=="PM+std")
  lines(aux$month, aux$arr, col="gray", type="l")
  aux <- subset(DD, source=="SecDb")
  lines(aux$month, aux$arr, col="darkgreen", type="o")
  legend("topright", legend=c("Hist", "PM", "PM+std",
    "PM-std", "SecDb"), col=c("blue", "red", "gray", "gray",
    "darkgreen"), pch=1, text.col=c("blue", "red", "gray", "gray",
    "darkgreen")) 
  
}




##########################################################################
# Load in the non-coincident peak load forecast as given by the strats
#
.plot.histfwd.ARR.marks <- function(HH, fwdARR.pm, fwdARR.SecDb,
  delivPt="CT")
{
  if (length(delivPt)>1)
    stop("One one delivPt at a time buddy!")
  
  thisDelivPt <- delivPt # thisDelivPt <- "CT"

  # the SecDb mark
  zfwdARR.SecDb <- subset(fwdARR.SecDb, location==thisDelivPt)
  aux <- cbind(zfwdARR.SecDb[,c("month", "location", "value")], source="SecDb")
  names(aux)[2:3] <- c("delivPt", "arr")
  DD <- aux

  # the PM view
  zfwdARR.pm <- subset(fwdARR.pm, delivPt==thisDelivPt)
  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM")
  names(aux)[3] <- "arr"
  DD <- rbind(DD, aux)

  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM+std")
  names(aux)[3] <- "arr"
  aux$arr <- aux$arr + zfwdARR.pm[,"arr.mark.se"]
  DD <- rbind(DD, aux)
  
  aux <- cbind(zfwdARR.pm[,c("month", "delivPt", "arr.mark")], source="PM-std")
  names(aux)[3] <- "arr"
  aux$arr <- pmax(aux$arr - zfwdARR.pm[,"arr.mark.se"],0)
  DD <- rbind(DD, aux)

  # the historicals
  allZones <- structure(as.character(4001:4008), names=c("ME", "NH",
     "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
  zone <- allZones[thisDelivPt]
  ind <- grep(paste("arr_",zone,"_dollars_peak.month", sep=""), colnames(HH))
  aux <- melt(as.matrix(HH[,ind]))
  aux <- aux[,c(1,3)]
  aux[,1] <- as.Date(aux[,1])
  colnames(aux) <- c("month", "arr")
  aux$delivPt <- thisDelivPt
  aux$source  <- "Hist"
  DD <- rbind(DD, aux)

  windows()
  plot(DD$month, DD$arr, type="n", ylab="ARR $/peak MW-month",
       main=paste("Zone", thisDelivPt))
  aux <- subset(DD, source=="Hist")
  lines(aux$month, aux$arr, col="blue", type="o")
  aux <- subset(DD, source=="PM")
  lines(aux$month, aux$arr, col="red", type="o")
  aux <- subset(DD, source=="PM-std")
  lines(aux$month, aux$arr, col="gray", type="l")
  aux <- subset(DD, source=="PM+std")
  lines(aux$month, aux$arr, col="gray", type="l")
  aux <- subset(DD, source=="SecDb")
  lines(aux$month, aux$arr, col="darkgreen", type="o")
  legend("topright", legend=c("Hist", "PM", "PM+std",
    "PM-std", "SecDb"), col=c("blue", "red", "gray", "gray",
    "darkgreen"), pch=1, text.col=c("blue", "red", "gray", "gray",
    "darkgreen")) 

  
}



##########################################################################
# Load in the non-coincident peak load forecast as given by the strats
#
.read.peak.load.forecast <- function()
{
  dirname <- paste("S:/All/Structured Risk/NEPOOL/Ancillary Services/",
    "ARR Valuation/PeakLoad", sep="")
  files <- sort(list.files(dirname, full.name=TRUE), decreasing=TRUE)

  con <- odbcConnectExcel(files[1])
  LL <- sqlFetch(con, "Sheet1")
  odbcCloseAll()

  colnames(LL) <- toupper(colnames(LL))
  colnames(LL)[1] <- "month"
  
  LL <- melt(LL, id=1)
  names(LL)[2:3] <- c("delivPt", "PeakLoad")
  LL$delivPt <- gsub(" DA$", "", LL$delivPt)

  LL
}


##########################################################################
# Calculate realized ARR prices 
#
## realized_ARR_prices_ccg <- function(zone, startDate=as.Date("2007-01-01"),
##   endDate=Sys.Date())
## {
##   # get the LoadShareDollars from ARRAWDSUM report
##   ARR <- get_hist_cne_ARRAWDSUM(startDate, endDate)
##   ARR <- ARR[, c("FlowDate", "LocationID", "LoadShareDollars")]
##   names(ARR)[1:2] <- c("month", "ptid")
##   ARR$month <- as.Date(ARR$month)
##   # aggregate over peak/offpeak monthly/annual 
##   ARR <- cast(ARR, month + ptid ~ ., sum, value="LoadShareDollars")
##   names(ARR)[3] <- "LoadShareDollars"
  
##   ARR <- merge(ARR, ZZ[, c("ptid", "zone")], all.x=TRUE)
  
##   # calculate the energy by month, zone
##   LL.iso <- get_hist_cne_loads_RTLOCSUM(startDate, endDate, as.zoo=TRUE)
##   EE.iso <- aggregate(LL.iso, format(index(LL.iso), "%Y-%m-01"), sum)
##   aux <- melt(as.matrix(EE.iso))
##   names(aux) <- c("month", "zone", "energy")
  
##   ARR <- merge(ARR, aux, by=c("zone", "month"))
##   ARR$realized <- ARR$LoadShareDollars/ARR$energy

##   #ARR <- cast(ARR, month ~ zone, I, fill=NA, value="realized")
##   return(ARR)
## }


##########################################################################
# Calculate realized ARR prices in $/MWh
#
realized_ARR_prices_cne <- function(startDate=as.Date("2007-01-01"),
  endDate=Sys.Date())
{
  # get the LoadShareDollars from ARRAWDSUM report
  ARR <- get_hist_cne_ARRAWDSUM(startDate, endDate)
  ARR <- ARR[, c("FlowDate", "LocationID", "LoadShareDollars")]
  names(ARR)[1:2] <- c("month", "ptid")
  ARR$month <- as.Date(ARR$month)
  # aggregate over peak/offpeak monthly/annual 
  ARR <- cast(ARR, month + ptid ~ ., sum, value="LoadShareDollars")
  names(ARR)[3] <- "LoadShareDollars"
  
  ARR <- merge(ARR, ZZ[, c("ptid", "zone")], all.x=TRUE)
  
  # calculate the energy by month, zone
  LL.iso <- get_hist_cne_loads_RTLOCSUM(startDate, endDate, as.zoo=TRUE)
  EE.iso <- aggregate(LL.iso, format(index(LL.iso), "%Y-%m-01"), sum)
  aux <- melt(as.matrix(EE.iso))
  names(aux) <- c("month", "zone", "energy")
  
  ARR <- merge(ARR, aux, by=c("zone", "month"))
  ARR$realized <- ARR$LoadShareDollars/ARR$energy

  #ARR <- cast(ARR, month ~ zone, I, fill=NA, value="realized")
  return(ARR)
}


##########################################################################
# Calculate realized ARR prices in $/MWh for the entire POOL
#
realized_ARR_pool <- function(what=c("prices", "dollars"),
  startDate=as.Date("2007-01-01"), endDate=Sys.Date())
{
  # get the LoadShareDollars from ARRAWDSUM report
  ARR <- get_hist_cne_ARRAWDSUM(startDate, endDate)
  ARR <- ARR[, c("FlowDate", "LocationID", "ZonalLoadShareDollars")]
  names(ARR)[1:2] <- c("month", "ptid")
  ARR$month <- as.Date(ARR$month)
  # aggregate over peak/offpeak monthly/annual 
  ARR <- cast(ARR, month + ptid ~ ., sum, value="ZonalLoadShareDollars")
  names(ARR)[3] <- "ZonalLoadShareDollars"
  
  ARR <- merge(ARR, ZZ[, c("ptid", "zone")], all.x=TRUE)

  if (what=="prices") {
    # calculate the energy by month, zone
    LL <- get_hist_zonal_load(startDate, endDate, market="RT")
    EE <- aggregate(LL, format(index(LL), "%Y-%m-01"), sum)
    aux <- melt(as.matrix(EE))
    names(aux) <- c("month", "zone", "energy")
  
    ARR <- merge(ARR, aux, by=c("zone", "month"))
    ARR$realized <- ARR$ZonalLoadShareDollars/ARR$energy
  } else if (what=="dollars") {
    ARR$realized <- ARR$ZonalLoadShareDollars
  } else {
    stop("Unknown `what` argument!")
  }
    
  ARR <- cast(ARR, month ~ zone, I, fill=NA, value="realized")
  return(ARR)
}



##########################################################################
# Check the quality of the regression, filter some points, etc. 
# Where HH - the data.frame you get with get.hist.ARR
#
.study.ARR.regression <- function(HH, focusZone, FILTER=NULL)
{
  if (!(focusZone %in% ZZ$zone))
    stop("focusZone", focusZone, "doesn't exist in global ZZ$name!")
  
  paths <- data.frame(path.no=1:8, source.ptid=4000,
    sink.ptid=4001:4008, class.type="ONPEAK")
  paths2 <- paths
  paths2$path.no <- 9:16
  paths2$class.type <- "OFFPEAK"
  paths <- rbind(paths, paths2)

  FTR.load.ISO.env("NEPOOL")
  NEPOOL$CP <- NEPOOL$.loadClearingPrice()
  CP <- FTR.get.CP.for.paths(paths, NEPOOL)
  CP <- merge(paths, CP)
  CP <- subset(CP, sink.ptid == ZZ$ptid[which(ZZ$zone==focusZone)])

  # separate only the zonal load share dollars by zones and make a df
  ind <- grep("ZLSD", colnames(HH))
  MM  <- as.matrix(HH[,ind])
  MM  <- melt(MM)
  MM$class.type <- gsub("NE_ARRAWD_.*_(.*)_.*_.*", "\\1", MM[,2])
  MM$class.type <- paste(MM$class.type, "PEAK", sep="")
  MM$sink.ptid  <- as.numeric(gsub("NE_ARRAWD_.*_(.*)_.*", "\\1", MM[,2]))
  MM$auction <- FTR.AuctionTerm(MM[,1], MM[,1])
  ind <- grep("_YR_", MM[,2])
  if (length(ind)>0)
    MM$auction[ind] <- paste(MM$auction[ind], "-1Y", sep="")

  MM <- merge(MM[,c("class.type", "sink.ptid", "auction", "value")],
    CP[,c("class.type", "sink.ptid", "auction", "CP")])
  names(MM)[which(names(MM)=="sink.ptid")] <- "ptid"
  
  MM <- merge(MM, ZZ)
  MM$auction.type <- "Monthly"
  ind <- grep("-1Y$", MM$auction)
  if (length(ind)>0)
    MM$auction.type[ind] <- "Annual"
  MM$auction.type <- factor(MM$auction.type)
  MM <- cbind(MM, FTR.AuctionTerm(auction=MM$auction))

  if (!is.null(FILTER)){   # apply a filter to historical data
    MM$month <- FTR.AuctionTerm(auction=MM$auction)$start.dt
    MM <- merge(MM, FILTER)
    MM <- MM[,-which(colnames(MM) %in% colnames(FILTER))] 
  }
  
  # do the plots ...
  windows(7,4)
  sym.col <- trellis.par.get("superpose.symbol")
  #sym.col$col <- c("blue", "red", "black", "green")
  sym.col$col <- c("maroon", "slateblue", "red", "black", "green")
  trellis.par.set("superpose.symbol", sym.col)

  print(xyplot(value ~ CP | class.type, data=MM, groups=auction.type,
    scales="free", xlab="FTR clearing price, $/MWh",
    ylab="Zonal load shared $", #subset=class.type=="ONPEAK",
    main=paste(focusZone, "zone"),
    panel=function(...){
      panel.xyplot(...)
      panel.loess(..., col="gray")
    },     
    key=list(points=Rows(sym.col, 1:2),
             text=list(levels(MM$auction.type)),
             columns = 2), layout=c(2,1)))

##   windows()
##   print(xyplot(value ~ CP | zone, data=MM, groups=auction.type,
##     scales="free", xlab="FTR clearing price, $/MWh",
##     ylab="Zonal load shared $", subset=class.type=="OFFPEAK",
##     main="Offpeak bucket",
##     panel=function(...){
##       panel.xyplot(...)
##       panel.loess(..., col="gray")
##     },     
##     key=list(points=Rows(sym.col, 1:2),
##              text=list(levels(MM$auction.type)),
##              columns = 2), layout=c(4,2)))

  
  return(MM)
}


