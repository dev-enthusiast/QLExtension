# Check to see how the CNE positions are off
# Propose a fix
#
# .calc_cne_arr_priceadjustment_ratio
# implied_cne_arr_marks                 -- divide total booked value to arr positions
# .read_arrawdsum_coincident_peaks      -- CNE historical load from ISO
# 
#


#########################################################################
# For CNE, ARR prices should be adjusted because of the fact that
# for pricing they use the historical peak and not the coincident peak.
# This function calculates that ratio by month of the year.
#
.calc_cne_arr_priceadjustment_ratio <- function(peakLoad)
{
  aux <- peakLoad
  aux$ratio <- aux$tsdb.coincident.peak/aux$tsdb.peak
  aux$date <- as.Date(paste(aux$yyyymm, "-01", sep=""))
  aux$variable <- as.factor(aux$variable)
  sym.col <- trellis.par.get("superpose.symbol")
  xyplot(ratio ~ date, data=aux, groups=variable,
    type="o", xlab="month", ylab="Ratio of coincident peak to true peak", 
    key = list(points=Rows(sym.col, 1:length(levels(aux$variable))),
      text=list(levels(aux$variable)),
      columns=length(levels(aux$variable))))


  # calculate adjustment factors for ARR's for CNE
  hratio <- lapply(split(aux, aux$yyyymm),
    function(x){
      sum(x$tsdb.peak * x$ratio)/sum(x$tsdb.peak)})

  hratio <- data.frame(ratio=do.call(rbind, hratio))
  hratio$mon <- as.numeric(format(as.Date(paste(rownames(hratio),
    "-01", sep="")), "%m"))
  ratio <- hratio[(nrow(hratio)-11):nrow(hratio),]
  ratio <- ratio[order(ratio$mon), ]
  if (nrow(ratio) != 12)
    stop("You don't have all the months!")
  ratio$ratio[1:2] <- mean(ratio$ratio[1:2])
  ratio$ratio[7:8] <- mean(ratio$ratio[7:8])
  rownames(ratio) <- NULL
  print(ratio, row.names=FALSE)


  fname <- paste(DIR, "ratio_", Sys.Date(), ".csv", sep="")
  ## write.csv("# Ratio of CNE coincident peak to CNE peak.", file=fname,
  ##   row.names=FALSE)
  write.csv(ratio, file=fname, append=FALSE, row.names=FALSE)
  rLog("Wrote", fname)  
  
}


#########################################################################
# Read the coincident peaks as they appear int he ARRAWDSUM report
#
.read_arrawdsum_coincident_peaks <- function(startTime, endTime)
{
  arrawdsum <- get_hist_cne_ARRAWDSUM(startTime, endTime)

  aux <- unique(arrawdsum[,c("FlowDate", "LocationName", "PeakHourLoad")])
  names(aux)[2] <- "locationName"
  
  aux <- merge(aux, ZZ, all.x=TRUE)
  aux$yyyymm <- format(aux$FlowDate, "%Y-%m")
  colnames(aux)[which(colnames(aux)=="zone")] <- "variable"
  colnames(aux)[which(colnames(aux)=="PeakHourLoad")] <-
    "cne.coincident.peak.arrawdsum"
  aux$cne.coincident.peak.arrawdsum <- -aux$cne.coincident.peak.arrawdsum
  
  
  aux <- aux[, c("yyyymm", "variable", "cne.coincident.peak.arrawdsum")]
  
  return(aux)
}


#########################################################################
# implied cne arr marks by zone/month
#
implied_cne_arr_marks <- function(asOfDate)
{
  QQ <- .get_cne_positions(asOfDate, what="ARR")
  QQ$value <- QQ$today.price * QQ$nominal.quantity

  dollarsMth <- cast(QQ, month ~ ., sum)
  names(dollarsMth)[2] <- "total.dollars"
  
  qqMth <- cast(QQ, month ~ ., sum, value="nominal.quantity")
  names(qqMth)[2] <- "total.qty"

  res <- merge(dollarsMth, qqMth)
  HRS <- FTR:::FTR.bucketHours(start.dt=min(res$month), end.dt=max(res$month),
                               buckets="FLAT")
  names(HRS)[1] <- "month"
  res <- merge(res, HRS[,c("month", "hours")])
  res$'arr($/MWh)' <- res$total.dollars/res$total.qty/res$hours
  res <- subset(res, month >= asOfDate)
  
  write.csv(res, file="C:/Temp/implied_cne_arr_marks.csv", row.names=FALSE)
  return(res)
}


#########################################################################
# get the historical CNE ARR positions from SecDb on the last day before
# flow date
#
.read_secdb_arr_positions <- function()
{
  dates <- seq(as.Date("2009-01-01"), as.Date(format(Sys.Date(), "%Y-%m-01")),
             by="1 month")
  QQ <- NULL
  for (d in dates){
    asOfDate <- as.Date(secdb.dateRuleApply(as.Date(d), "-1b"))
    if (asOfDate %in% as.Date(c("2008-12-31","2010-05-31")))
      asOfDate <- as.Date(secdb.dateRuleApply(asOfDate, "-1b"))
    if (as.character(d) %in% names(QQ))
      next
    QQ[[as.character(d)]] <- .get_cne_arr_positions(asOfDate)  # does not work anymore

    
  }
  rownames(QQ) <- NULL
  QQ$yyyymm <- format(QQ$month, "%Y-%m")
  names(QQ)[which(names(QQ)=="today.price")] <- "ARR.price"
  names(QQ)[which(names(QQ)=="quantity")]    <- "ARR.position"
  QQ$delivery.point[which(QQ$delivery.point=="MAINE")] <- "ME"
  
  QQ
}
    ## aux <- .get_cne_positions(asOfDate, what="ARR")
    ## colnames(aux)[ncol(aux)] <- "quantity"
    ## thisMonth <- as.Date(format(asOfDate, "%Y-%m-01"))
    ## QQ <- rbind(QQ, cbind(asOfDate=asOfDate,
    ##                 subset(aux, month == as.Date(d))))




#########################################################################
#########################################################################
require(CEGbase)
require(reshape)
require(SecDb)
Sys.setenv(tz="")
require(zoo)
require(xlsx)
require(lattice)


source("H:/user/R/RMG/Energy/Trading/Congestion/lib.load.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.nepool.marks.R")
# PM:::realized_ARR_prices_CNE  


ZZ <<- data.frame(ptid=c(4000:4008), zone=c("HUB", "ME", "NH", "VT",
  "CT", "RI", "SEMA", "WMA", "NEMA"), locationName=c(".H.INTERNAL_HUB",
  ".Z.MAINE", ".Z.NEWHAMPSHIRE", ".Z.VERMONT", ".Z.CONNECTICUT",
  ".Z.RHODEISLAND", ".Z.SEMASS", ".Z.WCMASS", ".Z.NEMASSBOST"),
  location=c("HUB", "MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA",
    "NEMA"))


DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/ARR/"

asOfDate  <- as.Date("2010-11-30")
startTime <- as.POSIXct("2009-01-01 00:00:00")
endTime   <- as.POSIXct(format(Sys.time(), "%Y-%m-01 00:00:00"))-1

#LL.tsdb <- get_hist_cne_loads_tsdb(startTime, endTime, agg.zone=TRUE)
LL.iso <- get_hist_cne_loads_RTLOCSUM(startTime, endTime, as.zoo=TRUE)

LL.zone <- get_hist_zonal_load(startTime, endTime, market="RT")


#peakLoad <- get_coincident_peak(LL.tsdb, LL.zone, byMonth=TRUE)
#names(peakLoad)[which(names(peakLoad)=="cpeak.x")] <- "tsdb.coincident.peak"
#names(peakLoad)[which(names(peakLoad)=="peak.x")] <- "tsdb.peak"
peakLoad <- get_coincident_peak(LL.iso, LL.zone, byMonth=TRUE)
names(peakLoad)[which(names(peakLoad)=="cpeak.x")] <- "cne.coincident.peak.iso"
names(peakLoad)[which(names(peakLoad)=="peak.x")] <- "cne.peak.iso"
names(peakLoad)[which(names(peakLoad)=="peak.X")] <- "tsdb.zonal.peak"
peakLoad <- peakLoad[order(peakLoad$variable, peakLoad$yyyymm), ]
# data for pool peak load checks with data from:
# http://www.iso-ne.com/markets/hstdata/znl_info/monthly/smd_monthly.xls

# add the peak load from arrawdsum report
arrawdsum <- .read_arrawdsum_coincident_peaks(startTime, endTime)
peakLoad <- merge(peakLoad, arrawdsum)
names(peakLoad)[which(names(peakLoad)=="variable")] <- "delivery.point"


# get the booked ARR position from SecDb
QQ <- .read_secdb_arr_positions()   # add SecDb positions
peakLoad <- merge(peakLoad, QQ[,c("yyyymm", "delivery.point",
   "ARR.position", "ARR.price")], all.x=TRUE)


aux <- melt(peakLoad[, c("delivery.point", "yyyymm", "cne.coincident.peak.iso",
  "ARR.position", "cne.coincident.peak.arrawdsum")], id=1:2)
aux$date <- as.Date(paste(aux$yyyymm, "-01", sep=""))
aux$variable <- as.character(aux$variable)

windows(10,6)
sym.col <- trellis.par.get("superpose.symbol")
xyplot(value ~ date | delivery.point, data=aux, groups=variable,
  type="o", xlab="month", ylab="Position, MW", scales="free",
  key = list(points=Rows(sym.col, 1:3), text=list(c("SecDb ARR position",
    "CNE coincident peak", "CNE ARRAWDSUM report")), columns=3))





# calculate energy by month
EE.iso <- aggregate(LL.iso, format(index(LL.iso), "%Y-%m"), sum)
EE.iso <- melt(as.matrix(EE.iso))
names(EE.iso) <- c("yyyymm", "delivery.point", "cne.energy")

EE.zone <- aggregate(LL.zone, format(index(LL.zone), "%Y-%m"), sum)
EE.zone <- melt(as.matrix(EE.zone))
names(EE.zone) <- c("yyyymm", "delivery.point", "zone.energy")

EE <- merge(EE.iso, EE.zone)
peakLoad <- merge(peakLoad, EE)
peakLoad$ratio.peak <- peakLoad$cne.coincident.peak.iso/peakLoad$tsdb.zonal.peak
peakLoad$ratio.energy <- peakLoad$cne.energy/peakLoad$zone.energy
peakLoad$ratio.cne  <- peakLoad$cne.coincident.peak.iso/peakLoad$cne.energy
peakLoad$ratio.zone <- peakLoad$tsdb.zonal.peak/peakLoad$zone.energy


windows(10,6)
xyplot(ratio.peak ~ ratio.energy|delivery.point, data=peakLoad, scales="free",
  layout=c(4,2),
  panel=function(x, y, ...){
    panel.xyplot(x, y, ...)
    panel.abline(lm(y ~ x -1), col="tomato", ...)
    panel.abline(lm(y ~ x), col="gray", ...)
  }, xlab="CNE Energy / Zonal Energy",
     ylab="CNE coincident peak / Zonal peak load")



## windows(9,5)
## xyplot(ratio.cne ~ ratio.zone|delivery.point, data=peakLoad, scales="free",
##   type=c("p", "r"), layout=c(4,2),
##   panel=function(...){
##     panel.xyplot(...)
##     panel.lmline(..., col="gray")
##     #panel.loess(..., col="red", span=0.90)
##   }, xlab="Zonal peak load / Zonal Energy",
##      ylab="CNE coincident peak / CNE energy")




regFun <- function(X){lm(ratio.peak ~ ratio.energy - 1, data=X)}
#regFun <- function(X){lm(ratio.peak ~ ratio.energy, data=X)}
#regFun <- function(X){coef(lm(ratio.cne ~ ratio.zone, data=X))}
reg <- dlply(peakLoad, c("delivery.point"), regFun)

reg.sum <- lapply(reg, summary)

coeff <- data.frame(do.call(rbind, lapply(reg, coef)))
if (ncol(coeff)==1){
  coeff$intercept <- 0
  colnames(coeff)[1] <- "slope"
} else {
  colnames(coeff)[1:2] <- c("intercept", "slope")
}
coeff <- data.frame(zone=rownames(coeff), coeff)
rownames(coeff) <- NULL
print(coeff)



# read the forward cne load forecasts for zone and pool
# calculate zonal peak load and zonal energy
startTimeFwd <- as.POSIXct("2010-06-01 01:00:00")                # <-- CHANGE ME
endTimeFwd   <- as.POSIXct("2016-01-01 00:00:00")
LL.cne.fwd   <- get_fwd_cne_loads_nepool(startTimeFwd, endTimeFwd)

LL.zone.fwd <- get_fwd_zonal_loads_racm(startTimeFwd, endTimeFwd)

zPL <- aggregate(LL.zone.fwd, format(index(LL.zone.fwd)-1, "%Y-%m"),
  max, na.rm=TRUE)  # zonal peak load
zPL <- data.frame(melt(as.matrix(zPL)))
names(zPL) <- c("yyyymm", "zone", "zPL")
zPL$zone   <- as.character(zPL$zone)
zPL$mon    <- as.numeric(substring(zPL$yyyymm, 6, 7))

zE <- aggregate(LL.zone.fwd, format(index(LL.zone.fwd)-1, "%Y-%m"),
  sum, na.rm=TRUE)  # zonal energy
zE <- data.frame(melt(as.matrix(zE)))
names(zE) <- c("yyyymm", "zone", "zE")
zE$zone   <- as.character(zE$zone)
zE$mon    <- as.numeric(substring(zE$yyyymm, 6, 7))


# forecasted cne energy
cneE <-  aggregate(LL.cne.fwd, format(index(LL.cne.fwd)-1, "%Y-%m"),
                    sum, na.rm=TRUE)
cneE <- melt(as.matrix(cneE))
names(cneE) <- c("yyyymm", "zone", "cneE")
cneE$mon <- as.numeric(substring(cneE$yyyymm, 6,7))
cneE$zone <- as.character(cneE$zone)
cneE$yyyymm <- as.character(cneE$yyyymm)

# get the booked positions
QQ.fwd <- .get_cne_positions(asOfDate, what="ARR")
QQ.fwd <- subset(QQ.fwd, month >= as.Date(format(asOfDate, "%Y-%m-01")))
names(QQ.fwd)[ncol(QQ.fwd)] <- "secdb.qty"
names(QQ.fwd)[which(names(QQ.fwd)=="delivery.point")] <- "zone"

# put them together, cne coincident peak load
cneCPL <- merge(cneE, zPL[,c("mon", "zone", "zPL")], by=c("mon", "zone"))
cneCPL <- merge(cneCPL, zE[,c("mon", "zone", "zE")],  by=c("mon", "zone"))
cneCPL <- merge(cneCPL, coeff, by="zone")

smoothFun <- function(X){
  a  <- X[1,"slope"]
  b  <- X[1,"intercept"]
  x0 <- -b/a
  x  <- X$cneE / X$zE
  if (x0 > 0){
    y <- a*x + b*(1 - exp(-x/x0))   # need smoothing
  } else {
    y <- a*x + b                    # don't need smoothing
  }
  y <- y * X$zPL
  return(data.frame(yyyymm=X$yyyymm, coincidentPL=y))
}
aux <- ddply(cneCPL, c("zone"), smoothFun)  # do the forecast by zone
cneCPL <- merge(cneCPL, aux)

cneCPL <- cneCPL[order(cneCPL$yyyymm, cneCPL$zone), ]
cneCPL$month <- as.Date(paste(cneCPL$yyyymm, "-01", sep=""))


# plot current positions vs. forecasted positions.
aux <- merge(cneCPL[,c("zone", "month", "coincidentPL")],
  QQ.fwd[, c("zone", "month", "secdb.qty")], all=TRUE)
aux <- melt(aux, id=1:2)

windows(8,5)
sym.col <- trellis.par.get("superpose.symbol")
xyplot(value ~ month | zone, data=aux, groups=variable,
  subset=zone %in% c("CT", "WMA", "NEMA", "SEMA"), 
  type="o", xlab="month", ylab="Position, MW", scales="free",
  key = list(points=Rows(sym.col, 1:2), text=list(c("CNE coincident peak", 
    "SecDb ARR position")), columns=2), layout=c(2,2))




####################################################################
# PNL impact of rebooking ARR positions
####################################################################

price.ARR <- PM:::get.SecDb.ARR(asOfDate=asOfDate, endDate=as.Date("2017-12-01"))
price.ARR <- merge(price.ARR, ZZ[,c("location", "zone")], all.x=TRUE)

qq <- merge(cneCPL[,c("zone", "month", "coincidentPL")],
  QQ.fwd[, c("zone", "month", "secdb.qty")], all=TRUE)

qq <- merge(qq, price.ARR[,c("month", "zone", "value")])
names(qq)[which(names(qq)=="value")] <- "price.ARR"
qq$coincidentPL <- round(qq$coincidentPL)

qq$PnL <- (qq$coincidentPL - qq$secdb.qty) * qq$price.ARR 

PnL <- cast(qq, zone ~ ., sum, na.rm=TRUE, fill=0, value="PnL")
names(PnL)[2] <- "PnL"
print(PnL, row.names=FALSE)

# total:
sum(qq$PnL, na.rm=TRUE)

# by month:
PnL.mth <- cast(qq, month ~ ., sum, na.rm=TRUE, fill=0, value="PnL")
names(PnL.mth)[2] <- "PnL"
print(PnL.mth, row.names=FALSE)






####################################################################
# save to xlsx
####################################################################

dirOut <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/ARR/" 
fname  <- paste(dirOut, "data_for_strats_", Sys.Date(),".xlsx", sep="")

write.xlsx(cneCPL, fname, sheetName="forecast", row.names=FALSE)
write.xlsx(peakLoad, fname, sheetName="historical", row.names=FALSE,
           append=TRUE)
write.xlsx(coeff, fname, sheetName="regression", row.names=FALSE,
           append=TRUE)
rLog("Wrote file ", fname)







####################################################################
####################################################################
####################################################################

cast(price.ARR, month ~ location, I, fill=0)



qq <- .get_cne_positions(asOfDate, what="ARR")
write.csv(qq, file="C:/Temp/cne_arr_2010-05-27.csv", row.names=FALSE)












