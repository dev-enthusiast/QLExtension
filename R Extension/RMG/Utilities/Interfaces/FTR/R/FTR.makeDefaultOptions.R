FTR.makeDefaultOptions <- function(auction, ISO.env, asOfDate=Sys.Date())
{
  # basic ones ... 
  options <- NULL
  options$asOfDate <- asOfDate
  options$auction  <- auction
  aux <- FTR.AuctionTerm(auction=auction)
  if (length(grep("-", auction))==0){
    options$prev.auction <- FTR.AuctionTerm(aux[1,"start.dt"]-1,
                                            aux[1,"start.dt"]-1)}
  options$start.dt <- aux[1,"start.dt"]
  options$end.dt   <- aux[1,"end.dt"]
  if (length(grep("-", auction))==0){
    options$month <- options$start.dt
    options$auction.name <- format(options$start.dt, "%b-%y monthly auction")
    options$nby.months <- c(seq(options$start.dt, by="-1 month",
      length.out=2)[2], seq(options$start.dt, by="1 month",
      length.out=2))
    options$nby.months <- as.numeric(substr(as.character(options$nby.months),
                                            6,7))
  } 
  options$region   <- ISO.env$region
 
  if (ISO.env$region == "NEPOOL")
    options <- .defaultOptions.NEPOOL(auction, ISO.env, options)
  if (ISO.env$region == "NYPP")
    options <- .defaultOptions.NYPP(auction, ISO.env, options)

  
#  options$bids$total.expense  <- 250000      # total amount to spend
#  options$bids$max.mw.path    <- 50
#  options$bids$max.daily.loss.cluster <- 0.20*options$bids$total.expense

  options$bids$max.mw.node <- 200
  options$bids$min.mw.path <- 2.5

  return(options)
}

#------------------------------------------------------------------
.defaultOptions.NEPOOL <- function( auction, ISO.env, options )
{
  options$days <- NULL
  ind <- secdb.getHoursInBucket(ISO.env$region, "PEAK",
    as.POSIXct(paste(format(options$start.dt), "00:00:00")),
    as.POSIXct(paste(format(options$end.dt),   "00:00:00")))
  ind$time <- format(ind$time, "%Y-%m-%d")
  options$days["ONPEAK"] <- length(unique(ind$time))
  ind <- secdb.getHoursInBucket(ISO.env$region, "OFFPEAK",
    as.POSIXct(paste(format(options$start.dt), "00:00:00")),
    as.POSIXct(paste(format(options$end.dt),   "00:00:00")))
  ind$time <- format(ind$time, "%Y-%m-%d")
  options$days["OFFPEAK"] <- length(unique(ind$time))
  options$days["FLAT"] <- options$end.dt - options$start.dt + 1

  if (!is.null(options$month)){
    aux <- subset(NEPOOL$HRS, contract.dt==options$month)
    options$hours <- aux$hours
    names(options$hours) <- aux$class.type
  } else {
    aux <- cast(NEPOOL$HRS, class.type ~., sum, value="hours",
      subset=(contract.dt>=options$start.dt &
      contract.dt <= options$end.dt))
    aux <- structure(aux[,2], names=aux[,1])
    options$hours <- aux
  }
    
##   aux <- !secdb.isHoliday(index(ISO.env$hP))
##   names(aux) <- index(ISO.env$hP)
##   options$days.ind$ONPEAK <- aux

  options$zones.ptid <- 4000:4008
  
  return(options)
}

#------------------------------------------------------------------
.defaultOptions.NYPP <- function( auction, ISO.env, options )
{
  ind <- seq(
    as.POSIXct(paste(format(options$start.dt), "00:00:00")),
    as.POSIXct(paste(format(options$end.dt), "23:00:00")), by="hour")
  options$hours["FLAT"] <- length(ind)
  options$days["FLAT"]  <- options$end.dt - options$start.dt + 1

  options$zones.ptid     <- c(61752:61762, 61844:61847)
  options$reference.ptid <- 24008  # MARCY node
  
  return(options)
}
