# Calculate the PnL effect due to ARRs change in marks
# 
# "S:/All/Structured Risk/NEPOOL/Ancillary Services/",
#  "ARR Valuation/Sent/"
#

QQ <- read.csv("C:/Temp/cne_arr_2010-12-10.csv")
res <- cast(QQ, ACTIVE_CONTRACTS + DELIVERY_POINT ~ ., sum, value="NOMINAL_QUANTITY")
res <- res[order(res$DELIVERY_POINT, res$ACTIVE_CONTRACTS), ]


require(CEGbase)
require(reshape)
require(zoo)
require(SecDb)
require(FTR)
require(PM)         # PM/R/libMarkARRs.R   
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkARRs.R")


date1 <- as.Date("2010-08-02") #as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
date2 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))

unit <- "CNE"

switch (unit, 
  CNE = {
    QQ <- .get_cne_positions(date1, what="ARR")   # get CNE ARR positions
    colnames(QQ)[ncol(QQ)] <- "quantity"},
  CCG = {
    QQ <- .get_ccg_arr_positions(date1)           # get CCG ARR positions
    colnames(QQ)[which(colnames(QQ)=="contract.dt")] <- "month"
    cast(QQ, delivery.point ~ ., sum, fill=0, value="quantity")}
)


PnL <- get.ARR.PnL(date1, date2, QQ)
sum(PnL$pnl, na.rm=TRUE)

res.mth <- cast(PnL, month ~ ., sum, na.rm=TRUE, fill=0, value="pnl")
#res.mth[,2] <- res.mth[,2]*0.65
print(res.mth, row.names=FALSE)


res.zone <- cast(PnL, delivery.point ~ ., sum, na.rm=TRUE, fill=0,
  value="pnl")
names(res.zone)[ncol(res.zone)] <- "PnL"
print(data.frame(res.zone), row.names=FALSE)




# get CNE Load positions
LL <- .get_cne_positions(date1, what="Load")
colnames(LL)[ncol(LL)] <- "quantity"
unique(LL$bucket)  # only 7X24 ?!

LL <- data.frame(cast(LL, delivery.point + month + bucket ~ ., sum, fill=0,
           value="quantity" ))
colnames(LL)[ncol(LL)] <- "quantity"

LL.ct <- subset(LL, delivery.point=="CT")


# get implied position from ARRs

zones <- c(4004)
startDate <- as.Date("2004-01-01")
HH <- get.hist.ARR(zones=zones, startDate=startDate)

MM <- PM:::.study.ARR.regression(HH, FILTER=FILTER)




#all.LL <- cast(LL, month ~ ., sum, fill=0, value="quantity")



#############################################################
loadName  <- "RiProG10"
loadClass <- "Large DS"
asOfDate  <- Sys.Date()

res <- secdb.getLoadObject(loadName, loadClass, asOfDate)
head(res)

res <- subset(res, time >= as.POSIXct("2010-04-01 00:00:00"))
res <- subset(res, time <= as.POSIXct("2010-06-30 23:00:00"))
res$mon <- format(res$time, "%m")
maxL <- cast(res, mon ~ ., max)
maxL[,2] <- maxL[,2]/2
names(maxL)[2] <- "Peak.Load"
print(maxL, row.names=FALSE)


#############################################################
dates <- seq(as.Date("2009-01-01"), as.Date("2010-02-01"),
             by="1 month")
res <- NULL
for (d in dates){
  asOfDate <- secdb.dateRuleApply(as.Date(d), "-1b")
  QQ <- .get_cne_positions(asOfDate, what="ARR")
  colnames(QQ)[ncol(QQ)] <- "quantity"
  thisMonth <- as.Date(format(asOfDate, "%Y-%m-01"))
  res <- rbind(res, cbind(asOfDate=asOfDate,
                    subset(QQ, month == as.Date(d))))
  #print(res)
}
rownames(res) <- NULL






dates <- seq(as.Date("2010-01-01"), as.Date("2010-02-01"),
             by="1 day")
res <- rep(NA, length=length(dates))
names(res) <- as.character(dates)
for (d in 1:length(dates)){
  asOfDate <- as.Date(secdb.dateRuleApply(as.Date(dates[d]), "-1b"))
  try(QQ <- .get_cne_positions(asOfDate, what="ARR"))
  if (is.null(QQ))
    next
  colnames(QQ)[ncol(QQ)] <- "quantity"
  #thisMonth <- as.Date(format(asOfDate, "%Y-%m-01"))
  thisMonth <- as.Date("2010-01-01")
  aux <- subset(QQ, delivery.point=="CT" & month == thisMonth)
  print(aux)
  res[d] <- aux$quantity
}







##########################################################################
# Calculate PnL impact after Ling's change
##########################################################################

dir <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/ARR/Data/2010-06-03/"
files <- list.files(dir, full.names=TRUE)

res <- vector("list", length=length(files))
for (f in 1:length(files)){
  rLog("Working on ", files[f])
  aux <- read.csv(files[f], skip=2)
  ind <- grep("Summary", aux[,1])
  aux <- aux[1:(ind-1),]
  aux$status <- "after"
  if (grepl("_bf", files[f]))
    aux$status <- "before"
  res[[f]] <- aux
}
res <- do.call(rbind, res)
res$contract.dt <- as.Date(res$Start.Date, format="%d-%B-%Y")

ARRs <- subset(res, Service.Type=="ARR")

PnL <- cast(ARRs, contract.dt ~ status, sum, fill=NA,
            value="Today.Value")
PnL$PnL <- PnL$after - PnL$before


total <- cast(res, Service.Type ~ status, sum, fill=NA, value="Today.Value")
total$PnL <- total$after - total$before




## ##########################################################################
## # format ARR's for spreadsheet for CNE marks -- not used anymore
## ##########################################################################
## require(SecDb); require(reshape); require(CEGbase)
## source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.nepool.marks.R")
## DIR <- "H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/CNE/ARR/"

## ratio <- read.csv(paste(DIR, "ratio_2010-02-26.csv", sep=""))
## asOfDate <- Sys.Date()
## maxDate  <- as.Date("2018-12-01")
## arr <- pull_arr_nepool(asOfDate, asOfDate, maxDate)
## arr <- scale_arr_prices(arr, ratio)
## res <- cast(arr, month ~ location, I, fill=NA)

## print(res, row.names=FALSE)


