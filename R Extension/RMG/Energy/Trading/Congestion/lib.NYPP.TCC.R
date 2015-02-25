# Utilities to deal with archiving of clearing prices, settle prices
#
# ..format_clearingprices_zema_table
# .make_valid_ptids                     -- the points you are allowed to bid on!
# .read_clearingprices_zema
# update_clearingprice_archive
#


#################################################################
# Return the months for the given season
# @param season is "Summer YYYY" or "Autumn YYYY",
#    "Winter YYYY-YYYY"

.season_months <- function(season)
{
  
}


#################################################################
# Calculate the clearing price in $/MWh and auctionName from
# Zema data.
# @param data is the data.frame from the query
# @return a data.frame
#
..format_clearingprices_zema_table <- function(data)
{
  # make the auctionName
  DD <- unique(data[,c("opr_date", "auction", "season", "period",
                       "round", "onemonth")])
  DD <- DD[order(DD$opr_date), ]
  DD$auctionName <- NA

  for (r in 1:nrow(DD)) {
    season <- strsplit(DD[r, "season"], " ")[[1]]
    
    if (DD[r, "period"] == "One Month") {
      month <- DD[r, "onemonth"]
      if (season[1] == "Summer") {
        # May, Jun, Jul, Aug, Sep, Oct
        year <- season[2]
      } else if (season[1] == "Winter") {
        # Nov to Apr
        year <- as.numeric(substring(season[2], 1, 4))
        if (!(month %in% c("November", "December"))) 
          year <- year + 1
      }
      startDt <- as.Date(paste("1-", month, "-", year, sep=""),
                           format="%d-%B-%Y")
      auctionName <- paste(format.dateMYY(startDt, direction=-1),
                          sep="-")
      
    } else {
      if (season[1] == "Autumn") {
        month <- "November"
      } else if (season[1] == "Spring") {
        month <- "May"
      } else {
        stop("Long term auctions should be only in Autumn or Spring season!")
      }
      year <- as.numeric(substring(season[2], 1, 4))
      startDt <- as.Date(paste("1-", month, "-", year, sep=""),
                           format="%d-%B-%Y")
      if (DD[r, "period"] == "Six Month") {
        period <- "6M"
      
      } else if (DD[r, "period"] == "One Year") {
        period <- "1Y"
      
      } else if (DD[r, "period"] == "Two Year") {
        period <- "2Y"
      
      } else {
        stop("Unknown period.")
      }
      
      if (DD[r, "round"] == "Not Available") {
        stop("A long term auction should have a round!")
      } else {
        round <- paste("R", DD[r, "round"], sep="")
      }
      auctionName <- paste(format.dateMYY(startDt, direction=-1),
                           period, round, sep="-")
    }
    
    DD[r, "auctionName"] <- auctionName
  }
  
  if (any(is.na(DD$auctionName))) 
    stop("Failed to construct auctionName")

  # add the hours to compute the price $/MWh
  HRS <- count_hours_auction(DD$auctionName, buckets="FLAT")
  colnames(HRS)[2] <- "auctionName"
  
  # add the auction price
  CP <- merge(data, DD)

  # add the hours
  CP <- merge(CP, HRS[,c("auctionName", "hours")],
                by="auctionName")
  CP$clearing.price.per.hour <- CP$price/CP$hours
  CP$class.type <- "FLAT"
  CP <- CP[, c("auctionName", "class.type", "ptid",
                   "price", "clearing.price.per.hour")]
  colnames(CP)[1] <- "auction"
  
  CP
}


#################################################################
# Read the latest pdf file and save the valid points to a file
# @param filename is the path to the pdf 
# @param fromDate and R Date from when the points are valid
# 
.make_valid_ptids <- function(filename, fromDate)
{
  source("H:/user/R/RMG/Utilities/read.pdf.R")
  x <- read.pdf(filename)

  res <- NULL
  i <- 1
  while (i <= length(x)) {
    if (grepl("^PTID Name", x[i])) {
      aux <- takeWhile(x[(i+1):length(x)], function(y) y != "")
      res <- c(res, aux)
      i <- i + length(aux)
    } else {       
      i <- i+1
    }
  }
  # zones come after!  what a silly design
  ind <- grep("^CAPITL", x)
  res <- c(res, takeWhile(x[(ind):length(x)], function(y) y != ""))
  
  y <- do.call(rbind, strsplit(res, "[[:space:]]{2,}"))

  y <- data.frame(ptid     = as.numeric(y[,2]),
                  fromDate = fromDate)
  y <- y[order(y$fromDate, y$ptid),]

  
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/"
  filename <- paste(DIR, "valid_ptids.RData", sep="")

  if (file.exists(filename)) {
    load(filename)
    if (!(fromDate %in% unique(valid_ptids$fromDate))) {
      valid_ptids <- rbind( valid_ptids, y)
      rownames(valid_ptids) <- NULL
     }
  } else {
    valid_ptids <- y
  }

   
  save(valid_ptids, file=filename)
  rLog("Wrote file", filename)
}


#################################################################
# Use Zema 
# @param days a vector of R dates that correspond to the OPR_DATE
#   in the table.
# @param season a string with the season name, e.g. 'Winter 2013-2014'
#
.read_clearingprices_zema <- function(days=NULL, season=NULL)
{
  require(RODBC)

  query <- paste(
    "select opr_date, auction, season, period, round, onemonth, ptid, price ",
    "from ZE_DATA.NY_ISO_FTR_NODAL_PRICES ",    
    sep="")
  #  "where season = 'Winter 2011-2012' ",
  #  "and onemonth = 'February'",

  filterWord <- "where"
  
  if (!is.null(days)) {
    aux <- paste(toupper(format(days, "%d-%b-%y")), sep="", collapse="', '")
    aux <- paste(filterWord, " opr_date in ('", aux, "')")
    query <- paste(query, aux, sep="")
    filterWord <- "and"
  }
  
  rLog("Querying ZEMA ...")
  con.string <- paste("FileDSN=",
    fix_fileDSN("S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMAP.dsn"),
    ";UID=ze_read;PWD=ze_read;", sep="")

  con <- odbcDriverConnect(con.string)
  if (con == -1) {
    rLog("Cannot connect to DB.  Exiting ...")
    q(save="no", 99)
  }
  
  data <- sqlQuery(con, query) 
  odbcCloseAll()

  colnames(data) <- tolower(colnames(data))
  
  
  ..format_clearingprices_zema_table(data)
}


#################################################################
# 
#
update_clearingprice_archive <- function()
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/"
  filename <- paste(DIR, "ftrcp.RData", sep="")

  # get all the data from the database
  CP <- .read_clearingprices_zema()

  save(CP, file=filename)
  rLog("Wrote file", filename)
}



#################################################################
#################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(SecDb)
  #require(FTR)
  require(CRR)
  load_ISO_environment("NYPP")

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.TCC.R")


  #update_awards_archive(files)

  update_clearingprice_archive()


  

  filename <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/TCC/ValidPoints/RawData/att_e_poi_pow_centralized_reconfig_tcc_auctions_AUT2014_CLEAN.pdf")
  fromDate <- as.Date("2014-11-01")
  .make_valid_ptids(filename, fromDate)
  

  
}
