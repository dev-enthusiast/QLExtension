# Utilities to deal with nepool ARRs, PnL etc. 
# See spreadsheet 
# 
#
# calc_pnl_impact_arr
# .get_iarr_dollars       <-- incremental ARRs
# .get_arr_dollars        <-- for building marks
# .get_peakLoad_avg       
# get_positions_arr
# .make_fwd_marks_arr
# .pull_hist_arr
# .plot_arr
# .pull_marks_arr
#
#

################################################################
# Calculate PnL impact on marks change
#
calc_pnl_impact_arr <- function(day1, day2, 
   endDate=as.Date("2018-12-01"))
{
  books <- PM:::secdb.getBooksFromPortfolio( "NSARR1" )
  qqA   <- cbind(company="ccg", get_positions_arr(day1, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_arr(day1, books))
  qq1   <- rbind(qqA, qqB)
  qq1   <- subset(qq1, month >= nextMonth())
  
  books <- PM:::secdb.getBooksFromPortfolio( "NEPOOL Load Portfolio" )
  qqA   <- cbind(company="ccg", get_positions_rmr(day2, books))
  books <- PM:::secdb.getBooksFromPortfolio( "CNE NewEngland Load Portfolio" )
  qqB   <- cbind(company="cne", get_positions_rmr(day2, books))
  qq2   <- rbind(qqA, qqB)
  qq2   <- subset(qq2, month >= nextMonth())

  
  PP1 <- .pull_marks_arr(day1,  startDate=day1, endDate)
  PP1$asOfDate <- NULL
  
  PP2 <- .pull_marks_arr(day2, startDate=day2, endDate)
  PP2$asOfDate <- NULL
  #PP2 <- NN
  
  aux <- calc_pnl_pq(PP1, PP2, qq1, qq2,
    groups=c("company", "year", "location"))  # from lib.pnl.R
  gPnL <- aux[[2]]
  cast(gPnL, company + year ~ location, I, fill=0, value="PnL")
  
  
  rLog("Total PnL change is", sum(gPnL$PnL))
  
  aux
}


#################################################################
# 
# @param month a vector of R bom dates
# @param 
#
.get_iarr_dollars <- function(month)
{
  reportName <- "SR_ARRDISTR"; tab <- 3
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
  }))
  AA <- .archive_report(fnames, tab=tab, save=TRUE)

  AA <- .load_archive(reportName, tab, onlyLatest=TRUE)

  cast(AA, reportDay ~ ., sum,
    value="Share.of.Incremental.Auction.Revenue.Dollars",
    subset=Upgrade.Project.Name == "Sithe Fore River")
  
}



#################################################################
# 
# @param HH the historical data as returned by .pull_hist_arr
# @param FF the fwd marks, as returned by .pull_marks_arr
#
.get_arr_dollars <- function(HH, FF)
{
  # dollars by year, bucket
  ind <- grep("_\\$", names(HH))
  YY <- aggregate(HH[,ind], as.numeric(format(index(HH), "%Y")), mean)
  YY <- cbind(year=index(YY), as.data.frame(YY))
  YY <- melt(YY, id=1)
  YY$ptid <- as.numeric(gsub("([[:digit:]]{4})_.*", "\\1", YY$variable))
  YY$bucket <- factor(gsub("[[:digit:]]{4}_.*_(.*)_.*", "\\1", YY$variable),
                      levels=c("on", "off"))
  aux <- cast(YY, year ~ ptid, sum)  # add the yr + mth payouts
  print(aux, row.names=FALSE)
  rLog("Paste it in the Ancillary Services/ARR Valuation/2013-03-27_remark_ARRs.xlsx")

  
  # dollars implied from historicals
  hh <- HH[,c("4001_dollars", "4002_dollars", "4003_dollars",
              "4004_dollars", "4005_dollars", "4006_dollars",
              "4007_dollars", "4008_dollars")]
  hh <- cbind(month=index(hh), as.data.frame(hh))
  colnames(hh) <- c("month", "MAINE", "NH", "VT", "CT", "RI", "SEMA",
                    "WMA", "NEMA")
  rownames(hh) <- NULL
  
  # dollars implied from the marks
  LL  <- PM:::.read.peak.load.forecast()
  colnames(LL)[2] <- "location"
  aux <- merge(FF[,c("location", "month", "value")], LL,
               all.x=TRUE, by=c("location", "month"))
  aux$value <- aux$value * aux$PeakLoad
  ff <- cast(aux, month ~ location, I)

  rbind(hh, ff)
}


#################################################################
# Average the historical peak load to estimate the future peak load
# I don't like the values I got from Ling a while ago
# see PM:::.read.peak.load.forecast().  For example CT is too low. 
#
.get_peakLoad_avg <- function(HH)
{
  LL <- HH[ ,grep("pkLoad", colnames(HH))]   # hist coincident peak load
  LL <- cbind(month=index(LL), as.data.frame(LL))
  LL <- melt(LL, id=1)
  LL$ptid <- as.numeric(substring(LL$variable, 1, 4))
  
  ZZ <- data.frame(
    ptid = 4001:4008,
    location = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"),
    zone = c("ME", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
  LL <- merge(LL[,c("month", "ptid", "value")], ZZ[,c("ptid", "location")])
  LL$ptid <- NULL
  LL$mon <- as.numeric(format(LL$month, "%m"))
  
  xyplot(value ~ mon|location, data=LL, type=c("g", "p"),
    xlab="Month of year",
    ylab="Peak load, MW"     
  )

  # weather normalized load as a simple average 
  wnPeakLoad <- cast(LL, location + mon ~ ., mean)
  colnames(wnPeakLoad)[3] <- "peakLoad"
 
  wnPeakLoad
}


#################################################################
# get ccg ARR positions.  
#
get_positions_arr <- function(asOfDate, books)
{
  QQ <- get.positions.from.blackbird(asOfDate=asOfDate, books=books,
    fix=FALSE, service="ARR")

  qq <- cast(QQ, contract.dt + delivery.point ~ ., sum, fill=NA,
             value="notional")
  names(qq) <- c("month", "location", "value")

  return(qq)
}


################################################################
# Construct the fwd arr marks ...
#
.make_fwd_marks_arr <- function(HH)
{
  require(xlsx)
  DIR <- "S:/All/Structured Risk/NEPOOL/Ancillary Services/ARR Valuation/"
  files <- list.files(DIR, pattern=".*_remark_ARRs.xlsx", full.names=TRUE)
  filename <- tail(sort(files),1)
  #fwd <- read.xlsx2(filename, sheetName="Dollars", startRow=2)
  rLog("Reading", filename)
  fwd <- read.xlsx(filename, sheetName="Dollars")
  colnames(fwd) <- fwd[1,]
  fwd <- fwd[-1,]
  fwd$year <- as.numeric(fwd$year)
  fwd <- fwd[!is.na(fwd$year),]
  aux <- melt(fwd, id=1:2)
  aux <- subset(aux, type == "fwd")
  aux$ptid <- as.numeric(as.character(aux$variable))
  aux$variable <- NULL
  months <- seq(nextMonth(), as.Date(paste(max(aux$year), "-12-01", sep="")),
    by="1 month")
  FF <- merge(aux, data.frame(month=months,
                              year=as.numeric(format(months, "%Y"))))
  FF$year <- FF$type <- NULL
  
  # add the zones
  ZZ <- data.frame(
    ptid = 4001:4008,
    location = c("MAINE", "NH", "VT", "CT", "RI", "SEMA", "WMA", "NEMA"))
  FF <- merge(FF, ZZ, by="ptid")
  FF$dollars <- FF$value
  FF$value <- NULL
  FF$mon <- as.numeric(format(months, "%m"))
  FF$ptid <- NULL
  
  wnPeakLoad <- .get_peakLoad_avg(HH)
  res <- merge(FF, wnPeakLoad, by=c("mon", "location"))
  res$value <- -res$dollars/res$peakLoad
  res <- res[order(res$location, res$month),]
  
  res[,c("month", "location", "value")]
}


################################################################
# pull hist data 
#
.pull_hist_arr <- function(start.dt=as.Date("2008-01-01"),
  end.dt=Sys.Date())
{
  symbs <- matrix(c(
    "ne_arrawd_yr_on_4001_zlsd",   "4001_yr_on_$",
    "ne_arrawd_yr_off_4001_zlsd",  "4001_yr_off_$",
    "ne_arrawd_mth_on_4001_zlsd",  "4001_mth_on_$",
    "ne_arrawd_mth_off_4001_zlsd", "4001_mth_off_$",
                    
    "ne_arrawd_yr_on_4002_zlsd",   "4002_yr_on_$",
    "ne_arrawd_yr_off_4002_zlsd",  "4002_yr_off_$",
    "ne_arrawd_mth_on_4002_zlsd",  "4002_mth_on_$",
    "ne_arrawd_mth_off_4002_zlsd", "4002_mth_off_$",
                    
    "ne_arrawd_yr_on_4003_zlsd",   "4003_yr_on_$",
    "ne_arrawd_yr_off_4003_zlsd",  "4003_yr_off_$",
    "ne_arrawd_mth_on_4003_zlsd",  "4003_mth_on_$",
    "ne_arrawd_mth_off_4003_zlsd", "4003_mth_off_$",

    "ne_arrawd_yr_on_4004_zlsd",   "4004_yr_on_$",
    "ne_arrawd_yr_off_4004_zlsd",  "4004_yr_off_$",
    "ne_arrawd_mth_on_4004_zlsd",  "4004_mth_on_$",
    "ne_arrawd_mth_off_4004_zlsd", "4004_mth_off_$",

    "ne_arrawd_yr_on_4005_zlsd",   "4005_yr_on_$",
    "ne_arrawd_yr_off_4005_zlsd",  "4005_yr_off_$",
    "ne_arrawd_mth_on_4005_zlsd",  "4005_mth_on_$",
    "ne_arrawd_mth_off_4005_zlsd", "4005_mth_off_$",

    "ne_arrawd_yr_on_4006_zlsd",   "4006_yr_on_$",
    "ne_arrawd_yr_off_4006_zlsd",  "4006_yr_off_$",
    "ne_arrawd_mth_on_4006_zlsd",  "4006_mth_on_$",
    "ne_arrawd_mth_off_4006_zlsd", "4006_mth_off_$",

    "ne_arrawd_yr_on_4007_zlsd",   "4007_yr_on_$",
    "ne_arrawd_yr_off_4007_zlsd",  "4007_yr_off_$",
    "ne_arrawd_mth_on_4007_zlsd",  "4007_mth_on_$",
    "ne_arrawd_mth_off_4007_zlsd", "4007_mth_off_$",

    "ne_arrawd_yr_on_4008_zlsd",   "4008_yr_on_$",
    "ne_arrawd_yr_off_4008_zlsd",  "4008_yr_off_$",
    "ne_arrawd_mth_on_4008_zlsd",  "4008_mth_on_$",
    "ne_arrawd_mth_off_4008_zlsd", "4008_mth_off_$",
                     
    "ne_arrawd_mth_on_4001_zaphl",  "4001_pkLoad",
    "ne_arrawd_mth_on_4002_zaphl",  "4002_pkLoad",
    "ne_arrawd_mth_on_4003_zaphl",  "4003_pkLoad",
    "ne_arrawd_mth_on_4004_zaphl",  "4004_pkLoad",
    "ne_arrawd_mth_on_4005_zaphl",  "4005_pkLoad",
    "ne_arrawd_mth_on_4006_zaphl",  "4006_pkLoad",
    "ne_arrawd_mth_on_4007_zaphl",  "4007_pkLoad", 
    "ne_arrawd_mth_on_4008_zaphl",  "4008_pkLoad"     # adjusted peak hourly load!
   ), ncol=2, byrow=TRUE)
   colnames(symbs) <- c("symbol", "name")

  HH <- FTR:::FTR.load.tsdb.symbols(symbs[,"symbol"],
    start.dt=start.dt, end.dt=end.dt)
  names(HH) <- symbs[, "name"]

  HH$`4001_dollars` <- (HH$`4001_yr_on_$` + HH$`4001_yr_off_$` +
                     HH$`4001_mth_on_$` + HH$`4001_mth_off_$`)
  HH$`4002_dollars` <- (HH$`4002_yr_on_$` + HH$`4002_yr_off_$` +
                     HH$`4002_mth_on_$` + HH$`4002_mth_off_$`)
  HH$`4003_dollars` <- (HH$`4003_yr_on_$` + HH$`4003_yr_off_$` +
                     HH$`4003_mth_on_$` + HH$`4003_mth_off_$`)
  HH$`4004_dollars` <- (HH$`4004_yr_on_$` + HH$`4004_yr_off_$` +
                     HH$`4004_mth_on_$` + HH$`4004_mth_off_$`)
  HH$`4005_dollars` <- (HH$`4005_yr_on_$` + HH$`4005_yr_off_$` +
                     HH$`4005_mth_on_$` + HH$`4005_mth_off_$`)
  HH$`4006_dollars` <- (HH$`4006_yr_on_$` + HH$`4006_yr_off_$` +
                     HH$`4006_mth_on_$` + HH$`4006_mth_off_$`)
  HH$`4007_dollars` <- (HH$`4007_yr_on_$` + HH$`4007_yr_off_$` +
                     HH$`4007_mth_on_$` + HH$`4007_mth_off_$`)
  HH$`4008_dollars` <- (HH$`4008_yr_on_$` + HH$`4008_yr_off_$` +
                     HH$`4008_mth_on_$` + HH$`4008_mth_off_$`)

  # ARRs in $/peakMW-month
  HH$`4001_arr` <- -HH$`4001_dollars`/HH$`4001_pkLoad`
  HH$`4002_arr` <- -HH$`4002_dollars`/HH$`4002_pkLoad`
  HH$`4003_arr` <- -HH$`4003_dollars`/HH$`4003_pkLoad`
  HH$`4004_arr` <- -HH$`4004_dollars`/HH$`4004_pkLoad`
  HH$`4005_arr` <- -HH$`4005_dollars`/HH$`4005_pkLoad`
  HH$`4006_arr` <- -HH$`4006_dollars`/HH$`4006_pkLoad`
  HH$`4007_arr` <- -HH$`4007_dollars`/HH$`4007_pkLoad`
  HH$`4008_arr` <- -HH$`4008_dollars`/HH$`4008_pkLoad`


  HH
}


################################################################
# plot ARR hist and marks side by side
# @param HH the historical data as returned by .pull_hist_arr
# @param FF the fwd marks, as returned by .pull_marks_arr
#
#
.plot_arr <- function( HH, FF, maxDate=as.Date("2018-12-01"))
{
  require(lattice)
  aux <- data.frame(month=index(HH), MAINE=HH$`4001_arr`,
    NH=HH$`4002_arr`, VT=HH$`4003_arr`, CT=HH$`4004_arr`,
    RI=HH$`4005_arr`, SEMA=HH$`4006_arr`, WMA=HH$`4007_arr`, NEMA=HH$`4004_arr`)

  aux <- melt(aux, id=1)
  colnames(aux) <- c("month", "location", "value")

  DD <- rbind(aux, FF[,c("month", "location", "value")])
  DD <- subset(DD, month <= maxDate)
  DD$location <- as.character( DD$location)
  
  print(xyplot(value ~ month,
    data=subset(DD, location %in% c("MAINE", "NH", "CT", "RI")),
    groups=location, type=c("g", "l"),
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=4),
    xlab="",      
    ylab="ARR, $/peakMW-month"))

  windows()
  print(xyplot(value ~ month,
    data=subset(DD, location %in% c("SEMA", "WMA", "NEMA")), 
    groups=location, type=c("g","l"),
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=3),
    xlab="",      
    ylab="ARR, $/peakMW-month"))

  
}


################################################################
# What we mark in SecDb $/peakMW-month
#
.pull_marks_arr <- function(asOfDate=Sys.Date(),
  startDate=nextMonth(asOfDate),
  endDate=as.Date("2019-12-01"), 
  zones=c("NEMA", "SEMA", "WMA", "MAINE", "NH", "RI", "CT", "VT"))
{
  location    <- zones
  bucket      <- c("FLAT")
  serviceType <- c("ARR")
  commodity   <- "COMMOD PWR NEPOOL PHYSICAL"
  fwd <- PM:::secdb.getElecPrices( asOfDate, startDate, endDate,
    commodity, location, bucket, serviceType, useFutDates=FALSE)

  
  fwd
}



#########################################################################
#########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)
  require(PM)
  require(lattice)
  
  source("H:/user/R/RMG/Utilities/Database/VCentral/get.positions.from.blackbird.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.positions.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.pnl.R")  
  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Ancillaries/lib.arr.R")

  HH <- .pull_hist_arr(start.dt=as.Date("2008-01-01"))
  FF <- .pull_marks_arr()
  .plot_arr(HH, FF)

  .get_arr_dollars(HH, FF)

  # current marks
  print(cast(FF,  month ~ location, I, fill=NA, value="value"), row.names=FALSE)
  # proposed marks
  NN <- .make_fwd_marks_arr(HH)
  aux <- cast(NN,  month ~ location, I, fill=NA, value="value") 
  print(aux, row.names=FALSE)
  #write.csv(aux, file="S:/All/Structured Risk/NEPOOL/Ancillary Services/ARR Valuation/Sent/2013-03-27_arr_marks.csv", row.names=FALSE)

  
  
  asOfDate  <- Sys.Date()-1
  portfolio <- "Nepool Load Portfolio"
  portfolio <- "PM East Retail Portfolio"
  QQ <- get_positions_arr( asOfDate )

  res <- cast(QQ, month ~ service, I, fill=0, value="qty")
  print(res, row.names=FALSE)  # put into spreadsheet
  

  day1 <- as.Date("2013-03-26")
  day2 <- as.Date("2013-03-27")
  calc_pnl_impact( as.Date("2012-05-15"), as.Date("2012-05-16"),
     portfolio=portfolio )


  #################################################################
  # get historical IARRs
  #
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  reportName <- "SR_ARRDISTR"; tab <- 3
  
  months <- seq(as.Date("2013-01-01"), nextMonth(), by="1 month")
  
  months <- as.Date("2014-01-01")
  fnames <- unlist(sapply(months, function(m){
    .get_report_csvfilenames(org="EXGN", month=m, reportName=reportName)
  }))

  

  
}





  ## filter <- function(x){ x$`Service Type` == "ARR" }  
  ## res <- lapply(as.list(books), function(x){
  ##   secdb.flatReport(asOfDate, x, filter=filter, to.df=TRUE)
  ## })
  ## res <- do.call(rbind, res)



## #################################################################
## # get the monthly actuals in $/peakMW-month, so I can 
## #
## .monthly_actuals_rmr <- function(asOfDate, byZone=TRUE)
## {
##   rLog("Pulling CNE ARR positions for:", as.character(asOfDate))
##   pName <- "CNE NewEngland Load Portfolio"              
##   books <- .getPortfolioBooksFromSecDb( pName )

##   QQ <- get.positions.from.blackbird(asOfDate=asOfDate, books=books,
##     fix=FALSE, service="ARR")
##   QQ$notional <- QQ$volume      # I don't know what this is
##   QQ$quantity <- QQ$volume

##   # aggregate the positions by zone
##   if (byZone){
##     aux <- cast(QQ, contract.dt + delivery.point ~ ., sum, value="notional")
    
##   }
  
##   return(QQ)
## }
