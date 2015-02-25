# Calculate the PnL effect due to ARRs change in marks
# 
# "S:/All/Structured Risk/NEPOOL/Ancillary Services/",
#  "ARR Valuation/Sent/"
#


require(CEGbase)
require(reshape)
require(zoo)
require(SecDb)
require(FTR)
require(PM)         # PM/R/libMarkARRs.R   
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libMarkARRs.R")


date1 <- as.Date("2010-01-29") #as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
date2 <- as.Date(secdb.dateRuleApply(Sys.Date(), "-0b"))

## get CNE ARR positions
## QQ <- .get_cne_positions(date1, what="ARR")
## colnames(QQ)[ncol(QQ)] <- "quantity"

# get CCG ARR positions
QQ <- .get_ccg_arr_positions(date1)
colnames(QQ)[which(colnames(QQ)=="contract.dt")] <- "month"
cast(QQ, delivery.point ~ ., sum, fill=0, value="quantity")



PnL <- get.ARR.PnL(date1, date2, QQ)


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

