# Find the implied default cdf probability given a set of cds quotes.
# See an example in demo.price.CDS.R
#
# The form of the cdf function is assumed to be an exponential.
# See the Bloomberg documentation (B. Flesaker [06]).
#
# Written by Adrian Dragulescu on 16-Mar-2007


business.day.adj <- function(dates){
  adj.dates <- ifelse(weekdays(dates) == "Saturday", dates + 2,
    ifelse(weekdays(dates)=="Sunday", dates+1, dates))+as.Date("1970-01-01")
  return(adj.dates)
}

calc.cdf <- function(quotes, asOfDate){
  # calculate the cdf given a series of spread quotes  
  dt  <- as.numeric(diff(c(asOfDate, quotes$dates)))/365
  quotes$cdf <- 1-exp(-cumsum(quotes$lambda*dt))
  quotes$cdf <- ifelse(quotes$cdf>1, 1, quotes$cdf)
  return(quotes)
}

objective.Fun <- function(lambda1, q, quotes, data, DF){
  quotes$lambda[q]  <- lambda1   # 1D optimization over this parameter
  quotes <- quotes[1:q,]         # remove later dates
  aux    <- calc.cdf(quotes, data$asOfDate)
  data$prob.default <- aux[,c("time", "cdf")]
  res <- price.CDS(data, DF)
  return(res[[1]]$market.val^2)
}

calibrate.default.cdf <- function(quotes, data, DF=NULL, options=NULL){
  if (length(data$asOfDate)==0){data$asOfDate <- Sys.Date()-1}
  if (length(data$start.date)==0){data$start.date <- data$asOfDate + 1}
  if (length(data$payment.freq)==0){data$payment.freq <- "quarterly"}
  if (length(DF)==0){
    options$asOfDate <- data$asOfDate
    end.date <- seq(data$start.date, by=paste(12*max(quotes$time)+1,
       "month"), length.out=2)[2]
    options$months      <- seq(data$asOfDate, end.date, by="1 day")
    DF <- try(get.discount.factor(options))
    if (class(DF)=="try-error"){
      options$errors <- "Could not get discount factor."}    
  }
  quotes$dates <- data$start.date
  for (q in 1:nrow(quotes)){
    months.out <- 12*quotes$time[q]
    quotes$dates[q] <- seq(data$start.date, by=paste(months.out,
                                      "month"), length.out=2)[2]
  }
  quotes$dates  <- business.day.adj(quotes$dates)
  quotes$lambda <- NA
  data$notional <- 1
  data$buy.sell <- "BUY"
  data$start.date <- max(data$start.date, business.day.adj(data$asOfDate+1))
  #------------------------------start calibration by bootstrap------
  for (q in 1:nrow(quotes)){
    data$deal.spread  <- quotes$cds.spread[q]
    data$end.date     <- quotes$dates[q]
    res <- optimize(objective.Fun, interval=c(0, 5), q=q, quotes=quotes,
                    data=data, DF=DF)
    (res)
    quotes$lambda[q] <- res$minimum
 }
 quotes <- calc.cdf(quotes, data$asOfDate) 
 return(quotes) 
}
