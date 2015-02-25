# Given an hourly time series, bucketize it. 
# Where data is a data frame with column names: year, month, day, hour, symbol 
#       res  is a data frame with column names: year, month, bucket
# Possible buckets are "daily", "monthly", "onpeak", "offpeak",
#                      "5x16", "2x16", "7x8"
#
# Last modified by Adrian Dragulescu on 2-Nov-2004

bucket.ts <- function(data, bucket){

  source("H:/user/R/RMG/TimeFunctions/select.onpeak.R")
  res=NULL
  meanFun  <- function(x){mean(x, na.rm=TRUE)}
  whichCol <- function(x){which(colnames(data)==x, arr.ind=TRUE)}

  #--------------------------------------------------------------
  if (bucket=="daily"){
    days <- as.Date(paste(data$year,data$month,data$day, sep="-"))
    ind  <- sapply(c("year","month","day","hour"), whichCol)
    realData <- as.data.frame(data[, -ind])
    colnames(realData) <- colnames(data)[-ind]
    for (c in 1:ncol(realData)){
      aux  <- tapply(realData[,c], days, mean, na.rm=TRUE)
      ind  <- which(aux=="NaN")
      if (length(ind)>0){aux[ind] <- NA}
      res  <- cbind(res, aux) 
    }
    colnames(res) <- colnames(realData)
    uDays <- unique(days)
    res  <- data.frame(year  = as.numeric(format(uDays, "%Y")),
                       month = as.numeric(format(uDays, "%m")),
                       day   = as.numeric(format(uDays, "%d")),
                       res)
    rownames(res) <- 1:nrow(res)
  }
  #--------------------------------------------------------------
  if (bucket=="onpeak"){
    data <- select.onpeak(data)
    ind  <- which(colnames(data)=="ind.PEAK")
    data <- data[,-ind]   
    res  <- bucket.ts(data,"daily")
  }
  #--------------------------------------------------------------
  if (bucket=="offpeak"){
    pdata <- select.onpeak(data)
    data  <- data[-pdata$ind.PEAK,]
    res   <- bucket.ts(data,"daily") 
  }
  #--------------------------------------------------------------
  if (bucket=="monthly"){
    ind  <- which(colnames(data)==c("day","hour"))
    if (length(ind)!=0){data <- data[, -ind]}
    ind  <- which(colnames(data)==c("year","month"))
    realdata <- data[,-ind]
    dates    <- as.Date(paste(data$year, data$month, "1", sep="-"))
    uMonths  <- unique(dates)
    stk.data <- cbind(stack(realdata), dates=rep(dates, ncol(realdata)))
    res <- tapply(stk.data$values, list(stk.data$dates,stk.data$ind), meanFun)
    rownames(res) <- format(uMonths, "%b %Y")
  }
  #--------------------------------------------------------------
  if (bucket=="7x8"){
    hourOFF <- (data$hour<=6) | (data$hour==23)
    data    <- data[hourOFF,]
    res     <- bucket.ts(data,"daily")
  }
  #--------------------------------------------------------------
  if (bucket=="2x16"){
    require(chron)  # for is.weekend
    dates   <- paste(data$month, data$day, data$year, sep="/")
    ind.WE  <- is.weekend(dates)
    hourON  <- (data$hour>6) & (data$hour<23)
    ind     <- which(hourON & ind.WE)
    data    <- data[ind,]
    res     <- bucket.ts(data,"daily")
  }
  
  return(as.data.frame(res))
} # end of function
