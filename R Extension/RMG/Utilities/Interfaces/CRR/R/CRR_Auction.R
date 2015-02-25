#
# CRR_Auction
# parse_auctionName
# available_auctions
# count_hours_auction
#
#

#################################################################
# Create an CRR_Auction object with all the temporal info you 
# need related to an auction.  
# @param endDt is the start date for the auction, e.g. 2012-01-31
# @param endDt is the end date for the month, e.g. 2012-12-31
# @param round, if there is a round
# @param region the name of the region
# @return a CRR_Auction object
#
CRR_Auction <- function(startDt, endDt, round=0, region="NEPOOL")
{
  months   <- seq(startDt, as.Date(format(endDt, "%Y-%m-01")), by="1 month")
  noMonths <- length(months)

  if (region != c("NEPOOL", "NYPP"))
    rLog("Region", region,  "has not been tested!")
  
  name <- if (noMonths == 1) {
    format.dateMYY(startDt, direction=-1)
  } else if (noMonths < 12) {
    paste( format.dateMYY(startDt, direction=-1), "-", noMonths, "M", sep="")
  } else if (noMonths == 12) {
    paste( format.dateMYY(startDt, direction=-1), "-1Y", sep="")
  } else if (noMonths == 24) {
    paste( format.dateMYY(startDt, direction=-1), "-2Y", sep="")
  }
  
  if (round != 0)
    name <- paste(name, "-R", round, sep="")
  
  structure(list(auctionName = name,
                 startDt = startDt,
                 endDt = endDt,
                 round = round, 
                 noMonths = length(months),
                 months = months,
                 region = region),
            class="CRR_Auction")
}

#############################################################
# have an as.data.frame method
as.data.frame.CRR_Auction <- function(x, ...) {
  data.frame(auctionName = x$auctionName,
             startDt     = x$startDt,
             endDt       = x$endDt,
             round       = x$round,
             noMonths    = x$noMonths,
             region      = x$region)
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
  switch(region,
         NEPOOL = parse_auctionName.NEPOOL(auctionName),
         NYPP   = parse_auctionName.NYPP(auctionName),
         SPP    = {
           auction <- parse_auctionName.NEPOOL(auctionName)
           auction$region <- "SPP"
           auction
         }, 
         "Unsupported region!")
  
}

parse_auctionName.NEPOOL <- function(auctionName)
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
                   region = "NEPOOL"),
            class=c("FTR_Auction", "CRR_Auction"))
    
  })

  if (length(aux) == 1)
    aux <- aux[[1]]

  aux
}

#################################################################
# Given a vector of auction names
# return a vector of TCC_Auction objects
# An auction name is made of at most 3 tokens separated by "-" 
# 1) the start date format YMM
# 2) OPTIONAL, the length of the auction in months "3M", or "1Y" OR
#    the round number, "R1"
# 3) OPTIONAL, the round number in format Rn, where n is an integer.
#
# parse_auctionName( c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2",
#                      "M13-12M", "M13-6M-R1") )
#
parse_auctionName.NYPP <- function(auctionName, region="NYPP")
{
  aux <- lapply(auctionName, function(x) {
    aux <- strsplit(x, "-")[[1]]
    startDt <- format.dateMYY(x, direction=1)

    
    if (length(aux) == 1) {  # it's a month style: F11
      endDt <-  nextMonth(startDt)-1
      noMonths <- 1
      round    <- 0
      
    } else if (length(aux) == 2) {  # it's a month M13-R3, or F10-1Y

      if (grepl("^R", aux[2])) {       # it's a month M13-R3
        endDt <-  nextMonth(startDt)-1
        noMonths <- 1
        round <- as.numeric(gsub("^R", "", aux[2]))

      } else if (grepl("Y$", aux[2])) {   # it's a  yearly auction
        if (aux[2] == "1Y") {
          endDt <- seq(startDt, by="12 months", length.out=2)[2]-1
          noMonths <- 12
        } else if (aux[2] == "2Y") {
          endDt <- seq(startDt, by="24 months", length.out=2)[2]-1
          noMonths <- 24
        }
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
      round <- as.numeric(gsub("^R", "", aux[3]))
      if (grepl("Y", aux[2])) {
        if (aux[2] == "1Y") {
          endDt <- seq(startDt, by="12 months", length.out=2)[2]-1 
          noMonths <- 12
        }  else if (aux[2] == "2Y") {
          endDt <- seq(startDt, by="24 months", length.out=2)[2]-1
          noMonths <- 24
        }
         
      } else if (grepl("M", aux[2])) {
        noMonths <- as.numeric(gsub("([[:digit:]]+)M", "\\1", aux[2])) 
        endDt <- seq(startDt, by=paste(noMonths, "month"), length.out=2)[2]-1
         
      } else {
        stop("Don't know how to parse ", x)
      }
       
    } else {
      stop("Cannot have more than 3 - in auctionName ", x)
    }

    months <- seq(startDt, by="1 month", length.out=noMonths)

    if (region=="NYPP" && !(noMonths %in% c(1,6,12,24))) {
      warning("NYPP long term auctions only have 6 months terms, not ", noMonths, "months.")
    }
    
    if (region=="NYPP" && noMonths %in% c(6,12,24) &&
        !(format(startDt, "%b") %in% c("May", "Nov")))  {
      warning("NYPP 6,12,24-month auctions start on May or Nov.  Check your start month.")
    }
    
    structure(list(auctionName = x,
                   startDt = startDt,
                   endDt = endDt,
                   round = round,
                   noMonths = noMonths,
                   months = months,
                   region = region),
            class=c("TCC_Auction","CRR_Auction"))
    
  })

  if (length(aux) == 1)
    aux <- aux[[1]]

  aux
}


#################################################################
# Get the past auctions that are saved in the archive. 
# @param region name
# @param startDt
# @param fromDt
# @param noMonths
# @param round
# @return a list of CRR_Auction objects
#
archived_auctions <- function(region="NEPOOL",
                              startDt=NULL,
                              fromDt=NULL,
                              noMonths=NULL,
                              round=NULL)
{
  CP <- .load_CP_archive(region=region)
  uAuctions <- unique(CP$auction)

  auctions <- parse_auctionName( uAuctions, region=region )
  if (inherits(auctions, "CRR_Auction"))
    auctions <- list(auctions)
  
  if (!is.null(startDt)) {
    auctions <- Filter( function(auction) {
      if (auction$startDt == startDt) TRUE else FALSE
      }, auctions)
  }
  
  if (!is.null(fromDt)) {
    auctions <- Filter( function(auction) {
      if (auction$startDt >= fromDt) TRUE else FALSE
      }, auctions)
  }

  if (!is.null(noMonths)) {
    auctions <- Filter( function(auction) {
      if (auction$noMonths == noMonths) TRUE else FALSE
      }, auctions)
  }

  if (!is.null(round)) {
    auctions <- Filter( function(auction) {
      if (auction$round == round) TRUE else FALSE
      }, auctions)
  }

  auctions
}


#################################################################
# Count the hours in an auction
# @param auctionName a vector of auctionNames
# @param buckets the name of the buckets
# @return a data.frame with columns "class.type", "auction", "hours"
#
count_hours_auction <- function(auctionName=c("F11", "F11-1Y"), region="NEPOOL",
  buckets=c("FLAT", "PEAK", "OFFPEAK"))
{
  auctionObj <- parse_auctionName(auctionName, region=region)
  if (length(auctionName)==1)
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
    aux <- cbind(bucket=buckets[ind.b], aux)

    res[[ind.b]] <- aux
  }

  res <- do.call(rbind, res)
  res$bucket <- toupper(as.character(res$bucket))

  
  res
}
