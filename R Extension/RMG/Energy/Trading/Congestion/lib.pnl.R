# General utilities to calculate PxQ PnL
# For examples, see NEPOOL/PnL/investigate_changes_pnl.R 
#
# calc_pnl_pq                workhorse
# .get_deltas_zone           from secdb
# .marks_spread              split the fix price into spreads/hub
#   .marks_spread_PWX
#   .marks_spread_PWY
# .positions_spread          split the positions into outright/spreads
# .pull_marks
# .spread_pnl                see the PnL from spreads not from FP
#
#

################################################################
# Calculate PnL impact on marks or positions change
#   PP1, PP2, QQ1, QQ2 - data.frames with at least (month, value) columns
# groups - can be "serviceType", "bucket", "zone", etc.  
# Correct PnL calculation should be: 
#   PnL = (P_2 - P_1)*Q_1 + dQ*(P_2 - P^*)
# where dQ  = Q_2 - Q_1, change in quantity
#       P^* = is the fix price of the new trades
#
# The current calculation PnL = P_2Q_2 - P_1Q_1 works when there are
# no new trades, quantity just resettles. 
#
calc_pnl_pq <- function( PP1, PP2, QQ1, QQ2, groups=c("year"),
  threshold=-1, do.format=FALSE )
{
  if (!identical(sort(names(PP1)), sort(names(PP2))) )
    stop("PP1 needs to have same columns as PP2")
  if (!identical(sort(names(QQ1)), sort(names(QQ2))) )
    stop("QQ1 needs to have same columns as QQ2")
  
  if ( any(!(c("month", "value") %in% names(PP1))) ) 
    stop("PP1, PP2 needs to have columns called month, value")
  if ( any(!(c("month", "value") %in% names(QQ1))) ) 
    stop("QQ1, QQ2 needs to have columns called month, value")

  ind <- groups %in% c("year", names(QQ1))
  if ( any(!ind) )
    stop("Position data.frame doesn't have columns", names(QQ1)[ind-1])
  

  # put the quantities side by side
  QQ <- rbind(cbind(QQ1, variable="Q1"),
              cbind(QQ2, variable="Q2"))
  QQ <- cast(QQ, ... ~ variable, I, fill=0)
  
  # put the prices side by side
  PP <- rbind(cbind(PP1, variable="P1"),
              cbind(PP2, variable="P2"))
  PP <- cast(PP, ... ~ variable, I, fill=NA)


  byColumns <- intersect(names(PP), names(QQ))
  rLog("Merging prices and qty by", byColumns)

  # put prices, qty together
  PnL <- merge(PP, QQ, all.y=TRUE)
  if (nrow(PnL)==0)
    stop("No common rows found when merging prices and positions!")

  PnL$PnL.dP <- PnL$Q1*(PnL$P2 - PnL$P1)     # due to price changes
  PnL$PnL.dQ <- PnL$P2*(PnL$Q2 - PnL$Q1)     # due to quantity changes
  PnL$PnL <- PnL$PnL.dP +  PnL$PnL.dQ        # total PnL 

  # always have the year
  PnL <- cbind(year=format(PnL$month, "%Y"), PnL)  

  # some cleaning
  ind <- which(is.na(PnL$PnL))
  if (length(ind)>0) {
    print(PnL[ind,])
    #browser()
    rLog("Some NA PnL from", paste(unique(PnL[ind,"location"])), 
      "that will be ignored!")
  }
  
  PnL <- PnL[!is.na(PnL$PnL), ]
  PnL <- PnL[abs(PnL$PnL)>=threshold, ]
  PnL <- PnL[order(PnL$month),]

  gPnL <- NULL
  # do some grouping
  if (nrow(PnL)>0 & !is.null(groups) ) {
    rLog("Group PnL by", groups)
    gPnL <- ddply(PnL, groups, function(x) {
      data.frame(PnL.dP=sum(x$PnL.dP),
                 PnL.dQ=sum(x$PnL.dQ),
                 PnL=sum(x$PnL)) 
    })
    
    if (do.format) {
      gPnL$PnL.dQ <- dollar(gPnL$PnL.dQ, 0)
      gPnL$PnL.dP <- dollar(gPnL$PnL.dP, 0)
      gPnL$PnL <- dollar(gPnL$PnL, 0)
    }
  }
  
  # return the PnL and the grouped PnL  
  list(PnL=PnL, gPnL=gPnL)
}




################################################################
# get the zonal positions from secdb
#  @param agg if TRUE you want to combine all the books into one data.frame
#  @param offpeak if TRUE combine the 2X16H, 7X8 buckets into OFFPEAK
#
.get_deltas_zone <- function(books, agg=TRUE, 
   asOfDate=as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")),
   peakOffpeak=FALSE)
{
  res <- lapply(books, function(book){
    aux <- secdb.readDeltas(asOfDate, book)
    qq  <- .format_secdb_deltas(aux)
  })
  names(res) <- books

  if (agg) {
    aux <- do.call(rbind, res)
    res <- cast(aux, market + bucket + location + month ~ ., sum, fill=0)
    names(res)[5] <- "value"
  } else {
    aux <- mapply(function(x, y){
      cbind(book=y, x)}, res, names(res), SIMPLIFY=FALSE)
    res <- do.call(rbind, aux)
    row.names(res) <- NULL
  }

  if (peakOffpeak) {
    X <- res
    colnames(X)[which(colnames(X)=="month")] <- "contract.dt"
    res <- PM:::combineBuckets(X, weight="none", buckets=c("2X16H", "7X8"),
      multiplier=rep(1, length(buckets)), name="OFFPEAK")
    res[which(res$bucket=="5X16"),"bucket"] <- "PEAK"
    colnames(res)[which(colnames(res)=="contract.dt")] <- "month"
  }
  
  res
}


################################################################
# Calculate the spread from a data.frame of marks
# You can use this for settled values, but you need to have the
# HUB values in, the code will not pull them correctly!
#
# @param X a data.frame with at least columns:
#    c("month", "mkt", "location", "bucket", "value")
#
#
.marks_spread <- function(X)
{
  if (!all(c("month", "mkt", "location", "bucket",
    "value") %in% colnames(X))) 
    stop("Wrong formatted data.frame.")

  Sx <- Sy <- NULL
  if ("PWX" %in% X$mkt) 
    Sx <- .marks_spread_PWX( subset(X, mkt=="PWX") )
  if ("PWY" %in% X$mkt) 
    Sy <- .marks_spread_PWY( subset(X, mkt=="PWY") )

  SS <- rbind(Sx, Sy)
  
  SS
}


################################################################
# Calculate the spread from a data.frame of marks
#
# @param X a data.frame with at least columns:
#    c("month", "mkt", "location", "bucket", "value")
# @param asOfDate used to pull the hub as of that date
#
.marks_spread_PWX <- function(X)
{
  if (!all(c("month", "mkt", "location", "bucket",
    "value") %in% colnames(X))) 
    stop("Wrong formatted data.frame.")
  if (!identical(unique(X$mkt), "PWX"))
    stop("You're got the wrong market!")

  ftrs <- spreads <- hub <- NULL
  
  # the FTRs
  ind <- grep("^MCC ", X$location)
  if (length(ind) > 0) {
    if (!("MCC HUB DA" %in% unique(X$location)))
      stop("Need to have MCC HUB DA in there!")
    hubF <- subset(X, location=="MCC HUB DA")
    colnames(hubF)[which(colnames(hubF)=="value")] <- "value.hub"
    ftrs <- merge(X[ind,], hubF[,c("mkt", "bucket", "month", "value.hub")])
    ftrs$value <- ftrs$value - ftrs$value.hub
    ftrs$value.hub <- NULL
    #ftrs <- subset(ftrs, location != "MCC HUB DA")  # don't take them out!
    X <- X[-ind,]
  }

  # the spreads
  if (nrow(X) > 0) {
    if (!("HUB DA" %in% unique(X$location)))
      stop("Need to have HUB DA in there!")
    hub <- subset(X, location=="HUB DA")
    colnames(hub)[which(colnames(hub)=="value")] <- "value.hub"
    
    spreads <- merge(X, hub[,c("mkt", "bucket", "month", "value.hub")])
    spreads$value <- spreads$value - spreads$value.hub
    spreads$value.hub <- NULL
    spreads <- subset(spreads, location != "HUB DA")
    
    colnames(hub)[which(colnames(hub)=="value.hub")] <- "value"
  }

  
  rbind(spreads, ftrs, hub)
}


################################################################
# Calculate the spread from a data.frame of marks
#
# @param X a data.frame with at least columns:
#    c("month", "mkt", "location", "bucket", "value")
#
#
.marks_spread_PWY <- function(X)
{
  if (!all(c("month", "mkt", "location", "bucket",
    "value") %in% colnames(X))) 
    stop("Wrong formatted data.frame.")
  if (!identical(unique(X$mkt), "PWY"))
    stop("You're got the wrong market!")

  # the FTRs
  ind <- grep("^DAM MCC ", X$location)
  if (length(ind) > 0)
    stop("NYISO TCCs not implemented!")
  
  if (!("DAM HUDV" %in% unique(X$location)))
    stop("Need to have DAM HUDV in the locations!")
  hub <- subset(X, location=="DAM HUDV")
  colnames(hub)[which(colnames(hub)=="value")] <- "value.hub"
  
  spreads <- merge(X, hub[,c("mkt", "bucket", "month", "value.hub")])
  spreads$value <- spreads$value - spreads$value.hub
  spreads$value.hub <- NULL
  spreads <- subset(spreads, location != "DAM HUDV")
  
  colnames(hub)[which(colnames(hub)=="value.hub")] <- "value"

  rbind(spreads, hub)
}






################################################################
# Split the position into a spread and a hub.
#
# @param X is a data.frame with columns c("book", "month", "bucket",
#   "mkt", "location", "value")
#
.positions_spread <- function(X)
{
 if (!all(colnames(X) %in% c("book", "month", "mkt", "location", "bucket",
    "value"))) 
    stop("Wrong formatted data.frame.")
  ftrs <- pwx <- pwy <- NULL

  # ftrs
  ind <- grep("^MCC ", X$location)
  if (length(ind) > 0) {
    aux <- sum(X[ind,"value"])
    if (abs(aux) >= 10)
      stop("FTRs are not balanced!")
    
    ftrs <- X[ind,]
    X <- X[-ind,]
  }

  spreads <- subset(X, !(location %in% c("HUB DA", "MCC HUB DA",
    "HUB", "DAM HUDV")))
  
  # aggregate everything under the nodal to the hub
  cnames <- setdiff(colnames(X), c("location", "value"))
  aux <- cast(X, as.formula(paste(paste(cnames, collapse=" + "), "~ .")),
    sum, fill=0, value="value")
  colnames(aux)[ncol(aux)] <- "value"

  ind <- which(aux$mkt == "PWX")
  if (length(ind) > 0) {
    pwx <- aux[ind,]
    pwx$location <- "HUB DA"
  }

  ind <- which(aux$mkt == "PWY")
  if (length(ind) > 0) {
    pwy <- aux[ind,]
    pwy$location <- "DAM HUDV"
  }
  
  rbind(ftrs, pwx, pwy, spreads)
}




################################################################
#
# @param delivPt is a vector, then mkt=PWX, else a data.frame with
#   columns c("location", "mkt")
#
.pull_marks <- function(asOfDate, deliveryPt,  
  startDate=nextMonth(), 
  endDate=NULL,
  bucket=c("5X16", "7X8", "2X16H"))
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(startDate, "%Y"))+5, "-12-01", sep=""))
  
  if (is.vector(deliveryPt)) 
    deliveryPt <- data.frame(location=deliveryPt, mkt="PWX")
  
  serviceType <- c("ENERGY")
  
  fwd.x <- fwd.y <- NULL
  if (any(deliveryPt$mkt == "PWX")) {
    aux <- subset(deliveryPt, mkt=="PWX")
    location <- aux$location
    commodity <- "COMMOD PWR NEPOOL PHYSICAL"
    fwd.x <- cbind(mkt="PWX",
      PM:::secdb.getElecPrices( asOfDate, startDate=startDate,
      endDate, commodity, location, bucket, serviceType, useFutDates=FALSE))
  }
  if (any(deliveryPt$mkt == "PWY")) {
    aux <- subset(deliveryPt, mkt=="PWY")
    location <- aux$location
    commodity <- "COMMOD PWR NYPP PHYSICAL"
    fwd.y <- cbind(mkt="PWY",
      PM:::secdb.getElecPrices( asOfDate, startDate=startDate,
      endDate, commodity, location, bucket, serviceType, useFutDates=FALSE))
  }
  if (any(deliveryPt$mkt == "PWJ")) {
    aux <- subset(deliveryPt, mkt=="PWJ")
    location <- aux$location
    commodity <- "COMMOD PWR PJM PHYSICAL"
    fwd.y <- cbind(mkt="PWJ",
      PM:::secdb.getElecPrices( asOfDate, startDate=startDate,
      endDate, commodity, location, bucket, serviceType, useFutDates=FALSE))
  }
  
  fwd <- rbind(fwd.x, fwd.y)
  
  fwd$asOfDate <- NULL
  fwd$serviceType <- NULL
  
  fwd
}


################################################################
# Given a PxQ PnL output from calc_pnl_pq, calculate the PnL
# from a spread point of view, to see which spread contributed
# @param X a data.frame with at least columns:
#    c("month", "mkt", "location", "bucket", "P1", "P2", "Q1", "Q2")
#
.spread_pnl <- function(X)
{
  if (!all(c("month", "mkt", "location", "bucket", "P1", "P2",
    "Q1", "Q2") %in% colnames(X))) 
    stop("Wrong formatted data.frame.")

  P1 <- X[,c("month", "mkt", "location", "bucket", "P1")]
  P2 <- X[,c("month", "mkt", "location", "bucket", "P2")]


  
}



################################################################
################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(SecDb) 
  require(reshape)

  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  #source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/PnL/investigate_changes_pnl.R")
  
  day1 <- as.Date("2012-11-12")
  day2 <- as.Date("2012-11-13")

  books <- "CONGIBT"
  books <- "NPFTRMT1"
  QQ1 <- .get_positions_book(books, day1)
  QQ2 <- .get_positions_book(books, day2)

  PP1 <- .pull_marks(day1, unique(QQ1$location))
  PP2 <- .pull_marks(day2, unique(QQ2$location))
  
  pnl <- calc_pnl_pq(PP1, PP2, QQ1, QQ2)
  PnL <- pnl[["PnL"]]
  gPnL <- pnl[["gPnL"]]
  gPnL

  sum(gPnL$PnL)

  aux <- cast(PnL, location ~ ., sum, value="PnL")
  print(aux[order(aux[,2]),], row.names=FALSE)

  PnL$dP <- PnL$P2 - PnL$P1
  plot(PnL$month, PnL$dP, type="o", col="blue")
 
   
  ############################################################
  PP <- .pull_marks(as.Date("2013-06-18"), c("MCC ADAMS.4310 DA"))

  
}












## ################################################################
## # Split the position into a spread and a hub.
## #
## # @param X is a data.frame with columns c("book", "month", "bucket",
## #   "mkt", "location", "value")
## #
## .positions_spread_PWX <- function(X, cnames)
## {  
##   # FTRs are always balanced
##   ind <- grep("^MCC ", X$location)
##   if (length(ind) > 0) {
##     aux <- sum(X[ind,"value"])
##     if (abs(aux) >= 10)
##       stop("FTRs are not balanced!")
    
##     X <- X[-ind,]
##   }

##   aux <- cast(X, as.formula(paste(paste(cnames, collapse=" + "), "~ .")),
##     sum, fill=0, value="value")

## }

