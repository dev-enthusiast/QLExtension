# Calculate payoff for a list of paths from the same region but 
# differnt auctions are possible.
# @params paths a list of paths, needs to be same region
#    but it can be different auctions
# @param period, what period to use "FROM_START", "CURRENT_MONTH", "NEXT_DAY"
# @param asOfDate
# @return a numeric vector  
#
get_payoff <- function(paths,
                       period=c("FROM_START", "CURRENT_MONTH", "NEXT_DAY"),
                       asOfDate=Sys.Date() )
{
  if (inherits(paths, "CRR_Path")) 
    paths <- list(paths)   
  
  CP <- get_clearing_price( paths )

  MW  <- sapply(paths, function(x){ x$mw })
  
  sph <- get_settle_price_hourly( paths )
  if (asOfDate != Sys.Date()) {
     sph <- lapply(sph,
       function(x) window(x,
         end=as.POSIXct(paste(format(asOfDate+2), "00:00:00"))))
  }
  
  # initialize result
  payoff <- vector("list", length=length(period))
  names(payoff) <- period
  lapply(payoff, function(x){ rep(NA, length=length(paths))})

  
  if ("FROM_START" %in% period) {
    SP  <- sapply(sph, function(x){ mean(x, na.rm=TRUE) })
    HRS <- sapply(sph, length)
    payoff[["FROM_START"]] <- HRS*(SP-CP)*MW
  }

  if ("CURRENT_MONTH" %in% period) {
    startDt <- as.POSIXct(paste(currentMonth(asOfDate), "01:00:00"))  
    endDt   <- as.POSIXct(paste(nextMonth(asOfDate), "00:00:00")) 
    SP  <- sapply(sph, function(x) {
      mean(window(x, start=startDt, end=endDt), na.rm=TRUE)
    })
    HRS <- sapply(sph, function(x) {
      length(window(x, start=startDt, end=endDt))
    })
    aux <- HRS*(SP-CP)*MW
    payoff[["CURRENT_MONTH"]] <- ifelse(is.finite(aux), aux, NA)
  }
  
  if ("NEXT_DAY" %in% period) {
    startDt <- as.POSIXct(paste(format(asOfDate+1), "01:00:00"))
    endDt   <- as.POSIXct(paste(format(asOfDate+2), "00:00:00")) 
    SP  <- sapply(sph, function(x){
      mean(window(x, start=startDt, end=endDt), na.rm=TRUE)
    })
    HRS <- sapply(sph, function(x) {
      length(window(x, start=startDt, end=endDt))
    })
    aux <- HRS*(SP-CP)*MW
    payoff[["NEXT_DAY"]] <- ifelse(is.finite(aux), aux, NA)
  }


  payoff
}
