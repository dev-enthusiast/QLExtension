# Utilities for different parts of the monthly FTR presentation
#
#
# make.CERpositions.table   - one month only
# make.CERpositions.table.secdb - one month only
# make.Constraints.table    - pull historical DA constraints
# make.DART.stats           - historical DART averages for the month
# make.FTR.PnL              - calculate historical PnL for a given auction
# make.GenOutage.table      - data ready for presentation
# make.HistoricalFTR.page   - pull historical prices for given paths
# make.HourlyLoads          -
# make.HourlyOTC            - 
# make.HR.plot              - show historical HR plots for the given month 
# make.Interfaces.table     - show historical flows accross Interfaces
# make.LoadTemperature.plot - by month
# plot.HistLoad             
# plot.LoadGenHedges        - by hour
#
# .read.bids.xlsx           - read the xlsx file with bids that I prepare by hand
#
# To download the pdf pages:
# 1) enter into NEPOOL SMD
# 2) Change the month to the Auction month you need
# 3) Take the "FTR Market" link from the main panel.  (Or, try this web site:
#    https://smd.iso-ne.com/ftr_markets/)
#
#


##################################################################
# Get the CER positions, for ONE month only 
# NOTE:
#  COG:Hub = R:Hub + R:.Ros
#
# month can be a vector of months too, for the annual auction
#
make.CERpositions.table <- function(asOfDate, month, hours, mw=TRUE)
{
  source("H:/user/R/RMG/Energy/VaR/Base/get.portfolio.book.R")
  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.blackbird.R")

  fileIn <- paste("S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/",
    "positionsAAD_", asOfDate, ".RData", sep="")
  load(fileIn)

  QQ <- subset(QQ, mkt=="PWX")
  QQ <- subset(QQ, contract.dt %in% month)
  
  whatToPull <- c(
    "Nepool Accrual Portfolio",
    "Nepool MTM Hedges of Accrual",              
    "Nepool Mixed Attr Portfolio",
    "PM East Retail Portfolio"               
  )
  res <- vector("list", length=length(whatToPull))
  names(res) <- toupper(whatToPull)
  for (w in whatToPull){
    what <- toupper(w)
    if (what %in% unique(QQ$portfolio)){  # if I pull it directly ... 
      qq <- subset(QQ, portfolio==what) 
    } else {                              # else it should be in NEPOOL ACCRUAL
      books <- .getPortfolioBooksFromSecDb( what )
      qq    <- subset(QQ, portfolio == "NEPOOL ACCRUAL PORTFOLIO")
      qq    <- subset(qq, trading.book %in% books)
    }   

    ind <- grep(" UCAP ", qq$curve)
    if (length(ind)>0)
      qq <- qq[-ind,]
    
    aux <- getPositionsZone(qq, NEPOOL$MAP, subset=TRUE) # mw=TRUE
    res[[what]] <- cbind(pfolio=what, aux)
    #aux$bucket <- factor(aux$bucket, levels=c("PEAK", "OFFPEAK"))
    ## aux <- cast(aux[,c("zone.name", "bucket", "value")], ... ~ bucket,
    ##             I, fill=NA)
    ## rLog("\n\nPortfolio:", what)
    ## print(aux, row.names=FALSE)
  }  

  res <- do.call(rbind, res)
  rownames(res) <- NULL
  if (!identical(unique(res$bucket), c("OFFPEAK", "PEAK"))){
    rLog("You've got some other buckets in here!")
    ind.7X24 <- grepl("7X24", res$bucket)
    if (any(ind.7X24)){
     rLog("Splitting 7X24 into peak & offpeak!")
     aux <- res[ind.7X24,]; aux$bucket <- NULL
      res <- res[-which(ind.7X24),]
      res <- rbind(res, expand.grid.df(aux, data.frame(bucket=c("OFFPEAK", "PEAK"))))
    }
  }

  if (mw == TRUE){      # put hours to calculate the MW's
    res$bucket <- gsub("^PEAK", "ONPEAK", res$bucket)
    HRS <- subset(NEPOOL$HRS, contract.dt %in% month)
    names(HRS)[which(names(HRS)=="class.type")] <- "bucket"
    res <- merge(res, HRS, by=c("contract.dt", "bucket"))
    
    res$value  <- res$value/res$hours
    
    res <- cast(res, contract.dt + zone.name ~ pfolio + bucket, I, fill=0)
    res[,3:ncol(res)] <- round(res[,3:ncol(res)])
  }
  
  
  return(res)
}


##################################################################
# Get the CER positions, for ONE month only 
# NOTE:
#  COG:Hub = R:Hub + R:.Ros
#
#
make.CERpositions.table.secdb <- function(asOfDate, month)
{
  # x is a list of curve 
  .format_one_pfolio <- function(x) {
    x <- x[sapply(x, length) > 0]  # there are some empty positions
    qq <- ldply(x, function(z){subset(z, date %in% month)})
    colnames(qq)[1:2] <- c("curvename", "contract.dt")

    qq <- cbind(qq, PM:::classifyCurveName(toupper(qq$curvename)))
    qq <- subset(qq, market == "PWX")
    qq <- subset(qq, !grepl("UCAP", qq$curvename))  # take out some UCAP curves
    qq <- subset(qq, !(location %in% c("ISOFEE", "VLRCST")))
    
    qq$bucket <- qq$pkoffpk
    qq$pkoffpk <- NULL
    qq <- cast(qq, contract.dt + location + bucket ~ ., sum)  # aggregate the 2x16H and 7x8
    qq$bucket <- gsub("^PEAK", "ONPEAK", qq$bucket)

        if (is.null(NEPOOL$HRS))
          NEPOOL$HRS <- FTR.bucketHours("NEPOOL", end.dt=as.POSIXlt("2017-12-31 23:00:00"))
        HRS <- subset(NEPOOL$HRS, contract.dt %in% month)
        names(HRS)[which(names(HRS)=="class.type")] <- "bucket"
        qq <- merge(qq, HRS, by=c("contract.dt", "bucket"))
        qq$mw <- qq[,"(all)"]/qq$hours

    qq$location <- gsub("^BOS$", "NEMA", qq$location)
    qq$location <- gsub("^MAHUB$", "Hub", qq$location)

    qq
  }
  
  whatToPull <- c(
    "Nepool Gen Portfolio",
    "Nepool Load Portfolio",              
    "Nepool Hedge Portfolio",              
    "CNE NewEngland Portfolio",
    "Nepool Basis Portfolio",
    "PM NorthEast Portfolio"
  )
  
  res <- vector("list", length=length(whatToPull))
  names(res) <- toupper(whatToPull)
  for (w in whatToPull){
    what <- toupper(w)

    x <-  secdb.readDeltas(asOfDate, what)
    res[[what]] <- cbind(pfolio=what, .format_one_pfolio(x))
  }  
  res <- do.call(rbind, res)
  rownames(res) <- NULL

  res <- cast(res, contract.dt + location ~ pfolio + bucket, I, fill=0, value="mw")
  res[,3:ncol(res)] <- round(res[,3:ncol(res)])

  res
}

##################################################################
# make a frequency table of the binding constraints
#
make.Constraints.table <- function(auction, ISO.env, options)
{  
  start.Mon <- options$similar.months[length(options$similar.months)]
  
  BC <- FTR.getBindingConstraints(region="NEPOOL", type="DA",
                                  start.dt=start.Mon)
  BC$month <- as.Date(format(BC$time, "%Y-%m-01"))
  BC <- subset(BC, month %in% options$similar.months)
  BC$value <- 1

  res <- cast(BC, constraint.name ~ month, sum)
  
  # aux <- subset(BC, month %in% options$similar.months[1:3])
  gt  <- cast(BC, constraint.name ~ ., sum) # grand total
  res <- res[order(-gt[,"(all)"]),]

  # pick top 15 most active from the last 3 months
  last3m <- cast(BC, constraint.name ~ ., sum,
    subset=month %in% options$similar.months[1:3])
  resLast3m <- last3m[order(-last3m[,"(all)"]),]
  resLast3m <- resLast3m[-grep("Export|Import", resLast3m$constraint.name),]
  resLast3m <- data.frame(constraint.name=resLast3m$constraint[1:15])

  # pick 15 of the most active, not in the last 3 months
  resOverall <- subset(res, !(constraint.name %in% resLast3m$constraint))
  
  res <- rbind(
    subset(res, constraint.name %in% resLast3m$constraint),
    resOverall[1:15,])    
  
   #  names(res)[-1] <- format(as.Date(names(res)[-1]), "%b%y")
  print(res)
  
  return(invisible(res))  
}


##################################################################
# Calculate DART averages for the month 
#
make.DART.stats <- function(month)
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.dart.R")
  thisMonth <- month
  
  asOfDate  <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
  firstYear <- as.numeric(format(Sys.Date(), "%Y"))-5
  startTime <- as.POSIXct(paste(firstYear,"-01-01 01:00:00", sep=""))
  endTime   <- Sys.time()                          # for hist data 

  HH <- .get_dart_hist(startTime, endTime)

  aux <- PM:::filterBucket(HH[, "dart", drop=FALSE], buckets=c("5X16", "2X16H", "7X8"))
  MM <- lapply(aux,
    function(x){aggregate(x, as.Date(format(index(x)-1, "%Y-%m-01")), mean, na.rm=TRUE)})
  
  dart <- lapply(MM, .format_mon_year_table)
  dart <- melt(dart)
  colnames(dart)[ncol(dart)] <- "bucket"
  
  res <- cast(subset(dart, month==format(thisMonth, "%b")), bucket ~ year,
    I, fill=NA, subset=year!=as.numeric(format(thisMonth, "%Y")))

  res
}


##################################################################
# Calculate the PnL for the FTR book between a start/end date.
# @param auctions a vector of auction names
# @param startOnDt an R Date to start calculating the PnL from
# @param endOnDt an R Date to end calculating the PnL from
#
# For example, if startOnDt/endOnDt are NULL, you calculate the PnL
# of an annual FTR from the trade start date.  If you specify a
# startOnDt/endOnDt, you'll calculate the PnL for that period only!
#
make.FTR.PnL <- function(auctions=c("K11", "F11-1Y"), startOnDt=NULL, endOnDt=NULL)
{
  aux <- get_ftr_trades(auctions)  # from lib.NEPOOL.FTR
  
  res <- ddply(aux, .(auction, eti), function(x){
    #rLog("Working on eti:", x$eti)
    if (nrow(x) != 1)
      stop("Wrong etis?!")
    x$path.no <- 1:nrow(x)
    startDt <- if (is.null(startOnDt)) {
        as.POSIXct(paste(format(x$start.dt), "01:00:00"))[1]
      } else {
        as.POSIXct(paste(format(startOnDt), "01:00:00"))[1]
      }
    endDt <- if (is.null(startOnDt)) {
        as.POSIXct(paste(format(x$end.dt+1), "00:00:00"))[1]
      } else {
        as.POSIXct(paste(format(endOnDt+1), "00:00:00"))[1]
      }
    paths <- x[,c("path.no", "source.ptid", "sink.ptid", "class.type")]

    y <- FTR.getPathReturns(paths, startDt, endDt, ISO.env=NEPOOL, melt=FALSE,
                            hourly=TRUE)
    SP  <- apply(y, 2, mean, na.rm=TRUE)
    hrs <- apply(y, 2, function(z){length(which(!is.na(z)))})
    res <- data.frame(path.no=as.numeric(names(SP)), settle.price=SP, hours=hrs)
    
    x <- merge(x, res, by="path.no", all.x=TRUE)  # add the PnL column
    x$path.no <- NULL
    x
  })
  res$PnL <- res$mw*res$hours*(res$settle.price - res$fix.price)

  res <- cbind(res, FTR.PathNamesToPTID(res[,c("source.ptid", "sink.ptid")],
    NEPOOL$MAP, to="name"))

  z <- res[,c("eti", "book", "auction", "class.type", "source.name",
    "sink.name", "mw", "settle.price", "fix.price", "PnL")]

  z <- ddply(z, c("auction"), function(x){x[order(-x$PnL),]})
  
  z
}


##################################################################
# select spreads and show a table with the realize congestion.
#
#
make.HistoricalFTR.page <- function(auction, ISO.env, options)
{
  paths <- data.frame(
    source.ptid=c(1616, 4000, 4000, 4000, 4000, 4000, 4000, 4001),
    sink.ptid  =c(4008, 4006, 4004, 4005, 4008, 4007, 4011, 4000))
  paths <- cbind(path.no=1:(2*nrow(paths)), 
    rbind(paths, paths),
    class.type=rep(c("ONPEAK", "OFFPEAK"), each=nrow(paths)))
  paths <- merge(paths, FTR.PathNamesToPTID(paths, ISO.env$MAP, to="name"),
                 all.x=TRUE)
  N <- nrow(paths)/2             # how many paths ... 

  # get the EXACT settles
  RR <- FTR.getPathReturns(paths, start.dt=min(options$similar.months),
    end.dt=Sys.Date(), ISO.env=ISO.env, hourly=TRUE)
  mRR <- aggregate(RR, format(index(RR), "%Y-%m-01"), mean, na.rm=TRUE)
  mRR <- melt(cbind(month=index(mRR), as.data.frame(mRR)), id=1)
  names(mRR)  <- c("month", "path.no", "SP")
  mRR$month   <- as.Date(mRR$month)
  mRR$path.no <- as.numeric(as.character(mRR$path.no))
  mRR <- subset(mRR, month %in% options$similar.months)
  
  # get the clearing price 
  CPSP <- FTR.get.CP.for.paths(paths, ISO.env, add.SP=FALSE)
  CPSP <- CPSP[-grep("-", CPSP$auction),]
  CPSP$month <- FTR.AuctionTerm(auction=CPSP$auction)$start.dt
  CPSP <- subset(CPSP, month %in% options$similar.months)

  CPSP <- merge(CPSP, mRR)  # put them together  
  CPSP <- melt(CPSP[,c("path.no", "month", "CP", "SP")], id=1:2)
  
  resPK <- data.frame(cast(CPSP, month ~ variable + path.no, I,
                         subset=path.no %in% 1:N))
  resPK[,-1] <- round(resPK[,-1], 2)
  resPK <- data.frame(resPK[,1:(N+1)], blank=" ", resPK[,(N+2):(2*N+1)])
  cat("\n\nPEAK:\n")
  print(resPK)  # paste this in the table

  resOFF <- data.frame(cast(CPSP, month ~ variable + path.no, I,
                         subset=path.no %in% (N+1):(2*N)))
  resOFF[,-1] <- round(resOFF[,-1], 2)
  resOFF <- data.frame(resOFF[,1:(N+1)], blank="", resOFF[,(N+2):(2*N+1)])
  cat("\n\nOFFPEAK:\n")
  print(resOFF)  # paste this in the table

  invisible()
}

##################################################################
# Show hourly positions by zone.
#  Doesn't work well yet!
#
make.HourlyGen <- function(month)
{
  stop("Doesn't work!")
  startDt  <- as.POSIXct(format(month, "%Y-%m-%d 01:00:00"))
  endDt    <- as.POSIXct(format(seq(month, by="1 month", length.out=2)[2],
                               "%Y-%m-%d 00:00:00"))
  QQ <- NULL
  QQ[["NEMA"]] <- {
    portfolios <- c("MYSTC89M", "MYSTIC7M", "nepool gen portfolio")
    filter <- "Region=NEPOOL;Zone=NEMA"
    cbind(zone="NEMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["SEMA"]] <- {
    portfolios <- c("FORERIVM", "nepool gen portfolio")
    filter <- "Region=NEPOOL;Zone=SEMA"
    cbind(zone="SEMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["WMA"]] <- {
    portfolios <- c("NPKLEEN", "nepool gen portfolio")
    filter <- "Region=NEPOOL;Zone=WMA"
    cbind(zone="WMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["CT"]] <- {
    portfolios <- c("NPKLEEN", "nepool gen portfolio")
    filter <- "Region=NEPOOL;Zone=CT"
    cbind(zone="CT", ntplot.rtrhq(portfolios, filter=filter))
  }

  QQ <- do.call(rbind, QQ)
  ## QQ <- cast(LL, zone + time ~ ., sum)  # aggregate CNE + CCG
  ## names(LL)[ncol(LL)] <- "value"
  
  QQ$type <- "Gen"
  
  QQ
}


##################################################################
# Show hourly positions by zone
# calls functions from lib.ccg.loads
#
make.HourlyLoads <- function(month)
{
  startDt  <- as.POSIXct(format(month, "%Y-%m-%d 01:00:00"))
  endDt    <- as.POSIXct(format(seq(month, by="1 month", length.out=2)[2],
                               "%Y-%m-%d 00:00:00"))

  LL <- NULL
  LL[["NEMA"]] <- cbind(zone="NEMA", .get_loads_zone_rtrhq(startDt, endDt, "NEMA"))
  LL[["SEMA"]] <- cbind(zone="SEMA", .get_loads_zone_rtrhq(startDt, endDt, "SEMA"))
  LL[["WMA"]]  <- cbind(zone="WMA", .get_loads_zone_rtrhq(startDt, endDt, "WMA"))
  LL[["CT"]]   <- cbind(zone="CT", .get_loads_zone_rtrhq(startDt, endDt, "CT"))

  LL <- do.call(rbind, LL)
  LL <- cast(LL, zone + time ~ ., sum)  # aggregate CNE + CCG
  names(LL)[ncol(LL)] <- "value"
  
  LL$type <- "Load"
  LL$value <- -LL$value  # make it positive!
  
  LL
}

##################################################################
# Show hourly positions by zone
# Doesn't work right for CNE loads, because of RM scaling....
#
make.HourlyLoads_AAD <- function(month)
{
  getLoads <- function(asOfDate, startDt, endDt, pfolio){
    lobj <- find_load_objects_SecDb(asOfDate=asOfDate, pfolio=pfolio)
    lobj <- unique(lobj[,c("load.name", "load.class", "share", "zone")])
    #lobj <- subset(lobj, share > 0)


    # by zone
    zLO <- split(lobj, lobj$zone)
    zLL <- lapply(zLO, read_secdb_load_objects, asOfDate=Sys.Date(),
      startDt=startDt, endDt=endDt, agg=TRUE)
    zLL
  }

  asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")) 
  startDt  <- as.POSIXct(format(month, "%Y-%m-%d 01:00:00"))
  endDt    <- as.POSIXct(format(seq(month, by="1 month", length.out=2)[2],
                               "%Y-%m-%d 00:00:00"))
 
  LL.ccg <- getLoads(asOfDate, startDt, endDt, pfolio="NEPOOL Load Portfolio")
  LL.cne <- getLoads(asOfDate, startDt, endDt, pfolio="CNE NewEngland Load Portfolio")

  formatLoad <- function(LL){
    zLL <- mapply(function(x, y){cbind(zone=y, x)}, LL, names(LL),
                  SIMPLIFY=FALSE)
    zLL <- do.call(rbind, zLL)
  }

  zLL <- rbind(formatLoad(LL.ccg), formatLoad(LL.cne))
  zLL$type <- "Load"
  rownames(zLL) <- NULL
  
  zLL
}


##################################################################
# Show hourly positions by zone from the OTC book  NPFTRMT1
#
make.HourlyOTC <- function(month)
{
  QQ <- NULL
  QQ[["NEMA"]] <- {
    portfolios <- c("NPFTRMT1", "NEPPWRBS")
    filter <- "Region=NEPOOL;Zone=NEMA"
    cbind(zone="NEMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["SEMA"]] <- {
    portfolios <- c("NPFTRMT1", "NEPPWRBS")
    filter <- "Region=NEPOOL;Zone=SEMA"
    cbind(zone="SEMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["WMA"]] <- {
    portfolios <- c("NPFTRMT1", "NEPPWRBS")
    filter <- "Region=NEPOOL;Zone=WMA"
    cbind(zone="WMA", ntplot.rtrhq(portfolios, filter=filter))
  }
  QQ[["CT"]] <- {
    portfolios <- c("NPFTRMT1", "NEPPWRBS")
    filter <- "Region=NEPOOL;Zone=CT"
    cbind(zone="CT", ntplot.rtrhq(portfolios, filter=filter))
  }

  QQ <- do.call(rbind, QQ)
  QQ <- cast(QQ, zone + time ~ ., sum, fill=0)  # aggregate over portfolios
  names(QQ)[ncol(QQ)] <- "value"
  QQ$type <- "OTC"

  QQ
}


##################################################################
# Show hourly positions by zone
# calls functions from lib.ccg.loads
#
make.HourlyTotals <- function()
{
  ## startDt  <- as.POSIXct(format(month, "%Y-%m-%d 01:00:00"))
  ## endDt    <- as.POSIXct(format(seq(month, by="1 month", length.out=2)[2],
  ##                              "%Y-%m-%d 00:00:00"))

  LL <- NULL
  LL[["NEMA"]] <- cbind(zone="NEMA",
    ntplot.rtrhq("pm northeast portfolio", "Region=NEPOOL;Zone=NEMA"))
  LL[["SEMA"]] <- cbind(zone="SEMA",
    ntplot.rtrhq("pm northeast portfolio", "Region=NEPOOL;Zone=SEMA"))                    
  LL[["WMA"]]  <- cbind(zone="WMA",
    ntplot.rtrhq("pm northeast portfolio", "Region=NEPOOL;Zone=WMA"))
  LL[["CT"]]   <- cbind(zone="CT",
    ntplot.rtrhq("pm northeast portfolio", "Region=NEPOOL;Zone=CT"))
  LL <- do.call(rbind, LL)
  LL <- cast(LL, zone + time ~ ., sum)  # aggregate CNE + CCG
  names(LL)[ncol(LL)] <- "value"
  
  LL$type <- "Total"

  LL
}



##################################################################
# Show historical HR
#
make.HR.table <- function(options)
{
  firstYear <- as.numeric(format(Sys.Date(), "%Y"))-5
  startTime <- as.POSIXct(paste(firstYear, "-01-01 01:00:00", sep=""))
  startDt <- startTime #as.POSIXct("2007-01-01 01:00:00")   # for hist data
  endDt   <- Sys.time()                          # for hist data 

  HH <- PM:::ntplot.bucketizeByDay("nepool_smd_da_4000_lmp",
    bucket=c("5X16", "2X16H", "7X8"), as.Date(startDt), as.Date(endDt))
  colnames(HH)[4] <- "hub.da"
  HH$symbol <- NULL
  
  GG <- FTR:::FTR.load.tsdb.symbols(c("gasdailymean_algcg",
    "gasdailymean_tetm3"),   startDt, endDt)
  colnames(GG) <- c("algcg", "tetm3")
  GG <- cbind(as.data.frame(GG), day=index(GG))

  # put the gas and power together
  aux <- merge(HH, GG, by="day")
  aux$hr   <- aux$hub.da/aux$algcg
  aux$year <- format(aux$day, "%Y")
  aux$mon  <- format(aux$day, "%m")
  aux$mmm  <- factor(format(aux$day, "%b"), levels=month.abb)
  aux$dayOfMonth <- as.numeric(format(aux$day, "%d"))
  aux <- aux[order(aux$dayOfMonth), ]
 
  res <- subset(aux, mon==format(options$month, "%m"))
  res <- ddply(res, .(year), function(x){
    y <- subset(x, bucket=="5X16")
    summary(y$hr)})
  res[,2:7] <- sapply(res[,2:7], round, 2)

  rLog("Heat rates for 5x16 bucket, month", format(options$month, "%b"))
  print(res, row.names=FALSE)

 
  focusMonths <- format(c(prevMonth(month), month), "%b")
  windows(7,5)
  print(xyplot(hr ~ dayOfMonth|mmm, data=aux, groups=year,
    ylab="Daily Heat Rate, 5x16",
    xlab="Day of month",
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=4),
    scales="free",     
    type=c("g","o"),
    subset=mmm %in% focusMonths & bucket=="5X16"))
  #subset(aux, mmm %in% "Apr" & bucket=="5X16" & year==2012)


  #xyplot(hr ~ algcg|year, data=aux, type=c("g","p"), scales=list(x=list(log=1)))
  
  res
}

##################################################################
# show the Gen outages 
#
make.GenOutage.table <- function( month )
{
  startDt <- month
  endDt <- nextMonth(month)-1
  GO <- FTR.getGenerationOutagesIIR("ISNE", start.dt=startDt, end.dt=endDt)
  
  z <- ddply(GO, c("unit.state"), function(x) {
    x <- subset(x, toupper(x$outage.sta) != "CANCELLED")
    y <- x[,c("unit.state", "unit.name", "out.design", "ta.start", "ta.end")]
    y <- y[order(-y$out.design),]
    y$mw.out <- round(y$out.design)
    y$start <- format(y$ta.start, "%d-%b")
    y$end   <- format(y$ta.end, "%d-%b")
    y$ta.start <- y$ta.end <- NULL
    y$out.design <- NULL
    y$prim.fuel <- x$prim.fuel
    y
  })
 
  z
}

##################################################################
# make a historical stats of the important interfaces
#
make.Interfaces.table <- function(auction, ISO.env, options)
{
  symbols <- c(
    "NEPL_DA_INTER_BSTN_MW",      "BSTN MW",
    "NEPL_DA_INTER_BSTN_LIMIT",   "BSTN LIMIT",
    "NEPL_DA_INTER_CTIMP_MW",     "CTIMP MW",
    "NEPL_DA_INTER_CTIMP_LIMIT",  "CTIMP LIMIT",
    "NEPL_DA_INTER_RIIMP_MW",     "RIIMP MW",
    "NEPL_DA_INTER_RIIMP_LIMIT",  "RIIMP LIMIT",
    "NEPL_DA_INTER_MENH_MW",      "MENH MW",
    "NEPL_DA_INTER_MENH_LIMIT",   "MENH LIMIT",
    "NEPL_DA_INTER_NEEW_MW",      "NEEW MW",
    "NEPL_DA_INTER_NEEW_LIMIT",   "NEEW LIMIT",
    "NEPL_DA_INTER_HQP2_MW",      "HQP2 MW",  
    "NEPL_DA_INTER_NENY_MW",      "NENY MW",
    "NEPL_DA_INTER_NENB_MW",      "NENB MW"           
    #"NEPL_DA_INTER_WESTCT_MW",    "WESTCT MW",
    #"NEPL_DA_INTER_WESTCT_LIMIT", "WESTCT LIMIT"
  )

  symbs <- data.frame(matrix(symbols, ncol=2, byrow=TRUE))
  colnames(symbs) <- c("symbol", "name")

  res <- ntplot.bucketizeByMonth(symbs[,1], bucket=c("Peak", "Offpeak"))
  res <- subset(res, month %in% options$similar.months)

  res <- merge(res, symbs, by="symbol")
  res$bucket <- as.character(res$bucket)
  
  resPK <- cast(res, month ~ name, I, subset=bucket=="Peak",
    add.missing=TRUE, fill=NA)
  resPK <- resPK[,c("month", symbs$name)]
  cat("\n\nPEAK Interfaces:\n")
  print(resPK)  # paste this in the table
  
  resOff <- cast(res, month ~ name, I, subset=bucket=="Offpeak",
    add.missing=TRUE, fill=NA)
  resOff <- resOff[,c("month", symbs$name)]
  cat("\n\nOFFPEAK Interfaces:\n")
  print(resOff)  # paste this in the table

  return(invisible(list(resPK, resOff)))
}  


##################################################################
# plot 
#
make.LoadTemperature.plot <- function(month)
{
  firstYear <- as.numeric(format(Sys.Date(), "%Y"))-5
  startDt <- as.POSIXct(paste(firstYear,"-01-01 01:00:00", sep=""))
  endDt <- Sys.time()

  mon <- as.numeric(format(month, "%m"))
  symbols <- c(
   "nepool_load_hist",               "pool.load",            
   "NEPOOL_zonal_4004_load_rt",      "ct.load",
   "NEPOOL_zonal_4008_load_rt",      "nema.load"
  )
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(HH) <- symbols[,"shortNames"]
   
  # keep only this month data
  aux <- subset(HH, as.numeric(format(index(HH)-1, "%m"))==mon)
  bux <- cbind(time=index(aux), data.frame(aux))
  bux$year <- format(bux$time, "%Y")
  SS <- ddply(bux, c("year"), function(x){summary(x$pool.load)})
  rLog("Historical load stats")
  print(SS, row.names=FALSE)
  
  LL <- PM:::filterBucket(aux, buckets=c("2X16H", "5X16", "7X8"))
  lapply(LL, head)
  
  LL <- do.call("rbind", mapply(function(x, y){
    cbind(time=index(x), bucket=y, data.frame(x))
    }, LL, names(LL), SIMPLIFY=FALSE))
  rownames(LL) <- NULL
  LL$day <- as.Date(format(LL$time, "%Y-%m-%d"))
  
  # get the temps
  symbols <- c(
   "temp_clean_bos_dly_max",         "bos.max",    # BOSTON
   "temp_clean_bos_dly_min",         "bos.min",
   "temp_clean_bdl_dly_max",         "ct.max",     # Hartford, CT
   "temp_clean_bdl_dly_min",         "ct.min"
  )  
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))
  TT <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
  colnames(TT) <- symbols[,"shortNames"]
  head(TT)
  TT <- cbind(day=index(TT), data.frame(TT))
  rownames(TT) <- NULL

  TT$mon <- as.numeric(format(TT$day, "%m"))
  
  # summary stats for CT and BOS temps
  aux <- subset(TT, mon==as.numeric(format(month, "%m"))) 
  print(apply(aux[,2:5], 2, summary))
  
  # calculate energy by day
  EE <- ddply(LL, .(day, bucket), function(x){
    data.frame(ct.engy=sum(x$ct.load),
               nema.engy=sum(x$nema.load),
               pool.engy=sum(x$pool.load))
  })
  EE <- merge(EE, TT, by="day")  # add the temps

  # calc a temperature index for the pool, weighted by the load
  EE$pool.Tmax <- (EE$bos.max*EE$nema.engy + EE$ct.max*EE$ct.engy)/(EE$nema.engy + EE$ct.engy)
  

  # calculate peak load
  pL <- ddply(LL, .(day, bucket), function(x){
    data.frame(pool.Lmax=max(x$pool.load))
  })
  EE <- merge(EE, pL, by=c("day", "bucket"))

  
  # plot peak load vs. max daily temperature
  print(xyplot(pool.Lmax ~ pool.Tmax, data=EE, subset=bucket=="5X16",
         panel = function(x, y, ...){
           panel.grid()
           panel.rug(x, y, ...)
           panel.points(x, y, ...)
           panel.loess(x, y, ...)
         },      
         col=c("steelblue"),
         col.line="black", main=paste(""),
         xlab = "Max Daily Temperature (load weighted)",
         ylab = "Daily peak load, MWh"))
  
  
  aux <- subset(EE, bucket=="5X16")
  model <- loess(pool.Lmax ~ pool.Tmax, data=aux, span=2/3, degree=1,
               family="symmetric")
  x <- round(seq(min(aux$pool.Tmax)+1, max(aux$pool.Tmax)-0.5, length.out=15))
  y <- predict(model, data.frame(pool.Tmax=x), se=TRUE)
  cumT <- ecdf(aux$pool.Tmax)
  fit <- data.frame(Tmax=x, peakLoad=round(y$fit), std=round(y$se.fit),
                    Prob=round(cumT(x),2))
  print(fit, row.names=FALSE)  # loess fit table at different temperature points

  print(as.data.frame(t(t(as.numeric(summary(aux$pool.Lmax))))), row.names=FALSE)

}


##################################################################
# TODO:  Need to align the weekends!
#
plot.HistLoad <- function(month)
{
  firstYear <- as.numeric(format(Sys.Date(), "%Y"))-7
  lastYear  <- as.numeric(format(Sys.Date(), "%Y"))-1 

  mon <- as.numeric(format(month, "%m"))
  symbols <- c("nepool_load_hist", "pool.load")
  symbols <- matrix(symbols, ncol=2, byrow=TRUE, dimnames=list(NULL,
    c("tsdb", "shortNames")))

  # Weekday as decimal number (0–6, Sunday is 0).
  firstDay <- as.numeric(format(month, "%w"))
  noDays   <- as.numeric(nextMonth(month) - month + 1)
  
  ## for (year in firstYear:lastYear) {
  ##   # first same weekday in this year
    
    startDt <- as.POSIXct(paste(firstYear,"-01-01 01:00:00", sep=""))
    endDt <- Sys.time()
    
    HH <- FTR:::FTR.load.tsdb.symbols(symbols[,"tsdb"], startDt, endDt)
    colnames(HH) <- symbols[,"shortNames"]

#  }
  

  aux <- cbind(time=index(HH), data.frame(HH))
  aux$year <- format(aux$time-1, "%Y")
  aux$mon  <- format(aux$time-1, "%m")
  
  aux <- subset(aux, mon == format(month, "%m"))
  y <- ddply(aux, c("year"), function(x) {
    x$dayOfMonth <- as.numeric(x$time - x$time[1])/(3600*24)
    #browser()
    x
  })

  windows(8,5)
  xyplot(pool.load ~ dayOfMonth, data=y, groups=year,
    type="l", xlab="Day of month",
    ylab="Nepool load",
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=4))
#      columns=length(unique(y$year))))


  
}



##################################################################
# 
plot.LoadGenHedges <- function(month, LL, QQ, HG, TT)
{
  data <- rbind(LL, QQ, HG, TT)

  QQHG <- merge(QQ[,c("time", "zone", "value")],
                HG[,c("time", "zone", "value")], by=c("time", "zone"))
  QQHG$value <- QQHG$value.x + QQHG$value.y
  QQHG$type  <- "Gen+OTC"
  data <- rbind(data, QQHG[,c("time", "zone", "value", "type")])

  aux <- subset(data, type %in% c("Load", "Gen+OTC", "Total"))
  aux <- aux[order(aux$zone, aux$type, aux$time),]
  
  startDt  <- as.POSIXct(format(month, "%Y-%m-%d 01:00:00"))
  aux <- subset(aux, time >= startDt & time < endDt)
  
  xyplot(value ~ as.POSIXct(as.numeric(time), origin="1970-01-01")|zone,
    data=aux, groups=type,
    subset= zone %in% c("CT", "NEMA", "SEMA", "WMA"),
    scales="free",
    ylab="MW", xlab="",
    panel=function(x, y, ...){
      panel.grid(h=-1, v=0, col="gray", lty=2)
      panel.xyplot(x, y, type="l", ...)
    },
    auto.key=list(columns=3)
  )

}


##################################################################
# Read the xlsx file that keeps all the bids
#
# return a data.frame with bids
#
.read_bids_xlsx <- function(filename, sheetName)
{
  require(xlsx)

  MM  <- read.xlsx2(filename, sheetName=sheetName, colIndex=1:7,
    colClasses=c("character", rep("numeric",3), "character",
      rep("numeric",2)))
  MM <- MM[!is.na(MM[,1]), ]
  MM <- MM[!MM[,1]=="",]
  bids  <- MM[,c("auction", "path.no", "source.ptid", "sink.ptid",
                 "class.type", "mw", "bid.price")]

  # check that the path.no are increasing by one
  path.no <- sort(unique(bids$path.no))
  if (path.no[1] != 1 || max(diff(path.no)) != 1)
    stop("Path.no are not strictly increasing by one!")

  # check that the path numbers are unique
  if (length(path.no) != nrow(unique(bids[, c("source.ptid", "sink.ptid",
                 "class.type")])))
    stop("Some path.no are not unique!")

  # check that the auctionName is unique
  if (length(unique(bids$auction)) != 1)
    stop("The auctionName has to be unique!")
  
  # check that the bid.prices are decreasing
  aux <- ddply(bids, .(auction, path.no), function(x) {
    if (!identical(x$bid.price, -sort(-x$bid.price)))
      stop("Bids for path ", x$path.no[1], " are not decreasing!")
  })

  
  # add the hours
  hrs <- count_hours_auction(unique(bids$auction), buckets=c("peak", "offpeak"))
  bids <- merge(bids, hrs, by=c("auction", "class.type"))
  
  bids$bid.price.month <- bids$bid.price * bids$hours
  bids$buy.sell <- "BUY"                 # all of them are buy!

  # add the node names
  bids2 <- ddply(bids, c("auction"), function(x) {
    rLog("Processing auction ", x$auction[1])
    merge(x, FTR.PathNamesToPTID(x, NEPOOL$MAP, to="name"))
  })
  
  # add bid.no column
  res3 <- ddply(bids2, .(auction, path.no), function(x) {
    x <- x[order(-x$bid.price),]
    cbind(bid.no=1:nrow(x), x)
  })
  i1 <- which(colnames(res3)=="path.no")
  i2 <- which(colnames(res3)=="bid.no")
  res4 <- res3[, c(i1, i2, setdiff(1:ncol(res3), c(i1, i2)))] 

  res4
}




##################################################################
##################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

  
  
  month   <- as.Date("2011-06-01")
  startDt <- as.POSIXct("2006-01-01 01:00:00")   # for hist data
  endDt   <- Sys.time()                          # for hist data 
  

  
  
}


## ##################################################################
## # The expected value of a discrete distribution
## #
## meanDiscrete <- function(pdf)
## {
##  sum(pdf$x * pdf$y)/sum(pdf$y)
## }



## ##################################################################
## # The density function
## # @param x a vector
## # @param pdf a density object
## # @return vector of values
## #
## dDiscrete <- function(x, pdf)
## {
##   # return a normalized function
##   N <- sum(est_pdf$y)*(est_pdf$x[2] - est_pdf$x[1])   # normalization constant
##   pdf <- approxfun(est_pdf$x, est_pdf$y/N, yleft=0, yright=0)
  
##   pdf(x)
## }

