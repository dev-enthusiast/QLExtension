# Written by Wouter Buitenhuis (mot of it)
# Version date: Dec 7 2007

tsdb.futStrip <- function( curveNames, histStart.dt, histEnd.dt, contract.dts,
                          debug=FALSE ){
  res <- NULL
  # Let's do the loops in SecDb if possible!   
  for (n in 1:length(curveNames))
  {
    symbol.root <- secdb.getValueType(curveNames[n], "tsdb symbol rule")
    mkt <- strsplit(curveNames[n], " ")[[1]][2]
    symbol.root <- gsub("!", mkt, symbol.root)
    for (m in 1:length(contract.dts))
    {
      yymm   <- format(contract.dts[m], "%y%m") 
      symbol <- gsub("\\^y2\\^m2", yymm, symbol.root)
      aux    <- secdb.invoke("_lib elec matlab dispatch fns",
                "TSDB::Read", symbol, histStart.dt, histEnd.dt)
      if (!is.null(aux)){     # if you request things that don't exist
        res <- rbind(res, data.frame(curve.name=curveNames[n],
          contract.dt=rep(contract.dts[m], length(aux$value)), data.frame(aux)))
      }
    }
  }
  if (!is.null(res)){
    names(res)[3] <- "pricing.dt"
    res$curve.name <- as.character(res$curve.name)
  }
  return(res)
}

get.fwd.prices <- function(symbols=c("COMMOD PWJ 5X16 PEPCO PHYSICAL", "COMMOD PWJ 7X8 PEPCO PHYSICAL", "COMMOD PWJ 2X16H PEPCO PHYSICAL"),
      evalDt=as.Date("2007-10-16"),startDt=as.Date("2008-06-01"),endDt=startDt+30*10){
# gets forward prices.
#
# inputs
# symbols : Vector with symbol names we want prcies for. SecDB notation
# evalDt  : Pricing date
# startDt : First contract expiration date
# nMonths : number of expirations after start date that needs to be returned. (assumes monthly contracts)

  source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/Database/R/get.marks.from.R.R")
  options <- NULL
  options$calc$hdays <- seq(evalDt, by="-1 day", length.out=1) # may want rela holidays here
  QQ <- expand.grid(curve.name=symbols, 
    contract.dt= seq(startDt, endDt, by="month"))  #not tested yet. should work
  system.time({
    res <- get.prices.from.R(QQ, options)    
  })
  colnames(res)[3] <- "prices"
  return(res)
}

get.historic.data <- function(symbols, dateSt, dateEnd, intraday=TRUE, union=FALSE){
# Download prices, uses NT PLot symbols.
#
# intraday  : for intraday data. for daily data set to FALSE.
# union     : take the union of two data sets, will eliminate NA's. Takes intersection otherwise.
  require(SecDb)
  data <- tsdb.readCurve(symbols[1], dateSt, dateEnd)
  if (intraday==TRUE) time.collumn <- "time"
  else time.collumn <- "date"
  colnames(data) <- c(time.collumn, symbols[1])
  n <- length(symbols)
  if (n > 1) {
    for (i in 2:n) {
      new.data <- tsdb.readCurve(symbols[i], dateSt, dateEnd)
      colnames(new.data) <- c(time.collumn, symbols[i])  # this line maybe redundant
      data <- merge(data, new.data, by=time.collumn, all=union)
    }
  }
  colnames(data[time.collumn]) <- "time"
  rownames(data) <- data$time
  data <- data[-1]
  return(data)
}

secdb.hour.mask <- function(buckets=c('5x16','2x16H','7x8'), region="NEPOOL", 
    st.date=(Sys.Date()-1), end.date = Sys.Date()){
# gets masks for different buckets
  require(SecDb)
  data <- secdb.getBucketMask(region, buckets[1], st.date, end.date)
  n <- length(buckets)
  if (n > 1) {
    for (i in 2:n) {
      new.data <- secdb.getBucketMask (region, buckets[i], st.date, end.date)
      data <- merge(data, new.data, by="time")
    }
  }
  rownames(data) <- data$time
  data <- data[-1]
  return(data)
}

vlr <- function(
      load.symbols = c("DCPEP_Load_LgComGT3A_ELIG",
      "DCPEP_Load_LgComRT_ELIG1",
      "DCPEP_Load_LgComGT3B_ELIG",
      "DCPEP_Load_LgComGTLV_ELIG",
      "DCPEP_Load_LgComGS3A_ELIG",
      "DCPEP_Load_LgComGSLVDEM_ELIG"),
      lmp.symbol = "PJMLMP_Hr_PEP_ZONE",
      fwd.price.symbols = c("COMMOD PWJ 5X16 PEPCO PHYSICAL", "COMMOD PWJ 7X8 PEPCO PHYSICAL", 
          "COMMOD PWJ 2X16H PEPCO PHYSICAL"),
      secdb.buckets = c('5x16','2x16H','7x8'),
      secdb.region = 'PJM',
      date.st = as.Date("2004-01-01"), # starting date for historic analysis
      date.end = Sys.Date()-1, # end date for historic analysis
      deal.st = as.Date("2008-06-01"), # deal start date
      deal.end = as.Date("2010-05-31"), # deal end date
      eval.dt = Sys.Date()-1, # evaluation date
      nsim = 1000, # number of simulations
      quantiles = c(0, 0.01, 0.05, 0.25, 0.5, 0.75, 0.95, 0.99, 1), #quantiles to display in output
      load.weights = 1, # not used yet. idea is to be able to give different laod object different weights
      logs = FALSE){ #logs is not tested yet

  #add check if eval date is in weekend or holiday.
  require(SecDb)
  data <- get.historic.data(c(lmp.symbol, load.symbols), date.st, date.end)
  if (sum(is.na(data))>0) print("error: missing values in historic data, typo in load object or lmp price symbol?")
  # col1 = prices, other cols are loads. need to aggregate loads
  bucket.filter <- secdb.hour.mask(secdb.buckets, secdb.region, date.st, date.end)
  # need to synchronize bucket.filter with data. 
  bucket.filter <- bucket.filter[is.element(rownames(bucket.filter),rownames(data)),]
  data <- data[is.element(rownames(data),rownames(bucket.filter)),]
  colnames(data)[1] <- "prices"
  prices <- data$prices
  data <- data[-1]
  load <- rowSums(data) # aggregate different load profiles
  names(load) <- names(prices) <- rownames(data)
  detrend.price <- normalize.data(prices, logs)
  detrend.load <- normalize.data(load, logs)
  load.regression <- derive.seasonal.means(detrend.load$norm.data, bucket.filter)
  price.regression <- derive.seasonal.means(detrend.price$norm.data, bucket.filter)
  if (logs == T) vlr.realizations <- load.regression$residuals + price.regression$residuals #think about this
  else vlr.realizations <- load.regression$residuals * price.regression$residuals
  vlr.distribution <- sample.from.vlr(vlr.realizations, fwd.price.symbols, 
        eval.dt, deal.st, deal.end, nsim, logs)  # multiply by forwards outside this function is better.
  results <- NULL
# save results
  results$inflation <- detrend.price$growth
  results$load.growth <-  detrend.load$growth
  results$avg.price <-  detrend.price$average # does not return anything
  results$avg.load <- detrend.load$average
#  results$vlr.realizations <- vlr.realizations
  results$total.vlr <- quantile(colSums(vlr.distribution),quantiles) / 
      (as.numeric(deal.end - deal.st)*24)                 

  #plot results
  x.axis <- secdb.convertTime(as.numeric(names(load.regression$fitted.values)))
  n <- length(load.regression$fitted.value)
  years.in.graphs <- 1
  point.size <- 0.5
  ind <- (n-years.in.graphs * 24*365*2):n
  layout(matrix(1:4, 2, 2, byrow=TRUE))  # make room for 4 plots
  layout.show(4)
  browser()
  plot(x.axis[ind], load.regression$fitted.values[ind],  xlab="date", ylab="normalized load",
     main="historic load estimate", col="blue", las=2, type="p", pch=".", cex=point.size, xaxt="n")
  r <- round(x.axis, "day")
  axis.Date(1, x=r, by="weeks")
  plot(x.axis[ind], detrend.load$norm.data[ind],  xlab="date", ylab="normalized load",
     main="historic load", col="brown", las=2, type="p", pch=".", cex=point.size) 
  plot(x.axis[ind], price.regression$fitted.values[ind],  xlab="date", ylab="normalized price",
     main="historic price estimate", col="blue", las=2, type="p",  pch=".", cex=point.size)
  plot(x.axis[ind], detrend.price$norm.data[ind],  xlab="date", ylab="normalized price",
     main="historic price", col="brown", las=2, type="p", pch=".", cex=point.size) 
  windows()
  layout(matrix(1:2, 2, 2, byrow=TRUE))  
  layout.show(2)
  plot(x.axis[ind], load.regression$residuals[ind],  xlab="date", ylab="normalized load",
     main="load residuals", col="green", las=2, type="p", pch=".", cex=point.size) 
  plot(x.axis[ind], price.regression$residuals[ind],  xlab="date", ylab="normalized price",
     main="price residuals", col="green", las=2, type="p", pch=".", cex=point.size)
  windows()
  plot(x.axis[ind], vlr.realizations[ind],  xlab="date", ylab="realized vlr",
     main="historic vlr realizations", col="black", las=2, type="p", pch=".", cex=point.size) 

  return(results)
}

normalize.data <- function(data, logs=FALSE){
  time <- as.numeric(names(data))           
  time <- (time - time[1])/(3600*24*365.25) # time in years since first observation 
  data <- data.frame(data, time)
  colnames(data)[1] <- "Y"
  if (logs==TRUE) data$Y <- log(data$Y)
  regression <- lm("Y ~ time", data)  # may want to take log of prices. Need to do something with negative numbers
  results <- NULL
  if (logs==TRUE) {
    results$growth <- regression$coefficients["time"]
    results$average <- exp(regression$coefficients["(Intercept)"])
    results$norm.data <- exp(data$Y/ regression$fitted.values)
  } else { # logs = false
    results$growth <- regression$coefficients["time"] / regression$coefficients["(Intercept)"]
    results$average <- regression$coefficents["(Intercept)"]
    results$norm.data <- data$Y/regression$fitted.values # normalized prices
  }
  return(results)
}

#\\nas-msw-07\e14196\My Documents\research\vlr\vlr 1.5.r
sample.from.vlr <- function(vlr.realizations, fwd.price.symbols, eval.dt, deal.st, 
    deal.end, nsim=100, logs=F, demean=T, relative=T) {
# sampling from vlr realizations
# need to test results more. focuss on vlr mean.
# can make a bunch of things neater:
# matching fwd prices with forecasts. breaks forecast in equal distance sets. this could be cummbersom to change with limited value
# do more with on and off peak fwd prices.

  require(SecDb)
  #fwds1 <- get.fwd.prices(fwd.price.symbols, eval.dt, deal.st, deal.end) 
  fwds <- tsdb.futStrip(fwd.price.symbols, eval.dt, eval.dt, seq(from=deal.st, to=deal.end, by="month"))
  fwds <- fwds[-3] #remove pricing date
  fwds[order(fwds$contract.dt),] # this is all doen to replace to olfd fwds function that was less robust.
  if (sum(is.na(fwds))>0) print("error: missing values in forwards, typo in symbol names?")
  #colnames(fwds)[3] <- "prices"
  deal.dates <- rep(seq(from=deal.st, to=deal.end, by="days"), each=24)
  n.forecasts <- length(deal.dates)
  vlr <- data.frame(vlr.realizations[-1], vlr.realizations[-length(vlr.realizations)])
  colnames(vlr) <- c("vlr.t" , "vlr.t.min.1")
  nn <- nrow(vlr)
  rownames(vlr) <- names(vlr.realizations[-1])
  vlr.mean.ls <- derive.seasonal.means(vlr.realizations[-length(vlr.realizations)], hour=FALSE)
  vlr.mean.adj <- vlr - matrix(rep(vlr.mean.ls$fitted.values,2),ncol=2)
  vlr.ls <- lm("vlr.t ~ vlr.t.min.1", vlr.mean.adj, singular.ok=FALSE)
  rho <- vlr.ls$coefficients["vlr.t.min.1"]
  vlr.projected <- forecast.seasonal.means(as.POSIXlt(deal.dates)$yday, vlr.mean.ls, logs=FALSE) #always retuns the same number! is constant
  vlr.drawings <- matrix(sample(vlr.ls$residuals, nsim * n.forecasts, replace=T), ncol=nsim)
  contract.months <- unique(fwds$contract.dt)
  n.months <- n.breaks <- length(contract.months)
  hours.in.month <- break.n <- n.forecasts %/% n.breaks # approximate hours in month
  remainder <- n.forecasts - n.breaks * break.n
  fwd.prices <- rep(fwds$value,each=hours.in.month %/% 3)  # 3 = number of buckets
  if (length(fwd.prices) < n.forecasts) fwd.prices[(length(fwd.prices)+1):n.forecasts] <- fwd.prices[length(fwd.prices)]
  persistence.hour <- matrix(0, hours.in.month, hours.in.month)  # make rho hour
  persistence.hour[lower.tri( persistence.hour)] <- 1
  persistence.month <- persistence.hour <- apply(persistence.hour,2,cumsum)
  persistence.hour <- rho^persistence.hour
  persistence.hour[upper.tri(persistence.hour)] <- 0
  persistence.month <- persistence.month - t(persistence.month) +  hours.in.month
  persistence.month <- rho ^ persistence.month
  #ind <- c(seq(from=1,by=hours.in.month,to=n.forecasts), hours.in.month*n.months+1)
  ind <- seq(from=1,by=hours.in.month,to=(n.forecasts+1))
  for (i in 1:n.months) {
    vlr.drawings[ind[i]:(ind[i+1]-1),] <-  persistence.hour %*% vlr.drawings[ind[i]:(ind[i+1]-1),]
    if (i > 1) {
       vlr.drawings[ind[i]:(ind[i+1]-1),] <-  vlr.drawings[ind[i]:(ind[i+1]-1),] + 
          persistence.month %*%  vlr.drawings[ind[i-1]:(ind[i]-1),]
    }
  }
  if (remainder > 0){
    vlr.drawings[ind[n.months+1]:n.forecasts,] <- (persistence.hour %*% 
        vlr.drawings[(n.forecasts-hours.in.month+1):n.forecasts,])[(hours.in.month-remainder+1):hours.in.month,]
    vlr.drawings[ind[n.months+1]:n.forecasts,] <-  vlr.drawings[ind[n.months+1]:n.forecasts,] * rep(fwd.prices[ind[n.months]],remainder)
         # not perfect, but good enough
  }
  vlr.drawings <- vlr.drawings + matrix(rep(vlr.projected,nsim),ncol=nsim)
  vlr.drawings <- -vlr.drawings * fwd.prices  #scale with prices and make negative. can do this on a better spot, more clearly.
  return(vlr.drawings)
}
                                 
seasonal.factors <- function(dates, hours=TRUE) {
  require(SecDb)
  if (is.numeric(dates)==TRUE) DOY <- dates 
  else DOY <- secdb.convertTime(as.numeric(dates))$yday  #day of year

  F1 <- cbind(cos(2*pi / 365. * 0.5*DOY), sin(2*pi / 365.0 * 0.5*DOY))
  F1 <- cbind(F1,cos(2*pi / 365. * DOY), sin(2*pi / 365.0 * DOY))
  F1 <- cbind(F1,cos(2*pi / 365.0 * 2*DOY), sin(2*pi / 365.0 * 2*DOY)) 
  colnames(F1) <- c("DF1","DF2","DF3", "DF4","DF5","DF6")
  if (hours==TRUE) {
    HOD <- secdb.convertTime(as.numeric(dates))$hour #hour of day
    F1 <- cbind(F1,cos(2*pi / 24. * 0.5 * HOD), sin(2*pi / 24 * 0.5*HOD)) 
    F1 <- cbind(F1,cos(2*pi / 24. * HOD), sin(2*pi / 24 * HOD))
    F1 <- cbind(F1,cos(2*pi / 24 * 2*HOD), sin(2*pi / 24 * 2*HOD)) 
    colnames(F1) <-  c("DF1","DF2","DF3", "DF4","DF5","DF6", "HF1", "HF2", "HF3", "HF4","HF5","HF6")
  }
  return(F1)
}

derive.seasonal.means <- function(market.data, 
    bucket.filter = matrix(1,ncol=1,nrow=length(market.data)),
    hours = TRUE) {
# market.data is a vector with either prices or loads. names are date / time numbers from secdb.
# bucket filter is a dataframe with buckets
  F1 <- seasonal.factors(names(market.data), hours)
  factor.names <-colnames(F1)
  nfactors <- ncol(F1)
  F1 <- matrix(rep(F1,ncol(bucket.filter)),ncol=nfactors*ncol(bucket.filter))
  filter <- matrix(rep(as.matrix(bucket.filter), each=nfactors),byrow=FALSE,ncol=nfactors*ncol(bucket.filter))
  F1 <- F1 * filter


  factor.matrix <- expand.grid(factor.names,paste("B",1:ncol(bucket.filter),".",sep=""))
  colnames(F1) <- paste(as.character(factor.matrix$Var2),as.character(factor.matrix$Var1),sep="")  
  regression.data <- as.data.frame(cbind(market.data,F1))
  colnames(regression.data)[1] <- "Y"
  equation <- paste("Y~",paste(colnames(F1), collapse="+"),sep="") #relation to be estimated
  ls.results <- lm(as.formula(equation), regression.data, singular.ok=F)  
  return(ls.results)
}

forecast.seasonal.means = function(future.days, regression, logs=FALSE) {
  F1 = cbind(matrix(1,nrow=length(future.days)), seasonal.factors(future.days, hours=FALSE))
  if (logs == TRUE) forward$pred <- exp(F1 %*% regression$coefficients) 
  else prediction <- F1 %*% regression$coefficients # can use predict.lm()?
  return(prediction)
}

basis.risk <- function(rt.symbols=c("NEPOOL_LMP_RT_NH", "NEPOOL_LMP_RT_HUB"),
  fwd.symbols.A=c("COMMOD PWX 5X16 NH PHYSICAL", "COMMOD PWX 7X8 NH PHYSICAL",
          "COMMOD PWX 2X16H NH PHYSICAL"), 
          fwd.symbols.B = c("COMMOD PWX 5x16 HUB PHYSICAL", "COMMOD PWX 7x8 HUB PHYSICAL", "COMMOD PWX 2x16H HUB PHYSICAL"),
          dt.start = as.Date("2004-01-01"), end.dt = Sys.Date()
          ){
  # calculate forward basis
  # calculate relative historic basis
  # modify relative historic basis to todays fwd basis.
  # sample from historic realizations 
}

analyse.regression <- function(){
  x.axis <- secdb.convertTime(as.numeric(names(load.regression$fitted.values)))
  plot(x.axis, merge(load.regression$fitted.values, detrend.load$norm.data),  xlab="date", ylab="fitted load",
     main="historic load estimate", col="blue", las=2, type="l")
  lines(as.Date(names(MM2)), MM2, type="o", col="brown")
}