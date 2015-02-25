# Get NEPOOL positions from PRISM.  It is customized for NEPOOL. 
#
# delivery.point=="PTF" is HUB before SMD

require(CEGbase); require(PM); require(reshape)
Sys.setenv(tz="")
dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
load(paste(dir, "positionsAAD_2009-06-24.RData", sep=""))
load("S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/RData/map.RData")

# For the FTR presentation position 
aux <- subset(QQ, contract.dt <= as.Date("2009-12-01") &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                    "NEPOOL MTM OTHER HEDGES", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=TRUE)
res <- cast(aux.zone, zone.name + delivery.point ~ bucket, I, fill=0)

subset(res, zone.name %in% c("VT", "SEMA"))

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=FALSE, mw=TRUE)
res <- cast(aux.zone, zone.name ~ bucket, I, fill=0)



###############################################################################
# MY position 
aux <- subset(QQ, contract.dt <= as.Date("2009-12-01") &
#                  contract.dt > Sys.Date() &
                  portfolio %in% c("NEPOOL CONGESTION PORTFOLIO", "NPFTRMT1",
                                   "NESTAT", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )
aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, mw=TRUE)
res <- cast(aux.zone, contract.dt ~ bucket + zone.name, I, fill=0)
res[,2:ncol(res)] <- round(res[,2:ncol(res)])
print(res[,c(1, grep("CT", names(res)))])




###############################################################################
# get attrition only 
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
books <- .getPortfolioBooksFromSecDb( "Nepool Mixed Attr Portfolio" )
aux <- subset(QQ, contract.dt <= as.Date("2009-12-01") &
                  contract.dt > Sys.Date() &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO") &
                  trading.book %in% books & 
                  mkt == "PWX"
              )





###############################################################################
aux <- subset(QQ, contract.dt==as.Date("2009-04-01") &
                  region == "NEPOOL" &
#                  bucket == "5X16"   &
                  service == "ENERGY"
             )
#aux <- aux[aux$curve %like% " SEMA ", ]
aux <- subset(aux, portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                                    "PM EAST RETAIL PORTFOLIO"))

aux.dp <- getPositionsDeliveryPoint(aux, subset=TRUE)

# load the NEPOOL map
load("S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/RData/map.RData")
aux.zone <- getPositionsZone(aux, MAP, subset=TRUE)



aggregate(aux$delta, list(aux$contract.dt), sum)


#############################################################################
# get CT positions

load("S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/RData/map.RData")  # NEPOOL$MAP

dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
load(paste(dir, "positionsAAD_2009-04-10.RData", sep=""))

aux <- subset(QQ,
              portfolio %in% c("NEPOOL CONGESTION PORTFOLIO", "NEPOOL FP PORTFOLIO",
                               "NPFTRMT1") &
              mkt =="PWX"
             )
aux <- aux[aux$curve %like% " CT ", ]

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, buckets="")
#aux.zone$bucket <- factor(aux.zone$bucket, levels=c("5X16", "2X16H", "7X8"))
cast(aux.zone, contract.dt ~ bucket, I, fill=0)


aux.dp <- getPositionsDeliveryPoint(aux, subset=TRUE)



#############################################################################
#############################################################################
#source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

require(CEGbase); require(PM); require(reshape); require(SecDb)
HRS <- ntplot.bucketHours(c("5X16", "2X16H", "7X8"), region="NEPOOL")

# load the NEPOOL map
load("S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/RData/map.RData")

dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
load(paste(dir, "positionsAAD_2009-02-24.RData", sep=""))

QQ$value <- QQ$delta
QQ <- subset(QQ, service=="ENERGY" & mkt=="PWX")

aux <- aggregate(QQ$value, list(mkt=QQ$mkt, delivery.point=QQ$delivery.point,
  bucket=QQ$bucket, contract.dt=QQ$contract.dt), sum, na.rm=TRUE)

bux <- subset(QQ, contract.dt >= as.Date("2017-12-01") & mkt == "PWY")


QQ.delivPt <- getPositionsDeliveryPoint(QQ, subset=TRUE)



QQ.zone <- getPositionsZone(QQ, MAP, subset=TRUE)
QQ.zone.agg <- aggregateByReportingPeriod(QQ.zone)

plotPositionsZone(QQ.zone.agg, plotName="zonal")





#############################################################################
#
#
require(SecDb); require(reshape); require(CEGbase); require(PM)
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

# get the gen positions ... 
#portfolio <- "NEPOOL GEN PORTFOLIO"
#portfolio <- "PM EAST PORTFOLIO"
#portfolio <- "NEPOOL ACCRUAL PORTFOLIO"  # very slow to do it like this ... 
tree <- .getPortfolioTreeFromSecDb(portfolio, tree=list())
#tree <- tree[c(2,3)]

QQ <- list()
for (p in seq_along(names(tree))){
  pName <- toupper(names(tree)[p])
  rLog("Getting ", pName)
  books <- .getPortfolioBooksFromSecDb( pName )
  aux   <- get.positions.from.blackbird( asOfDate=asOfDate, books=books)
  aux   <- cbind(portfolio=pName, aux)
  QQ[[pName]] <- subset(aux, CURVE != "USD")
}
QQ <- do.call(rbind, QQ)
rownames(QQ) <- NULL
colnames(QQ) <- gsub("_", ".", tolower(colnames(QQ)))
QQ <- subset(QQ, contract.date >= as.POSIXct(asOfDate))
QQ <- subset(QQ, curve != "CAD")
names(QQ)[which(names(QQ)=="contract.date")] <- "contract.dt"
QQ$contract.dt <- as.Date(QQ$contract.dt)
names(QQ)[which(names(QQ)=="qty.bucket.type")] <- "bucket"

QQ$value <- QQ$delta
save(QQ, file="C:/Temp/positions.RData")



HRS <- ntplot.bucketHours(c("5X16", "2X16H", "7X8"), region="NEPOOL")

# load the NEPOOL map
load("S:/All/Risk/Data/FTRs/NEPOOL/ISO/DB/RData/map.RData")

#-----------------------------------------------------------------
# aggregate deltas by delivery.pt, contract.dt, bucket 
QQ$mkt <- sapply(strsplit(QQ$curve, " "), function(x){x[2]})
subset <- QQ$service=="ENERGY" & QQ$mkt=="PWX" & 
  QQ$contract.dt==as.Date("2009-03-01")




QQ.zone <- getPositionsZone(QQ, MAP, subset=subset)

QQ.zone <- getPositionsZone(QQ, MAP, subset=TRUE)


QQ.zone <- aggregateByReportingPeriod(QQ.zone)

setwd("C:/Temp/")
QQ.zone$value <- QQ.zone$value/1000
plotPositionsZone(QQ.zone, xlab="Position in 1000 MWh",
  main=paste("Nepool Accrual + Nepool MTM HOA, as of", asOfDate))


require(lattice)




# not that good view 
xyplot(value/1000 ~ contract.dt | zone.id, groups=bucket, data=QQ.zone,
       scales="free", type="o")




#-----------------------------------------------------------------
# aggregate deltas by curve, contract.dt, bucket 
res <- cast(QQ, curve + contract.date + qty.bucket.type  ~.,
            sum, subset=(service=="ENERGY"))
names(res)[2:4] <- c("contract.dt", "class.type", "value")
res <- subset(res, contract.dt< as.POSIXct("2011-01-01"))

CT <- data.frame(res[res$curve %like% " CT ", ])
CT$contract.dt <- as.Date(CT$contract.dt)

aux <- merge(CT, HRS, all.x=TRUE)
aux$value <- aux$value/aux$hours     # in MW!

aux <- cast(aux, contract.dt ~ class.type, I, fill=NA)
rownames(aux) <- as.character(aux$contract.dt)
aux <- round(aux[,-1])












# aggregate deltas by delivery point.
res <- cast(QQ, portfolio + contract.date + curve + qty.bucket.type +
  delivery.point ~., sum, subset=(service=="ENERGY"))


require(FTR)
FTR.load.ISO.env("NEPOOL")
head(NEPOOL$MAP)

# add class.type PEAK/OFFPEAK
QQ$qty.bucket.type <- as.character(QQ$qty.bucket.type)
bucketTable <- data.frame(
  qty.bucket.type=c("2X16H", "5X16", "7X24", "7X8"),
  class.type=c("OFFPEAK", "ONPEAK", "FLAT", "OFFPEAK"))
QQ <- merge(QQ, bucketTable, all.x=TRUE)

# add location: remove DA/RT and MCC from delivery.pt


# aggregate by PEAK/OFFPEAK, delivery point
res <- cast(QQ, delivery.point + contract.date + class.type  ~.,
  sum, subset=(service=="ENERGY"))



  aux   <- get.positions.from.blackbird( asOfDate=asOfDate, books="NESTAT")
  aux   <- get.positions.from.blackbird( asOfDate=asOfDate, books="NESTAT")

  aux   <- get.positions.from.blackbird( asOfDate=asOfDate, books="NESTAT")


##################3

bux <- aux[aux$curve %like% "SEMA", ]
cux <- aggregate(bux$delta, list(bux$eti), sum)
cux <- cux[order(-cux$x),]
