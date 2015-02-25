# Deal with capacity prices in SecDb
#
# calc_pnl_impact
# get_ucap_position
# .pull_marks_capacity
# 
#


################################################################
# get_ucap_position
# asOfDate needs to be an archive date!
#
calc_pnl_impact_capacity <- function(day1, day2, endDate=as.Date("2020-12-01"))
{
  qqA <- cbind(company="ccg.load", get_capacity_position_load(day1, company="CCG"))
  qqB <- cbind(company="cne.load", get_capacity_position_load(day1, company="CNE"))
  qq1 <- rbind(qqA, qqB)
  qq1 <- subset(qq1, month >= nextMonth())
  colnames(qq1)[which(colnames(qq1) == "delta")] <- "value"

  qqA <- cbind(company="ccg.load", get_capacity_position_load(day2, company="CCG"))
  qqB <- cbind(company="cne.load", get_capacity_position_load(day2, company="CNE"))
  qq2 <- rbind(qqA, qqB)
  qq2 <- subset(qq2, month >= nextMonth())
  colnames(qq2)[which(colnames(qq2) == "delta")] <- "value"
 
  PP1 <- .pull_marks_capacity(day1, day1, endDate, service="UCAP",
    location=c("ROP", "CT", "NEMA", "MAINE"))
  PP1$asOfDate <- NULL
  PP1$location[which(PP1$location == "MAINE")] <- "ME"

  PP2 <- .pull_marks_capacity(day2, day2, endDate, service="UCAP",
    location=c("ROP", "CT", "NEMA", "MAINE"))
  PP2$asOfDate <- NULL
  PP2$location[which(PP2$location == "MAINE")] <- "ME"
  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]

  rLog("Total PnL change is", sum(gPnL$PnL))
 
  aux
}


################################################################
# from prism
#
get_capacity_positions <- function( asOfDate, books, portfolio=NULL,
  serviceType=c("UCAP", "PCAP") )
{
  qq <- get.positions.from.blackbird( asOfDate=asOfDate, 
    fix=FALSE, service=c("UCAP", "PCAP"), books=books )
  qq <- qq[,c("trading.book", "load.name", "service", "contract.dt",
    "notional")]
  qq <- subset(qq, contract.dt >= nextMonth())
  qq <- cast(qq, trading.book + load.name + service +
    contract.dt ~ ., sum, fill=0, value="notional")
  colnames(qq)[ncol(qq)] <- "notional"
  
  # add the hours, just for kicks
  b1 <- secdb.getBucketMask( "nepool", "7x24",
    as.POSIXct(paste(format(min(qq$contract.dt)), "01:00:00")),
    as.POSIXct(paste(format(nextMonth(max(qq$contract.dt))), " 00:00:00")))
  b1$contract.dt <- as.Date(format(b1$time-1, "%Y-%m-01"))
  hrs <- data.frame(cast(b1, contract.dt ~ ., sum, value="7x24"))
  colnames(hrs)[2] <- "hours"

  qq <- merge(qq, hrs, by="contract.dt")
  qq <- qq[order(qq$service, qq$load.name, qq$contract.dt), ]
  
  qq
}


################################################################
# get_ucap_position from SecDb (it's faster)
# asOfDate needs to be an archive date!
# 
#
get_capacity_position_load <- function(day1, company=c("CCG", "CNE"))
{
  if (company=="CCG") {
    pfolio <- "nepool load portfolio"
  } else if (company == "CNE") {
    pfolio <- "cne newengland load portfolio"
  } else {
    stop("Unknown company ", company)
  }
  
  rLog("Read positions from:", pfolio)
  qq <- secdb.readDeltas(day1, pfolio)
  qq <- qq[grep("PWX UCAP ", names(qq))]   # pick only the UCAP curves

  aux <- mapply(function(x,y){
    cbind(curve=x, y)
  }, names(qq), qq, SIMPLIFY=FALSE)
  aux <- do.call(rbind, aux)
  rownames(aux) <- NULL
  
  aux <- cbind(aux,
    location=PM:::classifyCurveName(aux$curve, by=c("location"))$location)
  names(aux)[c(2:3)] <- c("month", "delta")

  aux
}


################################################################
# git them from secdb
#
.pull_marks_capacity <- function(asOfDate=Sys.Date(), startDate=nextMonth(),
  endDate=NULL, service=c("UCAP", "RTEG", "PCAP"),
  location=c("ROP", "CT", "MAINE", "NEMA"))
{
  if (is.null(endDate))
    endDate <- as.Date(paste(as.numeric(format(asOfDate, "%Y"))+7, "-12-01", sep=""))
  
  rLog("Pull capacity NEPOOL ...")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  useFutDates <- FALSE
  location    <- location
  bucket      <- c("FLAT")
  serviceType <- service
  CC <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates)
  
  CC
}




################################################################
################################################################
# 
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(reshape)

  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Capacity/lib.capacity.PnL.R")

  asOfDate  <- Sys.Date()
  startDate <- nextMonth()
 
  PP <- .pull_marks_capacity(asOfDate, startDate,
    service=c("UCAP", "PCAP"), location=c("ROP", "NEMA", "CT", "MAINE"))
  colnames(PP)[4:6] <- c("service", "contract.dt", "price")
  
  cast(PP, contract.dt ~ location, I, fill=NA, value="price",
       subset=service=="UCAP" & contract.dt >= as.Date("2016-06-01"))

  
  require(lattice)
  # plot UCAP marks
  xyplot(price ~ contract.dt, data=PP, groups=PP$location,
    type=c("g","o"), ylab="UCAP Price, $/Mwh", xlab="", 
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(PP$location))),          
    subset=contract.dt <= as.Date("2020-05-01") & service == "UCAP")
  
  PPP <- cast(PP, contract.dt ~ service + location, I, fill=NA,
    value="price", subset= service == "UCAP" & contract.dt >= as.Date("2017-06-01"))
  print(PPP, row.names=FALSE)
  

  # for cne see 
  # S:\All\Structured Risk\NEPOOL\ConstellationNewEnergy\Capacity\HedgeCosts
  
  
  #============================================================
  ucap <- .pull_marks_capacity(asOfDate, nextMonth(), as.Date("2021-01-01"),
    service="UCAP", location=c("ROP"))

  hrs <- PM:::ntplot.bucketHours("FLAT", nextMonth(), as.Date("2021-01-01"),
    region="NEPOOL", period="Month")
  colnames(hrs)[2] <- "month"
  

  
  b1 <- secdb.getBucketMask( "nepool", "7x24",
    as.POSIXct("2012-04-01 01:00:00"),
    as.POSIXct("2021-01-01 00:00:00") )
  b1$month <- as.Date(format(b1$time-1, "%Y-%m-01"))
  hrs <- data.frame(cast(b1, month ~ ., sum, value="7x24"))
  colnames(hrs)[2] <- "hours"
  
  ucap <- merge(ucap, hrs)
  
  print(ucap, row.names=FALSE)


  print(subset(ucap, month >= as.Date("2015-06-01")), row.names=FALSE)


  ######################################################################
  # change in positions?
  day1 <- as.Date("2014-06-13")
  day2 <- as.Date("2014-06-16")
  company <- "CNE"
  #company <- "CCG"
  QQ1 <- get_capacity_position_load(day1, company)
  QQ2 <- get_capacity_position_load(day2, company)
  diff <- merge(QQ1, QQ2, by=c("curve", "month", "location"))
  diff$diff <- diff$delta.y - diff$delta.x
  head(diff)

  

  .get_positions <- function(asOfDate)
  {
    books <- c("MYSTC89M")
    qq <- get_capacity_positions(asOfDate, books)
    qq <- cbind(asOfDate=asOfDate,
          subset(qq, service == "UCAP" &
                 contract.dt >= as.Date("2014-06-01") & contract.dt <= as.Date("2015-05-01") ))
    qq[order(qq$contract.dt),]
  }
  
  q1 <- .get_positions(asOfDate=as.Date("2014-03-26"))  # wrong positions
  q2 <- .get_positions(asOfDate=as.Date("2013-03-26"))  # wrong positions
  q3 <- .get_positions(asOfDate=as.Date("2012-03-26"))  # good positions
  q4 <- .get_positions(asOfDate=as.Date("2012-07-02"))  # wrong positions
  q5 <- .get_positions(asOfDate=as.Date("2012-05-15"))  # good positions
  q6 <- .get_positions(asOfDate=as.Date("2012-06-15"))  # good positions
  q7 <- .get_positions(asOfDate=as.Date("2012-06-29"))  # wrong positions <-- when it happened!


  
  #========================================================
  # calculate the PnL once you change the marks
  # and don't forget to update the RTEG marks too!
  #
  asOfDate  <- Sys.Date()-1
  books <- c("MYSTC89M")
  qq <- get_capacity_positions(asOfDate, books)
  subset(qq, service == "UCAP" & 
         contract.dt >= as.Date("2014-06-01") & contract.dt <= as.Date("2015-05-01") )

  

  asOfDate  <- Sys.Date()-1
  books <- c("MYSTIC7M", "FORERIVM", "MYSTC89M")
  qq <- get_capacity_positions(asOfDate, books)
  

  res <- NULL
  for (day1 in c("2014-06-13", "2014-06-16", "2014-06-17", "2014-06-18")) {
    res[[day1]] <- cbind(asOfDate= as.Date(day1),
                         get_capacity_position_load(as.Date(day1), company="CNE"))
  }
  res <- do.call(rbind, res)
  print(res, row.names=FALSE) 



  #========================================================
  # change in PnL
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")
  day1 <- as.Date("2014-06-13")
  day2 <- as.Date("2014-06-16")

  res <- calc_pnl_impact_capacity(day1, day2)  
  print(cast(res$gPnL, company + location ~ year,
       function(x){round(x)}, fill=0, value="PnL"), row.names=FALSE)
  
  print(cast(res$gPnL, company ~ year, subset=company=="cne.load", 
       function(x){round(sum(x))}, fill=0, value="PnL"), row.names=FALSE)


  
  #========================================================
  # get the marks
  #
  asOfDate  <- Sys.Date()
  startDate <- as.Date("2012-04-01")
  endDate   <- as.Date("2020-12-01")

  ucap <- .pull_marks_capacity(asOfDate, startDate, endDate,
    service="UCAP", location=c("ROP", "MAINE", "CT", "NEMA"))
  print(cast(ucap, month ~ location, I, fill=NA), row.names=FALSE)
  
  rteg <- .pull_marks_capacity(asOfDate, startDate, endDate,
    service="RTEG", location=c("ROP", "MAINE", "CT", "NEMA"))
  print(cast(rteg, month ~ location, I, fill=NA), row.names=FALSE)
         
  pcap <- .pull_marks_capacity(asOfDate, startDate, endDate,
    service="PCAP", location=c("ROP", "MAINE", "CT", "NEMA"))
  pcap <- cast(pcap, month ~ location, I, fill=NA)
  print(pcap, row.names=FALSE)
  


  HRS <- FTR:::FTR.bucketHours("NEPOOL", end.dt=as.POSIXlt("2017-12-31 23:00:00"))
  names(HRS)[1] <- "month"
  HRS <- subset(HRS, class.type=="FLAT" & month >= Sys.Date() )
  print(HRS[,c("month", "hours")], row.names=FALSE)
  
  res <- merge(HRS[,c("month", "hours")], pcap[,c("month", "ROP")], all.x=TRUE)
  print(res, row.names=FALSE)
  

  

  
}
