# Keep various utilities needed for shaping
#
# .apply_shape_fwd
# .calc_cal_ratios        - check peak/flat and offpeak/flat ratios
# .calc_shapefactors
# .calc_shapefactors_hour
# .constrain_shapefactors
# .filter_hours
# 


#######################################################################
# return the LMP spread to Hub stacked for all the buckets you want
# for the months you indicate
#
.apply_shape_fwd <- function(spreadLMP, fwdmonths, shape, bucket="PEAK")
{
  spreadLMP$mon  <- as.numeric(format(spreadLMP$month, "%m"))
  spreadLMP$year <- as.numeric(format(spreadLMP$month, "%Y"))
  thisbucket <- bucket
  
  fwdSpread <- subset(spreadLMP, source=="Fwd" &
                      bucket==thisbucket)

  cnames <- c("mon", paste(tolower(thisbucket), ".shape", sep=""))
  mshape <- melt(shape[,cnames], id=1)
  colnames(mshape)[ncol(mshape)] <- "shape"
  mshape$bucket <- toupper(gsub("\\.shape", "", mshape$variable))
  
  aux <- merge(fwdSpread, mshape[,c("mon", "shape", "bucket")],
    all.x=TRUE, by=c("mon", "bucket"))

  # start from the beginning of the current year...
  avg.year <- cast(spreadLMP, year + bucket ~ ., mean, fill=NA,
    subset=month >= as.Date(format(Sys.Date(), "%Y-01-01")), value="spread")
  colnames(avg.year)[ncol(avg.year)] <- "bucket.avg"
  avg.year <- subset(avg.year, bucket!="FLAT")

  aux <- merge(aux, avg.year, all.x=TRUE)
  aux <- aux[order(aux$bucket, aux$month),]

  aux$newspread <- aux$bucket.avg*aux$shape

  return(aux)
}


#######################################################################
# Look at peak/flat and offpeak/flat ratios, hist vs. fwd.
#
.calc_cal_ratios <- function(spreadLMP)
{
  cal <- cast(spreadLMP, year ~ bucket, mean, fill=NA,
    value="spread")
  cal <- cal[,c("year", "PEAK", "OFFPEAK", "FLAT")]
  cal$`PEAK/FLAT`    <- cal$PEAK/cal$FLAT
  cal$`OFFPEAK/FLAT` <- cal$OFFPEAK/cal$FLAT
  
  aux <- melt(data.frame(cal[, c("year", "PEAK/FLAT", "OFFPEAK/FLAT")]),
              id=1)
  aux$variable <- gsub("\\.", "/", aux$variable)
  xyplot(value ~ year | variable, data=aux, scale="free",
    ylab="ratio", type=c("g", "o"))
  
  return(cal)
}


#######################################################################
# return the LMP spread to Hub stacked for all the buckets you want
# for the months you indicate
# If S_{b,y,m} if the spread price for bucket b, year y, month m
#
# shape_{b,m} = \frac{\sum_{y}S_{b,y,m}}{1/12 \sum_{y,m} S_{b,y,m}}
#
# This guarantees normalization the \sum_{m=1}{12} shape_{b,m} = 1.
#
.calc_shapefactors <- function(spreadLMP, months)
{
  if (any(!c("month", "bucket", "spread") %in% names(spreadLMP)))
    stop("Input spreadLMP needs columns month, bucket, spread!")
    
  hspreadLMP <- subset(spreadLMP, month %in% months)

  if (nrow(hspreadLMP)==0)
    stop("No data found after subsetting with months!")
  
  hist.mon <- cast(hspreadLMP, mon ~ bucket, mean, na.rm=TRUE,
    fill=NA, value="spread")
  hist.mon <- data.frame(hist.mon)

  if ("PEAK" %in% names(hist.mon)){
    hist.mon$`peak/flat.ratio` <- hist.mon$PEAK/hist.mon$FLAT  
    hist.mon$peak.shape <- hist.mon$PEAK/mean(hist.mon$PEAK)
  }
  if ("OFFPEAK" %in% names(hist.mon)){
    hist.mon$`offpeak/flat.ratio` <- hist.mon$OFFPEAK/hist.mon$FLAT
    hist.mon$offpeak.shape <- hist.mon$OFFPEAK/mean(hist.mon$OFFPEAK)
  }
  if ("X7X8" %in% names(hist.mon)){
    hist.mon$`X7X8/flat.ratio` <- hist.mon$`X7X8`/hist.mon$FLAT
    hist.mon$`7X8.shape` <- hist.mon$`X7X8`/mean(hist.mon$`X7X8`)
  }
  if ("X2X16H" %in% names(hist.mon)){
    hist.mon$`X2X16H/flat.ratio` <- hist.mon$`X2X16H`/hist.mon$FLAT
    hist.mon$`2X16H.shape` <- hist.mon$`X2X16H`/mean(hist.mon$`X2X16H`)
  }

  fspread <- subset(spreadLMP, source=="Fwd")
  fwd.mon <- cast(fspread, mon ~ bucket, mean, na.rm=TRUE, fill=NA,
    value="spread")
  fwd.mon <- data.frame(fwd.mon)
  if ("PEAK" %in% names(fwd.mon)){
    fwd.mon$`peak/flat.ratio` <- fwd.mon$PEAK/fwd.mon$FLAT  
    fwd.mon$peak.shape <- fwd.mon$PEAK/mean(fwd.mon$PEAK)
  }
  if ("OFFPEAK" %in% names(fwd.mon)){
    fwd.mon$`offpeak/flat.ratio` <- fwd.mon$OFFPEAK/fwd.mon$FLAT
    fwd.mon$offpeak.shape <- fwd.mon$OFFPEAK/mean(fwd.mon$OFFPEAK)
  }
  if ("X7X8" %in% names(fwd.mon)){
    fwd.mon$`X7X8/flat.ratio` <- fwd.mon$`X7X8`/fwd.mon$FLAT
    fwd.mon$`7X8.shape` <- fwd.mon$`X7X8`/mean(fwd.mon$`X7X8`)
  }
  if ("X2X16H" %in% names(fwd.mon)){
    fwd.mon$`X2X16H/flat.ratio` <- fwd.mon$`X2X16H`/fwd.mon$FLAT
    fwd.mon$`2X16H.shape` <- fwd.mon$`X2X16H`/mean(fwd.mon$`X2X16H`)
  }

  aux <- rbind(cbind(hist.mon[,c("mon", "offpeak.shape",
    "peak.shape")], type="hist"), cbind(fwd.mon[,c("mon",
    "offpeak.shape", "peak.shape")], type="fwd"))
  aux <- melt(aux, id=c(1,4))
  aux$type <- factor(aux$type)
  
  print(xyplot(value ~ mon|variable, data=aux, groups=type,
    type="o", scales="free", key=simpleKey(text=levels(aux$type),
    space="top", points=TRUE), ylab="Shape"))
  
  
  return(list(hist.mon, fwd.mon))
}


#######################################################################
# x is an hourly zoo object with data for one day
# problems with DST
# returns a numeric array
#
.calc_hourly_shape <- function(x)
{
  x/mean(x)  # don't like it
}

#######################################################################
# constraint the shape for some months to be the same.
# where shape is a data.frame with columns c("") ??
# For example, Jan-Feb, Jul-Aug.
#
.constrain_shapefactors<- function(shape, constraint)
{
  newshape <- shape

  ind <- grep("\\.shape", colnames(shape))
  
  for (i in ind){
    rLog("Make Jan-Feb the same shape.")
    newshape[1:2, i] <- mean(newshape[1:2, i])

    rLog("Make Jul-Aug the same shape.")
    newshape[7:8,ind] <- mean(newshape[7:8, ind])
  }
  
  return(newshape)
}


#######################################################################
# subset a time series for a given set of hours.
# @param X a data.frame with columns time, value, and optionally HE
# @return a data.frame 
#
.monthly_avg <- function(X, hours)
{
  X$HE  <- as.integer(format(X$time, "%H"))
  XX <- subset(X, HE %in% hours)
  XX$month <- as.Date(format(XX$time-1, "%Y-%m-01"))

  res <- cast(XX, month ~ ., mean)
  colnames(res)[2] <- "value"
  
  res
}


##############################################################################
##############################################################################
#
.main <- function()
{
  require(CEGbase)
  require(SecDb)
  require(lattice)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.shaping.R")


  # what do I need to charge for a special bucket 
  start.dt <- as.POSIXct("2005-06-01 01:00:00")
  end.dt   <- as.POSIXct("2013-05-01 00:00:00")  

  PP <- tsdb.readCurve("NEPOOL_SMD_DA_4000_LMP", start.dt, end.dt)


  x1 <- .monthly_avg(PP, hours=10:22)
  colnames(x1)[2] <- "10-22"
  x2 <- .monthly_avg(PP, hours=8:23)
  colnames(x2)[2] <- "peak"

  X <- merge(x1, x2)
  X$ratio <- X[,"10-22"]/X[,"peak"]
  X$mon <- as.numeric(format(X$month, "%m"))
  
  summary(X$ratio)

  summary(subset(X, mon %in% c(6,7,8))$ratio)  # summer only
  
  


}
