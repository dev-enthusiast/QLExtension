# Extract the ALCG next day and intra day trades to compare
# volumes, time of trades, intra day price spread, etc. 
#
# This script has been used to discuss with the ISO
#
# .analysis_algcg
# .analysis_nbp
# .analysis_nepoolda
# .get_trade_data_algcg
# .get_cal_trades_nepool
# .group_trades_timestamp
# .price_formation_nextday
#



#################################################################
#
.analysis_algcg <- function(mongo=mongo.create(host="10.100.101.82"))
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(rmongodb)
  require(reshape)
  require(reshape2)
  require(lattice)
  ns <- "ice.gas_trades"

  hub <- "AGT-CG"
  aux <- .filter_archive(mongo, ns,
    query=.query_nextday_gas_trades(hub=hub), 
    sort=mongo.bson.from.list(list(TIME=1L))) 
  trades <- .format_trades(aux)                  # custom formatter
  #write.csv(trades, file=paste("c:/temp/", hub, ".csv", sep=""), row.names=FALSE)
  colnames(trades) <- tolower(colnames(trades))
  
  unique(trades[,c("product", "strip")])

  
  # get the algcg index
  symbols <- c("GasDailyMean_algcg",
               "ICE_10xG_EA_ALGCG_Avg",
               "ICE_10xG_EA_ALGCG_Trds")
  aa <- FTR:::FTR.load.tsdb.symbols(symbols, as.Date("2010-01-01"), Sys.time())
  aa <- cbind(begin.date=index(aa), data.frame(aa))
  colnames(aa) <- c("day", "algcg.gdm", "algcg.ice", "ice.trades")


  trades$month <- format(trades$time, "%Y-%m-01") 
  trades <- subset(trades, month %in% c("2013-12-01", "2014-01-01", "2014-02-01") &
                           strip == "Next Day Gas" & product == "NG Firm Phys, FP" )

  # look at 5 min price buckets
  bucket5min_fun <- function(x) {
     secondsOfDay <- as.numeric(x) -  as.numeric(as.POSIXct(format(x, "%Y-%m-%d")))
     secondsOfDay %/% 300       # bucket in 5 min intervals 
   }
  agt <- .group_trades_timestamp(trades, bucket5min_fun)

  
  ## agt <- .price_formation_nextday( trades )
  colnames(agt)[3:5] <- paste("algcg.", colnames(agt)[3:5], sep="")
  agt <- merge(agt, aa[,c("day", "algcg.gdm", "algcg.ice")])
  head(agt)
  #write.csv(agt, file="c:/temp/algcg_wap_time_2013-12-12.csv", row.names=FALSE)

  

  xyplot(noDeals ~ day, data=res, type=c("g", "p", "smooth"),
         subset=res$.id == "before10am",
         ylab="Number of ICE trades for Algcg FP before 10AM")
  
  xyplot(noDeals ~ day, data=res, type=c("g", "p", "smooth"),
         subset=res$.id == "before9:30am",
         ylab="Number of ICE trades for Algcg FP before 9:30AM")
  
  aux <- cast(res, day ~ .id, I, value="noDeals")
  aux$'after10am' <- aux$allDay - aux$'before10am'
  xyplot(after10am ~ day, data=aux, type=c("g", "p", "smooth"),
         ylab="Number of ICE trades for Algcg FP after 10AM")
  



  
  
  TT <- subset(trades, strip %in% c("Same Day", "Next Day Gas",
    "NxDay Intra", "Custom Daily"))
  unique(TT[,c("product", "strip")])
  TT$day <- as.Date(TT$begin_date)
  TT <- merge(TT, aa, by="day")

  FP <- subset(TT, product == "NG Firm Phys, FP")
  unique(FP[,c("product", "strip")])

  table(FP$order_type)
##   Bid Offer 
## 16650 16757 
# pretty flat

  subset(FP, day==as.Date("2014-01-05"))

  
  vol <- ddply(FP, "day", function(x){
    ind1 <- format(x$time, "%H:%M:%S") < "09:30:00"
    ind2 <- format(x$time, "%H:%M:%S") < "10:00:00"
    ind3 <- format(x$time, "%H:%M:%S") < "12:00:00"
    data.frame(before930AM  = sum(x$total_volume[ind1]),
               before10AM = sum(x$total_volume[ind2]),
               beforeNoon = sum(x$total_volume[ind3]),
               allVolume  = sum(x$total_volume))
  })
  vol2 <- melt(vol, id=1)
  xyplot(value/100000 ~ day, data=vol2, groups=variable,
         type=c("g", "l"),
         main="Algonquin ICE Next Day FP",
         xlab="",
         ylab="Trading volume, 100,000 MMBTU",
         auto.key=list(space="top", points=FALSE, lines=TRUE,
           columns=2))
  
  sum(vol$before930AM)/sum(vol$allVolume)
  
  
  
  
  
  FP$time_bucket <- ifelse(format(FP$time, "%H:%M:%S") < "09:00:00", "before 9AM",
                    ifelse(format(FP$time, "%H:%M:%S") < "10:00:00", "before 10AM",
                    ifelse(format(FP$time, "%H:%M:%S") < "12:00:00", "before 12PM",  "after 12PM" )))
  
  

  
  # Distribution of trading volume
  ND <- subset(FP, strip == "Next Day Gas")
  aux <- ddply(ND, c("begin_date"), function(x){sum(x$total_volume)})
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
#
.analysis_nbp <- function(trades)
{
  TT <- trades
  TT$month <- as.Date(format(TT$TIME, "%Y-%m-01"))
  TT$mon   <- format(TT$TIME, "%b")
  aux <- as.POSIXlt(TT$TIME)
  TT$secondsOfDay <- aux$hour*3600 + aux$min*60 + aux$sec

  #xyplot(secondsOfDay ~ QUANTITY|)
 
  
  
}

#################################################################
# 
.analysis_nepoolda <- function(mongo=mongo.create())
{
  ns <- "ice.power_trades"
  aux <- .filter_archive(mongo, ns,
    query=.query_nepool_nextday(), 
    sort=mongo.bson.from.list(list(TIME=1L)))
  
  trades <- .format_trades(aux)                  # custom formatter
  colnames(trades) <- tolower(colnames(trades))
  unique(trades$strip)
  trades$month <- format(trades$time, "%Y-%m-01") 
  trades <- subset(trades, month %in% c("2013-01-01", "2013-02-01"))

  # look at 5 min price buckets
  bucket5min_fun <- function(x) {
     secondsOfDay <- as.numeric(x) -  as.numeric(as.POSIXct(format(x, "%Y-%m-%d")))
     secondsOfDay %/% 300       # bucket in 5 min intervals 
   }
  pwx <- .group_trades_timestamp(trades, bucket5min_fun)
  colnames(pwx)[3:5] <- paste("pwr", colnames(pwx)[3:5], sep=".")

  # look how price formed during the day
  ## pwx <- .price_formation_nextday(trades)
  ## colnames(pwx)[3:5] <- paste("pwr", colnames(pwx)[3:5], sep=".")

  # add the DA cleared price
  symbols <- c("nepool_smd_da_4000_lmp_5x16")
  aa <- FTR:::FTR.load.tsdb.symbols(symbols, as.Date("2010-01-01"), Sys.time())
  aa <- cbind(begin.date=index(aa), data.frame(aa))
  colnames(aa) <- c("day", "pwr.cleared.peak")
  pwx <- merge(pwx, aa)

  .make_bucketName <- function(i){
    seconds <- i * 300
    hour <- i %/% 12
    minutes <- (i*300 - hour*3600)/60
    paste(sprintf("%02i",hour), sprintf("%02i",minutes), sep=":")
  }

  all <- merge(agt, pwx, by=c("day", "bucket"))
  all$bucketName <- .make_bucketName( all$bucket )
  head(all)

  all$heatRate <- all$pwr.wap / all$algcg.wap

  hr <- ddply(all, c("day"), function(x) {
    data.frame(range.HR = max(x$heatRate)-min(x$heatRate))
  })
  print(hr, row.names=FALSE) 
  
  
  write.csv(all, file="c:/temp/algcg+pwr_price_formation.csv", row.names=FALSE)

}





#################################################################
# for analysis of Cal liquidity
#
.get_cal_trades_nepool <- function()
{
  mongo <- mongo.create()
  ns <- "ice.power_trades"

  sort(mongo.distinct(mongo, ns, "HUBS"))
 
  strips <- c("Cal 14", "Cal 15")
  hubs <- c("Nepool MH Day-Ahead",
            "Nepool MH DA",
            "Nepool MH Day-Ahead",
            "Nepool MH DA Mini",
            
            "Nepool Mass Hub  Off-Peak",
            "Nepool MH DA Off-Peak",
            "Nepool MH Day-Ahead Off-Peak",
            "Nepool MH DA Off-Peak Mini",

            "ISO-NE Con DA",
            "ISO-NE Con DA Off-Peak",
            "ISO-NE Maine DA",
            "ISO-NE Maine DA Off-Peak",
            "ISO-NE NE-Mass DA",
            "ISO-NE NE-Mass DA Off-Peak",
            "ISO-NE New Hampshire DA",
            "ISO-NE New Hampshire DA Off-Peak",
            "ISO-NE SE-Mass DA",
            "ISO-NE SE-Mass DA Off-Peak",
            "ISO-NE W Central Mass DA",
            "ISO-NE W Central Mass DA Off-Peak")
  
  query <- .query_trades(hubs=hubs, strips=strips)
  res <- .filter_archive(mongo, ns, query, asDataFrame=TRUE)

  write.csv(res, file="C:/temp/ice_ne_cal_trades.csv", row.names=FALSE)
  
  xyplot(QUANTITY ~ TIME|STRIP, data=res,
         layout=c(1,2))

}


#################################################################
# Group trades by timestamp
# @trades a data.frame with trades for ONE hub only
# @bucketFun a function to bucket.  Takes timestamp returns a bucket
#
.group_trades_timestamp <- function(trades,
   bucket_fun=function(x) {
     secondsOfDay <- as.numeric(x) - as.numeric(as.POSIXct(format(x, "%Y-%m-%d")))
     secondsOfDay%/%600       # bucket in 5 min intervals 
   })
{
  head(trades)
  trades$day <- as.Date(format(trades$begin_date, "%Y-%m-%d"))

  trades$bucket <- bucket_fun(trades$time) 
 
  res <- ddply(trades, c("day", "bucket"), function(x) {
    data.frame(wap     = sum(x$price*x$quantity)/sum(x$quantity),
               noDeals = nrow(x),
               volume  = sum(x$quantity))
  })
  #browser()
 
  res
}


#################################################################
# How does the price form in time
# @trades a data.frame with trades for ONE hub only
#
.price_formation_nextday <- function(trades)
{
  head(trades)
  trades$day <- as.Date(format(trades$begin_date, "%Y-%m-%d"))
  
  cuts <- list('before9am' = 9*3600,
               'before9:30am' = 9.5*3600,
               'before10am' = 10*3600,
               'before10:30am' = 10.5*3600,
               'allDay' = 24*3600)
  
  res <- ddply(trades, c("day"), function(x) {
    secondsOfDay <- as.numeric(x$time) -
      as.numeric(as.POSIXct(format(x$time, "%Y-%m-%d"))) 
    
    aux <- ldply(cuts, function(cut) {
      ind <- which(secondsOfDay < cut)
      if (length(ind) == 0) {
        data.frame(wap = NA, noDeals=NA, volume=NA)
      } else {
        data.frame(wap     = sum(x$price[ind]*x$quantity[ind])/sum(x$quantity[ind]),
             noDeals = length(ind),
             volume  = sum(x$quantity[ind]))
      }
    })
    #browser()
  })
  

  res
}


#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(rmongodb)
  require(lattice)
  require(reshape2)
  require(SecDb)
  require(zoo)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.mongodb.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Studies/NextDay/ice_liquidity_mongodb.R")

  
  # first update the data using the lib.ice.mongo.R file
  mongo <- mongo.create(host="10.100.101.82")
  ns <- "ice.gas_trades"
  
  #---------------------------------------------- AGT-CG, TGP, DRACUT--------------
  hub <- "TGP-Z6 200L"
  hub <- "Dracut"
  hub <- "AGT-CG"
  aux <- .filter_archive(mongo, ns,
    query=.query_nextday_gas_trades(hub=hub), 
    sort=mongo.bson.from.list(list(TIME=1L))) 
  trades <- .format_ice.gas_trades(aux)                  # custom formatter
  write.csv(trades, file=paste("c:/temp/", hub, ".csv", sep=""), row.names=FALSE)




  #---------------------------------------------- NBP ---------------------
  # products are c("UK Natural Gas Daily Futures", "UK Natural Gas Futures",
  #   "UK Natural Gas TAS", "UK Natural Gas Spr")
  mongo <- mongo.create()
  ns <- "ice.gas_trades"
  mongo.find.one(mongo, ns, .query_nbp_all())
  
  aux <- .filter_archive(mongo, ns,
    query=.query_nbp_all(products=NULL)) 
  trades <- .format_ice.gas_trades(aux)                  # custom formatter
  trades$STRIP <- .fix_strip(trades$STRIP)
  write.csv(trades[,-1], file="c:/temp/nbp.csv", row.names=FALSE)

  table(trades$PRODUCT)
  

  #---------------------------------------------- for Lori --------------------- 
  mongo <- mongo.create()
  ns <- "ice.power_trades"
  query <- .query_ercot_trades()
  aux <- .filter_archive(mongo, ns, query=query,
    sort=mongo.bson.from.list(list(TIME=1L)))
  trades <- .format_ice.gas_trades(aux)  
  write.csv(trades, file="c:/temp/ercot.csv", row.names=FALSE)


  
  x <- table(trades$FILENAME)
  days <- .extract_date_filename(names(x))
  plot(days, as.numeric(x), type="o", col="blue",
       ylab="number of trades per day")

  #---------------------------------------------- all Nepool ------------------

 

  
  x <- table(trades$FILENAME)
  days <- .extract_date_filename(names(x))
  plot(days, as.numeric(x), type="o", col="blue",
       ylab="number of trades per day",
       xlab="",
       main=hub)

  trades$month <- as.Date(format(trades$TIME, "%Y-%m-01"))
  trades$day <- .extract_date_filename(trades$FILENAME)
  volume <- cast(trades, month ~ PRODUCT, sum, fill=0, value="TOTAL_VOLUME")
  print(volume, row.names=FALSE)
  








  

  
  
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
