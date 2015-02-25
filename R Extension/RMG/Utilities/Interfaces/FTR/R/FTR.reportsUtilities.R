# Functions:
#
#  .addto_FTR.paths_file
#
#  .all.awards.stats.NEPOOL - simple stats on the awards of a given auction.
#
#  .AwardLikePrevAuction - how much we would have cleared if auction
#      cleared like previous month
#
#  .AuctionHighLow - see how much you're going to get if the auction  
#      clears high or low (if I make a two way)
#
#  .check.FTR.booking - compare the awards with PRISM output (next day)
#
#  .cp.bidrange.table - compare CP with the quantiles of the simulated PnL.
#      Good to see if the auction cleared High or Low.
#
#  .format_results_for_booking 
#
#  .getAwardedPaths.House - see which bids got awarded, so you can split them  
#      by owner.
#
#  .getAwardedPaths.NEPOOL - pass in the bids.xls spreadsheet for NESTAT
#
#  .getAwardedPaths.NYPP - pass in the bids.xls data.frame 
#
#  .getAwardedVolumesIntoZone 
#
#  .getAwardedVolumes.NEPOOL - do some stats with awarded volumes in NEPOOL
#
#  .getAwardedVolumes.NYPP - do some stats with awarded volumes in NYPP
#
#  .getAwardsByStrategy - predict how much money you will make by strategy
#
#  .getDollarsSpent.NEPOOL - get how many $ were spent in the auction by participant
#
#  .get.paths.performance.NEPOOL - make a table with path performance YTD, MTD
#
#  .get.awards.performance.NEPOOL - calculate the PnL of all NEPOOL awards
#      for each customer.name for a given auction.
#
#  .get.nestat.performance.NEPOOL - get performance of nestat paths for the
#      current month -- MAY BE OBSOLETE 3/11/09
#
#  .getPathsBigPayoff - subset paths with payoff bigger than a threshold
#
#  .get_zonal_cp  -- get clearing prices for this auction for the important paths we love 
#
#  .heatRateDependence - investigate the dependence on HR of path payoffs
#
#  .hist.cp.in.bid.range - calculate where paths have cleared relative to
#      our bid range
#
#  .historicalPortfolio.NEPOOL - calculate the historical return on have these
#      relative to our bids -- PROBABLY OBSOLETE -- Commented out 03/11/09
#
#  .historicalPortfolio.NYPP - same stuff for NYPP
#
#  .largest.awards - see the top 20 awars by quantity
#
#  .loadAllCEGBids - put all the bid files together(from veiga, house, nestat). 
# 
#  .netflow.awards - see where the power flows by area in the FTR auction are
#
#  .performancePrevAuction - PnL of awards like prev Auction.
#      Should be combined
#
#  .plot.bidCurve.intoZone - show your bid curve for all the paths that end in one zone
#
#  .plot.PnL.auction - for all participants, last 3 months
#
#  .plot.simulated.payoffs - boxplot of path returns
#
#  .risk.reward - some basic stats on the path return distributions
#
#  .simulated.payoffs - calculate the payoffs of simulated paths
#
#  .table.max.mw.node - get the max mw per node, to see if you get big numbers.
#
#  .table.PnL.auctions - loop over .get.awards.performance.NEPOOL
#
#  .temperatureDependence - commented out 03/11/09
#
#  .top20.awards - top 20 awards by $ amount
#
#  .who.else.got.awarded -  check on which paths we got, there were other 
#      players
#

#################################################################
# Get some stats on the NEPOOL awards
# 
.addto_FTR.paths_file <- function(options)
{
  rawdir <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
      options$auction, sep="")

  files <- tolower(list.files(rawdir))
  
  filein <- paste(rawdir, grep("bookingcheck", files, value=TRUE), sep="/")
  if (!file.exists(filein))
    stop("Cannot find file BookingCheck.xls!")

  require(xlsx)
  DD <- read.xlsx(filein, sheetIndex=1)
  rLog("Read", filein)
  DD <- DD[!apply(DD[,1:5], 1, function(x)any(is.na(x))), ]
  
  if ("ETI" %in% names(DD)){
    eti <- DD[,"ETI"]  # assumes you have it
  } else {
    eti <- DD[,4]
  }
  eti <- gsub("\\((.*)\\)", "\\1", eti)  # usually inside ()

  DD[,2] <- gsub(":", "", DD[,2])
  aux <- strsplit(DD[,2], " ")
  bucket <- sapply(aux, "[[", 1)
  bucket <- ifelse(bucket == "On", "ONPEAK", ifelse(bucket=="Off", "OFFPEAK", NA))
  
  sourceNode <- toupper(sapply(aux, "[[", 2))
  sinkNode   <- toupper(sapply(aux, "[[", 4))

  nodemap <- structure(NEPOOL$MAP$ptid, names=NEPOOL$MAP$deliv.pt)
  
  res <- data.frame(eti=eti, region="NEPOOL", book=toupper(DD$Book),
    counterparty="NEPOOL", start.dt=options$start.dt, end.dt=options$end.dt,
    buy.sell="BUY", class.type=bucket, mw=DD$Qty, 
    source.ptid=nodemap[sourceNode], sink.ptid=nodemap[sinkNode])

  # add the clearing price
  paths <- data.frame(path.no=1:nrow(res), class.type=res$class.type,
    source.ptid=res$source.ptid, sink.ptid=res$sink.ptid)
  aux <- FTR.get.CP.for.paths(paths, NEPOOL)
  aux <- subset(aux, auction==options$auction)
  aux <- aux[order(aux$path.no),]
  
  res <- cbind(path.no=1:nrow(res), res)
  res <- merge(res, aux[,c("path.no", "CP")], by="path.no", all.x=TRUE)
  res$path.no <- NULL
  res$auction <- options$auction
  
  res
}

#################################################################
# Get some stats on the NEPOOL awards
# 
.all.awards.stats.NEPOOL <- function(this.auction, ISO.env)
{
  AWD <- subset(ISO.env$AWD, auction == this.auction)
  
  AWD$path.no <- 1:nrow(AWD)
  AWD <- data.frame(AWD)     # I won't need this after Aug
  AWD <- merge(AWD, FTR.PathNamesToPTID(
    AWD[,c("source.ptid", "sink.ptid")], ISO.env$MAP, to="area"))

  AWD <- AWD[,c("customer.name", "class.type", "buy.sell",
                "source.area", "sink.area", "award.ftr.mw")]

  ind <- which(AWD$buy.sell=="SELL")
  if (length(ind)>0){
    aux <- AWD$source.area[ind]
    AWD$source.area[ind]  <- AWD$sink.area[ind]
    AWD$sink.area[ind]    <- aux
    AWD$award.ftr.mw[ind] <- -AWD$award.ftr.mw[ind]
  }
  AWD <- aggregate(AWD$award.ftr.mw, AWD[,c(1,2,4,5)], sum)

  TT.awd.mw.name <- aggregate(AWD$x, list(name=AWD$customer.name),sum)
  TT.awd.mw.name <- TT.awd.mw.name[order(-TT.awd.mw.name$x), ]
  names(TT.awd.mw.name)[2] <- "mw"

  top.names <- TT.awd.mw.name$name[TT.awd.mw.name$mw >= 1000]
  top.names <- TT.awd.mw.name$name[1:8]

  uCustomer <- sort(unique(AWD$customer.name))
  cust.map  <- read.csv(
    file="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/uCustomers.csv")
    
  aux <- aggregate(AWD$x, AWD[,1:3], sum)
  aux$x <- -aux$x
  names(aux)[3] <- "area"
  aux$type <- "source"
  bux <- aggregate(AWD$x, AWD[,c(1,2,4)], sum)
  names(bux)[3] <- "area"
  bux$type <- "sink"
  aux <- rbind(aux, bux)
  TT.awd.mw.name.class.area <- aux
  names(TT.awd.mw.name.class.area)[4] <- "mw"

  ## if (!missing(awds)){
  ##   aux <- aggregate(awds$award.ftr.mw, awds[,c("class.type", "source.area",
  ##                                           "sink.area")], sum)
  ##   nestat <- rbind(cbind(type="source", aux[,c(1,4)], area=aux[,2]),
  ##                 cbind(type="sink",   aux[,c(1,4)], area=aux[,3]))
  ##   nestat$x <- ifelse(nestat$type=="source", -nestat$x, nestat$x)
  ##   names(nestat)[3] <- "mw"
  ##   nestat$customer.name <- "NESTAT "
  ## } else {nestat <- NULL}
  
  TT.awds.top9 <- subset(TT.awd.mw.name.class.area, customer.name
                         %in% top.names)
  ## TT.awds.top9 <- rbind(TT.awds.top9, nestat)  # add nestat

  ## if (!missing(awds)){
  ##   # substract nestat from constellation
  ##   nestat$customer.name <- "Constellation Energy Commoditi"
  ##   nestat$mw <- -nestat$mw
  ##   TT.awds.top9 <- rbind(TT.awds.top9, nestat)  # substract nestat
  ## }
  
  TT.awds.top9 <- aggregate(TT.awds.top9$mw, TT.awds.top9[,-4], sum)
  names(TT.awds.top9)[5] <- "mw"
  ind <- which(TT.awds.top9$customer.name == "Constellation Energy Commoditi")
  TT.awds.top9$customer.name[ind] <- "House+Spec"

  return(list(TT.awd.mw.name, TT.awd.mw.name.class.area,
    top.names, TT.awds.top9))
}

#################################################################
# How much we would be awarded if cleared like in previous auction
#
.AwardLikePrevAuction <- function( prev.auction, bids.xls, ISO.env)
{
  awds <- FTR.awardPaths(prev.auction, bids.xls, ISO.env )

  TT.awds.strategy <- aggregate(awds$mw, list(awds$strategy), sum)
  names(TT.awds.strategy) <- c("strategy", "MW")

#  awds <- awds[order(-awds$mw), ]

  if (ISO.env$region == "NEPOOL")
    aux <- aggregate(awds$mw, awds[,c("class.type", "source.area",
                                      "sink.area")], sum)
  if (ISO.env$region == "NYPP")
    aux <- aggregate(awds$mw, awds[,c("class.type", "source.zone",
                                      "sink.zone")], sum)
      
  names(aux)[4] <- "mw"
  aux <- aux[order(-aux$mw),]
  aux <- subset(aux, mw >=5)
  TT.awds.area <- aux

  return(list(TT.awds.strategy, TT.awds.area, awds)) 
}

#################################################################
# See the bids.min if the auction clears High or Low
.AuctionHighLow <- function( paths, bids.xls, ISO.env )
{
  awds <- NULL
  paths.high <- subset(paths, is.counterflow %in% c(TRUE, NA))$path.no
  paths.low  <- subset(paths, is.counterflow %in% c(FALSE, NA))$path.no

  auxH <- subset(bids.xls, path.no %in% paths.high)
  awds$high   <- aggregate(auxH$mw, list(upto.bid.no=auxH$bid.no), sum)
  awds$high$x <- round(cumsum(awds$high$x))
  colnames(awds$high)[2] <- "mw"

  auxL <- subset(bids.xls, path.no %in% paths.low)
  awds$low   <- aggregate(auxL$mw, list(upto.bid.no=auxL$bid.no), sum)
  awds$low$x <- round(cumsum(awds$low$x))
  colnames(awds$low)[2] <- "mw"

  if (ISO.env$region == "NEPOOL"){
    CT <- subset(auxH, source.area=="CT" & sink.area != "CT")
    if (nrow(CT)>0){
      awds$high.CT <- aggregate(CT$mw, list(upto.bid.no=CT$bid.no), sum)
      awds$high.CT$x <- round(cumsum(awds$high.CT$x))
      colnames(awds$high.CT)[2] <- "mw"
    }

    CT <- subset(auxL, source.area=="CT" & sink.area != "CT")
    if (nrow(CT)>0){
      awds$low.CT <- aggregate(CT$mw, list(upto.bid.no=CT$bid.no), sum)
      awds$low.CT$x <- round(cumsum(awds$low.CT$x))
      colnames(awds$low.CT)[2] <- "mw"
    }
  }
    
  TT.awds <- cbind(data.frame(counterflow="yes", awds$high),
                   data.frame(counterflow="no",  awds$low))
  return(list(TT.awds, awds))
}

#########################################################################
#
.check.FTR.booking <- function(awds, last.bday=Sys.Date(), region=NULL)
{
  # check that the booking has been correct ... 
  booked <- FTR.loadPositions(books=c("NESTAT"), asOfDate=last.bday)
  booked$auction <- FTR.AuctionTerm(booked$start.dt, booked$end.dt)
  booked <- subset(booked, auction==awds$auction[1])
  if (!is.null(region))
    booked <- booked[booked$region==region,]
  names(booked)[which(names(booked)=="mw")] <- "booked.mw"
  
  # Check if all the awards have made it in ... 
  aux <- merge(booked[,c("class.type", "source.ptid", "sink.ptid",
    "booked.mw")], awds[,c("class.type", "source.ptid", "sink.ptid",
    "award.ftr.mw")], all.y=T)
  aux <- aux[apply(is.na(aux),1, any), ]
  if (nrow(aux)>0){
    cat("THE AWARDS BELOW HAVE NOT BEEN BOOKED!\n")
    print(aux)
  }
  # Check if I got stuff I did not bid on ... 
  aux <- merge(booked[,c("source.ptid", "sink.ptid", "class.type",
    "booked.mw", "buy.sell", "eti")], awds, all.x=T)
  aux <- aux[,c("source.ptid", "sink.ptid", "class.type",
                 "buy.sell", "award.ftr.mw", "booked.mw", "eti"),]
  aux <- aux[apply(is.na(aux),1, any), ]
  if (nrow(aux)>0){
    cat("THE TRADES BELOW ARE NOT MINE:\n")
    print(aux)
  }
}

#################################################################
# Compare CP with quantiles of simulated PnL.
# Used for zones in the report but can be any paths. 
#
.cp.bidrange.table <- function(paths, options, ISO.env,
  cut.quantile=list(c(0.01, 0.20)), add.CP=FALSE)
{
  paths <- paths[,c("path.no", "class.type", "source.ptid", "sink.ptid")]
  hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)
  hSP <- window(hSP,
   start=seq(options$start.dt, by="-28 months", length.out=2)[2])
  
  rLog("Simulate the paths settle price.")
  sSP <- FTR.simulate.SP(hSP, options, noSims=10000, noEns=100)
  
  # if you want to buy them
  BL <- FTR.makeBidLimits(paths, sSP=sSP, cut.quantile=cut.quantile)
  names(BL)[2:3] <- paste("q", cut.quantile[[1]]*100, sep="")

  paths$auction <- options$auction
  if (add.CP){
    CP <- FTR.get.CP.for.paths(paths, ISO.env)
    BL <- merge(BL, CP, all.x=T)
  }
  BL <- merge(paths, BL)
  BL$auction <- NULL
  BL <- BL[order(BL$class.type, BL$sink.ptid), ]  
  
  # if you want to sell the paths
  names(paths)[3:4] <- c("source.ptid", "sink.ptid")
  paths$auction <- NULL
  SL <- FTR.makeBidLimits(paths, sSP=sSP, 
    cut.quantile=list(rev(c(1-cut.quantile[[1]]))))
  names(SL)[2:3] <- paste("q", rev(1-cut.quantile[[1]])*100, sep="")
  BSL <- merge(BL, SL)   # put the bids & offers together

  names(paths)[3:4] <- c("sink.ptid", "source.ptid")  # revert back
  if (add.CP){  
    for (r in 1:nrow(BSL)){
      ind <- max(which(sort(sSP[r,]) < BSL$CP[r]),0)
      BSL$prob[r] <- ind/dim(sSP)[2]
    }
  }

  BSL[,5:ncol(BSL)] <- round(BSL[,5:ncol(BSL)],2)
  return(BSL)  
}


#########################################################################
# ISO changed the file format on Nov2012
# reformat in the old style for Jenny's utility
#
.format_results_for_booking <- function(auctionName)
{
  auction <- parse_auctionName(auctionName)
  DIR <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
               auction$auctionName, "/", sep="")
  aux <- read.csv(paste(DIR, "auction_results.csv", sep=""), skip=1)

  bux <- aux[,c(1,2,3,4,5,7,10,12,13,14,15,16 )]
  bux <- bux[-nrow(bux),]  # remove the comments line

  # subset only the month/auction you need!
  token <-  if (grepl("-1Y_R1", auction$auctionName)) {
      paste("LT1", format(auction$startDt, "%Y"))
    } else if (grepl("-1Y-R2", auction$auctionName)) {
      paste("LT2", format(auction$startDt, "%Y"))
    } else {
      toupper(format(auction$startDt, "%b %Y"))
    }
  bux <- subset(bux, FTR.Auction == token)
  if (nrow(bux)==0)
    stop("Cannot find any awards for ", token)
  
  TT <- capture.output(write.table(bux, file="", row.names=FALSE,
              col.names=FALSE, sep=",", quote=FALSE))
  TT <- c(TT,paste("C,END OF REPORT,", nrow(bux)+3, sep=""))
  TT <- c(
    paste("C,FTR,MUI,AUCTION RESULTS 2,", format(Sys.Date()), ",10:10:10", sep=""),
    paste("I,AUCTION RESULTS 2,RESULTS,1,Market,Origin Node,",
      "Destination Node,Owner,Class,Buy/Sell,Clearing MW,Clearing Price", sep=""),
    TT)

  fileout <- paste(DIR, "auctionResults.csv", sep="")
  writeLines(TT, fileout)
  rLog("Wrote file", fileout)

  
}

#########################################################################
#
.getAwardedPaths.House <- function(CEGbids, ISO.env, this.auction)
{
  # add the awarded quantities
  AWD <- subset(NEPOOL$AWD, auction == this.auction)
  AWD <- subset(AWD, customer.name %in% c("Constellation Energy Commoditi", 
    "Exelon Generation Company_ LLC"))
  if (nrow(AWD) == 0) {
    rLog("Now awards in the archive for Consto.  Looking at ISO extract...")
    fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
      this.auction, "/auction_results.csv", sep="")
    AWD <- read.csv(fname, skip=1)
    rLog("Read file ", fname)
    AWD <- AWD[AWD[,1] == "D",]
    colnames(AWD) <- tolower(colnames(AWD))
    names(AWD)[which(names(AWD) == "source.location.name")] <- "source.name"
    names(AWD)[which(names(AWD) == "sink.location.name")] <- "sink.name"
    names(AWD)[which(names(AWD) == "clearing.price")] <- "award.ftr.price"
    names(AWD)[which(names(AWD) == "clearing.mw")] <- "award.ftr.mw"
    names(AWD)[which(names(AWD) == "owner")] <- "customer.name"
    AWD$buy.sell <- toupper( AWD$buy.sell )
    
    AWD <- AWD[,c("source.name", "sink.name", "class.type", "award.ftr.mw",
                "buy.sell", "award.ftr.price")]
    AWD$class.type <- toupper( AWD$class.type )
    AWD$class.type <- factor(toupper(AWD$class.type), levels=c("OFF", "ON"))
    levels(AWD$class.type) <- c("OFFPEAK", "ONPEAK")
    AWD$class.type <- as.character(AWD$class.type)

    # aggregate by path
    AWD <- cast(AWD, source.name + sink.name + class.type + buy.sell +
      award.ftr.price ~ ., sum, value="award.ftr.mw")
    names(AWD)[ncol(AWD)] <- "award.ftr.mw"

  } else {
    ind <- c("customer.name", "start.dt", "end.dt")
    AWD <- AWD[, -which(colnames(AWD) %in% ind)]

    AWD <- cbind(AWD, FTR.PathNamesToPTID(AWD, NEPOOL$MAP, to="name"))
    AWD <- AWD[,c("source.name", "sink.name", "class.type", "award.ftr.mw",
                "buy.sell", "award.ftr.price")]
  }

  aux <- merge(CEGbids, AWD, all=FALSE, by=c("source.name", "sink.name",
    "class.type", "buy.sell"))
  aux <- subset(aux, bid.ftr.price >= award.ftr.price)

  # check if there are overlapping bids!
  uAwdsFile <- unique(aux[,c("class.type", "buy.sell", "source.name",
                             "sink.name", "file")])
  dups <- aggregate(rep(1, nrow(uAwdsFile)), as.list(uAwdsFile[,1:5]), sum)
  if (any(dups$x>1)){
    rLog("You have competing bids ...  Please be careful when booking!")
    dups <- dups[any(dups$x>1),]
    rLog("These awards are overlapping:")
    print(dups)
  } else {
    dups <- NULL
  }

  aux <- aux[order(aux$source.name, aux$sink.name, aux$class.type,
                   -aux$bid.ftr.price),]
  
  return(aux)
}

#########################################################################
#
.getAwardedPaths.NEPOOL <- function(bids.xls, ISO.env, this.auction)
{
  bids.xls <- merge(bids.xls, FTR.get.CP.for.paths(bids.xls, ISO.env))
  bids.xls <- bids.xls[order(bids.xls$path.no, -bids.xls$bid.price), ]
  
  bids.xls$CP.hour <- bids.xls$CP
  bids.xls$CP <- bids.xls$CP.hour * bids.xls$hours
    
  # add the awarded quantities 
  AWD <- subset(NEPOOL$AWD, customer.name=="Constellation Energy Commoditi" &
                auction == this.auction)
  ind <- c("customer.name", "start.dt", "end.dt")
  AWD <- AWD[, -which(colnames(AWD) %in% ind)]
  # sometimes the ISO screws up the ptids
#  AWD <- AWD[, -which(colnames(AWD) %in% c("source.ptid", "sink.ptid"))]

  myAwds <- merge(bids.xls, AWD)
  ind <- duplicated(myAwds[,c("source.ptid", "sink.ptid", "class.type", "buy.sell")])
  myAwds <- myAwds[!ind, ]
  myAwds <- myAwds[,c("source.ptid", "sink.ptid", "class.type", "buy.sell", 
    "award.ftr.price", "award.ftr.mw")]
  cat("I was awarded", nrow(myAwds), "paths.\n")
  
  bxls <- merge(bids.xls, AWD, all.x=T)
  bxls <- bxls[order(bxls$path.no, -bxls$bid.price), ]

#  aux <- unique(blxs[,c("path.no", "award.ftr.mw")]); sum(aux[,2], na.rm=T)
  
  return(bxls)
}


#########################################################################
#
.getAwardedPaths.NYPP <- function( this.auction, fileName, sheet=3 )
{
  require(xlsReadWrite)
  bids.xls <- read.xls(fileName, sheet=sheet)
  
  
  paths <- data.frame(path.no=1:nrow(bids.xls), source.ptid=bids.xls$POI,
    sink.ptid=bids.xls$POW, class.type="FLAT", mw=bids.xls$mw,
    auction=options$auction, bid.price=bids.xls$bid.price/options$hours["FLAT"])

  aux <- merge(paths, FTR.get.CP.for.paths(paths, NYPP), all.x=TRUE)
  
  return(subset(aux, bid.price >= CP))
}


#########################################################################
# How many MW cleared into a set of ptids
# @param auctions a vector of auctionNames, e.g.
#   c("F12-1Y", "F13-1Y-R1", "F13-1Y-R2")
#
# So I checked that into NEMA paths have consistently cleared about 1100
# in the annual auction, consistent with the 2400 MW limit.
#
.getAwardedVolumesIntoZone <- function(zonePtid=4008, auctions)
{
  source.ptids <- subset(NEPOOL$MAP, zone != zonePtid)$ptid  
  sink.ptids <- subset(NEPOOL$MAP, zone == zonePtid)$ptid
  awd <- subset(NEPOOL$AWD, auction %in% auctions
                & sink.ptid %in% sink.ptids
                & source.ptid %in% source.ptids)
  
  cast(awd, auction + class.type ~ ., sum, value="award.ftr.mw")

  ddply(awd, c("auction", "class.type"), function(x) {
    head(x[order(-x$award.ftr.mw),], 10)
  })

  
}


#########################################################################
# Show some statistics on cleared volumes in recent auctions.
# 
# paths <- data.frame(path.no=1:2, source.ptid=c(4000, 4004),
#   sink.ptid=c(4004, 4000), class.type="ONPEAK")
#
# 
.getAwardedVolumes.NEPOOL <- function(paths, expandAnnual=TRUE)
{
  if (!all.equal(unique(paths$path.no), c(1,2)))
    stop("You need exactly two paths.  Direct and reversed!")
  
  auctions <- unique(NEPOOL$AWD$auction)
  aux <- expand.grid.df(paths, data.frame(auction=auctions))
  aux <- merge(NEPOOL$AWD, aux)

  aux <- aux[,c("source.ptid", "sink.ptid", "class.type", "auction", 
    "buy.sell", "award.ftr.mw", "path.no", "customer.name")]

  CP <- FTR.get.CP.for.paths(paths, NEPOOL, panelName=FALSE)
  aux <- merge(aux, CP)

  # look at the sell 
  ind2 <- which(aux$path.no==2)
  if (length(ind2)>0){
    aux$path.no[ind2]     <- paths$path.no[1]
    aux$source.ptid[ind2] <- paths$source.ptid[1]
    aux$sink.ptid[ind2]   <- paths$sink.ptid[1]
    aux$CP[ind2]          <- -aux$CP[ind2]
    aux$buy.sell[ind2]    <- ifelse(aux$buy.sell[ind2]=="BUY", "SELL", "BUY")
  }

  if (expandAnnual){
    ind <- grep("-1Y", aux$auction)
    annuals <- unique(aux[ind,"auction"])
    
    bux  <- unique(aux[,"auction", drop=FALSE])
    bux  <- cbind(bux, FTR.AuctionTerm(auction=bux$auction))
    mths <- apply(bux, 1, function(x){
      seq(as.Date(x["start.dt"]), as.Date(x["end.dt"]), by="1 month")})
    names(mths) <- bux$auction
    mths <- melt(mths)
    colnames(mths) <- c("month", "auction")
    rownames(mths) <- NULL

    aux <- merge(aux, mths)
  }
  
  aux  
}



#########################################################################
#
.getDollarSpent.NEPOOL <- function(this.auction)
{
  AWDS <- NEPOOL$AWD[-grep("-", NEPOOL$AWD$auction), ]
  totalDS <- aggregate(AWDS$auction.dollars, list(auction=AWDS$auction), sum)
  totalDS$auctionChar <- ordered(totalDS$auction,
    levels=FTR.AuctionTermLevels(totalDS$auction))
  totalDS <- totalDS[order(totalDS$auctionChar),]
  
  #plot(totalDS$auctionChar, totalDS$x/1000000)

  plot(totalDS$x/1000000, xaxt="n", ylab="Auction Revenues, $", xlab="Month",
       type="o", col="blue")
  axis(1, at=1:nrow(totalDS), labels=totalDS$auctionChar, las=2, cex.axis=0.8)

  aux <- subset(AWDS, auction==this.auction)
  aux <- aggregate(aux$auction.dollars, list(bucket=aux$class.type), sum)
  names(aux)[2] <- "dollars.spent"
  print(aux, row.names=FALSE)
  
  return(totalDS)
}


#################################################################
#  the paths we love, one auction only
#
.get_favorite_paths <- function()
{
  paths <- data.frame(
    source.ptid=c(rep(4000, 8), 1478, 1616, 40327), 
    sink.ptid  =c(4001:4008,    4008, 4008, 4006 ))
  paths <- cbind(path.no=1:(2*nrow(paths)), 
    rbind(paths, paths),
    class.type=rep(c("ONPEAK", "OFFPEAK"), each=nrow(paths)))
  paths <- merge(paths, FTR.PathNamesToPTID(paths, NEPOOL$MAP, to="name"),
                 all.x=TRUE)

  paths
}



#################################################################
# Make a table with the performance of paths: YTD and MTD
#
.get.paths.performance.NEPOOL <- function(paths,
  startTime=as.POSIXct("2009-01-01 00:00:00"), endTime=Sys.time())
{
  RR  <- FTR.getPathReturns(paths, startTime, endTime, 
    ISO.env=NEPOOL, hourly=TRUE)
  RR[is.na(RR)] <- 0
  cRR  <- cumsum(RR)
  plot(index(cRR), rowSums(cRR), col="blue", type="l")
  YTD <- data.frame(path.no=as.numeric(colnames(cRR)),
    YTD=as.numeric(tail(cRR,1)))
  YTD <- merge(paths, YTD, by="path.no")

  MTD <- window(RR, start=as.POSIXct(format(Sys.time(),"%Y-%m-01 00:00:00")))
  MTD <- data.frame(path.no=as.numeric(colnames(MTD)),
    MTD=as.numeric(tail(cumsum(MTD),1)))
  YTD <- merge(YTD, MTD, by="path.no")

  YTD <- merge(YTD, FTR.PathNamesToPTID(YTD, NEPOOL$MAP, to="name"))
  res <- YTD[,c("source.name", "sink.name", "class.type", "mw", "CP",
                "YTD", "MTD")]
  res <- res[order(-res$YTD), ]
  names(res)[5] <- "clearing"
  
  return(res)  
}

#################################################################
# 
.get.awards.performance.NEPOOL <- function( this.auction )
{
  AWDS <- subset(NEPOOL$AWD, auction==this.auction)
  AWDS <- split(AWDS, AWDS$customer.name)

  for (i in seq_along(AWDS)){
    paths <- AWDS[[i]][,c("source.ptid", "sink.ptid", "class.type",
    "buy.sell", "award.ftr.mw")]
    names(paths)[5] <- "mw"
    paths <- cbind(path.no=1:nrow(paths), paths, auction=this.auction)
    paths <- merge(paths, FTR.get.CP.for.paths(paths, NEPOOL))
    aux <- FTR.AuctionTerm(auction=this.auction)
    RR <- FTR.getPathReturns(paths, aux[1,1], end.dt=aux[1,2], ISO.env=NEPOOL)
    AWDS[[i]]$PnL <- colSums(RR, na.rm=T)
  }

  return(AWDS)  
}

#################################################################
# get the uptodate nestat performance
.get.nestat.performance.NEPOOL <- function(last.bday =
   as.Date(secdb.dateRuleApply(Sys.Date(), "-1b")))
{

  QQ <- FTR.loadPositions(books="NESTAT", asOfDate=last.bday)
  QQ$path.no <- 1:nrow(QQ)
  QQ <- subset(QQ, as.Date(start.dt) <= last.bday)
  
  start.dt <- as.POSIXct(paste(format(Sys.Date(), "%Y-%m-01"),
                               "00:00:00"))
  QQ$auction <- FTR.AuctionTerm(QQ$start.dt, QQ$end.dt)
  # add the clearing prices
  QQ <- merge(QQ, FTR.get.CP.for.paths(QQ, NEPOOL))
  # get the historical hourly returns
  this.end.dt <- as.POSIXlt(format(Sys.time()+24*3600, "%Y-%m-%d %H:00:00"))
  hRR <- FTR.getPathReturns(QQ, start.dt, end.dt=this.end.dt,
    ISO.env=NEPOOL, hourly=TRUE)
  
  cRR <- hRR; cRR[is.na(cRR)] <- 0
  cRR <- apply(cRR, 2, cumsum)

  QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NEPOOL$MAP, to="area"), all.x=T)
  QQ$class.from.to <- paste(QQ$class.type, " ", QQ$source.area,
    " to ", QQ$sink.area, sep="")

  QQ <- cbind(QQ[order(QQ$path.no),], hPayoff=cRR[nrow(cRR),])

  TT.hpay <- aggregate(QQ[, c("mw", "hPayoff")],
                       list(group=QQ$class.from.to), sum)
  TT.hpay <- TT.hpay[order(TT.hpay$hPayoff, decreasing=T),]
  TT.hpay <- subset(TT.hpay, mw > 1)

  QQ <- merge(QQ, FTR.PathNamesToPTID(QQ, NEPOOL$MAP,
                to="name"), all.x=T)

  TT.rankpaths <- QQ[, c("class.type","source.name",
                         "sink.name", "mw", "hPayoff")]
  TT.rankpaths <- TT.rankpaths[order(-TT.rankpaths$hPayoff), ]
  TT.rankpaths <- rbind(head(TT.rankpaths, 10),
                        tail(TT.rankpaths, 10))
  TT.rankpaths$hPayoff <- round(TT.rankpaths$hPayoff)
  
  return(list(QQ, cRR, TT.hpay, TT.rankpaths))
}

#########################################################################
#
.getPathsBigPayoff <- function( awds, PP, cutoff=10000 )
{
  medPayoff <- apply(PP, 1, median)
  ind <- as.numeric(names(which(medPayoff >= cutoff)))

  TT.pathsBigPayoff <- subset(awds, path.no %in% ind)
  TT.pathsBigPayoff <- TT.pathsBigPayoff[, c("class.type", "source.name",
    "source.area", "sink.name", "sink.area", "path.no", "is.counterflow", 
    "award.ftr.mw")]
  colnames(TT.pathsBigPayoff)[c(1,8)] <- c("class", "MW")
  TT.pathsBigPayoff$class <- gsub("PEAK", "", TT.pathsBigPayoff$class)
  TT.pathsBigPayoff$PnL <- medPayoff[as.character(TT.pathsBigPayoff$path.no)]

  TT.pathsBigPayoff <-  TT.pathsBigPayoff[order(-TT.pathsBigPayoff$PnL),]
  TT.pathsBigPayoff$path.no <- NULL

  return(TT.pathsBigPayoff)
}



#################################################################
#  for the paths we love, one auction only
#
.get_zonal_cp <- function(auctionName, paths=.get_favorite_paths())
{
  CPSP <- FTR.get.CP.for.paths(paths, NEPOOL, add.SP=FALSE,
    panelName=FALSE)
  
  CP <- subset(CPSP, auction == auctionName)
  paths <- merge(paths, CP, all.x=TRUE)

  cast(paths, auction + source.name + sink.name ~ class.type,
    function(x){round(x,2)}, fill=NA, value="CP")
}


#################################################################
# 
.heatRateDependence <- function(paths, bids.min )
{
  HR <- FTR.load.tsdb.symbols(c("gasdailymean_trny"), Sys.Date()-2*365,
                              Sys.Date())

  symb <- c("nepool_smd_da_4000_lmp", "nepool_smd_da_4000_congcomp",
            "nepool_smd_da_4004_congcomp")
  pwr <- FTR.load.tsdb.symbols(symb, Sys.Date()-2*365,
                               Sys.Date())
  pwr <- aggregate(pwr, list(as.Date(index(pwr))), mean)
  colnames(pwr) <- c("pwr", "hub", "ct")
  
  HR  <- merge(HR, pwr)
  HR  <- cbind(HR, hr=HR[,2]/HR[,1])
  HR  <- cbind(HR, spread=HR[,"ct"]-HR[,"hub"])
  HR  <- na.omit(HR)

  plot(HR[,"hr"], HR[,"spread"])

  require(MASS)
  ff <- kde2d(HR[,"hr"], HR[,"spread"], c(1,1), n=1200)
  image(ff, col=terrain.colors(12), xlim=c(6,10), ylim=c(-1,6)) # nice

}

#################################################################
# Calculate where the paths have cleared relative to our min/max
# bid price.  [0,1] is [min,max] 
#
.hist.cp.in.bid.range <- function( bids )
{
  aux <- split(bids, bids$path.no)
  res <- sapply(aux, function(x){
    y <- (x$CP.hour[1]-min(x$bid.price))/
      (max(x$bid.price)-min(x$bid.price))
    return(y)})
  res <- round(res, 4)

  res <- cut(res, breaks=c(-Inf, -1, 0, 0.25, 0.5, 0.75,
    0.83, 0.92, 1, 2, Inf), right=TRUE)
  cp.cuts <- table(res)
  cp.cuts <- cp.cuts[cp.cuts != 0]
  cp.cuts <- data.frame(cp.cuts)
  names(cp.cuts) <- "count"

  cp.cuts <- list(table=cp.cuts, cp.cuts=res)
  
  return(cp.cuts)  
}

#################################################################
# How has this portfolio performed historically
#
.historicalPortfolio.NYPP <- function( hSP, bids.min )
{
  hrs <- matrix(options$hours, nrow=nrow(hSP), ncol=ncol(hSP))
  
  aux <- hSP - matrix(bids.min$bid.price, nrow=nrow(hSP),
                    ncol=ncol(hSP), byrow=T)
  aux[is.na(hSP)] <- 0
  aux <- aux * hrs                    # multiply with # hours
  hPayoff <- aux %*% bids.min$mw      # multiply with # mw  
  rownames(hPayoff) <- as.character(index(hSP))
  colnames(hPayoff) <- "hPayoff"
  hPayoff <- data.frame(date=as.Date(rownames(hPayoff)), hPayoff)
  rownames(hPayoff) <- NULL

  hPayoff <- hPayoff[order(hPayoff$hPayoff), ]
  TT.maxmin.hPayoff <- cbind(head(hPayoff,20),
                             tail(hPayoff,20)[20:1,])
  TT.maxmin.hPayoff[,1] <- as.character(TT.maxmin.hPayoff[,1]) 
  TT.maxmin.hPayoff[,3] <- as.character(TT.maxmin.hPayoff[,3]) 

  return(list(hPayoff, TT.maxmin.hPayoff)) 
}

#################################################################
# 
.largest.awards <- function(this.auction, ISO.env)
{
  AWD <- subset(ISO.env$AWD, auction == this.auction)
  AWD <- AWD[order(-AWD$award.ftr.mw),]
  head(AWD,20)
}

#################################################################
# 
.loadAllCEGBids <- function(allBidsFiles)
{
  MM <- NULL
  for (f in seq_along(allBidsFiles)){
    aux <- read.csv(allBidsFiles[f], skip=1)
    aux <- aux[-nrow(aux),]
    aux$FILE <- gsub("\\.csv", "", basename(allBidsFiles[f]))
    MM <- rbind(MM, aux)
  }

  names(MM) <- tolower(names(MM))
  MM <- MM[,c("begin", "end", "class", "buysell", "source", "sink",
    "mw", "price", "file")]
  names(MM) <- c("start.dt", "end.dt", "class.type", "buy.sell",
    "source.name", "sink.name", "bid.mw", "bid.ftr.price", "file")
  MM[,1] <- as.Date(MM[,1], format="%Y/%m/%d")
  MM[,2] <- as.Date(MM[,2], format="%Y/%m/%d")
  MM <- MM[,-(1:2)]  # I don't really need them
  
  return(MM)
}

#################################################################
# net flows 
.netflows.awards <- function(TT)
{
  TT <- TT.awd.mw.name.class.area
  TT <- aggregate(TT$mw, TT[,c("class.type", "area")], sum)
  names(TT)[3] <- "value"

  TT$type <- ifelse(TT$value<0, "source", "sink")

  aux <- sort(tapply(TT$value, TT$area, mean))  
  TT$area <- factor(TT$area, levels=names(aux))  # sort the areas
  
##   barchart( area ~ value|class.type, data=TT, groups=type,
##     stack=TRUE, xlab="MW", main="Net flows for all awards")

  return(TT)
}

#################################################################
# How much we would be awarded if cleared like in previous auction
#
.performancePrevAuction <- function( awds.prev, start.dt, ISO.env)
{
  rLog("Stop using this FUNCTION!  Use .AwardLikePrevAuction ... ")
  RR.prev <- FTR.getPathReturns(awds.prev, start.dt=seq(options$start.dt,
    by="-1 month", length.out=2)[2], ISO.env=NEPOOL, hourly=TRUE)  
  cRR.prev <- FTR.histPortfolioPerformance( awds.prev, RR.prev,
                                           region="NEPOOL")
  # not done 
}


 
#################################################################
# Plot the bid curves for all the paths that end into a zone
# @param bids a data.frame with bids, and an optional column "strategy"
#
.plot.bidCurve.intoZone <- function( allBids, zonePtid=4008 )
{
  source.ptids <- subset(NEPOOL$MAP, zone != zonePtid)$ptid  
  sink.ptids <- c(subset(NEPOOL$MAP, zone == zonePtid)$ptid, zonePtid)
  bidsZone <- subset(allBids,
                  sink.ptid %in% sink.ptids
                & source.ptid %in% source.ptids)

  res <- ddply(bidsZone, "class.type", function(x) {
    y <- x[order(-x$bid.price),]
    data.frame(mw=c(0, cumsum(y$mw)), bid.price=c(y$bid.price[1], y$bid.price))
  })


  print(xyplot(bid.price ~ mw|class.type, data=res,
         type=c("g", "p", "S"), layout=c(1,2),
         xlab="MW cleared", 
         ylab="Bid price, $/MWh"))

  resSpec <-  ddply(subset(bidsZone, strategy == "Spec"),
    "class.type", function(x) {
      y <- x[order(-x$bid.price),]
      data.frame(mw=c(0, cumsum(y$mw)), bid.price=c(y$bid.price[1], y$bid.price))
  })

  if (nrow(resSpec) > 0) {
    aux <- rbind(cbind(res, strategy="All"),
                 cbind(resSpec, strategy="Spec"))
    print(xyplot(bid.price ~ mw|class.type,
         data= aux,
         groups=aux$strategy,
         type=c("g", "p", "S"),
         layout=c(1,2),
         xlab="MW cleared", 
         ylab="Bid price, $/MWh",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2)))
  }
  
  
}

#################################################################
# 
.plot.PnL.auctions <- function( PnL )
{
  PnL$sign <- sign(PnL$PnL)
  PnL <- subset(PnL, abs(PnL) > 25000 )
  PnL$PnL <- PnL$PnL/1000
  
  print(barchart( participant ~ PnL | auction, data=PnL, 
    groups=sign, stack=TRUE, xlab="PnL, K$",
    panel = function(...){
      panel.grid(h=-length(unique(PnL$participant)), v=0)
      panel.barchart(...)},
    layout=c(length(unique(PnL$auction)),1), col=c("red", "green")))
}

#################################################################
# Plot simulated path payoff.
# The box plot in the Awards presentation.
#
.plot.simulated.payoffs <- function( PP )
{
  range.box <- apply(PP, 1, function(x){boxplot.stats(x)$stats})
  range.box <- range.box/1000  # divide by 1000
  
  PP  <- PP[order(range.box[3,]), ]/1000
  aux <- split(PP, rep(1:nrow(PP), ncol(PP)))
  boxplot(aux, outline=F, col="yellowgreen", ylim=range(range.box),
          ylab="Simulated payoff, K$")
  abline(h=0, col="gray")
}


#################################################################
# 
.risk.reward <- function( paths, sSP, bids )
{
  rLog("Hey, I'm still used!!!")
  probs <- matrix(c(0,1), ncol=nrow(paths),
    nrow=2, dimnames=list(c("out",1), paths$path.no))
  bids.min <- FTR.simulateAwards( bids, probs, noSims=1)
  bids.min <- merge(bids.min, paths[,c("path.no","hours")])
  bids.min$mwh <- bids.min$mw * bids.min$hours

  # simulated returns
  RR  <- sSP - matrix(bids.min$bid.price, nrow=nrow(sSP),
                      ncol=ncol(sSP))
  rownames(RR) <- rownames(sSP)

  # basic stats
  RR.bs <- cbind(t(apply(RR, 1, function(x){summary(na.omit(x))})),
           t(apply(RR, 1, quantile, probs=c(0.05, 0.95))))
  RR.bs <- cbind(RR.bs, IQR=RR.bs[,"3rd Qu."]-RR.bs[,"1st Qu."])
  RR.bs <- cbind(RR.bs, sigma.lt.0=RR.bs[,"Median"] - RR.bs[,"1st Qu."])
  RR.bs <- cbind(RR.bs, ratio= RR.bs[,"Median"]/RR.bs[,"sigma.lt.0"])
  
  return(list(RR, RR.bs, bids.min))  
}

#################################################################
# Calculate path payoff.  
# need: awds[,c("path.no","hours","award.ftr.mw","CP.hour")]
.simulated.payoffs <- function(sSP, awds)
{

  sSP.awd <- sSP[as.character(awds$path.no), ]

  sSP.IQR <- apply(sSP.awd, 1, IQR)
  
  RR <- sSP.awd - matrix(awds$CP.hour, nrow=nrow(sSP.awd),
                      ncol=ncol(sSP.awd))
  rownames(RR) <- rownames(sSP.awd)
  
  PP <- RR * matrix(awds$award.ftr.mw * awds$hours,
    nrow=nrow(sSP.awd), ncol=ncol(sSP.awd))

  return( PP )  
}

#################################################################
# 
.table.max.mw.node <- function( bids.max )
{
  max.source <- aggregate(bids.max$mw,
               list(source.name=bids.max$source.name), sum)
  names(max.source)[2] <- "mw"
  max.source <- max.source[order(-max.source$mw), ]
  max.sink <- aggregate(bids.max$mw,
               list(sink.name=bids.max$sink.name), sum)
  names(max.sink)[2] <- "mw"
  max.sink <- max.sink[order(-max.sink$mw), ]

  res <- cbind(max.source[1:20,], max.sink[1:20,])
  res <- res[-which(apply(is.na(res),1,all)), ]

  return(res)
}

#################################################################
# 
.table.PnL.auctions <- function( auctions )
{
  res <- NULL
  for (auction in auctions){
    cat(paste("Working on ", auction, "\n", sep=""))
    PnL <- .get.awards.performance.NEPOOL( auction )
    PnL <- sort(sapply(PnL, function(x){sum(x$PnL)}))
    res <- rbind(res, data.frame(auction=auction,
                                 participant=names(PnL), PnL=PnL))
  }
  rownames(res) <- NULL
  return(res)
}

#################################################################
# which ones were the highest awards ... 
.top20.awards <- function(this.auction, ISO.env)
{
  # ckeck the competitors for the same paths, same buy/sell
  AWD <- subset(NEPOOL$AWD, auction == this.auction)
  AWD <- AWD[order(-AWD$award.ftr.price), ]

  AWD <- head(AWD,20)
  AWD <- cbind(AWD, FTR.PathNamesToPTID(AWD, ISO.env$MAP, to="name"))
  AWD <- merge(AWD, FTR.PathNamesToPTID(data.frame(AWD), ISO.env$MAP,
                                        to="area"))
  AWD$path.no <- AWD$source.ptid <- AWD$sink.ptid <- NULL

  
  return(AWD)
}

#################################################################
# On how many paths did we set the clearing price?
#
.who.else.got.awarded <- function(this.auction, awds)
{
  # ckeck the competitors for the same paths, same buy/sell
  AWD <- subset(NEPOOL$AWD, auction == this.auction)
  AWD <- merge(AWD, awds[,c("class.type","source.ptid","sink.ptid"
               ,"buy.sell")], all.y=T)
  competitors <- subset(AWD, customer.name != "Constellation Energy Commoditi")
  if (nrow(competitors)>0){
    competitors <- aggregate(competitors$customer.name,
      competitors[, c("class.type","source.ptid","sink.ptid","buy.sell")],
                   function(x){return(length(x))})
    names(competitors)[5] <- "no.competitors"
  } 
  
  # ckeck the competitors for the same paths, opposite buy/sell
  AWD <- subset(NEPOOL$AWD, auction == this.auction)
  aux <- awds
  aux$buy.sell <- ifelse(aux$buy.sell=="BUY", "SELL", "BUY")
  AWD <- merge(AWD, aux[,c("class.type","source.ptid","sink.ptid"
               ,"buy.sell")], all.y=T)
  AWD <- subset(AWD, customer.name != "Constellation Energy Commoditi")
  reverse.direction <- AWD

  awd.stats <- list(competitors.same.direction=competitors,
                    competitors.reverse.direction=reverse.direction)
  
  return(awd.stats)  
}










## #################################################################
## #################################################################
## #################################################################
## #################################################################
## #################################################################
## #################################################################
## #################################################################



## #########################################################################
## # Show some statistics on cleared volumes in annual auctions.
## # 
## # paths <- data.frame(path.no=1:2, source.ptid=c(4000, 4004),
## #   sink.ptid=c(4004, 4000), class.type="ONPEAK")
## #
## # 
## .getAwardedVolumesAnnual.NEPOOL <- function(paths,
##   auctions=c("F06-1Y", "F07-1Y", "F08-1Y", "F08-1Y", "F10-1Y"))
## {

##   pathsR <- FTR.switchSourceSink(paths, paths$path.no)
##   pathsR$path.no <- (nrow(paths)+1):(2*nrow(paths))
##   allPaths <- rbind(paths, pathsR)
  
##   aux <- expand.grid.df(allPaths, data.frame(auction=auctions))
##   aux <- merge(NEPOOL$AWD, aux)

##   aux <- aux[,c("source.ptid", "sink.ptid", "class.type", "auction", 
##     "buy.sell", "award.ftr.mw", "path.no", "customer.name")]

##   ind <- which(aux$buy.sell=="SELL")
##   if (length(ind) > 0){
##     print("NEED to reverse paths!")
##   }

##   nameMap <- data.frame(ptid=4000:4008, name=c("Hub", "ME", "NH", "VT", "CT",
##     "RI", "SEMA", "WMA", "NEMA"))
##   aux <- merge(aux, data.frame(source.ptid=nameMap$ptid,
##                                source.name=nameMap$name), all.x=TRUE)
##   aux <- merge(aux, data.frame(sink.ptid=nameMap$ptid,
##                                sink.name=nameMap$name), all.x=TRUE)
##   aux[,"sink/source"] <- paste(aux$sink.name, aux$source.name, sep="/")
  
##   res <- cast(aux, customer.name + sink/source ~ auction, I,
##               value="award.ftr.mw", fill=0)
    
##   res2 <- res[which(res[,"F10-1Y"] != 0), ] 
##   print(res2, row.names=FALSE)
  
  
## }

## #########################################################################
## #
## .getAwardsByStrategy <- function( awds, PP )
## {
##   awds$count     <- 1
##   awds$medPayoff <- apply(PP, 1, median)
  
##   TT.awdsByStrategy <- aggregate(awds[,c("count", "award.ftr.mw",
##      "medPayoff")], as.list(awds[,c("strategy", "is.counterflow")]), sum)
##   colnames(TT.awdsByStrategy)[3:5] <- c("no.paths", "MW", "PnL")
##   TT.awdsByStrategy$PnL <- round(TT.awdsByStrategy$PnL/1000)*1000
  
##   return( TT.awdsByStrategy )
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

## #################################################################
## # 
## .temperatureDependence <- function( hSP )
## {

##   tsdb.symbol <- "temp_clean_bos_dly_max"
##   Tmax <- tsdb.readCurve(tsdb.symbol, index(hSP)[1], index(hSP)[nrow(hSP)])
##   Tmax <- zoo(Tmax$value, Tmax$date)
##   Tmax[Tmax<80] <- NA                  # <- good for summer only !!!
  
##   rho <- rep(NA, ncol(hSP))
##   for (c in 1:ncol(hSP)){
##     rho[c] <- cor(hSP[,c], Tmax, method="spearman",
##                   use="pairwise.complete.obs")
##   }
##   names(rho) <- colnames(hSP)
  
##   rho <- rho[order(rho, decreasing=TRUE)]
##   return(rho)
## }


## #########################################################################
## # Simulate the PnL for all other customers
## #
## .simulateAllCustomers.NEPOOL <- function( ISO.env )
## {
##   AWD <- subset(ISO.env$AWD, auction == this.auction)

##   uPaths <- unique(AWD[,c("class.type", "source.ptid", "sink.ptid")])
##   aux <- split(AWD, AWD$customer.name)
##   sort(sapply(aux, nrow))

##   # incomplete
## }

## #################################################################
## # What quantile of the price distribution is the CP of the
## # previous auction.
## #
## .QuantilesPrevAuction <- function( prev.auction, paths, sSP, ISO.env)
## {
##   aux <- paths[,c("path.no","source.ptid","sink.ptid", "class.type")]
##   aux$auction <- prev.auction
##   awds <- FTR.get.CP.for.paths(aux, ISO.env )

##   aux <- as.list(as.data.frame(t(sSP)))

##   findQ <- function(x,CP){
##     ind <- max(which(sort(x) < CP), 0)
##     return(ind/length(x))
##   }
##   qCP <- mapply(findQ, aux, as.list(awds$CP))

##   hist(qCP, 50, col="lavender")

##   ind <- subset(paths, strategy=="consistent winners")$path.no
##   hist(qCP[as.character(ind)], 30, col="lavender",
##        main="Consistent winners paths", xlab="quantile")
##   ind.notawd <- as.numeric(names(which(qCP[as.character(ind)]>0.3))) # <-- WHAT IS THIS 0.3 ?!
##   table(subset(paths, path.no %in% ind.notawd)$source.zone)

##   ind <- subset(paths, strategy!="consistent winners")$path.no
##   hist(qCP[as.character(ind)], 50, col="lavender",
##        main="Not consistent winners paths", xlab="quantile")
 
##   return( qCP ) 
## }


## #################################################################
## # 
## .plot.sim.boxwiskers <- function( RR, IQR, range.box )
## {
##   RR  <- RR[order(IQR), ]
##   aux <- split(RR, rep(1:nrow(RR), ncol(RR)))
##   boxplot(aux, outline=F, col="yellowgreen", ylim=range(range.box),
##     ylab="Simulated settle price - top bid ($/MWh)")
##   abline(h=0, col="gray")
## }


## #################################################################
## # Is this used?  9/12/2008 -- AAD, don't know 2/18/2009
## .loadBidfile.NEPOOL <- function(filepath, ISO.env="NEPOOL")
## {
##   bids <- read.csv(filepath, skip=1)
##   bids <- bids[-nrow(bids), ]             # remove comment line
##   colnames(bids) <- tolower(colnames(bids))
##   bids$begin <- as.Date(bids$begin)
##   bids$end   <- as.Date(bids$end)
##   names(bids)[c(9, 10, 11, 12, 13, 14)] <- c("class.type", "buy.sell",
##     "source.name", "sink.name", "mw", "bid.price.month")
  
##   this.auction <- FTR.AuctionTerm(bids$begin[1], bids$end[1])
##   bids  <- bids[,-(1:8)]
##   uBids <- unique(bids[,c("class.type", "buy.sell",
##     "source.name", "sink.name")])
##   uBids$path.no <- 1:nrow(uBids)
##   bids <- merge(bids, uBids)
##   bids <- merge(bids,
##     FTR.PathNamesToPTID(bids, ISO.env$MAP, direction="to.ptid"), all.x=T)
  
##   bids$hours <- options$hours[bids$class.type]
##   bids$bid.price <- bids$bid.price.month/bids$hours
  
##   bids  <- bids[order(bids$path.no, -bids$bid.price), ]

##   bids$bid.no <- as.numeric(sapply(split(rep(1,nrow(bids)),
##     bids$path.no), cumsum))
##   bids$cum.mw <- as.numeric(sapply(split(bids$mw, bids$path.no),
##                                    cumsum))
  
##   paths <- unique(bids[,c("path.no","class.type","buy.sell",
##     "source.name", "sink.name", "source.ptid", "sink.ptid",
##     "hours")]) 
##   paths <- merge(paths,
##     FTR.PathNamesToArea(paths, ISO.env$MAP), all.x=T)

##   bids$value <- bids$bid.price
##   bid.limits <- data.frame(cast(bids[,c("path.no", "value")],
##                                 path.no ~ ., range))
##   names(bid.limits)[2:3] <- c("min.bid", "max.bid")
##   bids$value <- NULL
  
##   return(list(bids, paths, bid.limits))
## }


## #################################################################
## # How has this portfolio performed historically
## #
## .historicalPortfolio.NEPOOL <- function( hSP, bids.min )
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
##   hrs[,which(off)]  <- ind
##   hrs[,which(!off)] <- 24-ind
  
  
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
##      offpeak=aux[,which(off)] %*% bids.min$mw[which(off)],
##      onpeak =aux[,which(!off)] %*% bids.min$mw[which(!off)])

  
##   summary(hPayoff$hPayoff)

##   hPayoff <- hPayoff[order(hPayoff$hPayoff), ]
##   TT.maxmin.hPayoff <- cbind(head(hPayoff,20),
##                              tail(hPayoff,20)[20:1,])
##   TT.maxmin.hPayoff[,1] <- as.character(TT.maxmin.hPayoff[,1]) 
##   TT.maxmin.hPayoff[,3] <- as.character(TT.maxmin.hPayoff[,3]) 

##   return(list(hPayoff, TT.maxmin.hPayoff)) 
## }

