# Get NEPOOL positions from PRISM.  It is customized for NEPOOL. 
#
# delivery.point=="PTF" is HUB before SMD

require(CEGbase); require(PM); require(reshape)
Sys.setenv(tz="")
dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
load(paste(dir, "positionsAAD_2011-04-28.RData", sep=""))
load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

# For the FTR presentation position 
aux <- subset(QQ, contract.dt <= as.Date("2015-12-01") &
                  contract.dt >= as.Date("2011-05-01") &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                    "NEPOOL MTM OTHER HEDGES", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )

# High level view  
aux <- subset(QQ, contract.dt <= as.Date("2014-12-01") &
                  contract.dt >= as.Date("2011-03-01") &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                    "NEPOOL MTM OTHER HEDGES", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )

#bux=subset(aux, grepl("COMMOD PWX .* SEMA PHYSICAL", aux$curve))
#bux <- subset(bux, delta > 0); bux <- bux[order(-bux$delta),]

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=TRUE, mw=TRUE)
#res <- cast(aux.zone, delivery.point ~ bucket + zone.name, I, fill=0)
aux.zone <- subset(aux.zone, zone.name %in% c("SEMA"))
cast(aux.zone, delivery.point + bucket ~ contract.dt, I, fill=0)

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=FALSE, mw=TRUE)
res <- cast(aux.zone,  contract.dt ~ bucket + zone.name, I, fill=0)
res[, c(1, grep("CT|NEMA|SEMA", colnames(res)))]

# find a trade/position by ETI 
fux <- aux[grep(" SEMA ", aux$curve), ]
fux <- aux[grep(" SEMA ", aux$curve), ]
fux <- cast(fux, delivery.point + eti + curve ~ contract.dt, sum, fill=0, value="delta")


###############################################################################
# MY position 
aux <- subset(QQ, contract.dt <= as.Date("2010-05-01") &
                  contract.dt >= as.Date("2010-05-01") &
                  portfolio %in% c("NEPOOL CONGESTION PORTFOLIO", "NPFTRMT1",
                                   "NESTAT", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, mw=TRUE)
res <- cast(aux.zone, contract.dt ~ bucket + zone.name, I, fill=0)
res[,2:ncol(res)] <- round(res[,2:ncol(res)])

print(res[,c(1, grep("SEMA", names(res)))])

# find a trade/position by ETI 
fux <- aux[grep(" MAINE ", aux$curve), ]
fux <- cast(fux, delivery.point + eti + curve ~ contract.dt, sum, fill=0, value="delta")

tux <- subset(QQ, eti=="PZS01GQ")
cast(tux, delivery.point + curve ~ contract.dt, sum, fill=0, value="delta")


###############################################################################
# one load deal only ...
aux <- subset(QQ, eti == "PZSFEMV" & portfolio == "NEPOOL ACCRUAL PORTFOLIO")
aux <- subset(aux, contract.dt == as.Date("2010-10-01"))
dim(aux)


###############################################################################
# one month/zone only ... 
aux <- subset(QQ, portfolio == "NEPOOL ACCRUAL PORTFOLIO")
aux <- subset(aux, contract.dt == as.Date("2010-10-01") & delivery.point == "MAINE")
aux <-
  
dim(aux)

 
  bux <- getPositionsZone(aux, NEPOOL$MAP, subset=TRUE) # mw=TRUE
 

###############################################################################
# check CNE blocks?!

aux <- subset(QQ, trading.book %in% c("NEBKNEM", "NEBKNEP", "NEBKNES"))
aux <- subset(aux, delivery.point=="MAINE")
QQ.delivPt <- getPositionsDeliveryPoint(aux, subset=TRUE,
  buckets=c("2X16H", "5X16", "7X8"))


###############################################################################
# Units position in PGD
#
aux <- subset(QQ, contract.dt <= as.Date("2014-12-01") &
                  contract.dt >= as.Date("2011-05-01") &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO") &
                  trading.book %in% c("NPKLEEN", "MYSTC89M", "FORERIVM", "NEPGDMOD") & 
                  mkt == "PWX"
              )

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=TRUE, mw=TRUE)
cast(aux.zone, contract.dt ~ bucket, I, fill=NA, subset=delivery.point=="KLN")


###############################################################################
# positions from FP portfolio
#
aux <- subset(QQ, contract.dt <= as.Date("2014-12-01") &
                  contract.dt >= as.Date("2011-05-01") &
                  portfolio %in% c("NEPOOL FP PORTFOLIO") &
                  mkt == "PWX"
              )

aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, showDeliveryPoint=TRUE, mw=TRUE)
cast(aux.zone, contract.dt ~ bucket, sum, fill=NA, subset=zone.name=="CT")




###############################################################################
# my NY position from BUSHRA
#
aux <- subset(QQ, contract.dt <= as.Date("2012-12-01") &
                  contract.dt >= as.Date("2009-09-01") &
                  portfolio %in% c("NEPOOL CONGESTION PORTFOLIO", "NPFTRMT1",
                                   "NESTAT") &
                  mkt == "PWY"
              )

#A1 <- cast(aux, contract.dt ~ delivery.point + bucket, sum, fill=0, value="delta")
A2 <- cast(aux, contract.dt ~ delivery.point, sum, fill=0, value="delta")
#A3 <- cast(aux, contract.dt ~ ., sum, fill=0, value="delta")



aux.zone <- getPositionsZone(aux, MAP, subset=TRUE, mw=TRUE)
res <- cast(aux.zone, contract.dt ~ bucket + zone.name, I, fill=0)
res[,2:ncol(res)] <- round(res[,2:ncol(res)])

# find a trade/position by ETI 
fux <- aux[grep(" HI A", aux$delivery.point), ]
fux <- cast(fux, delivery.point + eti + curve ~ contract.dt, sum, fill=0, value="delta")

tux <- subset(QQ, eti=="PZPD1UQ")
cast(tux, delivery.point + curve ~ contract.dt, sum, fill=0, value="delta")




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
# get SC positions
aux <- subset(QQ, contract.dt <= as.Date("2014-12-01") &
                  contract.dt >= as.Date("2011-03-01") &
                  portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO",
                    "NEPOOL MTM OTHER HEDGES", "PM EAST RETAIL PORTFOLIO") &
                  mkt == "PWX"
              )
aux <- subset(aux, grepl(" SC ", aux$curve))
unique(aux$eti)

unique(aux[,c("trading.book", "counterparty", "eti")])

dd <- cast(aux, trading.book + counterparty + eti ~ ., sum, value="delta")
dd

LL <- secdb.getLoadObject("cincap", "System")
LL$year <- format(LL$time, "%Y")
cast(LL, year ~ ., summary)
LL <- subset(LL, time >= Sys.time())
plot(LL$time, LL$value)  # weird stairstep but that's how it is!



###############################################################################
aux <- subset(QQ, #contract.dt==as.Date("2009-04-01") &
                  region == "NEPOOL" &
#                  bucket == "5X16"   &
                  service == "ENERGY"
             )
#aux <- aux[aux$curve %like% " SEMA ", ]
aux <- subset(aux, portfolio %in% c("NEPOOL ACCRUAL PORTFOLIO"))
sort(unique(aux$delivery.point))
sort(unique(QQ$counterparty))

aux <- subset(QQ, trading.book == "NSBHEBAR")

bux <- subset(aux, grepl("CL8", aux$delivery.point))
bux <- subset(aux, grepl("BUCKSPRT.4140 DA", aux$delivery.point))
bux <- subset(aux, grepl("MAINE-SC", aux$delivery.point) & trading.book == "NSSCHED1")

unique(bux$eti)

unique(bux$trading.book)





aux.dp <- getPositionsDeliveryPoint(aux, subset=TRUE)

# load the NEPOOL map
load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
aux.zone <- getPositionsZone(aux, MAP, subset=TRUE)



aggregate(aux$delta, list(aux$contract.dt), sum)


#############################################################################
# get CT positions

load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")  # NEPOOL$MAP

dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
load(paste(dir, "positionsAAD_2009-04-10.RData", sep=""))

aux <- subset(QQ,
              portfolio %in% c("NEPOOL CONGESTION PORTFOLIO",
                               "NEPOOL FP PORTFOLIO",
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
#  FOR MIKE ADAMS
#############################################################################

require(SecDb); require(reshape); require(CEGbase); require(PM)
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

#pfolio <- "NEPOOL FP PORTFOLIO"
#books <- .getPortfolioBooksFromSecDb( pfolio )
#books <- "NPFTRMT1" #"NSBHEBAR"
books <- "NEBKNEM"

aux <- get.positions.from.blackbird( asOfDate=asOfDate, books=books,
   fix=TRUE, service=NULL)

QQ <- cbind(portfolio=pfolio, aux)
QQ <- subset(QQ, !(curve %in% c("USD", "CAD")))

QQ.delivPt <- getPositionsDeliveryPoint(QQ, subset=TRUE,
  buckets=c("2X16H", "5X16", "7X8"))

write.csv(QQ.delivPt, file=paste("C:/Temp/positions_fp_", asOfDate,
                        ".csv", sep=""), row.names=FALSE)





QQ <- subset(aux, delivery.point=="MAINE")
QQ.delivPt <- getPositionsDeliveryPoint(QQ, subset=TRUE,
  buckets=c("2X16H", "5X16", "7X8"))






#############################################################################
#                 LOW LEVEL & TESTS  
#############################################################################
require(SecDb); require(reshape); require(CEGbase); require(PM)
Sys.setenv(tz="")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")
source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))

# Get positions for a load serving deal
aux <- get.positions.from.blackbird( asOfDate=asOfDate, books="NSCMPSOS")
aux <- subset(aux, eti=="PZSFEMV" & contract.dt==as.Date("2010-10-01"))




# Get positions 

# get the gen positions ... 
#portfolio <- "NEPOOL GEN PORTFOLIO"
#portfolio <- "PM EAST PORTFOLIO"
#portfolio <- "NEPOOL ACCRUAL PORTFOLIO"  # very slow to do it like this ... 
portfolio <- "NEPOOL FP PORTFOLIO"
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
load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

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

## #source("H:/user/R/RMG/Utilities/Interfaces/PM/R/libGetPositions.R")

## require(CEGbase); require(PM); require(reshape); require(SecDb)
## HRS <- ntplot.bucketHours(c("5X16", "2X16H", "7X8"), region="NEPOOL")

## # load the NEPOOL map
## load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

## dir <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
## load(paste(dir, "positionsAAD_2009-02-24.RData", sep=""))

## QQ$value <- QQ$delta
## QQ <- subset(QQ, service=="ENERGY" & mkt=="PWX")

## aux <- aggregate(QQ$value, list(mkt=QQ$mkt, delivery.point=QQ$delivery.point,
##   bucket=QQ$bucket, contract.dt=QQ$contract.dt), sum, na.rm=TRUE)

## bux <- subset(QQ, contract.dt >= as.Date("2017-12-01") & mkt == "PWY")


## QQ.delivPt <- getPositionsDeliveryPoint(QQ, subset=TRUE)



## QQ.zone <- getPositionsZone(QQ, MAP, subset=TRUE)
## QQ.zone.agg <- aggregateByReportingPeriod(QQ.zone)

## plotPositionsZone(QQ.zone.agg, plotName="zonal")



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
