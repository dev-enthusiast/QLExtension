# download the LFRM files, etc. 
#
# .archive_lfrm_requirements
#
# .download_lfrm_offers
# .download_lfrm_requirements
# .download_lfrm_results
#
# .make_lfrm_csvfile
#
# .read_lfrm_offers
# .read_lfrm_requirements
# .read_lfrm_results
#



############################################################################
# Read the lfrm requirements xls files and archive them
#
.archive_lfrm_requirements <- function(archive=FALSE)
{
  require(xlsx)
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FwdRes/Requirements/Raw"

  files <- list.files(DIR, full.names=TRUE)

  read.one <- function(fname) {      
    rLog("  Reading", fname)
    wb     <- loadWorkbook(fname)
    sheets <- getSheets(wb)

    res <- vector("list", length=length(sheets))
    for (i in 1:length(sheets)) {
      rLog("reading sheet ", i)
      sheet <- sheets[[i]]
      zone  <- names(sheets)[i]
      
      aux <- readColumns(sheet, startColumn=1, endColumn=5, startRow=8, endRow=9,
                         colClasses="character", header=FALSE)
   
      if ((nrow(aux) == 1) || (aux[1,1] == "")) {
        ind <- 9
      } else {
        ind <- 8
      }

       
      noCols <- 5
      aux <- readColumns(sheet, startColumn=1, endColumn=noCols, startRow=ind,
        colClasses=c("Date", rep("numeric", 3)))
      if (is.na(aux[1,1])) {
        aux <- aux[,-1]
      } else {
        aux <- aux[,-5]
      }
      
      bux <- cbind(aux, zone=zone)
      names(bux) <- c("date", "n-2.contingency.req", "ers", "lrr", "zone")
      if (class(bux[,1]) != "Date")
        bux[,1] <- as.Date(bux[,1]-25569, origin="1970-01-01")
      bux <- subset(bux, !is.na(date))

      res[[i]] <- bux
    }
    #lapply(res, head)

    y <- do.call(rbind, res)

    # calculate the auction this data is for ... 
    minDate <- min(y$date)
    m <- as.numeric(format(minDate, "%m"))
    season <- if (m %in% 6:9) "S" else "W"
    yyyy <- as.numeric(format(minDate, "%Y"))
     
    forAuction <- if (season == "S") {
      paste(season, substr(yyyy+2, 3, 4), sep="")
    } else {
      paste(season, substr(yyyy+2, 3, 4), "-", substr(yyyy+3, 3, 4), sep="")
    }    
    y$forAuction <- forAuction
    
    y
  }

  #aux <- read.one(files[9])
  
  res <- lapply(files, read.one)
  #lapply(res, head)

  lrr <- do.call(rbind, res)
  lrr <- lrr[order(lrr$zone, lrr$date), ]
  lrr$date <- trunc(lrr$date)   # it is not dates!  it's datetimes in the xls!!!!
  rownames(lrr) <- NULL

  if (archive) {
    DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
    fname <- paste(DIR, "hist_local_reserve_requirements.RData", sep="")
    save(lrr, file=fname)
    rLog("Archived ", fname)
  }

  lrr
}



############################################################################
# the historical offer data.  Iso changed the format on S14
# @param auctionNames a vector of auctionNames
# 
#
.download_lfrm_offers <- function( auctionNames )
{
  DIR_OUT <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FwdRes/FRM_Offers/Raw/"
  
  for (auctionName in auctionNames) {
    auction <- ForwardReserveAuction( auctionName )
    yyyymm <- format(auction$startDate, "%Y%m")
    link <- paste("http://www.iso-ne.com/transform/csv/hbforwardreserveauctionoffer?season=",
                  yyyymm, sep="")
      
    .download_and_archive(link, DIR_OUT=DIR_OUT,
                          filenames=paste("hbfwdrsvauction_", yyyymm, ".csv", sep=""))
  }
}



############################################################################
# the historical clearing price data.
# Iso changed the format with S14.
# @param auctionNames a vector of auctionNames
# @return void
#
.download_lfrm_requirements <- function(auctionNames)
{
  DIR_OUT <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FwdRes/Requirements/Raw/"
  url <- paste(
    "http://www.iso-ne.com/markets/othrmkts_data/res_mkt/cal_assump/",
     year, "/index.html", sep="")
  pattern <- "xls"

  links <- .getLinksInPage(url, pattern=pattern)

  .download_and_archive(links, DIR_OUT=DIR_OUT)
}


############################################################################
# the historical clearing price data
#
.download_lfrm_results <- function(year)
{
  # Just download this by hand as the URL changes all the time!  AAD - 1/9/2015
  #
  DIR_OUT <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FwdRes/AuctionResults/Raw/"
  url <- paste(
    "http://www.iso-ne.com/markets/othrmkts_data/res_mkt/summ/",
     year, "/index.html", sep="")
  pattern <- "auction_results.csv"

  links <- .getLinksInPage(url, pattern=pattern)

  .download_and_archive(links, DIR_OUT=DIR_OUT, overwrite=TRUE)
}




############################################################################
# Read all of them at once (not that many)
#
.read_lfrm_offers <- function()
{
  DIR <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
    "FwdRes/FRM_Offers/Raw", sep="")

  files <- list.files(DIR, full.names=TRUE)

  read.one <- function(file=file) {
    if (grepl("^WW_FROFFER", basename(file))) {
      aux <- ..read_offers_1(file)
      
    } else if (grepl("hbfwdrsvauction", basename(file))) {
      aux <- ..read_offers_2(file)
      
    } else {
      stop("Don't know how to process", file)
    }
    
    aux
  }
  
  res <- finalize(files, read.one)
  res[,c(3,6:8)] <- sapply(res[,c(3,6:8)], as.numeric)
  res <- res[,-2]


  # add the auctionName
  aux <- data.frame(startDate=unique(res$startDate),
               auctionName=fwdres_auction(unique(res$startDate)))
  aux$season <- sapply(lapply(aux$auctionName, ForwardReserveAuction), "[[", "season")
  res <- merge(res, aux)
  colnames(res)[which(colnames(res)=="Reserve.Zone")] <- "zone"

## > head(res)
##    startDate Masked.Customer.ID zone Reserve.Product.Type Segment.Number Segment.Quantity Segment.Price auctionName season
## 1 2007-06-01             140603 NEMA                 TMOR              1               27          8000         S07 Summer
## 2 2007-06-01             140603 NEMA                 TMOR              2               27          9500         S07 Summer
## 3 2007-06-01             140603 NEMA                 TMOR              3               28         11000         S07 Summer
## 4 2007-06-01             140603 NEMA                 TMOR              4               28         13995         S07 Summer
## 5 2007-06-01             140603  ROS                TMNSR              1               15         13995         S07 Summer
## 6 2007-06-01             140603  ROS                 TMOR              1               20         13995         S07 Summer


  res
}



############################################################################
# For the old format use the lib.NEPOOL.reports.R technology
#
..read_offers_1 <- function( fname )
{
  aux <- .process_report(fname)[[1]]
  aux <- cbind(startDate=as.Date(substring(basename(fname), 16, 23),
        format="%Y%m%d"), aux)
  aux$Reserve.Zone <- ifelse(aux$Reserve.Zone == "NEMA/Boston", "NEMA",
    ifelse(aux$Reserve.Zone == "Rest Of System", "ROS",
    ifelse(aux$Reserve.Zone == "Southwest Connecticut", "SWCT",
    ifelse(aux$Reserve.Zone == "Connecticut", "CT", NA))))
  
## > head(aux)
##    startDate H Masked.Customer.ID Reserve.Zone Reserve.Product.Type Segment.Number Segment.Price Segment.Quantity
## 1 2007-06-01 D             140603         NEMA                 TMOR              1          8000               27
## 2 2007-06-01 D             140603         NEMA                 TMOR              2          9500               27
## 3 2007-06-01 D             140603         NEMA                 TMOR              3         11000               28
## 4 2007-06-01 D             140603         NEMA                 TMOR              4         13995               28
## 5 2007-06-01 D             140603          ROS                TMNSR              1         13995               15
## 6 2007-06-01 D             140603          ROS                 TMOR              1         13995               20

  aux
}


############################################################################
# For the new format (started with S14)
#
..read_offers_2 <- function( fname )
{
  aux <- .process_report(fname)[[1]]

  x <- melt(aux, id=1:6)
  x$segment <- gsub("Segment\\.([[:digit:]]+)\\..*", "\\1", x$variable)
  x$name <- gsub("Segment\\.([[:digit:]]+)\\.(.*)", "Segment.\\2", x$variable)
  x <- subset(x, !is.na(value))
  x$variable <- NULL

  y <- cast(x, H + Masked.Customer.ID + Reserve.Zone + Product.Type + segment ~ name,
            I, value="value", fill=NA)
  colnames(y)[4:7] <- c("Reserve.Product.Type", "Segment.Number", "Segment.Quantity", "Segment.Price")
  
  y <- cbind(startDate=as.Date(x$Season[1], "%m/%d/%Y"), y)
## > head(y)
##    startDate H Masked.Customer.ID Reserve.Zone Reserve.Product.Type Segment.Number Segment.Quantity Segment.Price
## 1 2014-06-01 D             172669          ROS                TMNSR              1             38.9          3315
## 2 2014-06-01 D             206845          ROS                TMNSR              1             20.0          3709
## 3 2014-06-01 D             355376          ROS                TMNSR              1             27.5          3559
## 4 2014-06-01 D             355376          ROS                TMNSR              2             27.5          4458
## 5 2014-06-01 D             412080          ROS                TMNSR              1             58.0          4189
## 6 2014-06-01 D             412080          ROS                TMNSR              2             60.0          4199

  
  y
}



############################################################################
# the auction clearing prices, etc.  formatted so they are ready to use
#
.read_lfrm_results <- function()
{
  DIR <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/",
    "FwdRes/AuctionResults/Raw/", sep="")

  DD <- read.csv(paste(DIR, "forward_reserve_auction_results.csv", sep=""),
                 skip=3)
  DD <- DD[which(DD[,"H"]=="D"),]
  DD$H <- NULL
  
  names(DD) <- gsub("\\.+", "\\.", names(DD))
  names(DD) <- gsub("\\.$", "", names(DD))
  names(DD) <- gsub("^Forward\\.", "", names(DD))

  DD$startDate <- as.Date(paste("1/", DD$Procurement.Period.Begin.Month),
    format="%d/%m/%Y")
  DD$Procurement.Period.Begin.Month <- NULL

  DD$zone <- ifelse(DD$Reserve.Zone.Name == "NEMABSTN", "NEMA",
    ifelse(DD$Reserve.Zone.Name == "ROS", "ROS",
    ifelse(DD$Reserve.Zone.Name == "SWCT", "SWCT",
    ifelse(DD$Reserve.Zone.Name == "CT", "CT", NA))))
  DD$Reserve.Zone.Name <- NULL

  DD$auctionName <- fwdres_auction(DD$startDate)

  DD
}



#########################################################################
#########################################################################
#
# For analysis of OFFERS,
#   see Nepool/ISO_Data/ForwardReserveAuction/main_frm_auctions.R
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(zoo)
  require(SecDb)
  require(lattice)
  require(PM)
  require(DivideAndConquer)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R") 
  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Ancillaries/lib.fwd.reserves.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.LFRM.R")

  ################################################################
  # before the auction
  auctionNames <- c("S14")
  
  RR <- .download_lfrm_requirements( auctionNames )
  
  lrr <- .archive_lfrm_requirements(archive=FALSE)


  
  ################################################################
  # bidding behavior
  yearStart <- as.numeric(format(Sys.Date(), "%Y"))-5
  ccp <- seq(yearStart, yearStart+6, by=1)
  for (year in ccp) {
    rLog("Working on auction starting", year)
    .download_lfrm_offers(year)
  }

  .download_lfrm_offers(2013)
  results <- .download_lfrm_results( 2013 )

  
  
  HH <- .read_lfrm_offers()
  CP <- .read_lfrm_results()




  fname <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/hist_local_reserve_requirements.RData"
  load(fname)  # loads lrr data.frame
  lrr <- subset(lrr, forAuction != "S07")

  # historical requirements by zone
  aux <- cast(lrr, forAuction ~ zone, function(x){
    round(quantile(x, probs=c(0.95)))},
       value="lrr", fill=NA)
  print(aux, row.names=FALSE)

  
  require(lattice)
  xyplot(lrr ~ date, data=lrr, type=c("g","o"), 
         subset=zone=="NEMABSTN" & date >= as.Date("2008-01-01") )
  xyplot(ers ~ date, data=lrr, type=c("g","o"), 
         subset=zone=="NEMABSTN" & date >= as.Date("2008-01-01") )


  auctionName <- "S14"
  ddply(subset(lrr, forAuction == "S14"), c("zone"), function(x){
    quantile(x$ers, probs=c(0, 0.01, 0.02, 0.05, 0.50, 0.95, 1))
  })
  
  
  x <- subset(lrr, forAuction == "S14" & zone == "NEMABSTN")
  x <- x[order(-x$ers), ]
  

  
  summary(x$lrr)
  quantile(x$lrr, probs=c(0.95))
  

  ddply(lrr, c("forAuction", "zone"), function(x) {
    prb <- round(quantile(x$lrr, probs=c(0.80, 0.90, 0.95, 0.99)))
    c(summary(x$lrr), prb)
  })


  
  # download preliminary historical requirements data
  # http://www.iso-ne.com/markets/othrmkts_data/res_mkt/prlnry_hs_data/index.html



    

  
}
