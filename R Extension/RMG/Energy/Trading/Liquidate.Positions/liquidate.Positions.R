################################################################################
# External Libraries
#
require(reshape); require(SecDb); require(RODBC)

source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
source("H:/user/R/RMG/Energy/VaR/Base/aggregate.run.positions.R")


################################################################################
# File Namespace
#
LQ.Positions <- new.env(hash=TRUE)


################################################################################
# 
#
LQ.Positions$main <- function(LQ.options){

  LQ.options$save$datastore.dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/"

  rLog("Get the positions ...")
  QQ  <- LQ.Positions$.loadPositions(LQ.options)
  
  rLog("Read the map of curves to tsdb symbols ...")
  map <- LQ.Positions$.read.tsdb.map()

  rLog("Add the curve attributes (bucket, zone, tsdb symbol) ...")
  QQ <- LQ.Positions$.getCurveAttributes(QQ, map)

  rLog("Read historical forward prices for the curves we have positions on ...")
  hFWD <- LQ.Positions$.read.fwd.prices(QQ, LQ.options)

  rLog("Read historical spot prices ... ")
  hPP <- LQ.Positions$.read.spot.prices(QQ, LQ.options)
  
  rLog("Scale historical spot prices to current forwards ...")
  hPP <- LQ.Positions$scale.spot.prices(hPP, hFWD, QQ, LQ.options)
    
  rLog("Liquidate positions")
  PnL <- LQ.Positions$liquidate.positions(QQ, hPP, LQ.options)

  rLog("Report results")
  LQ.Positions$report.results(QQ, PnL)
  
}

################################################################################
# Report results
#
LQ.Positions$report.results <- function(QQ, PnL){
  
  agg.res <- aggregate(PnL$PnL/1000000, list(year=PnL$year),
                       function(x){round(sum(x),2)})
  names(agg.res)[2] <- "PnL"
  print(agg.res)

  rownames(QQ) <- NULL
  rLog("\nPositions used:")
  print(QQ[,c("curve.name", "contract.dt", "position")])
}

################################################################################
# Liquidate positions 
#
LQ.Positions$liquidate.positions <- function(QQ, hPP, LQ.options){

  nr.days <- melt(table(hPP[, c("curve.name", "year")]))
  names(nr.days)[3] <- "nr.days"
  ind <- which(sapply(nr.days, is.factor))
  nr.days[,ind] <- sapply(nr.days[,ind], as.character)
  
  # calculate daily positions to liquidate
  dQQ <- merge(QQ[, c("curve.name", "position")], nr.days)
  dQQ$daily.position <- dQQ$position/dQQ$nr.days  

  res <- merge(hPP, dQQ)
  res$PnL <- res$daily.position*(res$scaled.spot.price - res$F0)

  res <- aggregate(res$PnL, list(res$curve.name, res$year), sum)
  names(res) <- c("curve.name", "year", "PnL")
  
  return(res)
}

################################################################################
# Rescale spot prices based on forwards.
#
LQ.Positions$scale.spot.prices <- function(hPP, hFWD, QQ, LQ.options)
{
  SF <- merge(hFWD[, c("year", "curve.name", "value")], QQ[, c("curve.name", "F0")])
  names(SF)[3] <- "hist.FWD"

  SF$scale.factor <- SF$F0 / SF$hist.FWD
  SF <- SF[order(SF$curve.name, SF$year), ]
#  SF <- cast(SF, curve.name ~ year, I)
  
  hPP$year <- as.numeric(format(hPP$date, "%Y"))
  hPP <- merge(hPP, SF, all.x=T)
  hPP$scaled.spot.price <- hPP$spot.price * hPP$scale.factor

  ind <- which(is.na(hPP$scaled.spot.price))
  if (length(ind)>0){
    rLog("These curves were not scaled:")
    print(unique(hPP[ind, c("curve.name", "year")]))
  }
  
  return(hPP)
}

################################################################################
# Get historical spot price data.  Return a df with
# curve.name, year, day, price
#
LQ.Positions$.read.spot.prices <- function(QQ, LQ.options)
{
  years <- LQ.options$use.hist.years
  focus.month <- as.POSIXlt(LQ.options$focus.month)$mon + 1  # as numeric
  start.dt <- as.Date(paste(years[1], focus.month, 1, sep="-"))
  end.dt   <- as.Date(paste(tail(years,1), focus.month+1, 2, sep="-"))
  
  hPP <- NULL
  uTsdb <- unique(QQ$tsdb.da)
  for (ind in seq_along(uTsdb)){
    rLog("Pulling ", uTsdb[ind])
    hp <- tsdb.readCurve(uTsdb[ind], start.dt, end.dt)

    aux <- subset(QQ, tsdb.da == uTsdb[ind])
    for (r in 1:nrow(aux)){
      hrs <- secdb.getHoursInBucket(aux$iso[r], aux$bucket[r], start.dt, end.dt)       
      hp.bkt  <- merge(hp, hrs)
      hp.bkt$date <- as.Date(hp.bkt$time)
      month <- as.POSIXlt(hp.bkt$time)$mon + 1     # as numeric 
      hp.bkt <- subset(hp.bkt, month == focus.month)
      
      hp.bkt <- aggregate(hp.bkt$value, list(date=hp.bkt$date), mean)
      names(hp.bkt)[2] <- "spot.price"
      hp.bkt <- data.frame(curve.name=aux$curve.name[r], hp.bkt)
      
      hPP <- rbind(hPP, hp.bkt)  # append this bucket, this curve
    }
  }
  
  return(hPP)
}

################################################################################
# Get historical forward price data
#
LQ.Positions$.read.fwd.prices <- function(QQ, LQ.options)
{
  hPP <- NULL
  focus.month <- as.POSIXlt(LQ.options$focus.month)$mon + 1  # as numeric
  asOfMonth   <- as.POSIXlt(LQ.options$asOfDate)$mon + 1     # as numeric 
  asOfDay <- as.POSIXlt(LQ.options$asOfDate)$mday
  
  for (year in LQ.options$use.hist.years){ # loop over the years of interest
    pricing.dt  <- as.Date(paste(year, asOfMonth, asOfDay, sep="-"))
    pricing.dt  <- as.Date(secdb.dateRuleApply(pricing.dt, "-1b"))
    contract.dt <- as.Date(paste(year, focus.month, 1, sep="-")) 

    hPP <- rbind(hPP, data.frame(year=year, 
      tsdb.futStrip(QQ$curve.name, pricing.dt, pricing.dt, contract.dt)))
  }
  
  return(hPP)
}

################################################################################
# Load positions
#
LQ.Positions$.loadPositions <- function(LQ.options)
{
  run.all <-  get.portfolio.book(LQ.options)
  LQ.options$run$specification <- subset(run.all,
    RUN_NAME == toupper(LQ.options$run$run.name))
  if (nrow(LQ.options$run$specification)==0){
    rLog("Could not find the run.name in the portfolio hierarchy.")
  }
  
  # from VaR/Base/aggregate.run.positions.R
  QQ <- .getDeltas(LQ.options$run, LQ.options)  
  QQ$vol.type <- NULL
  
  # add current day prices
  filename <- paste(LQ.options$save$datastore.dir, "Prices/hPrices.",
    LQ.options$asOfDate, ".RData", sep="")
  load(filename)
  QQ <- merge(QQ, hP, all.x=T)
  names(QQ)[4] <- "F0"           # asOfDate forward prices  

  # subset your positions to the focus month
  QQ <- subset(QQ, contract.dt == LQ.options$focus.month)
  rownames(QQ) <- NULL
  
  return(QQ)
}

################################################################################
# Map the positions to a given bucket and tsdb symbol
#
LQ.Positions$.getCurveAttributes <- function(QQ, map){

  aux <- strsplit(QQ$curve.name, " ")
  QQ$bucket <- sapply(aux, function(x){x[3]})
  ind <- which(QQ$bucket %in% ("EXCHANGE"))
  if (length(ind)>0){QQ$bucket[ind] <- "7X24"}

  QQ$location <- sapply(aux, function(x){x[4]})
  ind <- which(QQ$location %in% ("PHYSICAL"))
  if (length(ind)>0){QQ$location[ind] <- NA}

  QQ$market <- sapply(aux, function(x){x[2]})

  QQ <- merge(QQ, map, all.x=T)  # put them together

  ind <- which(is.na(QQ[,c("tsdb.da")]))
  if (length(ind)>0){
     rLog("Did not know how to map these positions:")
     print(QQ[ind,])
     rLog("Dopping them.")
     QQ <- QQ[-ind,]
  }

  
  
  return(QQ)
}


################################################################################
# Read tsdb map xls
#
LQ.Positions$.read.tsdb.map <- function(){

  file  <- "H:/user/R/RMG/Energy/Trading/Liquidate.Positions/tsdb.table.xls"
  con    <- odbcConnectExcel(file)
  map   <- sqlFetch(con, "Sheet1")
  odbcCloseAll()

  names(map) <- gsub("#", ".", names(map))
  ind <- which(sapply(map, is.factor))
  map[,ind] <- sapply(map[,ind], as.character)

  return(map)
}


################################################################################
# Map a market to an ISO name, eg. PWX to NEPOOL, etc. 
#
.map.ISO <- function(market){
#                                    TO FIX!
# maybe make this a general data set in the SecDB package. 
#  
  ref.map <- data.frame()
  res <- switch(market,
                PWJ = "PJM",
                PWX = "NEPOOL",
                PWY = "NYPP")
  
}
