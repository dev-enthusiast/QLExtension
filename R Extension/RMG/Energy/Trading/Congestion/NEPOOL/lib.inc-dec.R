# Library of functions for daily inc-dec trading
# Used by Energy/Trading/Congestion/NEPOOL/main.inc-dec.R
# See demo.inc-dec.R for typical usage
#
# Functions: 
#   bucket.hprices.daily
#   calc_portfolio_pnl       # calc the historical PnL for tomorrow's position
#   inc.price.trades         # @deprecated, used in the report
#   find.price.arbitrage
#   fix.exceptions   for WRONG names in eMKT
#   format.next.day.trades   # output for xls upload
#   .get_last_incdec_filename
#   get.incdec.hprices
#   get_booked_virtuals      # get the actual trades!
#   inc.basic.stats          # retarded little function
#   inc.get.trading.signals  # 
#   max_min_locations        # find the locations with biggest spread
#   next.day.trades          # used by procmon job
#   price_one_incdec         # price one inc/decs if you have all inputs
#   price_pfolio_incdec      # price a list of inc/decs   
#   trigger.Persistence
#   trigger.1DayPanicDA
#   trigger.7DayDomination
#   .lookback1          # actual strategy, calculates PnL, and makes recommendation
#   read_incdec_xls     # read our own incdec xls (for checks!)
#   strategy_hub        # how to play the virts at the hub  (started Jan-2011)
#   strategy_hub_rpt    # how to report it

#####################################################################
# bucket hPP for a given set of buckets.  Should support SecDB ones
# sometime in the future.
#
bucket.hprices.daily <- function(hPP, bucket, do.split=TRUE)
{
  bPP <- vector("list", length=length(bucket))
  names(bPP) <- bucket
  
  dtime <- as.POSIXlt(index(hPP))
  
  if (bucket=="1X16"){
    ind.hour <- which(dtime$hour %in% 8:23)
    bPP <- aggregate(hPP[ind.hour,],
      list(as.Date(format(index(hPP)[ind.hour], "%Y-%m-%d"))),
                     mean, na.rm=T)
  } else 
  if (bucket=="1X8"){
    ind.hour <- which(dtime$hour %in% 0:7)
    bPP <- aggregate(hPP[ind.hour,],    # substract one second for HE
      list(as.Date(format(index(hPP)[ind.hour]-1, "%Y-%m-%d"))),
                     mean, na.rm=T) 
  } else {
    browser()
    ind.b <- secdb.getHoursInBucket(region, bucket,
      index(hPP)[1]+1, index(hPP)[ncol(hPP)])
    hPP <- window(hPP, index=ind.b$time)
    bPP <- aggregate(hPP,               # substract one second for HE
       list(as.Date(format(index(hPP)-1, "%Y-%m-%d"))), mean) 
  }  

  if (do.split){  # should you group the RT and DA from the same node
    ind <- split(colnames(bPP), gsub("RT_|DA_", "", colnames(bPP)))
    bPP <- lapply(ind, function(ind.x, bPP){bPP[,ind.x]}, bPP)
    names(bPP) <- paste(bucket, "_", names(bPP), sep="")
  }
  
  return(bPP)  # bucket prices
}

#####################################################################
# trd is a list as constructed in the demo.inc-dec.R file
#
calc_portfolio_pnl <- function(trd,
  days=seq(Sys.Date()-10, Sys.Date(), by="1 day"))
{
  PnL <- vector("list", length=length(trd))

  # loop over all trades
  for (i in 1:length(trd)) {
    info <- trd[[i]]
    if (!is.null(info$bucket)){
      info$hours <- switch(info$bucket,
                           `1X24` = 1:24,        # HourEnding
                           `1X8`  = c(1:7, 24),  # HourEnding
                           `1X16` = 8:23)        # HourEnding 
    } else {
      aux <- as.numeric(strsplit(info$hrs, "-")[[1]])  # hours are specified directly
      info$hours <- aux[1]:aux[2]                      # as a string, eg: "17-21"  
    }
    info$is.INC <- ifelse(info$inc.dec=="INCREMENT", TRUE, FALSE)
    price_rtda<- FTR:::.inc.dec.tsdbprices(info$ptid, "NEPOOL",
                                           min(days), max(days))
    PnL[[i]] <- info
    PnL[[i]]$pnl <- price_one_incdec(price_rtda, hrs=info$hours,
                                     qty=info$mw, is.INC=info$is.INC)
  }
  
  PnL
}




#####################################################################
# Pick distinct pairs for nodes.  Used in find.price.arbitrage
.pickNodes <- function(x, y, pick=3)
{
  if (pick > unique(length(x))){
    rLog("You want to pick more elements than you have!")
    return(NULL)
  }
  
  X <- rep(NA, pick)
  Y <- rep(NA, pick)
  X[1] <- x[1]
  Y[1] <- y[1]
  IND <- c(1, rep(NA, pick-1))  # interested in this one only
  
  ind.pk  <- 1
  ind.row <- 2    
  while (ind.row <= length(x) & ind.pk < pick){
    if ((x[ind.row] %in% X)|(y[ind.row] %in% Y)){
      ind.row <- ind.row + 1
      next
    }
    ind.pk <- ind.pk + 1
    X[ind.pk]   <- x[ind.row]
    Y[ind.pk]   <- y[ind.row]
    IND[ind.pk] <- ind.row
    ind.row <- ind.row + 1
  }

  return(na.omit(IND))
}

#####################################################################
# Occasionally prices among nodes close by are different in DA and
# identical in RT.  Check for such occurences.  These make for nice 
# arbitrage ideas.
#
# This is rare, so don't panick if you don't get them...
# trigger - the value that will trigger highligting. 
#
find.price.arbitrage <- function(MAP, threshold=0.5, pick=2, hDays=10)
{
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.DA.RData")  # hPP.DA
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.RT.RData")  # hPP
  hPP.RT <- hPP
  rm(hPP)
  if (ncol(hPP.DA) < 900 | ncol(hPP.RT) < 900){
    rLog("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    rLog("Not all nodes have been read from tsdb.  Please fix.")
    return(NULL)
  }
  
  hPP <- cbind(hPP.DA, hPP.RT)  
  colnames(hPP) <- gsub("NEPOOL_SMD_|_LMP","", colnames(hPP))
  
  MAP <- subset(MAP, in.pnode.table)
  MAP <- subset(MAP, !is.na(dispatch.zone))
  groups <- split(MAP, MAP$dispatch.zone)

  # look for arbitrage inside a dispatch.zone only
  res <- NULL
  for (g in seq_along(groups)){
    rLog("Working on:", names(groups)[g])
    cnames <- c(paste("DA_", groups[[g]]$ptid, sep=""),
                paste("RT_", groups[[g]]$ptid, sep=""))

    ind   <- which(colnames(hPP) %in% cnames)
    if (length(ind) == 0)
      next
    
    ptids <- table(gsub("RT_|DA_", "", colnames(hPP[,ind])))
    ptids <- sort(as.numeric(names(ptids[ptids==2])))
    hPP.g <- hPP[,ind]
    hPP.g[is.na(hPP.g)] <- 0

    DEC <- zoo(matrix(0, nrow=nrow(hPP), ncol=length(ptids)), index(hPP))
    colnames(DEC) <- ptids
    for (p in 1:length(ptids)){
      DEC[,p] <- hPP.g[,paste("RT_", ptids[p], sep="")]-
                 hPP.g[,paste("DA_", ptids[p], sep="")]
    }
    
    x  <- apply(tail(DEC,hDays*24),2, mean)  # last hDays
    DM <- as.matrix(dist(as.matrix(x), method="manhattan"))
    DM[lower.tri(DM)] <- NA
    ind.picks <- which(DM > threshold, arr.ind=TRUE)
    if (nrow(ind.picks)==0){next}  # nothing
    picks <- data.frame(zone  = names(groups)[g], 
                        dec   = rownames(DM)[ind.picks[,"row"]],
                        inc   = colnames(DM)[ind.picks[,"col"]])
    picks$value <- x[picks$dec]-x[picks$inc]

    ind <- which(picks$value < 0)
    if (length(ind)>0){   # need to reverse these guys
      aux <- picks$inc[ind]
      picks$inc[ind] <- picks$dec[ind]
      picks$dec[ind] <- aux
      picks$value[ind] <- -picks$value[ind]
    }
    picks <- picks[order(-picks$value), ]
    picks <- picks[.pickNodes(picks$dec, picks$inc, pick=3),]
    
    res[[g]] <- picks
  }

  res <- do.call(rbind, res)
  rownames(res) <- NULL
  colnames(res)[4] <- "value"
  
  return(res)
}



#####################################################################
# Select trades for next day based on the long list of recommendations. 
#
fix.exceptions <- function(x)
{
  x <- gsub("LD.BOGGY_BK46  ",     "LD.BOGGY_BK46 ",     x)
  x <- gsub("LD.CANDLE  13.2",     "LD.CANDLE 13.2",     x)
  x <- gsub("LD.CANTON  23",       "LD.CANTON 23",       x)
  x <- gsub("UN.BPT_ENER16  BHCC", "UN.BPT_ENER16 BHCC", x)
  x <- gsub("UN.JAY     13.8VCG1", "UN.JAY  13.8VCG1",   x)
  
  
  x
} 


#####################################################################
# Select trades for next day based on the long list of recommendations. 
#
format.next.day.trades <- function( NXT, ptids=NULL, quantity=15, book="VIRTSTAT", 
  maxPrice=300, minPrice=10, save=TRUE, asOfDate=Sys.Date()+1)
{
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")

  format1 <- function(x, quantity){
    if ("bucket" %in% names(x)){
      X <- data.frame(x[c("inc.dec", "ptid", "bucket")], first.HE="",
                        last.HE="")
    } 
    if ("hrs" %in% names(x)){
      aux <- strsplit(x$hrs, "-")
      if (any(sapply(aux, length)!=2))
        stop("Misspecified hrs element in the input list!")
      X <- data.frame(inc.dec=x$inc.dec, ptid=x$ptid, bucket="",
        first.HE=sapply(aux, "[", 1), last.HE=sapply(aux, "[", 2))
    }
    if ("mw" %in% names(x)){ X$mw <- x$mw 
    } else { X$mw <- quantity}
    if ("book" %in% names(x)){ X$book <- x$book 
    } else { X$book <- book}
    
    return(X)
  }

  
  if (class(NXT)=="data.frame"){
    if (!is.null(ptids))
      NXT <- subset(NXT, ptid %in% ptids)
  } else {  # if it's a list, one trade per element
      NXT <- do.call(rbind, lapply(NXT, format1, quantity)) 
  }

  res <- merge(NXT, MAP[,c("ptid", "name")], all.x=TRUE, sort=FALSE)
  if (!("inc.dec" %in% names(res)))
    res$inc.dec <- ifelse(res$dec, "DECREMENT", "INCREMENT")
  res$block    <- tolower(res$bucket)
  res$price    <- ifelse(res$inc.dec=="DECREMENT", maxPrice, minPrice)
  res$shape    <- 2
#  res$book     <- "NESTAT"
  
  res.csv <- res[,c("name", "inc.dec", "block", "first.HE", "last.HE",
                    "mw", "price", "shape", "book")]

  res.csv <- apply(res.csv, 1, function(x){
    paste(x, sep="", collapse=",")})
  res.csv <- paste(paste(res.csv, sep="", collapse="\n"), "\n", 
                   sep="")

  res.csv <- fix.exceptions(res.csv)
  cat(res.csv)

  # save the trades to file for reconciliation later!
  if (save){
    fileOut <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/AAD_incdec_bids.RData"
    if (file.exists(fileOut)){
      load(fileOut)
    } else {
      incdec <- NULL
    }
    incdec[[as.character(asOfDate)]] = res
    
    save(incdec, file=fileOut)
    rLog("\nSaved bids for", as.character(asOfDate), "to", fileOut)
  }
  
  
  invisible(res)
}


#################################################################
# Get the name of the lastest xls file from the share drive
#
.get_last_incdec_filename <- function(asOfDate)
{
  filename <- NULL
  mon <- format(asOfDate, "%b")
  yy  <- format(asOfDate, "%y")
  monyy <- paste(mon, yy)
  dirroot <- "S:/All/Structured Risk/NEPOOL/Incs & Decs/"
  dirs <- list.files(dirroot)

  ind <- grep(monyy, dirs)
  if (length(ind)==0){
    rLog("Cannot find folder", monyy)
  } else {
    files <- list.files(paste(dirroot, monyy, sep=""), full.names=TRUE)
    dates <- gsub(".*Dec (.*) Incs.*", "\\1", files)
    dates <- as.Date(dates, format="%d%b%y")
    files <- files[order(dates, na.last=FALSE)]   
    filename <- files[length(files)]
  }
  
  return(filename)
}


#####################################################################
# Get the actual inc-dec trades from the S:\ drive.  Don't know of
# a better way!
#
# Returns a list of INCDEC trades, ready for pricing
#
#
get_booked_virtuals <- function(days=c(Sys.Date()), books=c("VIRTSTAT"))
{
  require(RODBC)
  require(DivideAndConquer)
  require(zoo)

  DIR <- "S:/All/Structured Risk/NEPOOL/Incs & Decs/DailyIncsDecs/Cleared/"
  mths <- format(days, "%Y%m")
  daysf <- format(days, "%m%d%y")
  fnames <- paste(DIR, mths, "/NEPOOL ACCRUAL ", daysf, "Inc-Dec-Cleared.xls",
                  sep="")

  loadOneFile <- function(file=file, books) {
    if (file.exists(file)) {
      con <- odbcConnectExcel(file)
      aux <- sqlFetch(con, "Data")
      on.exit(odbcClose(con))
      day <- as.Date(gsub("NEPOOL ACCRUAL (.*)Inc-Dec-Cleared.xls", "\\1",
                          basename(file)), "%m%d%y")
      aux$trade.start.date <- day
      aux$trade.end.date <- day
      aux
    } else {
      NULL
    }
  }
  
  vrt <- finalize(fnames, loadOneFile, books=books)
  odbcCloseAll()
  vrt <- subset(vrt, `Book Name` %in% books)
  names(vrt)[c(2,4,6,7,10,13)] <- c("ptid", "trading.book", "virt.type",
    "hour.ending", "mw", "eti")
  vrt <- vrt[,c(2,4,6,7,10,13:15)]
  
  aux <- split(vrt, vrt$eti)
  trd <- vector("list", length=length(aux))  # construct the trade info

  trd <- lapply(aux, function(x){
    rLog("Working on eti:", x$eti[1])
    x <- subset(x, mw != 0)            # some trades have more entries
    x <- x[order(x$hour.ending),]
    y <- list()
    y$trading.book <- x$trading.book[1]
    y$eti  <- x$eti[1]
    y$ptid <- x$ptid[1]
    y$trade.start.date <- x$trade.start.date[1]
    y$trade.end.date   <- x$trade.end.date[1]
    y$hrs <- x$hour.ending
    y$qty <- x$mw
    y$is.INC <- ifelse(x$virt.type[1]=="Inc", TRUE, FALSE)

    y
  })

  trd   # ready for pricing with price_pfolio_incdec
}




#####################################################################
#
get.incdec.hprices <- function(buckets, ptids=NULL,
  start.dt=as.POSIXct(paste(min(Sys.Date()-31), "01:00:00")),
  end.dt=as.POSIXct(paste(Sys.Date()+1, "00:00:00")), do.split=TRUE,
  do.melt=FALSE, add.RT5M=TRUE, ...)
{
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
  if (is.null(ptids)){
    load(paste("//ceg/cershare/All/Risk/Data/FTRs/",
               "NEPOOL/ISO/DB/RData/map.RData", sep=""))
    ptids <- subset(MAP, in.pnode.table)$ptid
  }
  info <- file.info(paste(dir, "nepool.RT.RData", sep=""))
  if (is.na(info$size)){
    # get the RT prices
    tsdb.symbols <- paste("NEPOOL_SMD_RT_", ptids, "_LMP", sep="")
    end.dt.RT <- as.POSIXct(paste(Sys.Date(), "00:00:00", sep=""))
    hPP <- FTR.load.tsdb.symbols( tsdb.symbols, start.dt, end.dt.RT)
    cat("Saving the RT prices to nepool.RT.Data\n", sep="")
    save(hPP, file=paste(dir, "nepool.RT.RData", sep=""))
  } else {
    cat("Loading the RT prices from nepool.RT.Data\n")    
    load(paste(dir, "nepool.RT.RData", sep=""))
  }
  
  info <- file.info(paste(dir, "nepool.RT5M.RData", sep=""))
  if (is.na(info$size)){
    if (add.RT5M){
      # get the 5min RT prices ...
      tsdb.symbols <- paste("NEPOOL_SMD_RT5M_", ptids, "_LMP", sep="")
      hPP.5M <- FTR.load.tsdb.symbols( tsdb.symbols,
        as.POSIXct(paste(Sys.Date(), "00:00:01")), min(Sys.time(), end.dt))
      hPP.5M <- toHourly(hPP.5M, HE=TRUE)
      colnames(hPP.5M) <- gsub("5M","", colnames(hPP.5M))
      
      cat("Saving the RT5M prices to nepool.RT5M.Data\n")
      save(hPP.5M, file=paste(dir, "nepool.RT5M.RData", sep=""))
    }
  } else {
    cat("Loading the RT5M prices from nepool.RT5M.Data\n")    
    load(paste(dir, "nepool.RT5M.RData", sep=""))
  }

  # put them together ...
  hPP <- hPP[,colnames(hPP.5M)]  # some 5M symbols are missing
  hPP <- rbind(hPP, hPP.5M)

  info <- file.info(paste(dir, "nepool.DA.RData", sep=""))
  if (is.na(info$size)){
    # get the DA prices 
    tsdb.symbols <- paste("NEPOOL_SMD_DA_", ptids, "_LMP", sep="")
    hPP.DA <- FTR.load.tsdb.symbols( tsdb.symbols, start.dt, end.dt)
    
    cat("Saving the DA prices nepool.DA.Data\n")
    save(hPP.DA, file=paste(dir, "nepool.DA.RData", sep=""))
  } else {
    cat("Loading the DA prices nepool.DA.Data\n")    
    load(paste(dir, "nepool.DA.RData", sep=""))
  }
##  cnames <- gsub("_RT_", "_DA_", colnames(hPP))
##  hPP.DA <- hPP.DA[, cnames]
    
  # put RT and DA prices together ... 
  hPP <- cbind(hPP, hPP.DA)  
  colnames(hPP) <- gsub("NEPOOL_SMD_|_LMP","", colnames(hPP))
   
  bPP <- NULL
  if (!is.null(buckets)){
    rLog("Bucket prices:")
    for (b in buckets)
      if (do.split)
         bPP <- c(bPP, bucket.hprices.daily(hPP, bucket=b, ...))
      else 
        bPP[[b]] <- bucket.hprices.daily(hPP, bucket=b, ...)
    rLog("Done")
  }
  if (do.melt){
    aux <- lapply(bPP, as.data.frame)
    aux <- lapply(aux, melt)
    # needs work ... 
  }
  
  return(list(hPP, bPP))
}


#####################################################################
# Calculate some basic statistics to use in filters ...  
#
inc.basic.stats <- function(X)
{
  X <- na.omit(X)
  spread <- X[,1]-X[,2]
  res <- NULL
  res["avg.spread.all"]   <- mean(spread)
  res["avg.spread.last3"] <- mean(tail(spread,3))
  res["avg.spread.last1"] <- tail(spread,1)
  res["sd.spread.all"]    <- sd(spread)
  res["sd.spread.last3"]  <- sd(tail(spread,3))

  return(res)
}


#####################################################################
# Get the trading signals and historical PnL on a given strategy.
#
inc.get.trading.signals <- function(bPP, methods)
{
  TRD.PnL <- NULL
  for (method in methods){
    cat(paste("Working on method ", method, "\n", sep=""))
    TRD <- lapply(bPP, method)
    TRD <- do.call(rbind, TRD)

    if (!is.null(TRD)){
      TRD$bucket.ptid <- sapply(strsplit(rownames(TRD), "\\."),
                              function(x){x[1]}) 
      rownames(TRD) <- NULL
 
      TRD.PnL[[method]] <- inc.price.trades(TRD, bPP)
      TRD.PnL[[method]]$method <- method
    }
  }
  TRD.PnL <- do.call(rbind, TRD.PnL)
  rownames(TRD.PnL) <- NULL
  
  return( TRD.PnL )
}

#####################################################################
# Price the trades ... 
# TRD - the data.frame with trading signals 
# bPP - list of historical prices: (RT|DA) pairs for bucket.ptid
#
inc.price.trades <- function(TRD, bPP)
{
  DEC <- ldply(bPP, function(X){
    data.frame(start.dt=index(X), value=X[,1] - X[,2])})
  
  # subset DEC by only the dates you're interested ...
  ind <- which(DEC$start.dt %in% unique(TRD$start.dt))
  if (length(ind)==0){
    TRD$value <- TRD$PnL <- NA
  } else {
    DEC <- DEC[ind,]
    names(DEC)[1] <- "bucket.ptid"

    TRD <- merge(TRD, DEC, all.x=TRUE)
    TRD$value <- TRD$value*(2*TRD$dec - 1)
    hours <- as.numeric(gsub(".*X|_.*","",TRD$bucket.ptid))
    TRD$PnL   <- TRD$value*hours
  }
  
  return(TRD)
}

#####################################################################
# pick - the number of nodes to look at
#
max_min_locations <- function(files, pick=6)
{
  load(files[1])  # RT prices
  rt <- tail(hPP, 24)
  
  aux <- melt(apply(rt[8:23,], 2, mean))      # RT ONPEAK from yesterday
  aux <- data.frame(ptid=gsub("NEPOOL_SMD_RT_(.*)_LMP", "\\1", rownames(aux)),
                    price=aux$value)
  aux <- aux[order(aux$price),]
  res <- rbind(cbind(bucket="Onpeak", type="RT.Min", head(aux, pick)),
               cbind(bucket="Onpeak", type="RT.Max", tail(aux, pick)))
  
  aux <- melt(apply(rt[c(1:7,24)], 2, mean))  # RT OFFPEAK from yesterday 
  aux <- data.frame(ptid=gsub("NEPOOL_SMD_RT_(.*)_LMP", "\\1", rownames(aux)),
                    price=aux$value)
  aux <- aux[order(aux$price),]
  res <- rbind(res,
               rbind(cbind(bucket="Offpeak", type="RT.Min", head(aux, pick)),
               cbind(bucket="Offpeak", type="RT.Max", tail(aux, pick))))

  load(files[3])  # DA prices
  da <- tail(hPP.DA, 24)
  if (as.Date(index(da)[1]) != Sys.Date()+1)
    warning("Not using tomorrow's DA data!")
  
  aux <- melt(apply(da[8:23,], 2, mean))      # DA ONPEAK for tomorrow
  aux <- data.frame(ptid=gsub("NEPOOL_SMD_DA_(.*)_LMP", "\\1", rownames(aux)),
                    price=aux$value)
  aux <- aux[order(aux$price),]
  res <- rbind(res,
               cbind(bucket="Onpeak", type="DA.Min", head(aux, pick)),
               cbind(bucket="Onpeak", type="DA.Max", tail(aux, pick)))
  
  aux <- melt(apply(da[c(1:7,24)], 2, mean))  # DA ONPEAK for tomorrow 
  aux <- data.frame(ptid=gsub("NEPOOL_SMD_DA_(.*)_LMP", "\\1", rownames(aux)),
                    price=aux$value)
  aux <- aux[order(aux$price),]
  res <- rbind(res,
               rbind(cbind(bucket="Offpeak", type="DA.Min", head(aux, pick)),
               cbind(bucket="Offpeak", type="DA.Max", tail(aux, pick))))

  res <- merge(res, MAP[, c("ptid", "name")], by="ptid")
  res <- res[, c("bucket", "type", "ptid", "name", "price")]
  res <- res[order(res$bucket, res$type, res$price),]
  

  aux <- split(res, res$bucket)
  aux <- lapply(aux, function(x, pick){
    y <- split(x, rep(c("DA", "RT"), each=2*pick))
    y1 <- capture.output(print(y[[1]], row.names=FALSE))
    y2 <- capture.output(print(y[[2]][,-1], row.names=FALSE))
    paste(y1, y2, sep="  |  ")
  }, pick)
  
  out <- c("Min/Max nodes in the pool:\n", aux[[1]], "\n", aux[[2]], "\n\n")
  
  out  
}


#####################################################################
# See if there are next day trades, and analyze them better by pulling
# the 5m RT symbols
#
next.day.trades <- function( TRD.PnL, options )
{
  NXT <- subset(TRD.PnL, is.na(PnL))
  if (nrow(NXT)==0){
    rLog("No proposed trades for tomorrow")
    return(NXT)
  }
  aux <- strsplit(NXT$bucket.ptid, "_")
  NXT$bucket <- sapply(aux, function(x){x[1]})
  NXT$ptid   <- sapply(aux, function(x){x[2]})
  NXT$bucket.ptid <- NULL
  NXT$asOfDate <- options$asOfDate
  NXT$start.dt <- NXT$start.dt + 1  # for next day
  NXT <- NXT[,c("start.dt", "dec", "bucket", "ptid", "method")]
  
#  all.NXT <- NXT; save(all.NXT, file=options$trades.file)
  if (nrow(NXT)>0){
    cat("Saving the proposed trades for tomorrow into archive...")
    load(options$trades.file)
    all.NXT <- subset(all.NXT, start.dt != NXT$start.dt[1])
    all.NXT <- rbind(all.NXT, NXT)
    rownames(all.NXT) <- NULL
    save(all.NXT, file=options$trades.file)
    cat("Done.\n")
  }

  return(NXT)
}



#####################################################################
# Called by calc_portfolio_pnl, get_booked_virtuals
# price_rtda is a zoo objects with hourly data, two columns: rt, da
# qty is in MW (can be a vector, if you have a shaped qty)
# hrs are in HourEnding convention (1:24)
#
price_one_incdec <- function(price_rtda, hrs=8:23, qty=1, is.INC=TRUE)
{
  if (length(qty)==1)
    qty <- rep(qty, length(hrs))  # if the quantity is fixed for all hours
  HH  <- as.numeric(format(index(price_rtda), "%H"))
  HH  <- ifelse(HH == 0, 24, HH)
  pnl <- subset(price_rtda, HH %in% hrs)

  if (length(qty) != nrow(pnl)) browser()
  
  list(mean(pnl[,1]), mean(pnl[,2]),
       pnl=sum(qty*(pnl[,2]-pnl[,1]))*ifelse(is.INC, 1, -1))   
}

#####################################################################
# Price a list of inc/decs
#
# For performance improvement, consider doing some caching of the tsdb pull
#
price_pfolio_incdec <- function(trd)
{
  res <- lapply(trd, function(x){
    if (is.null(x$price_rtda)){
      x$price_rtda <-  FTR:::.inc.dec.tsdbprices(x$ptid, "NEPOOL",
        as.POSIXct(paste(x$trade.start.date, "01:00:00")),
        as.POSIXct(paste(x$trade.end.date+1, "00:00:00")))
    }
    rLog("Working on ETI:", x$eti)
    price_one_incdec(x$price_rtda, hrs=x$hrs, qty=x$qty,
                              is.INC=x$is.INC)$pnl
  })

  res
}


#####################################################################
# PnL is a list with all the info
#
report_portfolio_pnl <- function(pnl, MAP)
{
  res <- lapply(pnl, function(x){x$pnl[,"pnl"]})
  res <- do.call(cbind, res)
  colnames(res) <- 1:ncol(res)
  res <- t(res)
  res <- round(res)

  node.names <- structure(MAP$location, names=MAP$ptid)
  
  rownames(res) <- lapply(pnl, function(x, node.names){
    paste(substr(x$inc.dec, 1, 3), x$mw, "MW",
          node.names[as.character(x$ptid)], x$bucket)}, node.names)
  colnames(res) <- format(as.Date(colnames(res)), "%m/%d")

  print(res)
}



#####################################################################
# Read incdec xls to see what we WANTED to submit
#
read_incdec_xls <- function(fname)
{
  require(xlsx)

  fname <- "S:/All/Structured Risk/NEPOOL/Incs & Decs/Apr 11/Inc Dec 15Apr11 Incs and Decs.xls"

  
  source("H:/user/R/Adrian/findataweb/temp/xlsx/trunk/R/read.xlsx2.R")
  DD <- read.xlsx2(fname, sheetName="Bids", startRow=3, startColumn=2,
#    noRows=116, 
    colClasses=c("numeric", rep("character", 3),
      rep(c("numeric", "numeric", "character"), 10), "character"))
  colnames(DD) <- NULL


  
  
.jcall(Rintf, "[D", "readColDoubles",
        .jcast(sheet, "org/apache/poi/ss/usermodel/Sheet"),
        as.integer(5), as.integer(15), as.integer(i))
  
.jcall(Rintf, "[S", "readColStrings",
        .jcast(sheet, "org/apache/poi/ss/usermodel/Sheet"),
        as.integer(startRow-1), as.integer(125), 
        as.integer(startColumn-1+i-1))
  
}


#####################################################################
# New trading strategy
#
strategy_hub <- function(hrs, startDt=as.POSIXct("2010-01-01"),
  endDt=Sys.time())
{
  nextDate <- Sys.Date()+1
  
  symbs <- c("nepool_smd_da_4000_lmp", "nepool_smd_rt_4000_lmp",
             "nepool_smd_rt5m_4000_lmp")

  res <- FTR.load.tsdb.symbols(symbs, startDt, endDt,
    aggHourly=c(FALSE, FALSE, TRUE))  
  colnames(res) <- c("lmp.da", "lmp.rt", "lmp.rt5m")
  suppressWarnings(res <- data.frame(time=index(res), as.matrix(res)))

  res <- res[!is.na(res$lmp.da),]  # rm some missing data
  ind <- which(is.na(res$lmp.rt) & !is.na(res$lmp.rt5m))
  if (length(ind) > 0)
    res[ind, "lmp.rt"] <- res[ind, "lmp.rt5m"]
  
  res$`da/rt` <- res$lmp.da  - res$lmp.rt
  res <- subset(res, !is.na(res$`da/rt`))
  res$hour <- as.numeric(format(res$time, "%H"))
  res$day  <- as.Date(format(res$time, "%Y-%m-%d")) 


  # look at onpeak hours
  aux <- subset(res, hour %in% hrs)
  DD  <- aggregate(aux[,c(2:3,5)], list(day=aux$day), mean)
  DD  <- split(DD, DD$day)
  DD[[format(nextDate)]] <- data.frame(day=nextDate)
  
  on  <- .lookback1(DD)
  #PnL <- do.call(rbind, lapply(on, function(x)x[,c("day", "PnL")]))
  #plot(PnL$day, cumsum(PnL$PnL), col="blue", type="l")
  
  on
}


#####################################################################
# prepare the string output
#
strategy_hub_rpt <- function(X, txt)
{
  aux <- do.call(rbind, tail(X[-length(X)], 10))
  aux[,c(2:4,7)] <- sapply(aux[,c(2:4,7)], round, 2)
  totalPnL <- sum(aux$PnL)
  fcst <- tail(X,1)[[1]]

  out <- paste(txt, " ",format(fcst$day), " to ",
    fcst$strategy, " ", fcst$qty, "MW", sep="")    
  out <- c(out, "\nLast 10 days of PnL for this strategy:")
  out <- c(out, capture.output(print(aux, row.names=FALSE)))
  out <- c(out, paste("           Total PnL:", totalPnL))

  out
}



#####################################################################
# select dates that you want to place your inc or dec based on 
# lookback only!
# RULE: look if previous day > a large treshold is good indicative for next
#       day
#
trigger.Persistence <- function(X, limit=10)
{
  TRD <- NULL
  X <- na.omit(X)
  spread <- X[,1] - X[,2]    # RT - DA
  ind <- which(spread > limit)
  if (length(ind) > 0){
    TRD <- data.frame(dec=TRUE, start.dt=index(spread)[ind]+1)
  }
  ind <- which(spread < -limit)
  if (length(ind) > 0){
    TRD <- rbind(TRD, data.frame(dec=FALSE, start.dt=index(spread)[ind]+1))
  }
#  TRD$ptid <- as.numeric(gsub("RT_|DA_", "",colnames(X)[1]))
  return(TRD)
}


#####################################################################
# search for when RT > DA by $40.  Is there reversion? 
# lookback only!
# RULE: look if previous day > a treshold is good indicative for next
#       day
#
trigger.1DayPanicDA <- function(X, limit=40)
{
  TRD <- NULL
  X <- na.omit(X)
  spread <- X[,1] - X[,2]   # RT - DA
  ind <- which(spread > limit)
  if (length(ind) > 0){     # inc the next day 
    TRD <- data.frame(dec=FALSE, start.dt=index(spread)[ind]+1)
  }
  return(TRD)
}

#####################################################################
# search for when RT > DA consistently over the past 7 days.  
# 
#
trigger.7DayDomination <- function(X, limit=5, last=7)
{
  TRD <- NULL
  X <- na.omit(X)
  spread <- X[,1] - X[,2]    # RT - DA
  spread <- tail(spread, last)
  if ((mean(spread) > limit) && (all(spread > 0))){
    TRD <- data.frame(dec=TRUE, start.dt=index(spread)[last]+1)
  }
  if ((mean(spread) < -limit) && (all(spread < 0))){
    TRD <- rbind(TRD, data.frame(dec=FALSE, start.dt=index(spread)[last]+1))
  }
  return(TRD)
  
}



#####################################################################
# simple trend following strategy.  If the inc made money in the
# previous day, stay an inc, if it lost money in the prev day, switch
# to a dec.
#
# DD is a list with one element per day
#
.lookback1 <- function(DD)
{
  N <- length(DD)

  DD[[1]]$strategy <- "INC"    
  DD[[1]]$qty <- 1
  DD[[1]]$PnL <- DD[[1]]$`da/rt` * DD[[1]]$qty
  
  # loop over all the time steps (days)
  for (d in 2:N) {
    # get the strategy for day d
    if (DD[[d-1]]$PnL > 0) {    # made money in the previous period, 
      DD[[d]]$strategy <- DD[[d-1]]$strategy   # stay the same
      DD[[d]]$qty <- 1.5        # increase the qty a bit
    } else {                    # do the opposite
      if (DD[[d-1]]$strategy=="INC") {
        DD[[d]]$strategy <- "DEC"
      } else {
        DD[[d]]$strategy <- "INC"
      }
      DD[[d]]$qty <- 1           
    }
    
    # calculate PnL in this time step
    DD[[d]]$PnL <- if (d != N){
      if (DD[[d]]$strategy == "INC") {
          DD[[d]]$qty * DD[[d]]$`da/rt`
        } else {
          -DD[[d]]$qty * DD[[d]]$`da/rt`
        }
    } else { NA }
  }
 
  DD 
}



## #####################################################################
## #
## get.latest.st.outage <- function()
## {
##   dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages"
##   files <- list.files(dir, full.names=T)

##   file <- files[length(files)]
##   res  <- read.csv(file)
  
##   return(res)
## }

## #####################################################################
## #
## compare.last2.outage.rpts <- function(asOfDate=Sys.Date())
## {
##   dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/Outages"
##   files <- list.files(dir, full.names=T)

##   time <- sapply(strsplit(files, "\\."), function(x){x[3]})
  
##   files <- files[(length(files)-1):length(files)]
  
##   res.1  <- read.csv(files[1])
##   names(res.1)[1] <- "time.1"
##   res.2  <- read.csv(files[2])
##   names(res.2)[1] <- "time.2"

##   aux <- merge(unique(res.1[,c("time.1", "application.number")]),
##     unique(res.2[,c("time.2", "application.number")]), all=T)

##   res.2$actual.start.date <- as.POSIXct(res.2$actual.start.date)
## #  res.2$actual.start.dt <- as.POSIXct(res.2$actual.start.dt)
  
##   res.2 <- subset(res.2, )asOfDate
## }


#####################################################################
# Check which nodes have not been mapped and email Michelle
# Some nodes are not in SecDb when booking.  Stupid!
#
# NOT USED ANYMORE.
# HAVE A PROCMON JOB CONDOR/CHECK_NEPOOL_INCDEC_NODE_MAPPING.R
## check.node.mapping <- function(incdec, email=FALSE)
## {
##   require(xlsReadWrite)
##   require(CEGxls)

##   fname <- paste("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/",
##     "Nepool Nodes Status.xls", sep="")
##   NN <- read.xls(fname, sheet=1)
##   NN$nameWoSpaces <- gsub(" ", "", NN$MappedNodes)

##   incdec$nameWoSpaces <- gsub(" ", "", incdec$name)
##   incdec <- unique(incdec[,c("name","nameWoSpaces")])

##   miss <- merge(incdec[,c("name", "nameWoSpaces")], NN, all=TRUE)

##   out <- ""
##   ind <- which(is.na(miss$MappedNodes))

##   if (length(ind)>0){
##     out <- c("Following nodes need to be mapped:\n")
##     ind <- is.na(miss$MappedNodes)
##     out <- c(out, paste(miss$name[ind], sep="", collapse="\n"))
##     # add the missing nodes to the MappedNodes column ...
##     miss$MappedNodes[ind] <- miss$name[ind]
##   }
##   if (out[1] == "") out <- "OK.  No nodes need mapping."

    
##   if (email){
##     to <- paste(c(
##       "michelle.bestehorn@constellation.com",
##       "eric.riopko@constellation.com",
##       "adrian.dragulescu@constellation.com"), sep="", collapse=",")
##     from <- "PM_AUTOMATIC_CHECK@constellation.com"
##     if (out[1] != "OK.  No nodes need mapping")
##       sendEmail(from, to, "Nepool inc/dec nodes to map", out)

##     rLog("Updating spreadsheet...")
##     write.xls(miss[,"MappedNodes", drop=FALSE], file=fname)
##     rLog("Done.")
##   } else {
##     print(out)
##   }
  
## }


## #####################################################################
## # Plot the PnL from the inc/dec trades suggested by the given strategy.
## #
## plot.trd.pnl <- function( TRD.PnL)
## {
##   PnL.day <- aggregate(TRD.PnL$PnL, list(start.dt=TRD.PnL$start.dt), sum)
##   windows()
##   plot(PnL.day$start.dt, PnL.day$x/1000, type="o", col="blue",
##        ylab="PnL, K$")
##   abline(h=0, col="gray")
## }

## #####################################################################
## # Select trades for next day based on historical performance
## #
## select.historical <- function( NXT, TRD.PnL)
## {
##   NXT$bucket.ptid <- paste(NXT$bucket, NXT$ptid, sep="_")

##   hPnL <- merge(TRD.PnL, NXT[,c("bucket.ptid", "dec")])
##   hPnL <- hPnL[order(hPnL$bucket.ptid, hPnL$dec, hPnL$start.dt), ]

##   aux <- cast(hPnL, bucket.ptid + dec ~ ., mean, na.rm=TRUE)
##   names(aux)[3] <- "avg.PnL"
##   aux <- aux[order(-aux$avg.PnL), ]
  
## }

