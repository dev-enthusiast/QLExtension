#
#
FTR.reportBids.NEPOOL <- function(filepath, ISO.env)
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.analyze.paths.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  this.auction <- "G09"
  prev.auction <- "F08"
  root <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/2009-02/"
  setwd(root)
  
  library(FTR); library(gplots); library(xtable)
  FTR.load.ISO.env("NEPOOL")
  load("all.data.RData")
 
  # table with higest sources, sinks MW; SHOULD SPLIT BY BUCKET!!!!
  (TT.max.node.mw <- .table.max.mw.node( bids.max ))
  
  # check the historical behaviour of the portfolio
  aux <- .historicalPortfolio.NEPOOL( hSP, bids.min )
  hPayoff    <- aux[[1]]
  (TT.maxmin.hPayoff <- aux[[2]])
  (TT.sum.hPayoff <- as.data.frame(as.list(summary(hPayoff[,"hPayoff"]))))

  range.box <- apply(RR, 1, function(x){boxplot.stats(x)$stats})
  IQR <- range.box[4,] - range.box[2,]

##   # what I've done in the last auction. 
##   aux <- .get.nestat.performance()
##   hQQ          <- aux[[1]]   # positions for current month
##   cRR          <- aux[[2]]   # cumulative returns 
##   TT.hpay      <- aux[[3]]
##   TT.rankpaths <- aux[[4]]

  # where do I make the zonal market
  zpaths <- expand.grid(class.type=c("OFFPEAK", "ONPEAK"), sink.ptid=4001:4008)
  zpaths$source.ptid <- 4000
  zpaths <- cbind(path.no=1:nrow(zpaths), zpaths)
  
  ( TT.zonal.BO <- .cp.bidrange.table( zpaths, options, NEPOOL,
      cut.quantile=list(c(0.01,0.10)), add.CP=FALSE ))

  zCPSP <- FTR.get.CP.for.paths(zpaths, NEPOOL, add.SP=TRUE)
  RPT$plot.CP.vs.SP(zCPSP)


##   rho <- .temperatureDependence( hSP )
##   TT.Tmax.sensitive <- cbind(paths[as.numeric(names(rho))[1:20],
##     c("class.type", "source.name", "sink.name")], rho=rho[1:20])
##   TT.Tmax.sensitive$class.type <- gsub("PEAK","",
##     TT.Tmax.sensitive$class.type)
##   names(TT.Tmax.sensitive)[1] <- "class"

##   source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.awardPaths.R")
##   source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.getPathReturns.R")
##  bids.xls$strategy <- "Annual"
  aux <- .AwardLikePrevAuction(prev.auction, bids.xls, NEPOOL)
  (TT.awds.strategy.prev <- aux[[1]])
  (TT.awds.area.prev <- aux[[2]])
  awds.prev <- aux[[3]]
  
  start.dt <- Sys.Date()-31
  RR.prev.30 <- FTR.getPathReturns(awds.prev, start.dt=start.dt, ISO.env=NEPOOL,
                                   hourly=TRUE)  
#  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.histPortfolioPerformance.R")
  aux <- FTR.histPortfolioPerformance( awds.prev, RR.prev.30,
                                      region="NEPOOL", scale=5000)
  cRR.prev.30  <- aux[[1]]
  FTR:::.plot.cumulative.payoff(cRR.prev.30)

  qCP.prev <- .QuantilesPrevAuction( prev.auction, paths, sSP, NEPOOL)
  
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  aux <- .AuctionHighLow( paths, bids.xls, NEPOOL )
  (TT.awds <- aux[[1]])
  (aux[[2]])

  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.get.CP.for.paths.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.expectedAwards.R")
  eAwds <- FTR.expectedAwards(bids.xls, NEPOOL)
  eAwds <- merge(eAwds, FTR.PathNamesToPTID(eAwds, NEPOOL$MAP, to="area"))
  start.dt <- Sys.Date()-31
  aux <- eAwds; aux$CP <- eAwds$bid.price
  RR.eAwds.30 <- FTR.getPathReturns(aux, start.dt, ISO.env=NEPOOL,
                                    hourly=TRUE)
  aux <- FTR.histPortfolioPerformance(eAwds, RR.eAwds.30,
    region="NEPOOL", scale=10000)
  cRR.eAwds.30 <- aux[[1]]
  
  aux <- as.numeric(tail(cRR.eAwds.30,1)); names(aux) <- colnames(cRR.eAwds.30)
  sort(aux)[1:10]

  eAwds <- merge(eAwds, paths[,c("path.no","hours")])
  aux <- eAwds; aux$CP.hour <- eAwds$bid.price; aux$award.ftr.mw <- eAwds$mw
  
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  PP.eAwds <- .simulated.payoffs(sSP, aux)
  aux <- colSums(PP.eAwds)
  (TT.sum.eAwds <- as.data.frame(as.list(summary(aux))))
  
  save.image(file=paste(root,"Reports/Bids/bidsReport.RData",sep="")) 


##   load("S:/All/Structured Risk/NEPOOL/FTRs/Analysis/2008-08/Reports/Bids/bidsReport.RData")
  
}

## #################################################################
## # See the bids.min if the auction clears High or Low
## .AuctionHighLow <- function( paths, bids.xls )
## {
##   awds <- NULL
##   paths.high <- subset(paths, is.counterflow %in% c(TRUE, NA))$path.no
##   paths.low  <- subset(paths, is.counterflow %in% c(FALSE, NA))$path.no

##   aux <- subset(bids.xls, path.no %in% paths.high)
##   awds$high   <- aggregate(aux$mw, list(upto.bid.no=aux$bid.no), sum)
##   awds$high$x <- cumsum(awds$high$x)
##   colnames(awds$high)[2] <- "mw"

##   CT <- subset(aux, source.area=="CT" & sink.area != "CT")
##   if (nrow(CT)>0){
##     awds$high.CT <- aggregate(CT$mw, list(upto.bid.no=CT$bid.no), sum)
##     awds$high.CT$x <- cumsum(awds$high.CT$x)
##     colnames(awds$high.CT)[2] <- "mw"
##   }
  
##   aux <- subset(bids.xls, path.no %in% paths.low)
##   awds$low   <- aggregate(aux$mw, list(upto.bid.no=aux$bid.no), sum)
##   awds$low$x <- cumsum(awds$low$x)
##   colnames(awds$low)[2] <- "mw"

##   CT <- subset(aux, source.area=="CT" & sink.area != "CT")
##   if (nrow(CT)>0){
##     awds$low.CT <- aggregate(CT$mw, list(upto.bid.no=CT$bid.no), sum)
##     awds$low.CT$x <- cumsum(awds$low.CT$x)
##     colnames(awds$low.CT)[2] <- "mw"
##   }
  
##   TT.awds <- cbind(data.frame(clears="high", awds$high),
##                    data.frame(clears="low",  awds$low))
##   return(list(TT.awds, awds))
## }

## #################################################################
## # 
## .risk.reward <- function( paths, sSP, bids )
## {
##   probs <- matrix(c(0,1), ncol=nrow(paths),
##     nrow=2, dimnames=list(c("out",1), paths$path.no))
##   bids.min <- FTR.simulateAwards( bids, probs, noSims=1)
##   bids.min <- merge(bids.min, paths[,c("path.no","hours")])
##   bids.min$mwh <- bids.min$mw * bids.min$hours

##   # simulated returns
##   RR  <- sSP - matrix(bids.min$bid.price, nrow=nrow(sSP),
##                       ncol=ncol(sSP))
##   rownames(RR) <- rownames(sSP)

##   # basic stats
##   RR.bs <- cbind(t(apply(RR, 1, function(x){summary(na.omit(x))})),
##            t(apply(RR, 1, quantile, probs=c(0.05, 0.95))))
##   RR.bs <- cbind(RR.bs, IQR=RR.bs[,"3rd Qu."]-RR.bs[,"1st Qu."])
##   RR.bs <- cbind(RR.bs, sigma.lt.0=RR.bs[,"Median"] - RR.bs[,"1st Qu."])
##   RR.bs <- cbind(RR.bs, ratio= RR.bs[,"Median"]/RR.bs[,"sigma.lt.0"])
  
##   return(list(RR, RR.bs, bids.min))  
## }

## #################################################################
## # How has this portfolio performed historically
## #
## .historicalPortfolio <- function( hSP, bids.min )
## {
##   # find the peak/offpeak number of hours by day
##   ind <- secdb.getHoursInBucket("NEPOOL", "offpeak", index(hSP)[1],
##                                 index(hSP)[nrow(hSP)])
##   ind$date <- format(ind$time, "%Y-%m-%d")
##   aux <- table(ind$date)
##   ind <- zoo(as.numeric(aux), as.Date(names(aux)))
##   hrs <- matrix(NA , nrow=nrow(hSP), ncol=ncol(hSP))
##   colnames(hrs) <- colnames(hSP)
##   off <- bids.min$class.type=="OFFPEAK"
##   hrs[,as.character(which(off))]  <- ind
##   hrs[,as.character(which(!off))] <- 24-ind
  
  
##   aux <- hSP - matrix(bids.min$bid.price, nrow=nrow(hSP),
##                     ncol=ncol(hSP), byrow=T)
##   aux[is.na(hSP)] <- 0
##   aux <- aux * hrs                    # multiply with # hours
##   hPayoff <- aux %*% bids.min$mw      # multiply with # mw  
##   rownames(hPayoff) <- as.character(index(hSP))
##   colnames(hPayoff) <- "hPayoff"
##   hPayoff <- data.frame(date=as.Date(rownames(hPayoff)), hPayoff)
##   rownames(hPayoff) <- NULL

##   hPayoff.bkt <- data.frame(
##      offpeak=aux[,as.character(which(off))] %*% bids.min$mw[which(off)],
##      onpeak =aux[,as.character(which(!off))] %*% bids.min$mw[which(!off)])

  
##   summary(hPayoff$hPayoff)

##   hPayoff <- hPayoff[order(hPayoff$hPayoff), ]
##   TT.maxmin.hPayoff <- cbind(head(hPayoff,20),
##                              tail(hPayoff,20)[20:1,])
##   TT.maxmin.hPayoff[,1] <- as.character(TT.maxmin.hPayoff[,1]) 
##   TT.maxmin.hPayoff[,3] <- as.character(TT.maxmin.hPayoff[,3]) 

##   return(list(hPayoff, TT.maxmin.hPayoff)) 
## }


## #################################################################
## # get the uptodate nestat performance
## .get.nestat.performance <- function(last.bday =
##    as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")))
## {

##   QQ <- FTR.loadPositions(books="NESTAT", asOfDate=last.bday)
##   QQ$path.no <- 1:nrow(QQ)
##   QQ <- subset(QQ, as.Date(start.dt) <= last.bday)
  
##   start.dt <- as.POSIXct(paste(format(Sys.Date(), "%Y-%m-01"),
##                                "00:00:00"))
##   QQ$auction <- FTR.AuctionTerm(QQ$start.dt, QQ$end.dt)
##   # add the clearing prices
##   QQ <- merge(QQ, FTR.get.CP.for.paths(QQ, NEPOOL))
##   # get the historical hourly returns
##   this.end.dt <- as.POSIXlt(format(Sys.time()+24*3600, "%Y-%m-%d %H:00:00"))
##   hRR <- FTR.getPathReturns(QQ, start.dt, end.dt=this.end.dt,
##     ISO.env=NEPOOL, hourly=TRUE)
  
##   cRR <- hRR; cRR[is.na(cRR)] <- 0
##   cRR <- apply(cRR, 2, cumsum)

##   QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NEPOOL$MAP, to="area"), all.x=T)
##   QQ$class.from.to <- paste(QQ$class.type, " ", QQ$source.area,
##     " to ", QQ$sink.area, sep="")

##   QQ <- cbind(QQ[order(QQ$path.no),], hPayoff=cRR[nrow(cRR),])

##   TT.hpay <- aggregate(QQ[, c("mw", "hPayoff")],
##                        list(group=QQ$class.from.to), sum)
##   TT.hpay <- TT.hpay[order(TT.hpay$hPayoff, decreasing=T),]
##   TT.hpay <- subset(TT.hpay, mw > 1)

##   QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NEPOOL$MAP,
##                 to="name"), all.x=T)

##   TT.rankpaths <- QQ[, c("class.type","source.name",
##                          "sink.name", "mw", "hPayoff")]
##   TT.rankpaths <- TT.rankpaths[order(-TT.rankpaths$hPayoff), ]
##   TT.rankpaths <- rbind(head(TT.rankpaths, 10),
##                         tail(TT.rankpaths, 10))
##   TT.rankpaths$hPayoff <- round(TT.rankpaths$hPayoff)
  
##   return(list(QQ, cRR, TT.hpay, TT.rankpaths))
## }







## #################################################################
## # Superseeded by .cp.zonal.bidrange in FTR.reportAwards.NEPOOL.R
## .table.zonal.bidoffers <- function(ISO.env, cut.quantile=list(c(0.01, 0.20)))
## {
##   this.cut.quantile <- cut.quantile
##   paths <- expand.grid(class.type=c("OFFPEAK", "ONPEAK"), sink.ptid=4001:4008)
##   paths$source.ptid <- 4000
##   paths <- cbind(path.no=1:nrow(paths), paths)
  
##   rLog("Simulate the paths settle price.")
##   hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)
##   hSP <- window(hSP,
##    start=seq(options$start.dt, by="-28 months", length.out=2)[2])
##   sSP <- FTR.simulate.SP(hSP, options, noSims=10000, noEns=100)
  
##   # if you want to buy them
##   BL <- FTR.makeBidLimits(paths, sSP=sSP, 
##                            cut.quantile=this.cut.quantile)
##   BL  <- BL[,c("path.no", "min.bid", "max.bid")]
##   BL <- merge(paths, BL)
##   BL <- BL[order(BL$path.no), ]  
  
##   # if you want to sell the paths
##   names(paths)[3:4] <- c("source.ptid", "sink.ptid")
##   paths$auction <- NULL
##   SL <- FTR.makeBidLimits(paths, sSP=sSP, 
##     cut.quantile=list(rev(c(1-this.cut.quantile[[1]]))))
##   SL  <- SL[,c("path.no", "min.bid", "max.bid")]
##   names(SL)[2:3] <- c("min.offer", "max.offer")
## #  SL[,2:3] <- -SL[,2:3]
## #  SL[,2:3] <- SL[,3:2]
##   BSL <- merge(BL, SL)   # put the bids & offers together

##   BSL[,5:8] <- round(BSL[,5:8],2)
##   BSL$path.no <- NULL
##   return(BSL)    
## }



##   plot(hPayoff$date, hPayoff$hPayoff/1000000, col="blue")
##   temp.max <- tsdb.readCurve("temp_clean_bdr_dly_max",
##          as.Date("2004-01-01"), Sys.Date())
##   hPayoff <- merge(hPayoff, temp.max, all.x=T)
##   plot(hPayoff$value, hPayoff$hPayoff)
##   temp.min <- tsdb.readCurve("temp_clean_bdr_dly_min",
##          as.Date("2004-01-01"), Sys.Date())


#  range.box <- apply(RR, 1, function(x){boxplot.stats(x)$stats})
#  IQR <- range.box[4,] - range.box[2,]
#  RR  <- RR[order(IQR), ]

##   QQ   <- FTR:::.assignBidQuantity(RR) # from FTR.makeBids
##   QQ    <- aux[[1]]
##   RR.bs <- aux[[2]]
  

##   hRR <- zoo(diff(cRR), as.POSIXct(rownames(cRR)))
##   aux <- window(hRR, start = as.POSIXct("2008-06-08"),
##                      end   = as.POSIXct("2008-06-09"))
##   aux <- colSums(aux)
##   aux <- sort(aux)
##   ind <- as.numeric(names(aux)[which(aux < 0)])
##   aux <- subset(hQQ, path.no %in% ind)
##   aux <- aux[1:20,]
##   aux <- aux[,c("mw","sink.ptid","source.ptid", "source.area", "sink.area",
##                 "source.name", "sink.name")]
  
