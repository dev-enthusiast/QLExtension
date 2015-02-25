# Read/Write ftrbids csv files
#
#
#


#################################################################
# read the ftrbids from the csv file
#
read_ftrbids_csv_file <- function(filename)
{
  aux <- read.csv(filename, skip=1)
  aux <- subset(aux, I == "D")
 
  # add the auction name 
  auctionName <- if (ncol(aux) == 14) {
    FTR.AuctionTerm(as.Date(aux$BEGIN[1], "%Y/%m/%d"),
                    as.Date(aux$END[1],   "%Y/%m/%d"))
  } else {
    aux[1,15]
  }
  
  res <- data.frame(
    auction  = auctionName,                 
    start.dt = as.Date(aux$BEGIN, "%Y/%m/%d"),
    end.dt   = as.Date(aux$END,   "%Y/%m/%d"),
    class.type  = aux$CLASS,
    buy.sell    = aux$BUY,
    source.name = aux$SOURCE,
    sink.name   = aux$SINK,
    mw          = aux$MW,
    bid.price.month = aux$PRICE                
  )
  
  # add the path.no column
  uPaths <- unique(res[,c("class.type", "buy.sell", "source.name", "sink.name")])
  uPaths <- cbind(path.no=1:nrow(uPaths), uPaths)
  res2 <- merge(uPaths, res)

  # add bid.no column
  res3 <- ddply(res2, .(path.no), function(x) {
    x <- x[order(-x$bid.price.month),]
    cbind(bid.no=1:nrow(x), x)
  })
  i1 <- which(colnames(res3)=="path.no")
  i2 <- which(colnames(res3)=="bid.no")
  res4 <- res3[, c(i1, i2, setdiff(1:ncol(res3), c(i1, i2)))] 
  
  res4
}


#################################################################
# 
#
.make.NEPOOL.upload.sheet <- function(bids.xls, file, do.checks=TRUE)
{
  rLog("DEPRECATED.  Use write_ftrbids_csv_file!")
}



#################################################################
# bids is a data.frame with bids
#
write_ftrbids_csv_file <- function(bids, filename, do.checks=TRUE)
{
  if (!(all(c("class.type", "buy.sell",
        "source.name", "sink.name", "mw", "bid.price.month") %in% names(bids))))
    stop("Missing some columns in bids")
  
  if (do.checks)
    .check_bids(bids)
  
  auctionObj <- parse_auctionName( bids$auction[1] ) 
  if (auctionObj$startDt <= Sys.Date()) {
    rLog("You cannot bid for an auction that started in the past!")
    return()
  }
  bids$start.dt <- auctionObj$startDt
  bids$end.dt   <- auctionObj$endDt 

  
  MM <- matrix("", nrow=(nrow(bids)+3), ncol=14)
  MM[1,1:6] <- c("C","FTR","FTRBID", "ISONE", format(Sys.Date(), "%Y/%m/%d"),
                 format(Sys.time(), "%H:00 %p"))
  MM[2,1:14] <- c("I","FTRBID", "BID", "VERSION", "PARTICIPANT", "MARKET",
    "BEGIN", "END", "CLASS", "BUYSELL", "SOURCE", "SINK", "MW", "PRICE")
  MM[3:(nrow(MM)-1), 1:6] <- rep(c("D", "FTRBID", "BID","1",
    "Exelon Generation Company_ LLC", "A"), each=(nrow(MM)-3))
  MM[3:(nrow(MM)-1),7]  <- format(as.Date(bids$start.dt), "%Y/%m/%d")
  MM[3:(nrow(MM)-1),8]  <- format(as.Date(bids$end.dt), "%Y/%m/%d")
  MM[3:(nrow(MM)-1),9]  <- as.character(bids$class.type)
  MM[3:(nrow(MM)-1),10] <- as.character(bids$buy.sell)
  MM[3:(nrow(MM)-1),11] <- as.character(bids$source.name)
  MM[3:(nrow(MM)-1),12] <- as.character(bids$sink.name)
  MM[3:(nrow(MM)-1),13] <- as.character(round(bids$mw,1))
  MM[3:(nrow(MM)-1),14] <- as.character(round(bids$bid.price.month,2))
  
  MM[nrow(MM),1:3] <- c("C", "END OF REPORT", as.character(nrow(MM)))

  write.table(MM, file=filename, row.names=F, col.names=FALSE, sep=",",
              quote=FALSE)
  
  # remove extra commas from the csv file
  aux <- readLines(filename)
  aux[1] <- gsub(",,,,,,,,", "", aux[1])
  aux[length(aux)] <- gsub(",,,,,,,,,,,", "", aux[length(aux)])
  writeLines(aux, filename)

  rLog("Wrote file", filename)
}


#################################################################
#################################################################
# 
.test <- function()
{
  require(reshape)
  require(CEGbase)
  require(SecDb)
  require(FTR)
  FTR.load.ISO.env("NEPOOL")

  filename <- "S:/All/Structured Risk/NEPOOL/FTRs/Monthly Auction Analyses/Long-term Auctions/2012 Annual Auction/uploadBids1.csv"

  
  
}
