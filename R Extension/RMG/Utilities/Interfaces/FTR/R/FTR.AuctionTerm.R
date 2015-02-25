`FTR.AuctionTerm` <-
function(start.dts=NULL, end.dts=NULL, auction=NULL, region=NULL)
{
  if (!is.null(auction)){
    res <- .AuctionTermToDays(auction)
  } else {
    res <- .AuctionTerm(start.dts, end.dts)
  }
  return(res)
}


# needs functions from Congestion/lib.NEPOOL.FTR.R
# rewritten on 12/11/2012
.AuctionTermToDays <- function(auction)
{
  auctionObj <- parse_auctionName(auction)
  if (length(auction)==1) auctionObj <- list(auctionObj)  # nasty!
  
  aux <- ldply(auctionObj, function(x){
    c(x$startDt, x$endDt)
  })
  colnames(aux) <- c("start.dt", "end.dt")

  aux
}



.AuctionTerm <- function(start.dts, end.dts)
{
  start.dts <- as.POSIXlt(start.dts)
  end.dts   <- as.POSIXlt(end.dts)
  mdiff <- (12*end.dts$year + end.dts$mon)-(12*start.dts$year + start.dts$mon)

  term <- format.dateMYY(start.dts, -1)
  lvls <- format.dateMYY(sort(unique(start.dts)), -1)  # construct the levels
  ind  <- which(mdiff==5)                 # 6Mth auction
  if (length(ind)>0){
    term[ind] <- paste(term[ind], "-6M", sep="")
    lvls <- c(lvls, sort(unique(term[ind])))
  }
  ind  <- which(mdiff==11)                 # annual auction
  if (length(ind)>0){
    term[ind] <- paste(term[ind], "-1Y", sep="")
    lvls <- c(lvls, sort(unique(term[ind])))
  }
  return(term)
}

## .AuctionTermToDays <- function(auction)
## {
##   uAuction <- unique(auction)
##   aux <- strsplit(uAuction, "-")
##   maxSlot <- max(sapply(aux, length))  # how many splits to I have
##   fun.aux <- function(x, maxSlot){
##     y <- rep("", maxSlot)   
##     if (length(x)==1){x[2] <- "1"}  # for monthly auctions
##     y[1:length(x)] <- x
##     return(y)
##   }
##   aux <- data.frame(t(sapply(aux, fun.aux, maxSlot)))
  
##   # no auctions longer than 1 year!
##   aux[,2] <- gsub("1Y", "12M", aux[,2])
##   aux[,2] <- gsub("M", "", aux[,2])

##   fun.advance <- function(x){
##     seq(as.Date(x[1]), by=paste(as.numeric(x[2]), "month"),
##         length.out=2)[2]
##   }

##   aux$start.dt <- format.dateMYY(aux[,1], 1)
##   aux$end.dt   <- apply(aux[,c("start.dt","X2")], 1, fun.advance)-1
##   class(aux$end.dt) <- "Date"
##   aux$auction <- uAuction

##   res <- merge(aux[, c("auction", "start.dt", "end.dt")],
##                data.frame(path.no=1:length(auction), auction))
##   res <- res[order(res$path.no), ]

##   return(res[,c("start.dt", "end.dt")])
## }
