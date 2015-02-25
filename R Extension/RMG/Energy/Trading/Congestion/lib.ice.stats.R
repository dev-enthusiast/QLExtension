# Do simple statistics with ICE data
# on 10/13/2011 we decided to limit our activity with ICE because of fees
#
# .archive_ICE_data
# .get_gasdeals
# .get_InsideFerc_trades 
# .get_cal_trades
# .get_file_for_day  - get the archive filename given a Date
# .fix_strip         - excel gets confused and reads Feb10 as 2/10/2013!
# .read_file(day, tag)
# .weekly_stats
# .ice_volume_stats  - run the volume analysis 



#######################################################################
# Archive the NEPOOL power data to a csv file
# @param days a vector of R days
# @param filterFun, a function that takes a data.frame of trades
#   and filters it
# @param archivePrefix, what to add to the archive filename
# @save a boolean, should you save it or not?
#
.archive_ICE_data <- function(days, filterFun, archivePrefix="nepool",
                              save=FALSE)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"
  fname <- paste(DIR, "Filtered/", archivePrefix, "_ice_trades.csv", sep="")

  if (file.exists(fname)){
    AA <- read.csv(fname)
  } else {
    AA <- NULL
  }

  uFiles <- unique(AA$filename)
  daysInArchive <- as.Date(gsub("powerdeals_(.*)\\.dat.gz", "\\1", uFiles),
                           "%Y_%m_%d")
  daysToDo <- as.Date(setdiff(days, daysInArchive), origin="1970-01-01")
  
  res <- ldply(daysToDo, function(day) {
    filename <- paste(DIR, "Raw/EOD_Power_Enterprise_CSV/powerdeals_",
      format(day, "%Y_%m_%d"), ".dat.gz", sep="")
    if ( !file.exists(filename) )
      return(NULL)
    rLog("Working on", filename)
    res <- read.csv(filename, sep="|")
    if (nrow(res)==0)
      return(NULL)
    if (ncol(res)==1) {
      res <- read.csv(filename)  # day = as.Date("2012-11-29")
    }
    res$filename <- basename(filename)
    res <- filterFun(res)
    
    res
  })
  
  
  AA <- rbind(AA, res)
  AA <- AA[order(AA$filename),]
  
  if (save) {
    write.csv(AA, file=fname, row.names=FALSE)
    rLog("Wrote", fname)
  }  
}


#######################################################################
# Read the ICE gas data for one day.
# It is a spreadsheet with one sheet and filter it.
# e.g., hub = "AGT-CG", strip="May11"
#
.get_gasdeals <- function(filename, hub=NULL, strip=NULL)
{
  on.exit(odbcCloseAll())
  rLog("Working on ", basename(filename))
  con  <- odbcConnectExcel(filename)
  data <- sqlFetch(con, "gasdeals")
  odbcCloseAll()

  names(data) <- c("region", "hubs", "product", "strips", "begin.date",
    "end.date", "price", "time", "quantity", "total.volume",
    "order.type", "reference.id", "parent.id", "market.id",
    "ngx.volume", "block.trade")
  data <- data[-(1:4),]

  if (!is.null(hub)) 
    data <- subset(data, hubs %in% hub)

  if (!is.null(strip))
    data <- subset(data, strips %in% strip)

  data[,1:16]
}


#######################################################################
# day - an R date
# bucketize trades by bal-year, and then calendar years
#
.get_cal_trades <- function(day, hub="^NEPOOL", removeSpreads=TRUE,
   removeCash=TRUE, removePrompt=TRUE)
{
  fileName <- .get_file_for_day(day)
  rLog("Working on file", basename(fileName))
  if (!file.exists(fileName)) return(NULL)
  aux <- read.delim(.get_file_for_day(day), sep="|")

  if (!is.null(hub)){
    aux <- aux[grepl(toupper(hub), toupper(aux$HUBS)),]
  }  
  
  if (removeSpreads)
    aux <- subset(aux, TOTAL_VOLUME != 0)

  if (removeCash) {
    aux$startMth <- sapply(strsplit(aux$BEGIN_DATE, "/"), "[[", 1)
    aux$endMth   <- sapply(strsplit(aux$END_DATE, "/"), "[[", 1)
    aux$timeMth  <- sapply(strsplit(aux$TIME, "/"), "[[", 1)
    ind <- which((aux$startMth == aux$endMth) & (aux$startMth == aux$timeMth))
    if (length(ind) > 0) aux <- aux[-ind,]
  }

  if (removePrompt) {
    aux$startDt <- as.Date(aux$BEGIN_DATE, "%m/%d/%Y")
    aux$endDt   <- as.Date(aux$END_DATE, "%m/%d/%Y")
    trdDt <- max(as.Date(sapply(strsplit(aux$TIME, " "), "[[", 1), "%m/%d/%Y"))
    cashMth <- as.Date(format(trdDt, "%Y-%m-01"))
    bux <- seq(cashMth, by="1 month", length.out=3)
    promptStart <- bux[2]
    promptEnd   <- bux[3]-1  # end of month
    ind <- which(aux$startDt == promptStart & aux$endDt == promptEnd)
    if (length(ind) > 0) aux <- aux[-ind,]    
  }
  
  startYear <- gsub("[[:digit:]]+/[[:digit:]]+/([[:digit:]]{4})", "\\1",
                    aux$BEGIN_DATE)
  endYear   <- gsub("[[:digit:]]+/[[:digit:]]+/([[:digit:]]{4})", "\\1",
                    aux$END_DATE)
  ## if (any(startYear != endYear)) {
  ##   browser()
  ##   stop("multi calendar trade!")
  ## }

  aux$year <- endYear
  # PRODUCT can be not well specified,
  # "Nepool MH Day-Ahead"  "Nepool MH Day-Ahead  Off-Peak" "Nepool MH Day-Ahead Off-Peak" 
  # "Nepool MH DA" "Nepool MH Real-Time" "Nepool MH Real-Time Off-Peak" 
  
  res <- cast(aux, PRODUCT + year ~ ., sum, value="TOTAL_VOLUME")
  names(res)[ncol(res)] <- "total_volume"
  
  cbind(day=day, res)
}


#######################################################################
# Pick up the trades that fix the Inside Ferc index
#
.get_InsideFerc_trades <- function(month=as.Date("2012-01-01"), hub="AGT-CG")
{
  #browser()
  # pull the filenames for the days you need at the end of the month
  .get_filenames <- function(month) {
    mth <- format(as.Date(month-1), "%Y_%m_")
    files <- list.files(DIR, pattern=paste("EOD_gasenterprise_", mth, sep=""),
                        full.names=TRUE)
    tail(files, 5)  # the bid week
  }

  rLog("Working on month ", format(month))
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/Raw/EOD_Gas_Enterprise"  
  files <- .get_filenames(month)

  strip <- format(month, "%b%y")
  
  #data <- lapply(as.list(files), .get_gasdeals, hub=hub, strip=strip)
  ldply(as.list(files), .get_gasdeals, hub=hub, strip=strip)  
}


#######################################################################
# day - an R date
# commodity = c("power", "gas")
#
.get_file_for_day <- function(day, commodity="power")
{
  BASE_DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"
  if (commodity == "power") {
    paste(BASE_DIR, "Raw/Power/powerdeals_", format(day, "%Y_%m_%d"),
          ".dat.gz", sep="")
  } else if (commodity == "gas") {
    paste(BASE_DIR, "Raw/EOD_Gas_Enterprise/EOD_gasenterprise_", format(day, "%Y_%m_%d"),
          ".xls", sep="")
  } else {
    stop("Only commodity=gas and power are supported!")
  }
}


#######################################################################
# Excel gets confused and reads Feb10 as 2/10/2013!
# so modify Feb10 to 1Feb10
# @param x a vector of strings representing ice terms
#
.fix_strip <- function(x)
{
  ind5 <- nchar(x) == 5
  indMMM <- substr(x, 1, 3) %in% month.abb

  ind <- which(ind5 & indMMM)
  if (length(ind) > 0)
    x[ind] <- paste("1", x[ind], sep="")
  
  x
}


#######################################################################
# Read an ICE file
# @param day an R date
# @param tag a string with the tag.  See ICE$CONFIGURATION name
# @return a data.frame if file exists or NULL otherwise
#
.read_file <- function(day, tag)
{
  filename <- .get_filename(day, toupper(tag))   # from lib.ice.download
  rLog("Working on ", basename(filename))
  if (!file.exists(filename))
    return(NULL)

  #--------------------------------------------------  
  if (toupper(tag) == "POWER_TRADES") {
    res <- read.csv(filename, sep="|")
    if (nrow(res) == 0)
      return(NULL)          # nothing to do for this day 
    if (ncol(res) == 1) {
      res <- read.delim(filename, sep=",")   # 11/29/2012 file is , separated!
    }
    colnames(res)[grepl("REGION$", colnames(res))] <- "REGION"  # you have "ï..REGION" on 7/11/2011
    res <- res[, c("REGION", "HUBS", "PRODUCT", "STRIP", "BEGIN_DATE", "END_DATE",
    "PRICE", "TIME", "QUANTITY", "TOTAL_VOLUME", "ORDER_TYPE", "REFERENCE_ID",
    "PARENT_ID", "MARKET_ID", "NGX_VOLUME", "IS_BLOCK_TRADE" )]

    res$BEGIN_DATE <- strptime(paste(res$BEGIN_DATE, "00:00:00"), format="%m/%d/%Y %H:%M:%S")
    res$END_DATE <- strptime(paste(res$END_DATE, "00:00:00"), format="%m/%d/%Y %H:%M:%S")
    
  #--------------------------------------------------  
  } else if (toupper(tag) == "GAS_TRADES") {
    if (day < as.Date("2014-09-05") ) {
      con  <- odbcConnectExcel(filename)
      data <- sqlFetch(con, "gasdeals")
    } else {
      con  <- odbcConnectExcel2007(filename)
      data <- sqlFetch(con, "deals")
    }
    
    odbcCloseAll()

    names(data) <- c("REGION", "HUBS", "PRODUCT", "STRIP", "BEGIN_DATE", "END_DATE",
      "PRICE", "TIME", "QUANTITY", "TOTAL_VOLUME", "ORDER_TYPE", "REFERENCE_ID",
      "PARENT_ID", "MARKET_ID", "NGX_VOLUME", "IS_BLOCK_TRADE" ) 
    data <- data[-(1:4),]
    res <- data[,1:16]
    
  #--------------------------------------------------  
  } else if (toupper(tag) == "ICECLEARED_GAS") {
    con  <- odbcConnectExcel(filename)
    data <- sqlFetch(con, "Sheet1")
    odbcCloseAll()
    data <- data[-1,]
    
    names(data) <- c("TRADE_DATE", "HUB", "PRODUCT", "STRIP", "CONTRACT",
      "CONTRACT_TYPE", "STRIKE",
      "SETTLEMENT_PRICE", "NET_CHANGE", "EXPIRATION_DATE", "PRODUCT_ID")
    data <- data[which(!is.na(data$TRADE_DATE)),]
    res <- data[,1:11]
  
  } else {
    stop("Unimplemented tag ", tag)
  }
  if (nrow(res) == 0)
    return(NULL)

  if ("TIME" %in% colnames(res)) {
    #convert trade timestamp to POSIXct
    res$TIME <- as.POSIXct(res$TIME, "%m/%d/%Y %I:%M:%S %p", tz="")
  }
  res <- cbind(res, FILENAME=basename(filename))
  
  res
}


#######################################################################
# day - an R date
#
.weekly_stats <- function(day, volume)
{
  week <- format(day, "%W")
  res <- ddply(data.frame(day=day, week=week, volume=volume), .(week),
    function(x){
      data.frame(weekStarting=min(x$day),
        #min=min(x$volume),
        averageDailyVolume=dollar(round(mean(x$volume)), digits=0, do.dollar=FALSE))})
        #max=max(x$volume))})

  res$week <- NULL
  res
}


#######################################################################
# Look at the volume of trades done on ICE
#
.ice_volume_stats <- function(startDate, endDate=Sys.Date(), region="Nepool")
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(lattice)

  if (!(toupper(region) %in% c("NEPOOL", "PJM")))
    stop("Only regions Nepool or PJM are supported")
  
  days <- seq(startDate, endDate, by="1 day")
  BASE_DIR  <<- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"

  res <- vector(mode="list", length=length(days))
  for (i in seq_along(days)) {
    aux <- try(.get_cal_trades(days[i], hub=paste("^", toupper(region), sep=""),
      removeSpreads=TRUE, removeCash=TRUE, removePrompt=TRUE))
    if (class(aux)!="try-error")
      res[[i]] <- aux
  }
  
  resAll <- do.call(rbind, res)

  rLog("\n\nRegion", region)

  # add zeros if there are no transactions for that day 
  aux <- melt(cast(resAll, day ~ year ~ PRODUCT, I, fill=0, value="total_volume"))
  names(aux)[ncol(aux)] <- "total_volume"
  aux$day <- as.Date(aux$day)

  fname <- paste("H:/",tolower(region),"_ice_volume.csv", sep="")
  write.csv(aux, file=fname, row.names=FALSE)
  rLog("Wrote data to", fname)  

  bux <- cast(aux, day ~ ., sum, value="total_volume")
  names(bux)[ncol(bux)] <- "total_volume"
  windows(6,4)
  print(xyplot(total_volume/1000000 ~ day, data=bux, type=c("g", "o"),
         main=paste(region, "trade volumes on ICE, excluding Cash, Prompt"),
         ylab="Total volume, Million MWh"))

  # sorted volumes
  oux <- bux[order(bux$total_volume),]
  windows(6,4)
  plot(1:nrow(oux), oux$total_volume/1000000, col="blue",
       xlab="day", ylab="Daily volume, MMWh",
       main=paste(region, "trade volumes on ICE, excluding Cash, Prompt")) 
  abline(h=axTicks(2), col="gray", lty=1)
  ind <- which(oux$day >= as.Date("2011-10-14"))
  points(ind, oux$total_volume[ind]/1000000, pch=19, col="red")

  rLog("\n\n Lowest 15 volume days:\n")
  print(head(oux, 15), row.names=FALSE)

  rLog("\n\nWeekly volumes:\n")
  wk <- .weekly_stats(bux$day, bux$total_volume)
  print(wk, row.names=FALSE)

  invisible(resAll)
}


#######################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(DivideAndConquer)
  require(lattice)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")

  
  #===================================================================
  # SEE Nepool/Strudies/Gas/lib.iferc.index.formation.R for a study
  # on how the iferc index gets set

  #===================================================================
  # SEE Nepool/Studies/NextDay/ice_liquidity.R for a study
  # on ALG next day, intraday volumes, prices, time of trade, etc.
  # used for discussions with NEPOOL


  
  #===================================================================
  # Nepool Term Power trades filter 
  filterFun <- function(X) {
    aux <- X[grepl("^Nepool MH", X$HUBS),]
    aux <- subset(aux, !(STRIP %in% c("Bal Week", "Next Day",
                                      "Next Week", "Bal Month")))
    aux <- aux[!grepl("Week", aux$STRIP), ]
    aux <- aux[!grepl("Bal ", aux$STRIP), ]
    aux <- aux[!grepl("^Custom ", aux$STRIP), ]

    aux <- aux[, c("HUBS", "PRODUCT", "STRIP", "BEGIN_DATE", "END_DATE",
      "PRICE", "TIME", "QUANTITY", "TOTAL_VOLUME", "ORDER_TYPE", "filename")]    
    aux$STRIP <- .fix_strip(aux$STRIP)
    
    aux
  }
  
  days <- seq(as.Date("2012-11-20"), as.Date("2013-04-10"), by="1 day")
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")
  .archive_ICE_data(days, filterFun, archivePrefix="nepool_term", save=TRUE)



  

  #===================================================================
  # ICE VOLUME ANALYSIS
  #
  startDate <- as.Date("2012-01-01")
  endDate   <- Sys.Date()
  region    <- "Nepool"      # or "PJM"

  .ice_volume_stats(startDate, endDate, region)
  

  bux <- cast(aux, day ~ ., sum, value="total_volume")
  names(bux)[ncol(bux)] <- "total_volume"
  windows(6,4)
  xyplot(total_volume/1000000 ~ day, data=bux, type=c("g", "o"),
         main=paste(region, "trade volumes on ICE, excluding Cash, Prompt"),
         ylab="Total volume, Million MWh")

  # sorted volumes
  oux <- bux[order(bux$total_volume),]
  windows(6,4)
  plot(1:nrow(oux), oux$total_volume/1000000, col="blue",
       xlab="day", ylab="Daily volume, MMWh",
       main=paste(region, "trade volumes on ICE, excluding Cash, Prompt")) 
  abline(h=axTicks(2), col="gray", lty=1)
  ind <- which(oux$day >= as.Date("2011-10-14"))
  points(ind, oux$total_volume[ind]/1000000, pch=19, col="red")
  print(head(oux, 15), row.names=FALSE)

  # weekly volumes
  res <- .weekly_stats(bux$day, bux$total_volume)
  print(res, row.names=FALSE)
  
  xyplot(total_volume ~ day|PRODUCT, data=aux, groups=year,
    type="s", layout=c(1,2),
    auto.key=list(space="top", points=FALSE, lines=TRUE,
      columns=length(unique(aux$year))))

  barchart(total_volume ~ day|PRODUCT, data=aux, groups=year,
    horizontal=FALSE,
    #col=c("red", "blue", "darkgreen", "tomato"),        
    stack=TRUE, layout=c(1,2),
    auto.key=list(space="top", columns=length(unique(aux$year))),
    scales=list(x=list(rot=90)))

  
  
}
