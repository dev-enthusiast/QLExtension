# Extract the ALCG next day and intra day trades to compare
# volumes, time of trades, intra day price spread, etc. 
#
# This script has been used to discuss with the ISO
#
# .analysis
# .archive
# .get_trade_data_algcg
# .get_trade_data_nepoolda
#
#

#################################################################
#
.analysis <- function(trades)
{
  unique(trades[,c("product", "strips")])
  
  # get the algcg index
  aa <- tsdb.readCurve("GasDailyMean_algcg", as.Date("2010-01-01"), Sys.Date())
  colnames(aa) <- c("begin.date", "algcg.gdm")
  
  TT <- subset(trades, strips %in% c("Same Day", "Next Day Gas",
    "NxDay Intra", "Custom Daily"))
  unique(TT[,c("product", "strips")])
  TT$begin.date <- as.Date(TT$begin.date, "%m/%d/%Y")
  TT <- merge(TT, aa)
  TT$month <- as.numeric(format(TT$begin.date, "%m"))
  
  
  FP <- subset(TT, product == "NG Firm Phys, FP")
  unique(FP[,c("product", "strips")])

  table(FP$order.type)
##   Bid Offer 
## 16650 16757 
# pretty flat

  # Distirbution of trading volume
  ND <- subset(FP, strips == "Next Day Gas")
  aux <- ddply(ND, c("begin.date"), function(x){sum(x$total.volume)})
  hist(aux$V1, main="Next Day Gas total volume traded")

  
  
  # same day trades, how much over GDM?
  SS <- subset(FP, strips=="Same Day" & month %in% c(1,2,12))
  SS$diff  <- SS$price - SS$algcg.gdm
  SS$ratio <- (SS$price - SS$algcg.gdm)/SS$algcg.gdm
  
  round(c(summary( SS$diff ), std.dev=sd(SS$diff)), 2)
  round(c(summary( SS$ratio ), std.dev=sd(SS$ratio)), 2)

  # how much over GDD, you're willing to pay for the Physical gas?
  ID <- subset(TT, product=="NG Firm Phys, ID, GDD")
  ddply(ID, c("month"), function(x){
    round(summary(x$price), 2)
  })

  
  x <- ecdf(SS$diff)
  plot(x)
  
  densityplot( ~ diff, data=SS)
  
  
  ranges <- ddply(FP, c("product", "strips", "begin.date"), function(x){
    #browser()
    c(minPrice=min(x$price), maxPrice=max(x$price))
  })
  ranges <- subset(ranges, begin.date >= as.Date("2010-01-01"))
  ranges <- ranges[order(ranges$begin.date, ranges$strips), ]

  ranges <- merge(ranges, aa)
  aux <- melt(ranges, id=c(1:3, 6))
 
  
  xyplot(value ~ algcg.gdm, data=aux,
    groups=aux$variable,
    type=c("g", "p"), 
    ylab="Algonquin CG, $/MMBTU",
    xlab="Algonquin CG GDM, $/MMBTU")

  
    ## layout=c(1,3),
    ## scales=list(y=list(relation="free")))
 
   
  aux <- melt(ranges, id=1:3)
  aux$year <- format(aux$begin.date, "%Y")
  aux$dayofyear <- as.numeric(format(aux$begin.date, "%j"))

  
  xyplot(value ~ dayofyear|year, data=aux,
    groups=aux$variable,
    type=c("g", "o"), 
    xlab="Day of year",
    ylab="Algonquin CG, $/MMBTU",
    layout=c(1,3),
    scales=list(y=list(relation="free")))
  

  
  xyplot(value ~ dayofyear|year, data=aux,
    groups=aux$variable,
    type=c("g", "o"), 
    xlab="Day of year",
    ylab="Algonquin CG, $/MMBTU",
    layout=c(1,3),
    scales=list(y=list(relation="free")))
  
    
}



#################################################################
# archive the data
#
.archive <- function(tag="algcg")
{
  filename <- paste("C:/Temp/", tag, "_trades.csv", sep="")
  if (file.exists(filename)) {
    fnameNew <- paste("C:/Temp/", tag, "_tradesnew.csv", sep="")
    write.csv(trades, file=fnameNew, row.names=FALSE)
    tradesNew <- read.csv(fnameNew)
    trades0 <- read.csv(filename)
    
    # append the new data ...
    tstampNew <- setdiff(unique(trades$TIME), unique(trades0$TIME)) 
    tradesNew <- subset(trades, TIME %in% tstampNew)
    trades <- rbind(trades0, tradesNew)
  }
  
  trades$BEGIN_DATE <- as.Date( trades$BEGIN_DATE, "%m/%d/%Y" )
  #trades$TIME <- strftime( trades$TIME, "%m/%d/%Y %I:%M:%S %p")
  trades <- trades[order(trades$BEGIN_DATE), ]
  trades$BEGIN_DATE <- format(trades$BEGIN_DATE, "%m/%d/%Y")
  
  write.csv(trades, file=filename, row.names=FALSE)
  rLog("Wrote file ", filename)
}


#################################################################
# filter the individual daily files
#
.get_trade_data_algcg <- function(startDt, endDt=Sys.Date())
{
  days <- seq(startDt, endDt, by="1 day")

  hub   <- "AGT-CG"
  #strip <- c("Next Day Gas", "Same Day")
  trades <- ldply(as.list(days), function( day ) {
    filename <- .get_file_for_day(day, commodity="gas")
    #browser()
    res <- if (file.exists(filename)){
      .get_gasdeals(filename, hub=hub, strip=NULL)
    } else {
      NULL
    }
    res
  })

  trades
}

#################################################################
# filter the individual daily files
#
.get_trade_data_brent <- function(startDt, endDt=Sys.Date())
{
  days <- seq(startDt, endDt, by="1 day")

  hub   <- "NBP"
  #strip <- c("Next Day Gas", "Same Day")
  trades <- ldply(as.list(days), function( day ) {
    filename <- .get_file_for_day(day, commodity="gas")
    #browser()
    res <- if (file.exists(filename)){
      .get_gasdeals(filename, hub=hub, strip=NULL)
    } else {
      NULL
    }
    res
  })

  # take out the spreads
  trades <- trades[!grepl("/", trades$strips),]
  trades$strips <- .fix_strip(trades$strips)
  
  trades
}



#################################################################
# filter the individual daily files
#
.get_trade_data_nbp <- function(startDt, endDt=Sys.Date())
{
  days <- seq(startDt, endDt, by="1 day")

  hub   <- "NBP"
  #strip <- c("Next Day Gas", "Same Day")
  trades <- ldply(as.list(days), function( day ) {
    filename <- .get_file_for_day(day, commodity="gas")
    #browser()
    res <- if (file.exists(filename)){
      .get_gasdeals(filename, hub=hub, strip=NULL)
    } else {
      NULL
    }
    res
  })

  # take out the spreads
  trades <- trades[!grepl("/", trades$strips),]
  trades$strips <- .fix_strip(trades$strips)
  
  trades
}


#################################################################
# filter the individual daily files
#
.get_trade_data_nepoolda <- function(startDt, endDt=Sys.Date())
{
  days <- seq(startDt, endDt, by="1 day")

  trades <- ldply(days, function( day ) {
    filename <- .get_filename(day, tag="POWER_TRADES")
    #browser()
    res <- if (file.exists(filename)) {
      rLog("Working on ", format(day))
      aux <- read.delim(filename, sep="|")
      if (ncol(aux) == 1) {
        aux <- read.delim(filename, sep=",")   # 11/29/2012 file is , separated!
      }
      subset(aux, HUBS %in% c("Nepool MH DA (Daily)", "Nepool MH Day-Ahead") &
             STRIP == "Next Day" )
    } else {
      NULL
    }
    res
  })

  #write.csv(trades, "c:/Temp/next_day_trades.csv")
  
  .archive(tag="next_day")

  trades
}



#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(RODBC)
  require(lattice)
  require(reshape2)
  require(SecDb)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Studies/NextDay/ice_liquidity.R")

  
  # what I use to update the data for ALG next-day analysis
  startDt <- as.Date("2013-01-01")
  endDt   <- as.Date("2013-05-31")
  trades <- .get_trade_data_brent(startDt, endDt)
  write.csv(trades, "C:/temp/brent_trades.csv", row.names=FALSE)


  
  #trades <- .get_trade_data_algcg(startDt, endDt)
  #write.csv(trades, "C:/temp/algcg_trades.csv", row.names=FALSE)
  
  trades <- .get_trade_data_nbp(startDt, endDt)
  write.csv(trades, "C:/temp/nbp_trades_all.csv", row.names=FALSE)
  # copy and paste this file into the one on S:/ drive for Scott.
  save(trades, file="C:/temp/nbp_trades_all.RData")

  
  trades <- read.csv("C:/Temp/algcg_day_trades.csv")

  

  # look a bit at the NBP trades
  trades <- read.csv("C:/Temp/nbp_trades.csv")
  head(trades)
  trades$trade.day <- as.Date(trades$time)
  sort(unique(trades$strip))

  # volume by strip
  aux <- ddply(trades, c("strips"), function(x){
    sum(x$quantity)*100 # in mmbtu/day
  })
  colnames(aux)[2] <- "mmbtu"
  aux <- aux[order(-aux$mmbtu),]
  
  
  
  # volume by day
  aux <- ddply(trades, c("trade.day", "strips"), function(x){
    sum(x$quantity)*100 # in mmbtu/day
  })
  colnames(aux)[3] <- "mmbtu/day"
  bux <- cast(aux, strips ~ ., function(x)round(mean(x)), value="mmbtu/day")
  colnames(bux)[2] <- "avg.mmbtu/day"
  bux <- bux[order(-bux$"avg.mmbtu/day"),]
  print(bux, row.names=FALSE)
  
  
  aux <- aux[order(-aux$mmbtu),]



  
  aux <- ddply(trades, c("strips", "trade.day"), function(x){
    summary(x$price)
  })
  bux <- subset(aux, strips=="Winter14")
  bux$range <- bux$Max. - bux$Min.
  plot(bux$trade.day, bux$range)
  summary(bux$range)
  
  # a
  
  

  
  

}
