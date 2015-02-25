# General definitions potentially useful to other regions
# Maybe move into FTR2 at some point
#
#
# FTR_Auction                 - define an FTR auction term
# parse_auctionName           - from a string
# auctions_in_date            - get all the auctions that fall over a date
# auctions_in_term            - get all the auctions that overlap a start/end date
# count_hours_auction
# previousAuctions            - previous 12 monthlies and the closest annual
#
#

#################################################################
# Create an FTR_Auction object with all the temporal info you 
# need related to an auction.
# @param endDt is the start date for the auction, e.g. 2012-01-31
# @param endDt is the end date for the month, e.g. 2012-12-31
# @param round, if there is a round
# @param region the name of the region
# @return and FTR_Auction object
#
FTR_Auction <- function(startDt, endDt, round=0, region="NEPOOL")
{
  months   <- seq(startDt, as.Date(format(endDt, "%Y-%m-01")), by="1 month")
  noMonths <- length(months)
  
  name <- if (noMonths == 1) {
    format.dateMYY(startDt, direction=-1)
  } else if (noMonths == 12) {
    paste( format.dateMYY(startDt, direction=-1), "-1Y", sep="")
  }
  
  if (round != 0)
    name <- paste(name, "-R", round, sep="")
  
  structure(list(auctionName = name,
                 startDt = startDt,
                 endDt = endDt,
                 round = round, 
                 noMonths = length(months),
                 months = months,
                 #hours = count_hours_auction(auctions=name, region=region),
                 region = region),
            class="FTR_Auction")
}


#################################################################
# Given a vector of auction names
# return a vector of FTR_Auction objects
# An auction name is made of at most 3 tokens separated by "-" 
# 1) the start date format YMM
# 2) OPTIONAL, the length of the auction in months "3M", or "1Y" OR
#    the round number, "R1"
# 3) OPTIONAL, the round number in format Rn, where n is an integer.
#
# parse_auctionName( c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
#                      "M13-12M", "M13-6M-R1") )
#
parse_auctionName <- function(auctionName, region="NEPOOL")
{
  aux <- lapply(auctionName, function(x) {
    #rLog("Working on ", x)
    aux <- strsplit(x, "-")[[1]]
    startDt <- format.dateMYY(x, direction=1)

    
    if (length(aux) == 1) {  # it's a month style: F11
      endDt <-  nextMonth(startDt)-1
      noMonths <- 1
      round    <- 0
      
    } else if (length(aux) == 2) {  # it's a month M13-R3,  F10-1Y

      if (grepl("^R", aux[2])) {       # it's a month M13-R3
        endDt <-  nextMonth(startDt)-1
        noMonths <- 1
        round <- as.numeric(gsub("^R", "", aux[2]))

      } else if (grepl("Y$", aux[2])) {   # it's a legacy yearly auction
        endDt <- as.Date(paste(as.numeric(format(startDt, "%Y")), "-12-31", sep=""))
        noMonths <- 12
        round <- 0
        
      } else if (grepl("^[[:digit:]]+M", aux[2])) {   # it's "M13-6M"
        noMonths <- as.numeric(gsub("([[:digit:]]+)M", "\\1", aux[2])) 
        endDt <- seq(startDt, by=paste(noMonths, "month"), length.out=2)[2]-1
        round <- 0
        
      } else {
        stop("Don't know how to parse ", x)
      }
      
    } else if (length(aux) == 3) { 
      # it's an annual with rounds F13-1Y-R2, or multi round multi month "M13-6M-R1"
      if (grepl("Y", aux[2])) {
        endDt <- as.Date(paste(as.numeric(format(startDt, "%Y")), "-12-31", sep=""))
        noMonths <- 12
        round <- as.numeric(gsub("^R", "", aux[3]))
        
      } else if (grepl("M", aux[2])) {
        noMonths <- as.numeric(gsub("([[:digit:]]+)M", "\\1", aux[2])) 
        endDt <- seq(startDt, by=paste(noMonths, "month"), length.out=2)[2]-1
        round <- 0
        
      } else {
        stop("Don't know how to parse ", x)
      }
       
    } else {
      stop("Cannot have more than 3 - in auctionName ", x)
    }

    months <- seq(startDt, by="1 month", length.out=noMonths)
    
    structure(list(auctionName = x,
                   startDt = startDt,
                   endDt = endDt,
                   round = round,
                   noMonths = noMonths,
                   months = months,
                   region = region),
            class="FTR_Auction")
    
  })

  if (length(aux) == 1)
    aux <- aux[[1]]

  aux
}


#################################################################
# Find the auctions that contain a given start/end interval. 
# The match should be exact.  The term needs to be completely inside
# the start/end of the auction. 
# @param auctions the auctions you want to look through, e.g. "F13-1Y-R1"
# @param startDt the start date you want to check, an R Date
# @param endDt   the end date you want to check, an R Date
#
auctions_in_term <- function( auctions, startDt, endDt )
{
  auctionObj <- parse_auctionName(auctions)
  res <- NULL
  for (obj in auctionObj) {
    if ((startDt >= obj$startDt) && (endDt <= obj$endDt))
      res <- c(res, obj$auctionName)
  }

  res
}



#################################################################
# 
# @param auctions a vector of auctionNames
# @param buckets the name of the buckets
# @return a data.frame with columns "class.type", "auction", "hours"
#
count_hours_auction <- function(auctions=c("F11", "F11-1Y"), region="NEPOOL",
  buckets=c("flat", "peak", "offpeak"))
{
  auctionObj <- parse_auctionName(auctions)
  if (length(auctions)==1)
    auctionObj <- list(auctionObj) # annoying bug!
  
  DF <- ldply(auctionObj, function(x){
    data.frame(auction = x$auctionName, month=x$months, 
               startDt=x$startDt, endDt=x$endDt)})
    
  start.dt <- as.POSIXlt(paste(min(DF$startDt), "01:00:00"))
  end.dt   <- as.POSIXlt(paste(max(DF$endDt)+1, "00:00:00"))

  res <- NULL
  for (ind.b in seq_along(buckets)) {
    aux <- secdb.getBucketMask(region, buckets[ind.b], start.dt, end.dt)
    aux <- aux[aux[,2], ]
    aux$month <- as.Date(format(aux$time-1, "%Y-%m-01"))  # use HE!

    aux <- data.frame(table(aux$month))
    names(aux)[1] <- c("month")
    aux$month  <- as.Date(aux$month)

    aux <- merge(aux, DF)
    aux <- cast(aux, auction ~ ., sum, value="Freq", fill=NA)
    colnames(aux)[2] <- "hours"
    aux <- cbind(class.type=buckets[ind.b], aux)

    res[[ind.b]] <- aux
  }

  res <- do.call(rbind, res)
  res$class.type <- toupper(as.character(res$class.type))
  if (region == "NEPOOL")
    res$class.type <- gsub("^PEAK", "ONPEAK", res$class.type)
  
  res
}


#################################################################
# Get the auctions before a given auction (the previous annual auction
# and the 12 preceding monthly auctions).
#
# @param auction an FTR_Auction object
# @return a vector with previous 12 monthly auctions and the
#    previous annual auction.
#
previousAuctions <- function( auction )
{
  year <- as.numeric(format(auction$startDt, "%Y")) - 1
  annual <- paste("F", year-2000, "-1Y", sep="")
  if (year >= 2013)
    annual <- paste(annual, c("-R1", "-R2"), sep="")

  startDt <- auction$startDt
  startDts <- seq(startDt, by="-1 month", length.out=13)
  endDts <- startDts - 1

  monthlies <- apply(data.frame(startDts[-1], endDts[-13]), 1,
    function(x){ FTR_Auction(as.Date(x[1]), as.Date(x[2]))$auctionName })

  
  c(annual, monthlies)  
}


#################################################################
#################################################################
#
.main <- function()
{
  require(reshape)
  require(CEGbase)
  require(SecDb)
  require(FTR)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.FTR.auction.R")

  startDt <- as.Date("2012-11-01")
  endDt   <- as.Date("2012-11-30")
  FTR_Auction(startDt, endDt)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  parse_auctionName( c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
                       "M13-10M", "M13-1Y-R2") )

  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  count_hours_auction(c("F13-R1", "G13-R1", "F13-1Y-R2"), 
    buckets=c("peak", "offpeak"))
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  auctions <- previousAuctions( parse_auctionName("F14-1Y-R1") )
  


  
  
}
