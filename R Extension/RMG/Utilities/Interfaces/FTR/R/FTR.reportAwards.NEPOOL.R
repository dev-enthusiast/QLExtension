# Show various stats regarging the FTR auctions in NEPOOL
#
#


reportAwards.NEPOOL <- function()
{
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  require(CEGbase)
  require(FTR)

  
  FTR.load.ISO.env("NEPOOL")
  NEPOOL$AWD <- NEPOOL$.loadAwards()
  NEPOOL$CP <- NEPOOL$.loadClearingPrice()
  
  NESTAT       <- TRUE    # did NESTAT participate
  this.auction <- "V13"
  this.month   <- "2013-10"
  options <- FTR.makeDefaultOptions(this.auction, NEPOOL)

  dirBids <- paste("S:/All/Structured Risk/NEPOOL/FTRs/",
    "Monthly Auction Analyses/2013/", this.month, "/", sep="")
  allBidsFiles <- c(paste(dirBids, "uploadBids1_", this.auction, ".csv", sep=""),   # House
                    paste(dirBids, "uploadBids2_", this.auction, ".csv", sep=""))   # NESTAT
  CEGbids <- .loadAllCEGBids(allBidsFiles)

  # all awards by bid file, including overlapping awards
  CEGawds <- .getAwardedPaths.House(CEGbids, NEPOOL, this.auction)
  split(CEGawds, CEGawds$file)

  # total awards
  aux <- unique(CEGawds[,c("source.name", "sink.name", "class.type", 
    "buy.sell", "award.ftr.price", "award.ftr.mw")])
  rownames(aux) <- NULL; print(aux)

  
  # download the auction_results.csv file and then run
  .format_results_for_booking(this.auction)

  # get zonal clearing prices
  print(.get_zonal_cp(this.auction), row.names=FALSE)

  
  # the awards of other big participants 
  aux <- .all.awards.stats.NEPOOL(this.auction, NEPOOL)
  TT.awd.mw.name <- aux[[1]]
  TT.awd.mw.name.class.area <- aux[[2]]
  top.names <- aux[[3]]
  TT.awds.top9 <- aux[[4]]
  
  # top 20 awards
  (TT.top20.awds <- .top20.awards(this.auction, NEPOOL))

  # AWDs net flows
  TT.netflows.all <- .netflows.awards( TT.awd.mw.name.class.area )

  # see the largest awards
  print(.largest.awards(this.auction, NEPOOL), row.names=FALSE)

  # who else got Nema
  aux <- subset(NEPOOL$AWD, auction==options$auction & sink.ptid == 4008)
  aux[order(aux$class.type, -aux$award.ftr.mw),]


  
  # where did the zones clear relative to enhanced history
  zpaths <- expand.grid(class.type=c("OFFPEAK", "ONPEAK"), sink.ptid=4001:4008)
  zpaths$source.ptid <- 4000
  zpaths <- cbind(path.no=1:nrow(zpaths), zpaths)
  
  ( TT.zonal.BO <- .cp.bidrange.table( zpaths, options, NEPOOL,
      cut.quantile=list(c(0.01,0.10)), add.CP=TRUE ))

  # PnL for the previous auctions for the top participants.
  auctions <- c("Z12", "F13")
  PnL.auctions <- .table.PnL.auctions(auctions)


  # MW awarded by participant
  aux <- cast(NEPOOL$AWD, customer.name ~ ., sum, value="award.ftr.mw",
              subset=auction==options$auction)
  colnames(aux)[2] <- "total.mw"
  print(head(aux[order(-aux$total.mw), ], 20), row.names=FALSE)
  
  # book the FTR trades NOW!
  # add to the FTR.paths.xlsx file ...
  source("H:/user/R/RMG/Utilities/Interfaces/FTR/R/FTR.reportsUtilities.R")
  aux <- .addto_FTR.paths_file(options)
  print(aux, row.names=FALSE)
  # paste into xlsx sheet "Paths"

  
  # see how many dollars were awarded (do this for Kevin):
  dollarSpent <- .getDollarSpent.NEPOOL(this.auction)

  aux <- subset(NEPOOL$AWD, auction==options$auction)
  subset(aux, sink.ptid == 4008)
  
  aux <- aggregate(aux$auction.dollars, list(class.type=aux$class.type,
    buy.sell=aux$buy.sell), function(x)round(sum(x)))
  names(aux)[3] <- "auction.dollars"
  print(cbind(aux, auction=this.auction), row.names=FALSE)

  
  
  # only if you participated with NESTAT ... 
  if (NESTAT){
    load("all.data.RData")   # all the info for NESTAT  
    bids  <- .getAwardedPaths(bids.xls, NEPOOL, this.auction)
    paths <- unique(bids[,-which(names(bids) %in%
            c("mw", "bid.price", "bid.price.month", "cum.mw", "bid.no"))])
    paths$from.to <- paste(paths$source.area, paths$sink.area,  sep=" to ")
    paths$won <- ifelse(is.na(paths$award.ftr.mw), "lost", "won")
  
    awds  <- paths[!is.na(paths$award.ftr.mw), ]

    TT.awds <- aggregate(rep(1,nrow(paths)), paths[,c("from.to",
      "class.type", "won")], sum)
    names(TT.awds)[4] <- "value"
    TT.awds <- cast(TT.awds, from.to + class.type ~ won, I)
  
    # check that the booking was right
    # .check.FTR.booking(awds)

    # See where the CP fell in the min/max bid range.
    cp.cuts <- .hist.cp.in.bid.range( bids )

    # See who else got awarded
    awd.stats <- .who.else.got.awarded(this.auction, awds)

    awds$cp.cuts <- cp.cuts$cp.cuts[awds$path.no]
    awds$cp.cuts <- factor(awds$cp.cuts)

    TT.bid.range.region <- table(awds$from.to, awds$cp.cuts)
    TT.bid.range.region[which(TT.bid.range.region==0)] <- NA 

    # distribution of simulated PnL 
    PP     <- .simulated.payoffs(sSP, awds)
    PP.all <- apply(PP, 2, sum)  # all the portfolio
    TT.sum.hPayoff <- as.data.frame(as.list(summary(PP.all)))
  }
    
  save.image(file=paste(root, "Reports/Awards/awardsReport.RData", sep=""))
   
  
}













## #########################################################################
## #
## .getAwardedPaths <- function(bids.xls, ISO.env, this.auction)
## {
##   bids.xls <- merge(bids.xls, FTR.get.CP.for.paths(bids.xls, ISO.env))
##   bids.xls <- bids.xls[order(bids.xls$path.no, -bids.xls$bid.price), ]
  
##   bids.xls$CP.hour <- bids.xls$CP
##   bids.xls$CP <- bids.xls$CP.hour * bids.xls$hours
    
##   # add the awarded quantities 
##   AWD <- subset(NEPOOL$AWD, customer.name=="Constellation Energy Commoditi" &
##                 auction == this.auction)
##   ind <- c("customer.name", "start.dt", "end.dt")
##   AWD <- AWD[, -which(colnames(AWD) %in% ind)]             

##   bxls <- merge(bids.xls, AWD, all.x=T)
##   bxls <- bxls[order(bxls$path.no, -bxls$bid.price), ]

## #  aux <- unique(blxs[,c("path.no", "award.ftr.mw")]); sum(aux[,2], na.rm=T)
  
##   return(bxls)
## }

## #########################################################################
## #
## .check.FTR.booking <- function(awds, last.bday=Sys.Date())
## {
##   # check that the booking has been correct ... 
##   booked <- FTR.loadPositions(books=c("NESTAT"), asOfDate=last.bday)
##   booked$auction <- FTR.AuctionTerm(booked$start.dt, booked$end.dt)
##   booked <- subset(booked, auction==awds$auction[1])

##   # Check if all the awards have made it in ... 
##   aux <- merge(booked[,c("class.type", "source.ptid", "sink.ptid", "mw")],
##     awds[,c("class.type", "source.ptid", "sink.ptid", "award.ftr.mw")],
##     all.y=T)
##   aux <- aux[apply(is.na(aux),1, any), ]
##   if (nrow(aux)>0){
##     cat("THE AWARDS BELOW HAVE NOT BEEN BOOKED!\n")
##     print(aux)
##   }
##   # Check if I got stuff I did not bid on ... 
##   aux <- merge(booked, awds, all.x=T)
##   aux <- aux[apply(is.na(aux),1, any), ]
##   if (nrow(aux)>0){
##     cat("THE TRADES BELOW ARE NOT MINE:\n")
##     print(aux[,1:11])
##   }
## }
## #################################################################
## # 
## .largest.awards <- function(this.auction, ISO.env)
## {
##   AWD <- subset(ISO.env$AWD, auction == this.auction)
##   AWD <- AWD[order(-AWD$award.ftr.mw),]
##   print(head(AWD,20))
## }

## #################################################################
## # Calculate where the paths have cleared relative to our min/max
## # bid price.  [0,1] is [min,max] 
## #
## .hist.cp.in.bid.range <- function( bids )
## {
##   aux <- split(bids, bids$path.no)
##   res <- sapply(aux, function(x){
##     y <- (x$CP.hour[1]-min(x$bid.price))/
##       (max(x$bid.price)-min(x$bid.price))
##     return(y)})
##   res <- round(res, 4)

##   res <- cut(res, breaks=c(-Inf, -1, 0, 0.25, 0.5, 0.75,
##     0.83, 0.92, 1, 2, Inf), right=TRUE)
##   cp.cuts <- table(res)
##   cp.cuts <- cp.cuts[cp.cuts != 0]
##   cp.cuts <- data.frame(cp.cuts)
##   names(cp.cuts) <- "count"

##   cp.cuts <- list(table=cp.cuts, cp.cuts=res)
  
##   return(cp.cuts)  
## }


## #################################################################
## # On how many paths did we set the clearing price?
## #
## .who.else.got.awarded <- function(this.auction, awds)
## {
##   # ckeck the competitors for the same paths, same buy/sell
##   AWD <- subset(NEPOOL$AWD, auction == this.auction)
##   AWD <- merge(AWD, awds[,c("class.type","source.ptid","sink.ptid"
##                ,"buy.sell")], all.y=T)
##   AWD <- subset(AWD, customer.name != "Constellation Energy Commoditi")
##   competitors <- aggregate(AWD$customer.name,
##     AWD[, c("class.type","source.ptid","sink.ptid","buy.sell")],
##                    function(x){return(length(x))})
##   names(competitors)[5] <- "no.competitors"
  
##   # ckeck the competitors for the same paths, opposite buy/sell
##   AWD <- subset(NEPOOL$AWD, auction == this.auction)
##   aux <- awds
##   aux$buy.sell <- ifelse(aux$buy.sell=="BUY", "SELL", "BUY")
##   AWD <- merge(AWD, aux[,c("class.type","source.ptid","sink.ptid"
##                ,"buy.sell")], all.y=T)
##   AWD <- subset(AWD, customer.name != "Constellation Energy Commoditi")
##   reverse.direction <- AWD

##   awd.stats <- list(competitors.same.direction=competitors,
##                     competitors.reverse.direction=reverse.direction)
  
##   return(awd.stats)  
## }






## #################################################################
## #################################################################
##                  OLD STUFF BELOW 
## #################################################################
## #################################################################





## #################################################################
## # Get some stats on the awards
## # 
## .all.awards.stats <- function(this.auction, ISO.env)
## {
##   AWD <- subset(ISO.env$AWD, auction == this.auction)
  
##   AWD$path.no <- 1:nrow(AWD)
##   AWD <- data.frame(AWD)     # I won't need this after Aug
##   AWD <- merge(AWD,
##     FTR.PathNamesToArea(AWD[,c("source.ptid", "sink.ptid")], ISO.env$MAP))

##   AWD <- AWD[,c("customer.name", "class.type", "buy.sell",
##                 "source.area", "sink.area", "award.ftr.mw")]

##   ind <- which(AWD$buy.sell=="SELL")
##   if (length(ind)>0){
##     aux <- AWD$source.area[ind]
##     AWD$source.area[ind]  <- AWD$sink.area[ind]
##     AWD$sink.area[ind]    <- aux
##     AWD$award.ftr.mw[ind] <- -AWD$award.ftr.mw[ind]
##   }
##   AWD <- aggregate(AWD$award.ftr.mw, AWD[,c(1,2,4,5)], sum)

##   TT.awd.mw.name <- aggregate(AWD$x, list(name=AWD$customer.name),sum)
##   TT.awd.mw.name <- TT.awd.mw.name[order(-TT.awd.mw.name$x), ]
##   names(TT.awd.mw.name)[2] <- "mw"

##   top.names <- TT.awd.mw.name$name[TT.awd.mw.name$mw >= 1000]
##   top.names <- TT.awd.mw.name$name[1:9]

##   aux <- aggregate(AWD$x, AWD[,1:3], sum)
##   aux$x <- -aux$x
##   names(aux)[3] <- "area"
##   aux$type <- "source"
##   bux <- aggregate(AWD$x, AWD[,c(1,2,4)], sum)
##   names(bux)[3] <- "area"
##   bux$type <- "sink"
##   aux <- rbind(aux, bux)
##   TT.awd.mw.name.class.area <- aux
##   names(TT.awd.mw.name.class.area)[4] <- "mw"

##   return(list(TT.awd.mw.name, TT.awd.mw.name.class.area, top.names))
## }

## #################################################################
## # which ones were the highest awards ... 
## .top20.awards <- function(this.auction, awds, ISO.env)
## {
##   # ckeck the competitors for the same paths, same buy/sell
##   AWD <- subset(NEPOOL$AWD, auction == this.auction)
##   AWD <- AWD[order(-AWD$award.ftr.price), ]

##   AWD <- head(AWD,20)
##   AWD <- AWD[,c(1:8, 11)]
##   AWD$path.no <- 1:nrow(AWD)
##   AWD <- merge(AWD, FTR.PathNamesToArea(data.frame(AWD), ISO.env$MAP))
##   AWD$path.no <- NULL
  
##   return(AWD)
## }

## #################################################################
## # net flows 
## .netflows.awards <- function(TT)
## {
##   TT <- TT.awd.mw.name.class.area
##   TT <- aggregate(TT$mw, TT[,c("class.type", "area")], sum)
##   names(TT)[3] <- "value"

##   TT$type <- ifelse(TT$value<0, "source", "sink")

##   aux <- sort(tapply(TT$value, TT$area, mean))  
##   TT$area <- factor(TT$area, levels=names(aux))  # sort the areas
  
## ##   barchart( area ~ value|class.type, data=TT, groups=type,
## ##     stack=TRUE, xlab="MW", main="Net flows for all awards")

##   return(TT)
## }

## #################################################################
## # Calculate path payoff
## #
## .simulated.payoffs <- function(sSP, awds)
## {

##   sSP.awd <- sSP[as.character(awds$path.no), ]

##   sSP.IQR <- apply(sSP.awd, 1, IQR)
  
##   RR <- sSP.awd - matrix(awds$CP.hour, nrow=nrow(sSP.awd),
##                       ncol=ncol(sSP.awd))
##   rownames(RR) <- rownames(sSP.awd)

##   PP <- RR * matrix(awds$award.ftr.mw * awds$hours,
##     nrow=nrow(sSP.awd), ncol=ncol(sSP.awd))

##   return( PP )  
## }

## #################################################################
## # Provide a range for the zonal paths 
## .cp.zonal.bidrange <- function(options, ISO.env)
## {
##   paths <- expand.grid(class.type=c("OFFPEAK", "ONPEAK"), sink.ptid=4001:4008)
##   paths$source.ptid <- 4000
##   paths <- cbind(path.no=1:nrow(paths), paths)
  
##   hSP <- FTR.get.hSP.for.paths(paths, NEPOOL, melt=F)
##   hSP <- window(hSP,
##    start=seq(options$start.dt, by="-28 months", length.out=2)[2])
  
##   rLog("Simulate the paths settle price.")
##   sSP <- FTR.simulate.SP(hSP, options, noSims=10000, noEns=100)
  
##   # if you want to buy them
##   aux <- FTR.makeBidLimits(paths, ISO.env, month=options$start.dt, enh=3,
##                            sSP=sSP)
##   BL  <- aux[[2]]
##   BL  <- BL[,c("path.no", "min.bid", "max.bid")]

##   paths$auction <- options$auction
##   CP <- FTR.get.CP.for.paths(paths, ISO.env)
##   BL <- merge(BL, CP, all.x=T)
##   BL <- merge(paths, BL)
##   BL$auction <- NULL
##   BL <- BL[order(BL$class.type, BL$sink.ptid), ]  
  
##   # if you want to sell the paths
##   names(paths)[3:4] <- c("source.ptid", "sink.ptid")
##   paths$auction <- NULL
##   aux <- FTR.makeBidLimits(paths, ISO.env, month=options$start.dt, enh=3,
##                            sSP=-sSP)
##   SL  <- aux[[2]]
##   SL  <- SL[,c("path.no", "min.bid", "max.bid")]
##   names(SL)[2:3] <- c("min.offer", "max.offer")
##   SL[,2:3] <- -SL[,2:3]
##   SL[,2:3] <- SL[,3:2]
##   BSL <- merge(BL, SL)   # put the bids & offers together

##   names(paths)[3:4] <- c("sink.ptid", "source.ptid")  # revert back
##   for (r in 1:nrow(BSL)){
##     ind <- max(which(sort(sSP[r,]) < BSL$CP[r]),0)
##     BSL$prob[r] <- ind/dim(sSP)[2]
##   }
  
##   BSL[,5:10] <- round(BSL[,5:10],2)
##   BSL$path.no <- NULL
##   return(BSL)  
## }

## #################################################################
## # Read raw file from ISO - super hot
## #
## .loadRawWebResults.NEPOOL <- function( bids.xls )
## {
##   aux <- read.csv(paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
##                   "RawResultsFromWebAug08.csv", sep=""), skip=1)
##   aux <- aux[,c(6:12)]
##   names(aux) <- c("source.name", "sink.name", "owner", "class.type",
##                   "buy.sell", "mw", "clearing.price")
##   aux$class.type <- ifelse(aux$class.type == "On", "ONPEAK", "OFFPEAK")
##   aux$buy.sell <- toupper(aux$buy.sell)

##   aux <- subset(aux, owner=="Constellation Energy Commoditi")
##   aux$owner <- NULL
  
##   bux <- aggregate(aux$mw, aux[,-5], sum)
##   names(bux)[6] <- "cleared.mw"

##   cux <- merge(unique(bids.xls[,c("path.no","source.name", "sink.name",
##                                   "class.type", "buy.sell")]), bux)

##   return(cux)   
## }




##   bids <- read.csv(filepath, skip=1)
##   bids <- bids[-nrow(bids), ]             # remove comment line
##   colnames(bids) <- tolower(colnames(bids))
##   bids$begin <- as.Date(bids$begin)
##   bids$end   <- as.Date(bids$end)
##   names(bids)[c(9, 10, 11, 12, 13, 14)] <- c("class.type", "buy.sell",
##     "source.name", "sink.name", "bid.mw", "bid.price")
  
##   this.auction <- FTR.AuctionTerm(bids$begin[1], bids$end[1])
##   bids  <- bids[,-(1:8)]
##   uBids <- unique(bids[,c("class.type", "buy.sell",
##     "source.name", "sink.name")])
##   uBids$path.no <- 1:nrow(uBids)
##   bids <- merge(bids, uBids)
##   bids <- bids[order(bids$path.no, bids$bid.mw), ]

##   bids <- merge(bids,
##     FTR.PathNamesToPTID(bids, ISO.env$MAP, direction="to.ptid"), all.x=T)
##   bids$auction <- this.auction
  
##   bids <- merge(bids, FTR.get.CP.for.paths(bids, ISO.env))
##   bids <- bids[order(bids$path.no, bids$bid.mw), ]
