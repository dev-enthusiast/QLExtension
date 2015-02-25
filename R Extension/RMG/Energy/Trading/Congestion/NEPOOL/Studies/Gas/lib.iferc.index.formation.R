# Study how the iferc index gets set.
#
# 
#


#######################################################################
# Calculate the index from the individual trades and see if ICE trades
# are a good indicator
#
.aggregate_trades <- function(trades)
{
  trd <- subset(trades, product=="NG Firm Phys, BS, LD1")
  trd$date <- format(trd$time, "%Y-%m-%d")

  res <- ddply(trd, .(strips, date), function(x){
    data.frame(
      iceAvgPrice = sum(x$quantity*x$price)/sum(x$quantity),
      iceTotalVolume = sum(x$quantity))
  })
  res$month <- as.Date(paste("1", res$strips, sep=""), format="%d%b%y")
  
  res
}


#######################################################################
# Pull the prices from tsdb
#
.get_iferc_tsdb <- function(months, hub)
{
  if (hub == "AGT-CG") {
    symbols <- c(
      "Platts_InsFERC_IGBEE36u",  "iferc.price",  # Index price (basis)
      "Platts_InsFERC_IGBEE36w",  "iferc.volume" # Index volume        
      )
  } else {
    stop("Hub ", hub, "not implemented")
  }
  symbols <- matrix(symbols, ncol=2, byrow=TRUE)

  
  res <- FTR:::FTR.load.tsdb.symbols(symbols[,1], min(months), max(months)+7)
  res <- cbind(date=index(res), as.data.frame(res))
  colnames(res)[2:ncol(res)] <- symbols[,2]
  rownames(res) <- NULL
  res$month <- as.Date(format(res$date, "%Y-%m-01"))

  res[,c("month", "iferc.price", "iferc.volume")]  
}



#######################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(lattice)
  require(SecDb)
  require(zoo)
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Studies/Gas/lib.iferc.index.formation.R")

  
  #===================================================================
  require(RODBC)
  asOfDate <- Sys.Date()
  currentMonth <- as.Date(format(asOfDate, "%Y-%m-01"))
  months <- rev(seq(currentMonth, by="-1 month", length.out=12))

  hub <- "AGT-CG"  
  trades <- ldply(as.list(months), .get_InsideFerc_trades, hub=hub)
  
  iferc <- .get_iferc_tsdb(months, hub)

  res <- .aggregate_trades(trades)
  ice <- ddply(res, .(month), function(x) {
    data.frame(
      ice.price=sum(x$iceAvgPrice*x$iceTotalVolume)/sum(x$iceTotalVolume),
      ice.volume=sum(x$iceTotalVolume))
  })


  data <- merge(iferc, ice, all=TRUE)

  write.csv(trades, "c:/Temp/ice_bidweek_trades.csv", row.names=FALSE)

  
  
  
  
  


  

 
  
  

  
  
}
