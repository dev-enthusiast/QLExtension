##########################################################################
# For a given set of paths, get the settle price, clearing price, and
# suggest a bid price.  Use less than 10000 paths for best results
# month can be an R Date, then return only M-1, M, M+1
# enh is the number of times you weight extra the M-1, M, M+1 months
#
FTR.makeBidLimits <- function(paths, sSP, cut.quantile=list(0,0.25))
{
  if (length(cut.quantile)==1){
    cut.quantile <- rep(cut.quantile, nrow(sSP))}

  res <- mapply(quantile, as.list(as.data.frame(t(sSP))),
                          probs=cut.quantile)

  res <- data.frame(path.no=paths$path.no, matrix(res, ncol=2, byrow=TRUE))
  colnames(res) <- c("path.no", "min.bid", "max.bid")
  
##   # if the bid range is less than $0.10, increase it to $0.10.
##   res$min.bid <- ifelse((res$max.bid - res$min.bid)<0.1, res$max.bid-0.1,
##                         res$min.bid)
  
  return(res)
}


##   this.month <- month
##   if (length(cut.quantile)==1){cut.quantile <- rep(cut.quantile, nrow(sSP))}
##   ind <- which(names(paths) %in% "auction")
##   if (length(ind) > 0){paths <- paths[,-ind]}  # conflict with CP!
  
##   # get the settle prices
##   hSP <- FTR.get.hSP.for.paths(paths, ISO.env, melt=F)
##   SP <- aggregate(hSP, list(format(index(hSP), "%Y-%m-01")), mean, na.rm=T)
##   SP <- melt(as.matrix(SP))
##   SP <- SP[is.finite(SP$value), ]
##   colnames(SP) <- c("month", "path.no", "SP")
##   SP$path.no <- as.numeric(as.character(SP$path.no))
  
##   # get the cleared prices
##   CP <- FTR.get.CP.for.paths(paths, ISO.env)
##   CP <- CP[-grep("-",CP$auction), ]  # remove the multi-months
##   CP <- cbind(CP, month=FTR.AuctionTerm(auction=CP$auction)$start.dt)
##   CP <- CP[,-which(colnames(CP)=="auction")]
##   if (causal & !is.null(month)) CP <- subset(CP, month <= this.month)
  
##   # merge them
##   CPSP <- merge(CP, SP)
##   CPSP <- cbind(CPSP, auction=FTR.AuctionTerm(CPSP$month, CPSP$month))

##   CPSP$return <- CPSP$SP -  CPSP$CP
##   CPSP.enh <- CPSP

##   if (!is.null(month)){
##     months <- c(seq(month, by="-1 month", length.out=2)[2],
##                 seq(month, by="month", length.out=2))
##     months <- as.numeric(format(months, "%m"))
##     ind <- as.POSIXlt(CPSP$month)$mon+1
##     ind <- which(ind %in% months)
##     CPSP.nby <- CPSP[ind,]             # the similar months
##     for (r in 1:enh)
##       CPSP.enh <- rbind(CPSP.enh, CPSP.nby)
##   }

##   aux <- CPSP.enh[, c("path.no", "CP")]
##   names(aux)[2] <- "value"
##   res <- cast(aux, path.no ~ ., quantile, probs=c(0, 0.05, 0.25, 0.5))
##   names(res)[2:5] <- c("CP.min", "CP.enh.p5", "CP.enh.p25", "CP.enh.p50")

##   aux <- CPSP.enh[, c("path.no", "SP")]
##   names(aux)[2] <- "value"
##   bux <- cast(aux, path.no ~ ., quantile, probs=c(0, 0.25))
##   names(bux)[2:3] <- c("SP.min", "SP.enh.p25")
##   res <- merge(res, bux)

##   aux <- CPSP.nby[, c("path.no", "SP")]
##   names(aux)[2] <- "value"
##   bux <- cast(aux, path.no ~ ., quantile, probs=c(0.25))
##   names(bux)[2] <- c("SP.nby.p25")
##   res <- merge(res, bux)
  
##   res$min.bid <- pmin(res$CP.min, res$SP.min)  # <---
## #  res$max.bid <- pmin(res$CP.p25, res$SP.p25)  # <---
##   res$max.bid <- res$SP.enh.p25  # <---  
