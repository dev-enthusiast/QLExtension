# Basic functions for reporting.
#

############################################################################
# File Namespace
#
RPT <- new.env(hash=TRUE)

EWin    <- function(x){mean(x[x>0], na.rm=T)}  # expected size of wins
ELoss   <- function(x){-mean(x[x<0], na.rm=T)} # expected size of losses
Probs.0 <- function(x, counts=FALSE){
  x <- x[!is.na(x)]
  res <- c(length(which(x<0)), length(which(x==0)), length(which(x>0)))
  if (!counts) res <- res/length(x)
  return(res)
}

##########################################################################
# Calculate some basic statistics.
# Use less than 10000 paths for best results
#
RPT$basic.stats <- function( paths, hSP=NULL, ISO.env )
{
  if (is.null(hSP))
    hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)

  # calculate some statistics
  rLog("Calculate quantiles ...")
  probs <- c(0, 0.01, 0.05, 0.25, 0.5, 0.75, 0.95, 0.99, 1)
  aux <- FTR.pathApply( NULL, hSP, ISO.env, quantile, prob=probs, na.rm=T )
  names(aux)[-1] <- paste("p", 100*probs, sep="")
  paths <- merge(paths, aux, all.x=T)

  rLog("Calculate mean ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env,  mean, na.rm=T )
  names(aux)[2] <- "mean"
  paths <- merge(paths, aux, all.x=T)
  
  rLog("Calculate sd ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env,  sd, na.rm=T )
  names(aux)[2] <- "sd"
  paths <- merge(paths, aux, all.x=T)

  rLog("Calculate EWin ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, EWin)
  names(aux)[2] <- "EWin"
  paths <- merge(paths, aux, all.x=T, sort=F)

  rLog("Calculate ELoss ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, ELoss)
  names(aux)[2] <- "ELoss"
  paths <- merge(paths, aux, all.x=T, sort=F)

  rLog("Calculate days below, at, above zero ...")
  aux <- FTR.pathApply( NULL, hSP, ISO.env, Probs.0, counts=TRUE )
  names(aux)[2:4] <- c("days.lt.0", "days.eq.0", "days.gt.0")
  paths <- merge(paths, aux, all.x=T, sort=F)

  return(paths)  
}


##########################################################################
# Cluster a set of paths paths given hSP in melted format.
# Returns a data.frame with the mapping of the path to cluster and a
# weight assigned to it.
# if select, you return only one representative from the cluster.
#
RPT$cluster.paths <- function( hSP, percent=0.10, select=FALSE, plots=TRUE)
{
  hSP <- as.matrix(hSP)
  ind.NA <- which(is.na(hSP), arr.ind=T)
  if (nrow(ind.NA)>0){hSP[ind.NA] <- 0} # for ONPEAK !!  

  cSP <- t(apply(hSP, 2, cumsum))   # cannot have NA's in cumsum
  if (plots){windows(); matplot(t(cSP), type="l")}
  
  dPP <- dist(cSP, method="manhattan")
  clusters <- hclust(dPP)
  if (plots){windows(); plot(clusters)}

  cuts <- cutree(clusters, h=percent*rev(clusters$height)[1])
  freq <- as.data.frame(table(cuts))
  cuts <- data.frame(cuts=cuts, path.no=as.numeric(names(cuts)))
  cuts <- merge(cuts, freq)
  cuts$path.weight <- 1/(cuts$Freq*nrow(freq))
  names(cuts)[1] <- "cluster.no"

  if (select){
    aux <- data.frame(cluster.no=cuts[, "cluster.no"], value=cuts$path.no)
    cuts <- cast(aux, cluster.no ~ .,
      function(x){as.numeric(sample(as.character(x),1))}, fill=NA)
    names(cuts)[2] <- "path.no"
  }

  return(list(cuts, clusters))
}

##########################################################################
# Do the cluster analysis for a set of paths.  I'm using it so much, 
# that I'd better have a function for it.
# 
RPT$clusterByAreaBucket <- function(paths, percent=0.10, select=FALSE,
  plots=FALSE)
{
  uAreas <- unique(paths[, c("class.type", "source.area", "sink.area")])
  uAreas <- cbind(id=1:nrow(uAreas), uAreas)
  rownames(uAreas) <- NULL

  cluster <- NULL
  for (r in 1:nrow(uAreas)){
    rLog(paste("Working on row", r))
    cpaths <- merge(paths, uAreas[r, ])
    if (nrow(cpaths)<5){
      cluster <- rbind(cluster, cbind(id=r, path.no=cpaths$path.no))
      next()
    }
    hSP  <- FTR.get.hSP.for.paths(cpaths, NEPOOL, melt=F)
    cuts <- RPT$cluster.paths( hSP, select=select, plots=plots,
                              percent=percent)[[1]]
    cluster <- rbind(cluster, cbind(id=r, path.no=cuts[,"path.no"]))
  }

  cluster <- data.frame(cluster)
  rLog("These are the clusters:")
  print(table(cluster[,"id"]))

  paths <- merge(paths, cluster, by="path.no", all.y=TRUE)

  return(paths)
}


##########################################################################
# Using this function to see how many times SP > CP
RPT$CPSP.persistence <- function(filename, no.months=28)
{
  rLog(paste("Working on", filename))
  load(filename)
  CPSP <- res
  CPSP$value <- CPSP$SP - CPSP$CP

  months <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="-1 month",
                length.out=no.months)
  CPSP <- subset(CPSP, month %in% months)
  
  count <- as.data.frame(table(CPSP$path.no))
  colnames(count)[1] <- "path.no"
  count$path.no <- as.numeric(as.character(count$path.no))
  
  # count how many times SP > CP 
  aux <- cast(CPSP, path.no ~ ., function(x){length(which(x>0))})
  colnames(aux)[2] <- "no.SP.gt.CP"
  count <- merge(count, aux)
  
  # count how many times SP < CP 
  aux <- cast(CPSP, path.no ~ ., function(x){length(which(x<0))})
  colnames(aux)[2] <- "no.SP.lt.CP"
  count <- merge(count, aux)
  
  return(count)
}

##########################################################################
# Use this function to investigate pattern changes in congestion 
# nC - number of months to check for change
# nH - number of months to go back in history
# threshold - $ amount by which the period (nH, nC) needs to be different
#   from period (nC, present). 
#
RPT$CPSP.patternChange <- function(filename, nC=1, nH=6, threshold=0.50)
{
  rLog(paste("Working on", filename))
  load(filename)
  CPSP <- res
  CPSP$value <- CPSP$SP - CPSP$CP

  mChange <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="-1 month",
                length.out=nC)[nC]
  mHist <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="-1 month",
                length.out=nH)[nH]
  CPSP <- subset(CPSP, month >= mHist)   # all the history of interest

  CPSP$flag <- ifelse(CPSP$month >= mChange, "post", "pre")
  
  rangeCPSP <- cast(CPSP, path.no ~ flag, c(min, max), fill=NA)

  res <- rbind(
    subset(rangeCPSP, post_min > pre_max  + threshold),
    subset(rangeCPSP, pre_min  > post_max + threshold))
  
  return(res) 
}


##########################################################################
# Using this function to see how many times SP > CP
RPT$CPSP.recent.congestion <- function(filename)
{
  rLog(paste("Working on", filename))
  load(filename)
  CPSP <- res
  CPSP$value <- CPSP$SP - CPSP$CP

  months.recent <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="-1 month",
                length.out=6)
  aux <- seq(as.Date(format(Sys.Date(), "%Y-%m-01")), by="-1 month",
                length.out=25)[-(1:3)]
  months.nby <- aux[as.numeric(format(aux, "%m")) %in% options$nby.months]
  months <- unique(sort(c(months.recent, months.nby)))

  CPSP.recent <- subset(CPSP, month %in% months.recent)
##   aux <- cast(CPSP.recent, path.no ~ ., function(x){c(mean(x), sd(x))})
##   names(aux)[2:3] <- c("mean.recent", "sd.recent")
  aux <- cast(CPSP.recent, path.no ~ ., range)
  names(aux)[2:3] <- c("min.recent", "max.recent")

  CPSP.nby <- subset(CPSP, month %in% months.nby)
##   bux <- cast(CPSP.nby, path.no ~ ., function(x){c(mean(x), sd(x))})
##   names(bux)[2:3] <- c("mean.nby", "sd.nby")
  bux <- cast(CPSP.nby, path.no ~ ., range)
  names(bux)[2:3] <- c("min.nby", "max.nby")

  res <- merge(aux, bux)

  res <- res[which(res$min.recent > res$max.nby), ]
  
  return(res)
}









## ##########################################################################
## # bids is a data.frame with c("path.no", "bid.price", "mw")
## # 
## RPT$payoff.hPortfolio <- function( paths, bids, hSP )
## {
##   bids <- bids[, c("path.no", "bid.price", "mw")]
##   bids <- merge(bids, paths[, c("path.no","hours")])
##   bids$mwh <- bids$mw * bids$hours
  
##   # historical payoff
##   aux <- hSP - matrix(bids$bid.price, nrow=nrow(hSP),
##                       ncol=ncol(hSP), byrow=T)
##   aux[is.na(hSP)] <- 0
##   hPayoff <- zoo(aux %*% bids$mwh, index(hSP))
##   colnames(hPayoff) <- "hPayoff"

##   hIQR <- IQR(coredata(hPayoff))
##   summary.hPayoff <- summary(as.numeric(hPayoff))
  
##   ind.losses <- which(hPayoff < (summary.hPayoff[2] - 10*hIQR))
##   hLosses <- as.matrix(hPayoff[ind.losses,])/1000000
##   hLosses <- hLosses[order(hLosses, decreasing=FALSE),, drop=FALSE]

##   plot(index(hPayoff[ind.losses,]))
  
## }

















## ##########################################################################
## # Calculate a score for a set of paths in order to rank them. 
## #
## RPT$calc.score <- function( paths )
## {
##   criteria <- c("ratio.EWin.ELoss", "days.gt.0/days.lt.0",
##     "mean", "p5", "Total")
##   score <- matrix(NA, nrow=nrow(paths), ncol=length(criteria),
##     dimnames=list(paths$path.no, criteria))
##   rLog(paste("Using ", length(criteria)-1, "criteria for total score."))
  
##   if ("ratio.EWin.ELoss" %in% criteria){
##     ind <- which(criteria == "ratio.EWin.ELoss" )
##     score[,ind] <- 1/(1 + exp(-paths$ratio.EWin.ELoss/4))
##   }
##   if ("days.gt.0/days.lt.0" %in% criteria){
##     ind <- which(criteria == "days.gt.0/days.lt.0" )
##     aux <- paths$days.gt.0/paths$days.lt.0
##     score[,ind] <- 1/(1 + exp(-aux/4))
##   }
##   if ("mean" %in% criteria){   
##     ind <- which(criteria == "mean" )
##     score[,ind] <- 1/(1 + exp(-paths$mean))
##   }
##   if ("p5" %in% criteria){   
##     ind <- which(criteria == "p5" )
##     score[,ind] <- 1/(1 + exp(paths$p5))
##   }
  
##   score[,"Total"] <- rowSums(score[,-ncol(score)])
## #  paths$score <- score[, "Total"]
  
##   return(score)
## }

## ##########################################################################
## # cuts is the path/cluster/weight data.frame
## # bids is the proposed bids.lim data.frame
## # Linearly interpolate the min.bid, max.bid between fraction.min, 1.
## #
## RPT$make.bids <- function( bids, cuts, fraction.min=0.10,
##                            bids.per.path=5)
## {
##   if (any(!(c("path.no", "min.bid", "max.bid") %in% names(bids))))
##     stop("Missing min/max bid per path.")

##   fractions <- seq(fraction.min, 1, length.out=bids.per.path)

##   bid.expand <- function(y, fractions, fraction.min){
##     if (round(y[2],4) == round(y[1],4))
##       res <- c(rep(NA, bids.per.path-1), y[2])
##     else
##       res <- approx(c(1, fraction.min), y, fractions)$y
##     return(res)
##   }
##   res <- t(apply(bids[,c("min.bid", "max.bid")], 1, bid.expand,
##                  fractions, fraction.min))
##   res <- data.frame(path.no=rep(bids$path.no, bids.per.path),
##                     fraction.mw=rep(fractions, each=nrow(bids)),
##                     bid.price = as.numeric(res))
##   res <- na.omit(res)

##   # some paths may be dropped here, because cuts does not have them
##   res <- merge(res, cuts)
##   res <- res[order(res$path.no, res$fraction.mw), ]
  
##   return(res)
## }

## ##########################################################################
## # Apply the quantity logic on each path to find the bid quantity.
## #
## RPT$find.bid.quantity <- function( bids, paths, hSP, options )
## {
##   paths <- paths[order(paths$path.no), ]
##   cols <- c("path.no", "class.type", "source.ptid", "sink.ptid",
##                   "sink.name", "source.name", "strategy")
##   res <- merge(paths[,cols], bids)
##   res <- res[order(res$path.no, res$fraction.mw), ]

##   # add the hours by bucket, calc bid.price.month
##   res <- merge(res, subset(NEPOOL$HRS, contract.dt == options$month), sort=F)
##   res$bid.price.month <- res$bid.price * res$hours
  
##   QQ <- data.frame(path.no=paths$path.no, mw.port=NA, mw.1dayLoss=NA,
##                    mw.limit=options$bids$max.mw.path, mw=NA)

##   # find the worst 1 day loss relative to the max bid 
##   aux <- bids
##   names(aux)[3] <- "value"
##   aux <- cast(aux, path.no ~ ., max)
##   names(aux)[2] <- "max.bid"

##   aux <- merge(aux, hSP[,c("path.no","value")])
##   aux$value <- aux$value - aux$max.bid   # my payoff
##   aux <- cast(aux, path.no ~ ., function(x){max(c(-min(x),0))})
##   names(aux)[2] <- "worst.1day.loss"
##   aux <- merge(aux, unique(bids[,c("path.no","Freq")]))
##   QQ[, "mw.1dayLoss"] <- options$bids$max.daily.loss/
##     (aux$worst.1day.loss * 24 * aux$Freq)

##   # find quantity using the portfolio constraint
##   for (p in 1:nrow(paths)){
##     aux <- subset(res, path.no==paths$path.no[p])
##     # try to impose the cluster weight constraint
##     if (all(aux$bid.price>=0)){
##       QQ[p,"mw.port"] <- options$bids$total.expense * aux$path.weight[1]/
##         sum(aux$bid.price.month * aux$fraction.mw)
##     }
##   }

##   QQ$mw   <- pmin(QQ$mw.port, QQ$mw.1dayLoss, QQ$mw.limit, na.rm=T)
##   res     <- merge(res, QQ[, c("path.no", "mw")])
##   res$mw  <- res$mw * res$fraction.mw
##   res$mwh <- res$mw * res$hours

##   cols <- c("Freq", "cluster.no", "path.weight", "contract.dt",
##             "fraction.mw")
##   res  <- res[, -which(names(res) %in% cols)]
##   res  <- res[order(res$path.no, res$mw), ]

##   #add a start.dt, end.dt 
##   res$start.dt <- options$start.dt
##   res$end.dt   <- options$end.dt
##   res$buy.sell <- "BUY"
  
##   #add a bid.no
##   aux <- melt(mapply(seq, table(res$path.no), SIMPLIFY=FALSE))
##   names(aux)  <- c("bid.no", "path.no")
##   res <- cbind(res, bid.no=aux$bid.no)   # they are properly ordered

##   # calculate the MWh
##   aux <- merge(cbind(path.no=paths[, "path.no"], contract.dt=options$month,
##                      class.type=paths[, "class.type"]), NEPOOL$HRS)
##   QQ <- merge(QQ, aux[,c("path.no", "hours")])
##   QQ$mwh <- QQ$mw * QQ$hours
  
##   return(list(res, QQ))
## }

## ##########################################################################
## # Historical simulation settle price
## # Where hSP is not melted
## #
## RPT$simulate.SP <- function( paths, hSP, enh=3, no.sims=1000, options )
## {
##   hSP.enh <- as.matrix(hSP)
##   if (enh>0){
##     ind <- as.POSIXlt(index(hSP))$mon+1
##     ind <- which(ind %in% options$nby.months)
##     hSP.nby <- as.matrix(hSP[ind,])         # the similar months
##     for (r in 1:enh)
##       hSP.enh <- rbind(hSP.enh, hSP.nby)
##   }
##   if (options$region=="NEPOOL"){
##     ind.onpeak <- secdb.isHoliday(c(index(hSP),
##       rep(index(hSP), enh)))
    
##   } else {   # sample blindly
##     ind.days <- sample(nrow(hSP.enh), no.sims*options$days["FLAT"],
##       replace=TRUE)
##   }

##   sSP <- hSP.enh[ind.days, ]
##   sSP <- array(sSP, dim=c(options$days["FLAT"], no.sims, ncol(hSP)))
##   sSP <- colMeans(sSP, na.rm=TRUE)

##   colnames(sSP) <- colnames(hSP)
  
## #  sSP <- apply(sSP, 2, quantile, probs=seq(0,1,by=0.01))
##   return(sSP)  
## }
## ##########################################################################
## # simulate awards.  Return an array of possible awards, 
## # pick random collection of paths, and then a random bid for a given
## # number of repetitions (reps) 
## #
## RPT$simulate.QQ <- function( paths, bids.xls, groups=c(30, 60), reps=100 )
## {
##   # groups of random paths
##   if (!is.null(groups)){
##     rpaths <- paste("r", groups, sep="")
##   } else {
##     rpaths <- NULL}
 
##   sQQ <- array(NA, dim=c(nrow(paths), 2, (1+length(groups)), reps),
##     dimnames=list(paths$path.no, c("mwh", "bid.price"),
##     c("min.QQ", rpaths), 1:reps))

##   # add the min quantities
##   aux <- aggregate(bids.xls[,"bid.no"], list(bids.xls[,"path.no"]), min)
##   names(aux) <- c("path.no", "bid.no")
##   aux <- merge(bids.xls[,c("mwh", "bid.price", "path.no", "bid.no")], aux)
##   aux <- aux[order(aux$path.no), ]
##   sQQ[,,1,] <- as.matrix(aux[,c("mwh", "bid.price")])

##   # get the number of bids by path.no
##   map <- mapply(seq, table(bids.xls$path.no), SIMPLIFY=FALSE)
  
##   for (g in rpaths){     # loop over the path groupings
##     rLog(paste("Working on group", g))
##     for (r in 1:reps){   # do 100 replications to remove bias 
##       no.paths <- as.numeric(gsub("r", "", g))
##       ind <- sample(nrow(paths), no.paths, replace=FALSE)
##       selected.paths <- names(map)[ind]
##       aux <- lapply(map[selected.paths], function(x){sample(length(x),1)}) 
##       aux <- melt(aux)
##       names(aux) <- c("bid.no", "path.no")
##       aux$path.no <- as.numeric(aux$path.no)
##       aux <- merge(bids.xls[,c("mwh", "bid.price", "path.no", "bid.no")], aux)
##       sQQ[as.character(aux$path.no),,g,r] <-
##         as.matrix(aux[,c("mwh", "bid.price")])
##     }
##   }
##   return(sQQ)
## }




##########################################################################
# 
#
## RPT$plot.cluster.payoff <- function( hSP, cuts, paths.xls, options )
## {
##   aux <- aggregate(paths.xls[, c("bid.price.month", "mw")],
##                    list(path.no=paths.xls$path.no), max)
##   # conservative, put all quantity at max bid.
##   aux$expense <- aux$bid.price.month * aux$mw
##   aux <- merge(aux, cuts[,c("path.no", "cluster.no")])
##   aux <- aux[, c("cluster.no", "expense")]
##   aux <- melt(aux, id=1)
##   aux <- cast(aux, cluster.no + variable ~ ., sum)
##   names(aux)[3] <- "value"

##   hSP <- merge(hSP, unique(paths.xls[,c("path.no", "class.type")]))
##   hSP <- merge(hSP, cuts[,c("path.no", "cluster.no")])
##   hSP <- merge(hSP, aux[, c("path.no", "mw")])
##   hSP$contract.dt <- as.Date(format(hSP$day, "%Y-%m-01"))
##   hSP <- merge(hSP, NEPOOL$HRS)
##   hSP$value <- hSP$mw * hSP$hours * hSP$value

##   hSP <- cast(hSP, cluster.no + day ~ ., sum)
##   names(hSP)[3] <- "value"

##   if (!is.null(enh)){
##     months <- c(seq(month, by="-1 month", length.out=2)[2],
##                 seq(month, by="month", length.out=2))
##     months <- as.numeric(format(months, "%m"))
##     ind <- as.POSIXlt(hSP$month)$mon+1
##     ind <- which(ind %in% months)
##     CSP.nby <- CSP[ind,]             # the similar months
##     for (r in 1:enh)
##       CSP.enh <- rbind(CSP.enh, CSP.nby)
##   }  
## ##   aux <- hSP[]  
## ##   densityplot( ~ value, data=hSP, groups=)
## }

