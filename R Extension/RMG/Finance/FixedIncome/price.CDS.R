# Price a CDS and return the parspread, DV01, and the cashflows.
# Check demo.price.CDS.R to see how to format the parameters. 
#
#
# Written by Adrian Dragulescu on 8-Mar-2006 


par.spread.CDS <- function(data, DF){
#  browser()
  #-------------------------Premium(fixed) leg--------------------------------
  dt  <- as.numeric(diff(c(data$asOfDate, data$pay.dates)))/365
  cdf <- merge(DF, data.frame(Date=data$pay.dates))$interp.cdf
  df  <- merge(DF, data.frame(Date=data$pay.dates))$DF 
  no.default <- sum(dt*df*(1-cdf))
  accrued.interest <- 0   # last coupon matches settlement for now.
  premium <- data$notional*(no.default + accrued.interest)   # * data$cds.rate 
  #-------------------------Floating(default) leg-----------------------------
  DF <- subset(DF, Date <= data$end.date & Date >= data$start.date) 
  int.DF.dCDF <- sum(DF$DF[2:nrow(DF)]*diff(DF$interp.cdf))
  default.leg <- data$notional*(1-data$recovery.rate)*int.DF.dCDF

  data$par.cds.spread     <- default.leg/premium     
  data$par.cds.spread.bps <- data$par.cds.spread*10000  # in bps
  if (data$deal.spread > 1){   # originnaly given in bps
    data$deal.spread <- data$deal.spread/10000}
  data$market.val <- (data$par.cds.spread-data$deal.spread)*premium
#  data$spread.DV01 <- spread.DV01(data, DF)  # not implemented yet
  data$cashflow <- data.frame(date=data$pay.dates,
     cashflow=data$notional*data$deal.spread*dt*df)
  if (data$buy.sell=="SELL"){
    data$market.val <- -data$market.val
    data$cashflow$cashflow <- -data$cashflow$cashflow
  }
  return(data)
}

price.CDS <- function(data, DF=NULL, options=NULL){

  if (length(options$is.official)==0){options$is.official <- FALSE}
  if (length(options$do.plots)==0){options$do.plots <- FALSE}
  if (length(data$buy.sell)==0){data$buy.sell <- "BUY"}
  if (length(data$recovery.rate)==0){data$recovery.rate <- 0.40}
  payment.freq <- switch(tolower(data$payment.freq),
                         quarterly   = "-3 month",
                         semiannually = "-6 month",
                         annually    = "-12 month")
  pay.dates <- rev(seq(data$end.date, data$asOfDate, by=payment.freq))
  if (data$asOfDate < data$start.date){pay.dates <- pay.dates[-1]}
  data$pay.dates <- business.day.adj(pay.dates)
  #------------------map prob. of default curve to actual dates-----------
  if (data$prob.default$time[1]!=0){
    data$prob.default <- rbind(data.frame(time=0, cdf=0), data$prob.default)}
  data$prob.default$dates <- data$asOfDate
  for (d in 1:nrow(data$prob.default)){
    months.out <- 12*data$prob.default$time[d]
    data$prob.default$dates[d] <- seq(max(data$asOfDate, data$start.date), 
      by=paste(months.out, "month"), length.out=2)[2]
  }
  data$prob.default$dates <- business.day.adj(data$prob.default$dates)
  if (options$do.plots){
    dev.off(which=2); windows(5,4)
    plot(data$prob.default$dates, data$prob.default$cdf*100, col="blue", 
      type="o", ylab="Cummulative prob default, %")
  }
  #-------------------------get discount factor if not passed as input-------
  if (length(DF)==0){  
    options$asOfDate    <- data$asOfDate
    options$months      <- seq(data$asOfDate, data$end.date, by="1 day")
    DF <- try(get.discount.factor(options))
    if (class(DF)=="try-error"){
      options$errors <- "Could not get discount factor."}
  }
  DF$interp.cdf <- approx(data$prob.default$dates, data$prob.default$cdf,
                          DF$Date)$y  # get the cdf by day 
  data <- try(par.spread.CDS(data, DF))
  return(list(data, options))
}


