# Check demand bids for problems 
#
# .check_demand_bids
# .get_demand_bids
# .test_01     - day on day totals by zone
# .test_02     - compare nepool fcst vs. ours
# .test_03     - check weekend bidding vs. weekday     
# .send_email
#


#####################################################################
#
.check_demand_bids <- function( asOfDate=Sys.Date()+1, email=FALSE )
{
  DB <- .get_demand_bids(asOfDate)
 
  t1 <- .test_01(asOfDate, DB)
  #t2 <- .test_02(asOfDate, DB)   # DISABLED as symbols are wrong!
  #t3 <- .test_03(asOfDate, DB)

  #allTests <- c(t1, t2, t3)
  allTests <- c(t1)
  
  res <- allTests[seq(2,length(allTests),2)]

  if (email) 
    .send_email(res)

  return(res)
}


#####################################################################
# Test if a value is in an interval
#
in.Interval <- function(x, xLow, xHigh)
{
  ifelse(x >= xLow, TRUE, FALSE) & ifelse(x <= xHigh, TRUE, FALSE)
}


#####################################################################
# Read the demand bids for next day and the previous few days
# @return a data.frame with columns
#    c("datetime", "day", "company", "zone", "value")
#
.get_demand_bids <- function( asOfDate, daysBack=7)
{
  startDt <- as.POSIXct(paste(asOfDate - daysBack, "01:00:00"))
  endDt   <- as.POSIXct(paste(asOfDate + 8, "00:00:00"))
  
  ccg <- get_ccg_shortterm_zonal_load(startDt, endDt,
     oneDayForecast=TRUE, asZoo=TRUE)
  aux <- cbind(datetime=index(ccg), as.data.frame(ccg))
  res1 <- cbind(company="exgn", melt(aux, id=1))

  
  aux <- get_cne_shortterm_zonal_load(startDt, endDt)
  res2 <- cbind(company="cne", melt(aux, id=1))

  res <- rbind(res1, res2)
  res$day <-  as.Date(format(res$datetime - 1, "%Y-%m-%d"))
  colnames(res)[which(colnames(res)=="variable")] <- "zone"
  
  res[, c("datetime", "day", "company", "zone", "value")]
}



#####################################################################
# Compare nextDay with today changes, flag big changes in a zone only
# @return data and an output string
#
.test_01 <- function( asOfDate, DB )
{
  prevDay <- asOfDate - 1
  db <- subset(DB, day %in% c(prevDay, asOfDate))
  tot <- cast(db, company + zone ~ day, sum, fill=0)
  tot$rel.change <- (tot[,ncol(tot)] - tot[,ncol(tot)-1])/tot[,ncol(tot)-1]
  tot <- subset(tot, is.finite(rel.change))

  # calculate the median and the IQR
  res <- cast(tot, company ~ ., function(x){
    c(median(x[is.finite(x)]), IQR(x))}, value="rel.change")
  colnames(res)[2:3] <- c("median", "IQR")
  tot <- merge(tot, res)
  tot$isOutlier <- !in.Interval(tot$rel.change, tot$median-2*pmax(tot$IQR, 0.01),
                                tot$median+2*pmax(tot$IQR,0.01)) 
  aux <- subset(tot, isOutlier)
  rownames(aux) <- NULL
  out <- if (nrow(aux) > 0) {
    aux[,3:4] <- sapply(aux[,3:4], round)
    out <- c("Test 01 (day on day change of total MWh by zone) found outliers:\n",
      capture.output(print(aux, row.names=FALSE)))
  } else {NULL}
  
  return( list(tot, out) )
}


#####################################################################
# Compare nepool next day vs. our demand bids
# Flag if our load increases, pool decreases case (or opposite)
# @return data and an output string
#
.test_02 <- function( asOfDate, DB )
{ 
  prevDay <- asOfDate - 1

  startDt <- as.POSIXct(paste(asOfDate - 1, "01:00:00"))
  endDt   <- as.POSIXct(paste(asOfDate + 1, "00:00:00"))
  nn <- tsdb.readCurve("ISONE_LOADFORECAST_2NDDAY", startDt, endDt)
  nn$day <- as.Date(format(nn$time-1, "%Y-%m-%d"))
  nn <- cast(nn, day ~ ., mean)
  colnames(nn)[2] <- "pool"
  if (!(asOfDate %in% nn$day)) {
    # try the 3rd day out, maybe the forecast hasn't been updated yet
    nn <- tsdb.readCurve("ISONE_LOADFORECAST_3RDDAY", startDt, endDt)
    nn$day <- as.Date(format(nn$time-1, "%Y-%m-%d"))
    nn <- cast(nn, day ~ ., mean)
    colnames(nn)[2] <- "pool"
  } 
  
  db <- subset(DB, day %in% c(prevDay, asOfDate))
  aux <- cast(db, company + day + zone ~ . , mean, fill=0)
  colnames(aux)[ncol(aux)] <- "value"
  tot <- cast(aux, company ~ day, sum, fill=0)  # calculate the total mw by company
  tot$rel.change  <- (tot[,ncol(tot)] - tot[,ncol(tot)-1])/tot[,ncol(tot)-1]
  tot$pool.change <- (nn[2,2]-nn[1,2])/nn[1,2]
  tot$isOutlier <- ifelse( (tot$rel.change*tot$pool.change < 0) &
     abs(tot$rel.change - tot$pool.change)>0.02, TRUE, FALSE)
  
  aux <- subset(tot, isOutlier)
  rownames(aux) <- NULL
  out <- if (nrow(aux) > 0) {
    #aux[,3:4] <- sapply(aux[,3:4], round)
    out <- c("Test 02 (day on day changes of avg MW vs. pool avg MW) found outliers:\n",
      capture.output(print(aux, row.names=FALSE)))
  } else {NULL}
 
  return( list(NULL, out) )
}


#####################################################################
# Flag if change in pool vs. our load is by more than a factor of 10!
# @return data and an output string
#
.test_03 <- function( asOfDate, DB )
{ 
  prevDay <- asOfDate - 1

  startDt <- as.POSIXct(paste(asOfDate - 1, "01:00:00"))
  endDt   <- as.POSIXct(paste(asOfDate + 1, "00:00:00"))
  nn <- tsdb.readCurve("ISONE_LOADFORECAST_2NDDAY", startDt, endDt)
  nn$day <- as.Date(format(nn$time-1, "%Y-%m-%d"))
  nn <- cast(nn, day ~ ., mean)
  colnames(nn)[2] <- "pool"

  db <- subset(DB, day %in% c(prevDay, asOfDate))
  tot <- cast(db, company ~ day , mean, fill=0)
  tot$rel.change  <- (tot[,ncol(tot)] - tot[,ncol(tot)-1])/tot[,ncol(tot)-1]
  tot$pool.change <- (nn[2,2]-nn[1,2])/nn[1,2]
  tot$isOutlier <- !in.Interval(abs(tot$rel.change/tot$pool.change), 0.03, 30)
  
  aux <- subset(tot, isOutlier)
  rownames(aux) <- NULL
  out <- if (nrow(aux) > 0) {
    aux[,3:4] <- sapply(aux[,3:4], round)
    out <- c("Test 03 (day on day changes of avg MW vs. pool avg MW different by a factor of 10) :\n",
      capture.output(print(aux, row.names=FALSE)))
  } else {NULL}
 
  return( list(NULL, out) )
}



#####################################################################
# Send email
# @param res, the output results of the tests as a list (the second
#   component from each test, the text output) 
#
.send_email <- function( res )
{
  out <- unlist(lapply(res, function(x){c(x,"")}))

  if ( any(out != "") ) {
    txt <- c(out, "", "", 
      "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/nepool_check_demandbids", 
      "Contact: Adrian Dragulescu")
    rLog("Email results")
    to <- paste(c("CCGLoadForecastSupport@constellation.com", "peng.wu2@constellation.com", 
      "adrian.dragulescu@constellation.com", "kate.norman@constellation.com"),
      sep="", collapse=",")
    #to <- "adrian.dragulescu@constellation.com"
  
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
      to,
      paste("FLAGGED Nepool demand bids for", format(asOfDate)),
      txt)
  }
}


#####################################################################
#####################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(zoo)
  require(reshape)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.cne.loads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.check.demandbids.R")
 
  
  asOfDate <- Sys.Date()+1
  res <- .check_demand_bids( asOfDate )
  res

  
  
}


