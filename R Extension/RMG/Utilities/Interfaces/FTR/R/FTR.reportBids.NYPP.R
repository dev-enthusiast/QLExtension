# 
#
FTR.reportBids.NYPP <- function(filepath, ISO.env)
{
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportBids.NYPP.R")
  this.auction <- "Z08"
  prev.auction <- "X08"
  root <- "S:/All/Structured Risk/New York/TCC/NESTAT/2008-12/"
  setwd(root)
  
  library(FTR); library(gplots); library(xtable)
  FTR.load.ISO.env("NYPP")
  load("all.data.RData")
 
  # table with higest sources, sinks MW
  (TT.max.node.mw <- .table.max.mw.node( bids.max ))
  
  # check the historical behaviour of the portfolio
  aux <- .historicalPortfolio.NYPP( hSP, bids.min )
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
  zpaths <- data.frame(class.type="FLAT", sink.ptid=options$zones.ptid)
  zpaths$source.ptid <- 61752 # options$reference.ptid
  zpaths <- cbind(path.no=1:nrow(zpaths), zpaths)
  zpaths <- subset(zpaths, sink.ptid <= 61762)  # ignore the external ties
   
  (BSL <- .cp.bidrange.table(zpaths, options,  NYPP,
      cut.quantile=list(c(0.01,0.49))) )
  BSL <- merge(BSL, FTR.PathNamesToPTID(BSL, NYPP$MAP, to="name"))
  BSL$source.ptid <- BSL$sink.ptid <- BSL$path.no <- NULL
  (TT.zonal.BO <- BSL[-1,c(1, 6:7, 2:5)])

##   rho <- .temperatureDependence( hSP )
##   TT.Tmax.sensitive <- cbind(paths[as.numeric(names(rho))[1:20],
##     c("class.type", "source.name", "sink.name")], rho=rho[1:20])
##   TT.Tmax.sensitive$class.type <- gsub("PEAK","",
##     TT.Tmax.sensitive$class.type)
##   names(TT.Tmax.sensitive)[1] <- "class"

  aux <- .AwardLikePrevAuction(prev.auction, bids.xls, NYPP)
  (TT.awds.strategy.prev <- aux[[1]])
  (TT.awds.area.prev <- aux[[2]])
  awds.prev <- aux[[3]]

  start.dt <- Sys.Date()-31
  RR.prev.30  <- FTR.getPathReturns(awds.prev, start.dt, ISO.env=NYPP, hourly=TRUE)
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.histPortfolioPerformance.R")
  aux <- FTR.histPortfolioPerformance(awds.prev, RR.prev.30, region="NYPP")
  cRR.prev.30 <- aux[[1]]
 
  qCP.prev <- .QuantilesPrevAuction( prev.auction, paths, sSP, NYPP)

  # See the awarded quantities if the auction clears High or Low
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  aux <- .AuctionHighLow( paths, bids.xls, NYPP )
  (TT.awds <- aux[[1]])
  (aux[[2]])

  # 
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.get.CP.for.paths.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.expectedAwards.R")
  eAwds <- FTR.expectedAwards(bids.xls, NYPP)
  eAwds <- merge(eAwds, FTR.PathNamesToPTID(eAwds, NYPP$MAP, to="zone"))

  RR.eAwds  <- sSP - matrix(eAwds$bid.price, nrow=nrow(sSP), ncol=ncol(sSP))
  PnL.eAwds <- t(eAwds$mw) %*% RR.eAwds
  round((options$hours*summary(as.numeric(PnL.eAwds))),0)
  
  start.dt <- Sys.Date()-31
  eAwds$CP <- eAwds$bid.price
  RR.eAwds.30 <- FTR.getPathReturns(eAwds, start.dt, ISO.env=NYPP, hourly=TRUE)
  aux <- FTR.histPortfolioPerformance(eAwds, RR.eAwds.30, region="NYPP")
  cRR.eAwds.30 <- aux[[1]]
  
  save.image(file=paste(root,"Reports/Bids/bidsReport.RData",sep="")) 


##   load("S:/All/Structured Risk/NYPP/FTRs/Analysis/2008-08/Reports/Bids/bidsReport.RData")
  
}




## #################################################################
## # get the uptodate nestat performance
## .get.nestat.performance <- function(last.bday=as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")))
## {

##   QQ <- FTR.loadPositions(books="NESTAT", asOfDate=last.bday)
##   QQ$path.no <- 1:nrow(QQ)
##   QQ <- subset(QQ, as.Date(start.dt) <= last.bday)
  
##   start.dt <- as.POSIXct(paste(format(Sys.Date(), "%Y-%m-01"),
##                                "00:00:00"))
##   QQ$auction <- FTR.AuctionTerm(QQ$start.dt, QQ$end.dt)
##   # add the clearing prices
##   QQ <- merge(QQ, FTR.get.CP.for.paths(QQ, NYPP))
##   # get the historical hourly returns
##   this.end.dt <- as.POSIXlt(format(Sys.time()+24*3600, "%Y-%m-%d %H:00:00"))
##   hRR <- FTR.getPathReturns(QQ, start.dt, end.dt=this.end.dt,
##     ISO.env=NYPP, hourly=TRUE)
  
##   cRR <- hRR; cRR[is.na(cRR)] <- 0
##   cRR <- apply(cRR, 2, cumsum)

##   QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NYPP$MAP, to="area"), all.x=T)
##   QQ$class.from.to <- paste(QQ$class.type, " ", QQ$source.area,
##     " to ", QQ$sink.area, sep="")

##   QQ <- cbind(QQ[order(QQ$path.no),], hPayoff=cRR[nrow(cRR),])

##   TT.hpay <- aggregate(QQ[, c("mw", "hPayoff")],
##                        list(group=QQ$class.from.to), sum)
##   TT.hpay <- TT.hpay[order(TT.hpay$hPayoff, decreasing=T),]
##   TT.hpay <- subset(TT.hpay, mw > 1)

##   QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NYPP$MAP,
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
## # 
## .table.zonal.bidoffers <- function(ISO.env, cut.quantile=list(c(0.01, 0.20)))
## {
##   this.cut.quantile <- cut.quantile
##   aux <- subset(NYPP$MAP, type=="ZONE")
##   paths <- data.frame(path.no=1:(nrow(aux)-1), class.type="FLAT",
##     source.ptid=aux$ptid[1], sink.ptid=aux$ptid[2:nrow(aux)])
##   paths <- paths[1:10,]
  
##   rLog("Simulate the paths settle price.")
##   hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)
##   hSP <- window(hSP,
##    start=seq(options$start.dt, by="-28 months", length.out=2)[2])
##   sSP <- FTR.simulate.SP(hSP, options, noSims=10000, noEns=100)
  
##   # if you want to buy them
##   BL <- FTR.makeBidLimits(paths, sSP=sSP, 
##                            cut.quantile=this.cut.quantile)
##   names(BL)[2:3] <- paste("q", cut.quantile[[1]]*100, sep="")
##   BL <- merge(paths, BL)
##   BL <- BL[order(BL$path.no), ]  
  
##   # if you want to sell the paths
##   names(paths)[3:4] <- c("source.ptid", "sink.ptid")
##   paths$auction <- NULL
##   SL <- FTR.makeBidLimits(paths, sSP=sSP, 
##     cut.quantile=list(rev(c(1-this.cut.quantile[[1]]))))
##   names(SL)[2:3] <- paste("q", rev(1-cut.quantile[[1]])*100, sep="")
##   BSL <- merge(BL, SL)   # put the bids & offers together

##   BSL <- merge(BSL, FTR.PathNamesToPTID(BSL, NYPP$MAP, to="name"))
##   BSL$source.ptid <- BSL$sink.ptid <- NULL
##   BSL <- BSL[,c(1:2, 7:8, 3:6)]
  
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
  
