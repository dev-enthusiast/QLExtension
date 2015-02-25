# Take a gas curve and create the equivalent hourly curve.
# Note that gas trades 10AM to 10AM
#
# monthly_to_hourly
# upsample_hourly
#

############################################################
# @param X a zoo time series, with one or more columns 
# @return an hourly zoo ts
#
monthly_to_hourly <- function(X)
{
  aux <- data.frame(ind__=seq(as.POSIXct(paste(index(X)[1], "01:00:00")),
    as.POSIXct(paste(nextMonth(index(X)[length(index(X))])+1, "00:00:00")),
    by="1 hour"))
  aux$mth__ <- format(aux$ind__-1, "%Y-%m")
 

  bux <- merge(aux[,c("mth__", "ind__")],
               data.frame(mth__=format(index(X), "%Y-%m"), X))
  
  ind <- bux$ind__
  bux$mth__ <- bux$ind__<- NULL
  Y <- zoo(bux, ind)

  
  Y
}



############################################################
# @param X a zoo time series, with one or more columns and
#   a Date index
# @result Y a hourly (HE) zoo time series
#
upsample_hourly <- function( X, rule=c("gas", "normal"))
{
  if (rule == "gas") {
    aux <- data.frame(ind__=seq(as.POSIXct(paste(index(X)[1], "11:00:00")),
      as.POSIXct(paste(index(X)[length(index(X))]+1, "10:00:00")),
      by="1 hour"))
    aux$hour <- as.numeric(format(aux$ind__, "%H"))
    aux$day <- as.Date(format(aux$ind__, "%Y-%m-%d"))
    aux$day__ <- as.Date(ifelse(aux$hour <= 10, aux$day-1, aux$day), origin="1970-01-01")
    #head(aux, 36)

     
  } else if (rule == "normal") {
    aux <- data.frame(ind__=seq(as.POSIXct(paste(index(X)[1], "01:00:00")),
      as.POSIXct(paste(index(X)[length(index(X))]+1, "00:00:00")),
      by="1 hour"))
    aux$hour <- as.numeric(format(aux$ind__, "%H"))
    aux$day__<- as.Date(format(aux$ind__, "%Y-%m-%d"))
    #head(aux, 36)

 } else {
    rLog("Not implemented for rule =", rule)
  }
  
  bux <- merge(aux[,c("day__", "ind__")],
               data.frame(day__=index(X), X))
  #head(bux, 36)
  ind <- bux$ind__
  bux$day__ <- bux$ind__<- NULL
  Y <- zoo(bux, ind)

  
 
  
  Y
}



############################################################
#
.test <- function()
{
  require(CEGbase)
  require(zoo)
  require(reshape)
  require(SecDb)
  
  
  startDt  <- as.POSIXct("2013-07-01")
  endDt    <- as.POSIXct("2013-07-14")
  
  # get the gas, gen out
  GG <- FTR:::FTR.load.tsdb.symbols(c("gasdailymean_algcg",
    "gasdailymean_tetm3", "fo6_spot_northeast"), startDt, endDt)
  colnames(GG) <- c("algcg", "tetm3", "fo6")
  GG
  
  HH <- upsample_hourly(GG, rule="gas")
  head(HH, 36)

  X <- zoo(c(1,2,3),
           as.Date(c("2014-01-01", "2014-02-01", "2014-03-01")))

  monthly_to_hourly(X)
  
}
