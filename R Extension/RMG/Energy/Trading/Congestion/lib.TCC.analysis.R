# Utilities to deal with TCC's before the auction
#
# 
# 
# ..export_tsdb_json
# ..binding_constraints_as.zoo
# .binding_constraints_stats    - count/severity of constraints
# best_node_by_constraint       - which node responds
# bids_to_paths                 - from bids to Paths
# cluster_paths                 - cluster paths 
# .exposure_bids_constraints
# get_constraints_for_paths     - what constraints affect these paths?
# make_upload_xlsx              - bring the TCC bids in NYISO format
# nfind
# node_highlow_zone
# plot_price_and_constraints    - 
# .read_bids_xlsx               - from my bids spreadsheet
# regress_mcc_shadowprice       - which constraints affect a given node
# .regress_shadowprice 
# TCC.plot
# 



#################################################################
# try to build a dart application 
#
..export_tsdb_json <- function()
{
  load_valid_ptids()

  startDt <- as.POSIXct("2014-06-01 01:00:00")
  endDt   <- as.POSIXct("2014-07-15 00:00:00")
  symbs   <- CRR:::tsdb_symbol_fun("NYPP")(NYPP$valid_ptids[seq(1,300,by=15)])
  res <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)

  aux <- cbind(datetime=as.numeric(index(res)), as.data.frame(res))
  colnames(aux) <- gsub(".*_([[:digit:]]+)", "\\1", colnames(aux))

  
  #aux <- melt(cbind(datetime=as.numeric(index(res)), as.data.frame(res)), id=1)
  #aux$variable <- gsub(".*_([[:digit:]]+)", "\\1", aux$variable)
  
  require(df2json)

  writeLines(df2json(aux),
    con=file("C:/dart/congestion_viewer/resources/data.json"))

  # write the MAP 
  aux <- subset(NYPP$MAP[,c("ptid", "name", "zone", "type", "subzone")],
                ptid %in% NYPP$valid_ptids)
  writeLines(df2json(aux),
    con=file("C:/dart/congestion_viewer/resources/nypp_map.json"))
}


#################################################################
# Simple binding constraints statistics for an interval 
# @param interval 
# @param endDt
#
.binding_constraints_stats <- function(
   interval = Interval(prevMonth(), Sys.Date()+1))
{
  BC <- get_binding_constraints(region="NYPP", startDt=interval$startDt,
                                endDt=interval$endDt,
                                type=c("DA", "RT"))
  
  BC$day <- as.Date(format(BC$time-1, "%Y-%m-%d"))

  tb1 <- ddply(BC, c("type", "constraint.name"), function(x){
    data.frame(hours=nrow(x), avg.shadow.price=round(mean(x$shadowprice),2))
  })
  tb1 <- tb1[order(tb1$type, tb1$avg.shadow.price), ]

  aux <- split(tb1, tb1$type)
  aux <- merge(aux[["DA"]][,-1], aux[["RT"]][,-1], by="constraint.name",
    suffixes=c(".DA", ".RT"), all=TRUE)

  #cast(tb1, constraint.name + hours + avg.shadow.price ~ type, I, fill=NA)
  #cast(BC, type ~ ., sum, value="shadowprice")  # 10 times more RT constraints
  
  rLog("All binding constraints between", format(interval$startDt), "and", format(endDt))
  print(aux, row.names=FALSE)

  tb1
} 


#################################################################
# Get the NYPP binding constraints and format them as a zoo object
# @param interval an list of Interval objects
# @param 
##
..binding_constraints_as.zoo <- function( intervals )
{
  if (class(intervals) == "Interval")
    intervals <- list(intervals)
  
  res <- lapply(intervals, function(interval) {
    startDt <- interval$startDt
    endDt   <- interval$endDt
  
    BC <- get_binding_constraints(region="NYPP", startDt=startDt, endDt=endDt,
      type=c("DA"))
    subset(BC, time >= startDt & time <= endDt)
   })

  BC <- do.call(rbind, res)
  x <- cast(BC, time ~ constraint.name, max, fill=0, value="shadowprice")
  
  time <- toHourly( intervals[[1]] )
  for (i in seq_along(intervals)[-1]) 
    time <- c(time, toHourly(intervals[[i]]))
 
  ind <- data.frame(time=time)
  x <- merge(x, ind, all=TRUE)
  x[is.na(x)] <- 0

  zoo(x[,-1], x$time)
}


#################################################################
# Find which node has best response to a given constraint 
# @param reg the regression coefficients by constraint
# @param top, how many to keep
#
best_node_by_constraint <- function( reg=NULL, top=3)
{
  if ( is.missing(reg) ){}
}


#################################################################
# Make some paths from a set of bids.
# @param bids a data.frame of paths as returned from .read_bids_xlsx
# @rule a function that constructs the quantity for the path. The argument
#    qty is the vector of bid quantities for this path.
# @return a list of Path
#
bids_to_paths <- function( bids, rule=function(qty, path.no)sum(qty) )
{
  auction <- parse_auctionName( bids$auction[1], region="NYPP" )
   
  paths <- dlply(bids, c("path.no"), function(x) {
    Path(source.ptid = x$source.ptid[1],
         sink.ptid   = x$sink.ptid[1],
         bucket      = "FLAT",
         auction     = auction,
         mw          = rule(x$mw, x$path.no[1]))
  })

  
  paths
}


#################################################################
# Do a cluster analsyis on some ptids
# @param ptids a vector of ptids
# @param interval what historical data to use
# @param percent, what percent of the cluster tree to discard 
# @return 
#
cluster_MCC <- function( ptids,
                         interval=Interval(prevMonth(), Sys.Date()),
                         percent = 0.10,
                         do.plots = TRUE)
{
  symbs <- CRR:::tsdb_symbol_fun("NYPP")(ptids)

  hSP <- FTR:::FTR.load.tsdb.symbols(symbs, interval$startDt, 
            interval$endDt)
  colnames(hSP) <- gsub(".*_(.*$)", "\\1", colnames(hSP))

  # no NA's allowed
  ind.NA <- which(is.na(hSP), arr.ind=T)
  if (nrow(ind.NA) > 0)
    hSP[ind.NA] <- 0 

  cSP <- apply(hSP, 2, cumsum)
  if (do.plots){windows(); matplot(cSP, type="l")}

  dPP <- dist(t(cSP), method="manhattan")
  clusters <- hclust(dPP)
  if (do.plots) {
    if (length(ptids) > 30) {
      do.labels <- FALSE
    } else {
      do.labels <- NULL
    }
    windows()
    plot(clusters, labels=do.labels, xlab="")
  }

  # find the optimal height to cut the tree ...
  #plot(log(clusters$height)) # use the point where the linear part start bending up.
  
  cuts <- cutree(clusters, h=percent*max(clusters$height))
  freq  <- as.data.frame(table(cuts))
  cuts2 <- data.frame(cuts=cuts, ptid=as.numeric(names(cuts)))
  cuts2 <- merge(cuts2, freq)
  cuts2$weight <- 1/(cuts2$Freq*nrow(freq))
  names(cuts2)[1] <- "cluster.no"


  colors <- structure(cuts2$cluster.no, names=cuts2$ptid)
  colors <- colors[order(as.numeric(names(colors)))]
  windows(); matplot(cSP, type="l", col=colors)
  print(cuts2[,c("cluster.no", "ptid")], row.names=FALSE)

  cuts2
}


#################################################################
# What constraints are influencing this list of paths
# @param paths a list of paths
# @return a vector of constraints ordered
#
get_constraints_for_paths <- function(paths)
{ 
  ptids <- sort(unique(unlist(
    lapply(paths, function(path) c(path$source.ptid, path$sink.ptid)))))

  # load the regression info
  auctionName <- paths[[1]]$auction$auctionName
  fname <- paste("//CEG/CERShare/Trading/Nodal FTRs/NYISO/Auctions/",
               auctionName, "/Data/mcc_regression.RData", sep="")
  load(fname)
  reg <- subset(reg, ptid %in% ptids)
  cnames <- sort(unique(reg$constraint.name))

  cnames
}


#################################################################
# Massage the date in the NYISO format.
# @param bids a data.frame that you get from .read_bids_xlsx
#
make_upload_xlsx <- function( bids, DIR )
{
  MM <- data.frame(bid.no      = 1:nrow(bids),
                   mw          = round(bids$mw))
  MM$bid.price   <- round(bids$bid.price * bids$hours,2)
  MM$total.price <- MM$bid.price * round(bids$mw)
  MM$POI         <- bids$source.ptid
  MM$POW         <- bids$sink.ptid
  MM$request     <- "U"

  filename <- paste(DIR, "/iso_upload_spec_", bids$auction[1], ".xlsx", sep="")
  write.xlsx2(MM, filename, sheetName="toUpload", row.names=FALSE)
  rLog("Wrote ", filename)
}


#################################################################
# Find nodes that satisfy certain conditions
#
nfind <- function(x)
{
  if (tolower(x) == "zone") {
    subset(NYPP$MAP, ptid > 61000 & ptid < 62000)
  } else if (is.character(x)) {
    NYPP$MAP[grep(toupper(x), NYPP$MAP$name),]
  } else if (is.numeric(x)) {
    subset(NYPP$MAP, ptid == x)
  }
}


#################################################################
# Calculate the high/low of nodes by sub/zone
#
node_highlow_zone <- function(month=currentMonth(), show=3)
{
  this.month <- month
  # load SP
  load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/ftrsp.RData")

  sp <- subset(SP, month %in% this.month)
  sp <- sp[order(sp$ptid),]
  if (nrow(sp) == 0)
    stop("Please update the SP arvhive first Congestion/lib.NYPP.TCC.archive_SP.R")
  
  res <- ddply(NYPP$MAP, c("zone"), function(map) {
    rLog("working on zone", map$zone[1])
   
    sp.zone <- subset(sp, ptid %in% map$ptid)
    zone.ptid <- subset(map, name == map$zone[1])$ptid
    zone.sp <- subset(sp.zone, ptid == zone.ptid)  # pick the zone sp
    colnames(zone.sp)[4] <- "SP.zone"
    sp.zone <- merge(sp.zone, zone.sp[,c("bucket", "month", "SP.zone")])
    sp.zone$diff.SP <- round(sp.zone$SP - sp.zone$SP.zone, 3)

    ddply(sp.zone, c("bucket"), function(x) {
      x <- x[order(x$diff.SP),]
      obs <- if (nrow(x) < 2*show) {max(c(nrow(x) %/% 2, 1))} else {show}
      if (obs > 1)   rbind(head(x, obs), tail(x, obs)) else x
    })
  })

  res$SP <- round(res$SP, 3)
  res$SP.zone <- round(res$SP.zone, 3)

  res
}

#################################################################
# Explore the way a path responds to different constraints ...
#
plot_price_and_constraints <- function(path, bc)
{
  ptids <- sort(unique(unlist(sapply(paths, function(path) c(path$source.ptid, path$sink.ptid)))))
  symbs <- CRR:::tsdb_symbol_fun("NYPP")(ptids)

  pp <- get_settle_price_hourly( path )

  
}



..get_symbs_tcc.plot <- function(source.ptid, sink.ptid)
{
  # hourly data
  fun.da <- function(ptid) {
    paste(ifelse(ptid %in% c(61752:61762, 61844:61847),
      "nypp_dam_zone_lbm_", "nypp_dam_gen_lbm_"), ptid, sep="")
  }
  symb.da <- fun.da( c(source.ptid, sink.ptid) )

  # 5-min data
  fun.rt <- function(ptid) {
    paste(ifelse(ptid %in% c(61752:61762, 61844:61847),
      "nypp_rt_zone_lbm_", "nypp_rt_gen_lbm_"), ptid, sep="")  
  }
  symb.rt <- fun.rt( c(source.ptid, sink.ptid) )  
  
  symb.cong <- CRR:::tsdb_symbol_fun("NYPP")(c(source.ptid, sink.ptid))

  #c(symb.da, symb.rt, symb.cong)
  c(symb.da, symb.cong)
}


#################################################################
# Read my spreadsheet with bids for ISO submission 
# For file format see: "//CEG/CERShare/Trading/Nodal FTRs/NYISO/Auctions/Q14.xlsx"
#
# @return a list of CRR_Bids objects
#
.read_bids_xlsx <- function(filename, sheetName)
{
  require(xlsx)

  MM  <- read.xlsx2(filename, sheetName=sheetName, colIndex=1:6,
    colClasses=c("character", rep("numeric",3), "character",
      rep("numeric",2)))
  MM <- MM[!is.na(MM[,1]), ]
  MM <- MM[!MM[,1]=="",]
  bids  <- MM[,c("auction", "path.no", "source.ptid", "sink.ptid",
                 "mw", "bid.price")]

  # check that the path.no are increasing by one
  path.no <- sort(unique(bids$path.no))
  if (path.no[1] != 1 || max(diff(path.no)) != 1)
    stop("Path.no are not strictly increasing by one!")

  # check that the path numbers are unique
  if (length(path.no) != nrow(unique(bids[, c("source.ptid", "sink.ptid")]))){
    rLog("duplicate paths!")
    browser()
    stop("Some path.no are not unique!")
  }

  # check that the auctionName is unique
  if (length(unique(bids$auction)) != 1)
    stop("The auctionName has to be unique!")
  
  # check that the bid.prices are decreasing
  aux <- ddply(bids, .(auction, path.no), function(x) {
    if (!identical(x$bid.price, -sort(-x$bid.price)))
      stop("Bids for path ", x$path.no[1], " are not decreasing!")
  })

  # check that the quantities are whole numbers
  bids$mw <- as.numeric(bids$mw)
  if (any(bids$mw != as.integer(bids$mw)))
    stop("All path quantities need to be exact integers.  Please fix.")
  
  hrs <- count_hours_auction(auctionName=bids$auction[1],
                             region="NYPP",  buckets="FLAT")

  # add the node names
  bids2 <- ddply(bids, c("path.no"), function(x) {
    x$source.name <- CRR:::.get_name_from_ptid(x$source.ptid[1], NYPP$MAP)
    x$sink.name   <- CRR:::.get_name_from_ptid(x$sink.ptid[2], NYPP$MAP)
    
    x
  })
  
  bids2$hours <- hrs$hours
  
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



#################################################################
# Do a regression between MCC ~ binding.constraints
# to see what constraints influence this node.
# 
# @param bc a zoo object with the historical shadow prices for
#    each constraint 
# @param interval a list of Interval objects
# @param
#
#
regress_mcc_shadowprice <- function(ptids, bc=NULL,
  intervals=Interval(prevMonth(), Sys.Date()+1), threshold=10^-8 )
{
  if (class(intervals) == "Interval")
    intervals <- list(intervals)  
  
  if (is.null(bc)) 
    bc <- ..binding_constraints_as.zoo( intervals )  
  
  res <- vector("list", length=length(ptids))
  names(res) <- ptids
  for (ptid in ptids) {
    rLog("\nWorking on ptid ", ptid)
    
    symbs   <- CRR:::tsdb_symbol_fun("NYPP")(ptid)
    y <- unlist(lapply(intervals, function(interval) {
      HH <- FTR:::FTR.load.tsdb.symbols(symbs, interval$startDt, interval$endDt)
      if (is.null(HH))
        return(NULL)
      colnames(HH) <- gsub(".*_([[:digit:]]+)", "\\1", colnames(HH))
      as.numeric(HH[, as.character(ptid)])
    }))

    if (length(y) != nrow(bc)) {
      rLog("Missing MCC data for the node")
      next
    }
    
    # there are about 10-15 relevant constraints per node ...
    res[[as.character(ptid)]] <- cbind(
      ptid = ptid,
      .regress_shadowprice(y, bc, threshold=threshold)
    )
  }
  reg <- do.call(rbind, res)
  rownames(reg) <- NULL

  
  reg 
}

#################################################################
# Regress a time series against the shadow prices of constraints.
# For example, you can regress an MCC, or the portfolio payoff, etc.
# @param y a zoo object
# @param bc a zoo object with shadow price by constraint
# @return a data.frame with regression coefficients
#
.regress_shadowprice <- function(y, bc, threshold=10^-8)
{
  if (length(y) != nrow(bc)) {
    rLog("Inputs don't match exactly. ")
  }
  cnames <- colnames(bc)
  colnames(bc) <- paste("x", 1:ncol(bc), sep="")
  
  aux <- merge(y, bc, all=FALSE)
  rLog("Regress using data from", print(Interval(index(aux)[1], index(aux)[nrow(aux)])))
  
  
  data <- as.data.frame(aux) 
  formula <- as.formula(paste("y ~", paste(colnames(bc), collapse=" + "), "-1" ))
  reg <- lm(formula, data)
  sum.reg <- summary(reg)

  # keep only the significant constraints and regress again
  ind <- which(sum.reg$coeff[,4] < threshold )
  if (length(ind) == 0) {
    next
  }
  formula2 <- as.formula(paste("y ~", paste(colnames(bc)[ind], collapse=" + "), "-1" ))
  reg2 <- lm(formula2, data)    
  #print(summary(reg2))
  print(summary(reg2$residuals))

  res <- data.frame(
    constraint.name = cnames[ind],
    regression.coef = coef(reg2)                                      
  )

  
  res
}



#################################################################
# Look at TCC.  4 plots in the window. 
# @ path a CRR Path object
#
TCC.plot <- function(path, hist=365, wdim=c(12,9), save_svg=FALSE,
                     filename=NULL)
{
  if (!("package:SecDb" %in% search()))
    require(SecDb)

  symbols <- ..get_symbs_tcc.plot(path$source.ptid, path$sink.ptid)
  
  start.dt <- as.POSIXct(paste(format(currentMonth(Sys.Date() - 3*hist-3)),
                                      "01:00:00"))
  end.dt   <- Sys.time() + 24*3600
  MM <- FTR:::FTR.load.tsdb.symbols(symbols, start.dt, end.dt,
     aggHourly=c(FALSE, FALSE, FALSE, FALSE))

  # aggregate daily
  dMM <- aggregate(MM, as.Date(format(index(MM)-1, "%Y-%m-%d")), mean,
                   na.rm=TRUE)
  dMM <- dMM[(nrow(dMM)-hist):nrow(dMM),]
  #dMM <- window(dMM, start=Sys.Date()-hist)       # TODO!
  # begining of the month vertical bars
  h0 <- which(as.numeric(format(index(dMM), "%d"))==1)

  # do the plots
  if (save_svg) {
    svg(filename=filename, width=wdim[1], height=wdim[2])
  } else {
    windows(wdim[1], wdim[2])
  }
  layout(matrix(1:2, 2, 1))
  par(mar=c(2,4,1.5,1))
  # plot DA1c-DA2c
  plot(dMM[,3]-dMM[,4], col="blue", xlab="", cex.main=1,  
       main=paste(path$source.ptid, " to ", path$sink.ptid,
         ", ", path$source.name, " to ",  path$sink.name, sep=""), 
       ylab=paste("MCC DA", path$source.ptid, " - ", path$sink.ptid), type="h")
  abline(h=0, col="#BEBEBE80")
  abline(v=index(dMM)[h0], col="#BEBEBE80")
  

  # calculate the monthly settle price
  aux <- aggregate(MM[,3]-MM[,4],
    as.Date(format(index(MM)-1, "%Y-%m-01")), mean, na.rm=TRUE)
  SP <- data.frame(month=index(aux), SP=as.numeric(aux))

  # get the monthly clearing prices
  auctions <- archived_auctions(region="NYPP", noMonths=1,
    fromDt=min(SP$month))
  auctions <- do.call(rbind, lapply(auctions, as.data.frame))

  CP <- CRR:::.load_CP_archive("NYPP", auctions$auctionName)
  CP <- subset(CP, ptid %in% c(path$source.ptid, path$sink.ptid))
  CP <- cast(CP, auction ~ ptid, I, fill=NA, value="clearing.price.per.hour")
  CP$CP <- CP[,as.character(path$sink.ptid)] - CP[,as.character(path$source.ptid)]
  colnames(CP)[1] <- "auctionName"
  
  CP <- merge(CP[,c("auctionName","CP")],
              auctions[,c("auctionName","startDt")])
  colnames(CP)[3] <- "month"
  
  
  CPSP <- merge(CP, SP)
#browser()
  
  plot(range(CPSP$month), range(c(CPSP$CP, CPSP$SP), na.rm=TRUE), type="n",
       ylab=paste(path$source.ptid, "-", path$sink.ptid))
  points(CPSP$month, CPSP$CP, col="red") 
  points(CPSP$month, CPSP$SP, col="blue", pch=3) # a cross 
  abline(h=0, col="#BEBEBE80")
  legend("topleft", col=c("red", "blue"), pch=c(1,3), legend=c("CP", "SP"),
         text.col=c("red", "blue"))
 
  
#  on.exit(par(opar))
  CPSP[,3:4] <- sapply(CPSP[,3:4], function(x)round(x,2))

  #print(as.data.frame(path), row.names=FALSE)

  if (save_svg) {
    dev.off()
    rLog("Wrote file ", filename)
  }
  return(invisible(CPSP))
}

  

#################################################################
#################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(CRR)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.analysis.R")

  
  load_ISO_environment(region="NYPP")

  auction <- parse_auctionName("M14", region="NYPP")
  paths <- Path(source.ptid = 61752,
                sink.ptid   = 61758,
                auction     = parse_auctionName("F14", region="NYPP"))
  get_clearing_price( paths )
  get_settle_price( paths )

  
  #################################################################
  # 
  ptids <- c(23562, 24039)

  startDt <- as.POSIXct(paste(prevMonth(), "01:00:00"))
  endDt   <- as.POSIXct(paste(Sys.Date()+1, "00:00:00"))
  bc <- ..binding_constraints_as.zoo( Interval(startDt, endDt) )  
  

  #################################################################
  # a cluster analysis
  #
  require(CEGbase)
  require(CRR)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.interval.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.analysis.R")

  load_ISO_environment(region="NYPP")
  load_valid_ptids()

  ptids <- NYPP$valid_ptids
  
  cluster_MCC(ptids, Interval("2014-07-01", "2014-09-15"))

  

}











## #################################################################
## # Find the nodes that respond best to a given constraint by doing
## # a regression.
## #
## # @param constraint.name a String
## # @param HH is the 
## #
## binding_constraints_effect <- function(constraint.name, HH=NULL, startDt=prevMonth(),
##   endDt=Sys.Date()+1)
## {
##   # make them datetimes
##   startDt <- as.POSIXct(paste(startDt, "01:00:00"))
##   endDt   <- as.POSIXct(paste(endDt, "00:00:00"))
  
##   if (is.null(HH)) {
##     rLog("reading historicall MCC data ... ")
##     symbs   <- CRR:::tsdb_symbol_fun("NYPP")(NYPP$valid_ptids)
##     HH <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
##     colnames(HH) <- gsub(".*_([[:digit:]]+)", "\\1", colnames(HH))
##   }

##   BC <- get_binding_constraints(region="NYPP", startDt=startDt, endDt=endDt,
##     type=c("DA"), constraintNames=constraint.name)
##   BC <- subset(BC, time >= startDt & time <= endDt)

##   # keep only the max shadowprice per hour (may be several contingencies)
##   uBC <- ddply(BC, c("time"), function(x){
##     x[which.max(x$shadowprice),]
##   })
  
##   shadowprice <- zoo(1, seq(startDt, endDt, by="1 hour"))
##   shadowprice <- merge(zoo(uBC$shadowprice, uBC$time), shadowprice, all=TRUE)
##   colnames(shadowprice)[1] <- "shadowprice"
##   shadowprice <- shadowprice[,1]
##   shadowprice[which(is.na(shadowprice))] <- 0

##   if (length(shadowprice) != nrow(HH))
##     stop("Out of alignment.  Please check!")



## #################################################################
## # How well do you explain MCC values using the regression on
## # binding constraints from binding_constraints_finder
## # Plot it
## # @param ptid
## # @param bc a zoo object with the binding constraints shaddow prices
## #   as returned by ..binding_constraints_as.zoo
## # @param reg a list of data.frames with regression results as
## #   returned by binding_constraints_finder 
## #
## .explain_mcc_price <- function(ptids, bc, reg)
## {
##   startDt <- index(bc)[1]
##   endDt   <- index(bc)[length(index(bc))]

##   symbs   <- CRR:::tsdb_symbol_fun("NYPP")(ptid)
##   HH <- FTR:::FTR.load.tsdb.symbols(symbs, startDt, endDt)
##   colnames(HH) <- gsub(".*_([[:digit:]]+)", "\\1", colnames(HH))

##   for (ptid in ptids) {
##     y <- as.numeric(HH[, as.character(ptid)])

##     lm.res <- reg[[as.character(ptid)]]$regression
     
##     plot(index(HH), y, col="red")
##     points(index(HH), lm.res$fitted.values, col="blue")

##     windows()
##     plot(index(HH), lm.res$residuals)
##   }  
## }



##   aux <- HH[which(shadowprice != 0),]
##   ind <- which(apply(aux, 2, function(x){all(x != 0)}))
    
  
##   res <- NULL
##   for (ic in ind) {
##     reg <- lm(HH[,ic] ~ shadowprice)
##     aux <- summary(reg)
##     if ((aux$coefficients[,"Pr(>|t|)"][2] < 10^-4) &&      # a good fit
##         (abs(aux$coefficients[,"Estimate"][2]) > 0.1)) {   # and some response
##       bux <- list(aux$coefficients)
##       names(bux) <- colnames(HH)[ic]
##       res <- c(res, bux)
##     }
##   }
##   windows();plot(shadowprice, HH[,"24000"], col="blue")
##   length(res)
##   sapply(res, function(x){x[2,1]})

    
##   list(res, shadowprice)  
## }
