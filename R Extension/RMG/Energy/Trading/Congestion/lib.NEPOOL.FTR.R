# Download and Archive FTR's 
# moved functionality from lib.NEPOOL.iso.R
#
#
# .download_csv_from_url
# .get_auction_from_filename
# .get_filename_from_auction
#
# download_FTR_awards
# download_FTR_clearedprices
# download_FTR_FA                 - financial assurance
#
# get_ftr_trades                  - what you have booked
#
# make_SP_archive                 - calculate settle prices and store them
# make_LT_FTR_auction_bids_RData
# make_FTR_interface_limit_archive 
#
#
# .read_DALMP_file        <- the financial assurance for FTRs
# .read_FTRCP_file        <- the clearing prices file 
# .read_FTRSLT_file       <- the awards file
#
# update_awards_archive
# update_clearingprice_archive
#



#################################################################
# Find the auctions that overlap a given date. 
# e.g. for date = "2013-04-10", you can have J13-R1, J13-R2, ... F13-1Y-R1, F13-1Y-R2
# Where
#   date - the date you want to check, an R Date
#   auctions - the auctions you want to look through
#
auctions_in_date <- function( date, auctions=NULL )
{
  bom  <- currentMonth(date)
  eom  <- nextMonth(date)-1
  year <- format(date, "%Y")
  monthlyPattern <- FTR_Auction(bom, eom)$auctionName
  yearlyPattern  <- FTR_Auction(as.Date(paste(year, "-01-01", sep="")),
                                as.Date(paste(year, "-12-31", sep="")))$auctionName
  
  if (!is.null(auctions)) {
    res <- c(grep(monthlyPattern, auctions, value=TRUE),
             grep(yearlyPattern, auctions, value=TRUE))
    
  } else {
    stop("Argument auction should not be NULL for now!")
  }

  res
}







#################################################################
# returns a data.frame with the data
#
## .download_csv_from_url <- function(link, skip.lines=0)
## {
##   rLog("DEPRECATED.  Use lib.NEPOOL.downloads.R!")
##   con <- url(link)
##   on.exit(close(con))
  
##   res <- try(open(con), silent=T)
##   if (class(res)[1] == "try-error"){
##     rLog(paste("Cannot read url", link))
##     close(con)
##     next
##   }
##   rLog(paste("Reading", link))
##   res <- NULL
##   res <- try(read.csv(con, sep=",", skip=skip.lines))
##   if (class(res)[1]=="error"){return(NULL)}
##   res[res[,1]=="D", ]
## }



#################################################################
# it's vectorized!
# return a valid auction name
#
.get_auction_from_filename <- function(filename)
{
  fname <- basename(filename)
  aux <- gsub(".*_(.*)\\.csv", "\\1", fname)  # term

  aux
}


#################################################################
# create the url for a given auction name
# not used anymore
#
.get_url <- function(month)
{
  site  <- "http://www.iso-ne.com/markets/othrmkts_data/ftr/auction_results/"

  folder <- paste(site, format(month, "%Y"), "/",
    tolower(format(month, "%b")), "/", sep="")

  paste(folder, "index.html", sep="")
}



#################################################################
# download and return the name of the file you want
# if there's nothing to download, just return the filename
#
download_FTR_awards <- function(months)
{  
  fileroot <- "ftrslt_"
  DIRCSV   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTRmodel/"
  URL <- "http://www.iso-ne.com/static-assets/documents/" #2014/09/ftrslt_oct2014.csv"
  
  files <- NULL
  for (month in format(months)) {
    links   <- paste(URL, format(currentMonth(), "%Y/%m"), "/ftrslt_",
                     tolower(format(month, "%b%Y")), ".csv", sep="")
    aux     <- .download_and_archive(links, DIRCSV)
    if (is.null(aux) & !is.null(links))
      aux <- basename(links)  # you already have them!
    files <- c(files, aux)
  }

  files
}


#################################################################
#
download_FTR_clearedprices <- function(months)
{
  fileroot <- "ftrcp_"
  DIRCSV   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTRmodel/"
  URL <- "http://www.iso-ne.com/static-assets/documents/"
  
  files <- NULL
  for (month in format(months)) {
    links   <- paste(URL, format(currentMonth(), "%Y/%m"), "/ftrcp_",
                     tolower(format(month, "%b%Y")), ".csv", sep="")
    aux     <- .download_and_archive(links, DIRCSV)
    if (is.null(aux)  & !is.null(links))
      aux <- basename(links)  # you already have them!
    files <- c(files, aux)
  }

  files
}


################################################################################
# Download financial assurance (credit requirements)
#
download_FTR_FA <- function( year=2012 )
{
  site  <- paste("http://www.iso-ne.com/stlmnts/assur_crdt/reports/dalmpc/",
    year, "/", sep="") 

  links <- .getLinksInPage(site, pattern="WW_DALMPC_ISO_")
  
  DIR_OUT <-  "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTR_FA/Raw/"
  .download_and_archive(links, DIR_OUT, prefix="")  
}


################################################################################
# Get the booked ftr position
#
get_ftr_trades <- function(auctions=c("K11", "F11-1Y"))
{
  fname <- paste("S:/All/Structured Risk/NEPOOL/FTRs/RawData/",
    "FTR.paths.xlsx", sep="")

  aux <- read.xlsx(fname, sheetName="Paths", colIndex=1:13)
  aux <- aux[which(!is.na(aux[,1])),]
  
  aux <- subset(aux, auction %in% auctions)
  aux <- subset(aux, start.dt <= Sys.Date() ) # may have some future oblig!

  aux
}

##########################################################################
# Calculate congestion settle prices by year and month
# and save it to a file
# 
make_SP_archive <- function()
{
  startDt <- as.POSIXct("2007-01-01 01:00:00")
  endDt <- Sys.time()
  
  ptids <- NEPOOL$MAP$ptid
  #ptids <- ptids[1:10]

  res <- lapply(ptids, function(ptid)
  {  
    rLog("Working on ptid", ptid)
    symb <- paste("nepool_smd_da_", ptid, "_congcomp", sep="")
    hp <- tsdb.readCurve(symb, startDt, endDt)
    if (nrow(hp) == 0)
      return( NULL )
      
    hp$year  <- as.numeric(format(hp$time-1, "%Y"))
    hp$month <- as.Date(format(hp$time-1, "%Y-%m-01"))
    bucketMask <- secdb.getBucketMask("NEPOOL", "Peak", hp$time[1], hp$time[nrow(hp)]) 
    bucketMask$bucket <- ifelse(bucketMask$Peak, "ONPEAK", "OFFPEAK")
    hpB <- merge(hp, bucketMask[,c("time", "bucket")], by="time")

    mm <- cast(hpB, bucket + month ~ ., mean, value="value")
    mm <- cbind(ptid=ptid, as.data.frame(mm))

    yy <- cast(hpB, bucket + year ~ ., mean, value="value")
    yy <- cbind(ptid=ptid, as.data.frame(yy))
 
    list(month = mm,
         year  = yy)
  })

  SP.month <- do.call(rbind, lapply(res, "[[", "month"))
  colnames(SP.month)[ncol(SP.month)] <- "SP"
  SP.year <- do.call(rbind, lapply(res, "[[", "year"))
  colnames(SP.year)[ncol(SP.year)] <- "SP"

  SP <- list(month=SP.month,
             year=SP.year,
             lastUpdated=Sys.time())
  
  dir.rdata <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"
  filename <- paste(dir.rdata, "ftrsp.RData", sep="")
  save(SP, file=filename)
  rLog("Saved settle price archive to", filename)
  
  
}


##########################################################################
# Aggregate the ISO csv files and save to LT_FTR_BIDS.RData in package FTR
#
make_LT_FTR_auction_bids_RData <- function()
{
  require(DivideAndConquer)

  rLog("Check if new files need to be downloaded!")
  dir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTRmodel/Bids/LongTerm/Raw/"
  #files <- list.files(dir, pattern="WW_FTRLT.*", full.names=TRUE)
  files <- list.files(dir, pattern="hbftrannualauction_.*", full.names=TRUE)

  cFun <- function(file=file){
    aux <- read.csv(file, skip=4)
    aux <- subset(aux, H=="D")
    year <- gsub("hbftrannualauction_20([[:digit:]]{2}).*", "\\1", basename(file))
    round <- ""                   # for older files there was no round
    if ("Auction.Name" %in% colnames(aux)) {
      if (grepl("^LT", aux$Auction.Name[1])) {
        round <- paste("-R", gsub("LT(.*) .*", "\\1", aux$Auction.Name), sep="")
      } 
    }
    
    aux <- aux[,c(3,6,9:13)]
    aux  <- cbind(auction=paste("F", year, "-1Y", round, sep=""), aux)
    aux
  }
  
  res <- finalize(files, cFun)
  names(res)[2:4] <- c("source.ptid", "sink.ptid", "buy.sell")
  names(res) <- tolower(names(res))
  res[,c(2,3,6,7)] <- sapply(res[,c(2,3,6,7)], as.numeric)

  LT_FTR_BIDS <- res
  fName <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/LT_FTR_BIDS.RData"
  save(LT_FTR_BIDS, file=fName)
  rLog("Saved file",  fName)
}



##########################################################################
# Read the pdfs and make an RData file with the interface info that
# have been modeled in the FTR auction
# See also NEPOOL/lib.NEPOOL.other.R
#   function: RPT$move.FTR.interfaces.pdf.to.archive
#
# Extract the limits only!
#
make_FTR_interface_limit_archive <- function()
{
  require(CEGbase)
  source("H:/user/R/RMG/Utilities/read.pdf.R")

  baseDir <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/"
  dirIn <- paste(baseDir, "FTRmodel/Interfaces", sep="")
  files <- list.files(dirIn, full.names=TRUE, pattern="\\.pdf$")

  TT <- vector("list", length=length(files))
  for (f in files){
    rLog("Processing file:", f)  
    aux <- toupper(read.pdf(f))
    auction <- aux[grep("FTR AUCTION:", aux)][1]  # usually there are 2
    auction <- gsub(".*: (.*)", "\\1", auction, perl=TRUE)
    if (length(auction) != 1){
      browser()
      rError("Cannot find auction:", auction)
    }
    aux[aux==""] <- "                "  # why do I have this here?! 3/15/2010

    
    # Find the index of the on peak lines.  Above them are the interface
    # names.  One row below is the limit on the offpeak.
    # some interface names are split on two rows ... silly!
    indOnpeak  <- grep("ON PEAK LIMIT", aux)
    interfaces <- rep("", length(indOnpeak))
    for (i in 1:length(indOnpeak)){
      indStart <- indOnpeak[i]-1
      bux <- aux[indStart]      
      while (substr(bux, 1, 16) != "                "){
        interfaces[i] <- paste(bux, interfaces[i])
        indStart <- indStart-1
        bux <- aux[indStart]
      }
    }
    
    interfaces <- gsub("^ +", "", interfaces)    # remove spaces in front
    interfaces <- gsub("  +", " ", interfaces)   # remove multiple spaces
    interfaces <- gsub("\f ", "", interfaces)    # some rogue characters
    # some interfaces have soft hyphens vs. hyphens.  Cannot distinguish
    # except if you to a do a charToRaw("-")
    interfaces <- gsub("\xad", "\x2d", interfaces)
    # remove spaces around the operators - and +
    interfaces <- gsub(" ?- ?", "-", interfaces, perl=TRUE)
    interfaces <- gsub(" ?\\+ ?", "+", interfaces, perl=TRUE)

    onLmt <- as.numeric(gsub(".* ([[:digit:]]+) .*", "\\1", aux[indOnpeak],
      perl=TRUE))
    offLmt <- as.numeric(gsub(".* ([[:digit:]]+) .*", "\\1", aux[indOnpeak+1],
      perl=TRUE))
    if (length(onLmt) != length(offLmt))
      stop("Not equal number of onpeak and offpeak limits!")
    if (length(onLmt) != length(interfaces))
      stop("Not equal number of interfaces and limits!")
    
    TT[[f]] <- data.frame(auctionName=auction, interface=interfaces,
      onpeakLimit=onLmt, offpeakLimit=offLmt)
  }

  # some issues with Nov 2004.  I don't care now....
  TT <- do.call(rbind, TT)
  rownames(TT) <- NULL

  TT$auctionName <- gsub(",.*", "", TT$auctionName)     # some revisions
  TT$auctionName <- gsub("\\(.*$", "", TT$auctionName)  # some revisions
  TT$auctionName <- gsub("\xad", "\x2d", TT$auctionName)
  
  
  uAN <- ldply(unique(TT$auctionName), function(x){
    if (grepl("-", x)){
      aux <- paste("1", gsub(" +", "", strsplit(x, "-")[[1]]), sep="")
      y <- c(as.Date(aux[1], "%d%B%Y"), as.Date(aux[2], "%d%B%Y"))
    } else {
      y <- as.Date(paste("1", x), "%d %B %Y")
      y <- c(y, y)
    }
    data.frame(auctionName=x, startDate=y[1], endDate=y[2])
  })
  uAN$auction <- FTR.AuctionTerm(uAN$startDate, uAN$endDate)

  TT <- merge(TT, uAN[,c("auctionName", "auction")], by="auctionName")
  comment(TT) <- paste("Historical interface data from FTR auctions. ",
                       "Last modified on", Sys.Date())
  TT$interface <- gsub(" +$", "", TT$interface)
  InterfaceLimits <- TT
  
  fname <- paste(baseDir, "/RData/ftr.auction.interface.limits.RData", sep="")
  save(InterfaceLimits, file=fname)
  rLog("Wrote ", fname)  
}


##############################################################
# Parse a date from different strings.  You only get the month, year 
#
.parse_date <- function(x) {

  noDigits <- suppressWarnings(
    length(na.omit(as.numeric(strsplit(x, "")[[1]]))))
  
  # formats supported 
  formats <- if (noDigits == 2) {
    c("%b%y", "%B%y", "%y-%B", "%y-%b", "%y%b")
  } else if (noDigits == 4) {
    c("%b%Y", "%B%Y", "b%Y", "%Y-%b", "%Y-%B")
  } else {
    stop("Check your input, only 2 or 4 digits allowed for the year!")
  }
  
  for (fmt in formats) {
    month <- tryCatch(
      as.Date(paste("01-", x, sep=""), format=paste("%d-", fmt, sep="")),
        error=function(e) NA)
    if ( !is.na(month) )
      break
  }

  if (is.na(month))
    stop("Don't know how to parse ", x)
  
  month
}

..test.parse_date <- function()
{
  x <- c("sep09", "SEP2009", "13-Oct")
  res <- as.Date(unlist(lapply(x, .parse_date)))
  identical(res, as.Date(c("2009-09-01", "2009-09-01", "2013-10-01")))
}


##############################################################
# Nepool puts this together at
# http://www.nepool.com/stlmnts/assur_crdt/reports/dalmpc/index.html
#
.read_DALMP_file <- function(filename=NULL)
{
  if (is.null(filename)) {
    DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTR_FA/Raw/"
    files <- list.files(DIR, full.names=TRUE)
    files <- files[order(files)]
    filename <- files[ length(files) ]
  }

  rLog("Reading ", filename)
  RR <- read.csv(filename, skip=3)
  RR <- RR[-1,]
  RR <- RR[-nrow(RR),]
  RR$Month <- as.Date(RR$Month, "%m/%d/%Y")
  RR[,c(3,5,6)] <- sapply(RR[,c(3,5,6)], as.numeric)
  
  names(RR)[c(2:6)] <- c("month", "ptid", "name", "ONPEAK", "OFFPEAK")
  RR <- RR[,-1]  # useless column
  RR <- melt(RR, id=1:3)
  names(RR)[4:5] <- c("class.type", "MCC")

  RR$name <- NULL # it's useless, ISO is not consistent with the names!
  
  RR
}

##############################################################
# read one clearing prices file to be ready for archival
#
.read_FTRCP_file <- function(filename, round=0)
{
  rLog("Reading ", filename)
  
  # cannot use .process_report because no header, no quotes  
  # find the first row data starts ... 
  ind <- grep("^D", readLines(filename, n=20))[1]-2
  ind <- ifelse(is.na(ind), 0, ind)
  res <- read.csv(filename, skip=ind)
  names(res) <- tolower(names(res))
  res <- res[res[,1]=="D",]


  # figure out the term
  if ("market" %in% colnames(res)) {
    colName  <- "market"
    colNames <- c("class", "node.id", "node", "clearing.price")
  } else if ("ftr.auction" %in% colnames(res)) {
    colName <- "ftr.auction"
    colNames <- c("class.type", "node.id", "node.name", "clearing.price")
  } else {
    stop("In .read_FTRCP_file.  Cannot find the term column!")
  }
  
  term  <- gsub(" +", "", res[1, colName])
  noDigits <- suppressWarnings(
    length(na.omit(as.numeric(strsplit(term, "")[[1]]))))
  
  if (grepl("^LT", toupper(term))) {    # the new Long term auctions, LT12013
    year  <- as.numeric(substring(term, 4, 7))
    round <- as.numeric(substring(term, 3, 3))
    startDt <- as.Date(paste(year, "-01-01", sep=""))
    endDt <- as.Date(paste(year, "-12-31", sep=""))
    
  } else if (noDigits == 8) {  # yearly auction JAN2012-DEC2012
    if (grepl("-", term)) {
      aux <- lapply( strsplit(term, "-"), function(x) gsub(" ","", x))[[1]]
      year <- gsub(".*([[:digit:]]{4})", "\\1", aux[1])
    } else {
      stop("Unknown term ", term)
    } 
    startDt <- as.Date(paste(year, "-01-01", sep=""))
    endDt <- as.Date(paste(year, "-12-31", sep=""))
    
  } else {   # assume it's monthly
    if (grepl("-", term) )   # Oct-09
      term <- gsub("-", "", term)
  
    startDt <- .parse_date( term )
    endDt   <- nextMonth(startDt) - 1
  }
  
  auction <- FTR_Auction(startDt, endDt, round)
  rLog("This is auction", auction$auctionName)
  
  
  if (!all(colNames %in% names(res)))
    stop("Missing columns class, node.id, node, clearing.price in file!")
  res <- res[, colNames]
  names(res)[1:3] <- c("class.type", "ptid", "name")
  res$start.dt <- auction$startDt
  res$end.dt   <- auction$endDt
  
  res$class.type <- factor(toupper(res$class.type), levels=c("OFF", "ON"))
  levels(res$class.type) <- c("OFFPEAK", "ONPEAK")
  res$class.type <- as.character(res$class.type)
  res$auction <- auction$auctionName

  # for jun_2008 there are some commas missing!
  suppressWarnings(res$clearing.price <- as.numeric(res$clearing.price))
  
  HRS.AUCT <- count_hours_auction(auctions=auction$auctionName, "NEPOOL")

  res <- merge(res, HRS.AUCT, all.x=T)
  res$clearing.price.per.hour <- res$clearing.price/res$hours
  res$hours <- NULL   
  
  res
}

##############################################################
# 
#
.read_FTRSLT_file <- function(filename, round=0)
{
  rLog("Reading ", filename)
  
  # find the first row data starts ... 
  ind <- grep("^D", readLines(filename, n=20))[1]-2
  ind <- ifelse(is.na(ind), 0, ind)
  res <- read.csv(filename, skip=ind)
  #res <- res[, -(1:4)]   # cut come slack

  if ("FTR.Auction" %in% names(res)) {   # new format Nov-2012
    term <- gsub(" +", "", res[1, "FTR.Auction"])
  } else {  
    # figure out the auction from the header
    aux <-  read.csv(textConnection(readLines(filename, n=2)))
    term <- gsub(" +", "", aux$AUCTION.PERIOD)
  } 
  noDigits <- suppressWarnings(
    length(na.omit(as.numeric(strsplit(term, "")[[1]]))))

  if (grepl("^LT", toupper(term))) {    # the new Long term auctions, LT12013
    year  <- as.numeric(substring(term, 4, 7))
    round <- as.numeric(substring(term, 3, 3))
    startDt <- as.Date(paste(year, "-01-01", sep=""))
    endDt <- as.Date(paste(year, "-12-31", sep=""))
    
  } else if (noDigits == 8) {  # yearly auction JAN2012-DEC2012
    aux <- lapply( strsplit(term, "-"), function(x) gsub(" ","", x))[[1]]
    year <- gsub(".*([[:digit:]]{4})", "\\1", aux[1])
    startDt <- as.Date(paste(year, "-01-01", sep=""))
    endDt <- as.Date(paste(year, "-12-31", sep=""))
    
  } else {   # assume it's monthly
    if (grepl("-", term) )   # Oct-09
      term <- gsub("-", "", term)
  
    startDt <- .parse_date(term)
    endDt   <- nextMonth(startDt) - 1
  }
  
  auction <- FTR_Auction(startDt, endDt, round)
  rLog("This is auction", auction$auctionName)
 
  res$Award.FTR.Price <- as.numeric(gsub(",", "", res$Award.FTR.Price))

  # aggregate over the bids submitted
  # too big to keep the way it is, you don't see their bid price anyhow
  names(res)[which(names(res)=="Award.FTR.MW")] <- "value"
  y <- cast(res, Customer.ID + Customer.Name + Source.Location.Id +
      Source.Location.Name + Sink.Location.Id +
      Sink.Location.Name + Buy.Sell + Class.Type +
      Award.FTR.Price  ~ ., sum)
  names(y)[ncol(y)] <- "Award.FTR.MW"
  y <- y[!is.na(y$Award.FTR.Price), ]   # sometimes, empty lines in CSV, XLS

  #y$Auction.Dollars <- y$Award.FTR.MW * y$Award.FTR.Price
  names(y) <- gsub("_", "\\.", tolower(names(y)))
  names(y) <- gsub("location.", "", names(y))
  names(y) <- gsub(".id", ".ptid", names(y))
  y <- y[,-which(names(y) %in% c("customer.ptid",
     "source.name", "sink.name"))]  # don't need them
  y$auction <- auction$auctionName

  data.frame(y)
}



################################################################################
# Update the awards archive with a given auction results
#
update_awards_archive <- function(files)
{
  dir.csv <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTRmodel/"
  dir.rdata <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"

  # load the existing archive
  filenameAWD <- paste(dir.rdata, "ftrslt.RData", sep="")
  if (file.exists(filenameAWD)) {
    load(filenameAWD)
  } else {
    AWD <- NULL   # when you start the archive from scratch ... 
  }

  uAuctions <- unique(AWD$auction)
  for (fname in files) {
    filename <- paste(dir.csv, fname, sep="")

    res <- .read_FTRSLT_file(filename)
    auction <- res$auction[1]
    
    if (auction %in% uAuctions) {   
      rLog("Remove", auction, "from the AWD")
      ind <- which(AWD$auction == auction)
      AWD <- AWD[-ind,]
    }
    AWD <- rbind(AWD, res)
    rLog("Added", auction, "to the AWD")
  }
  
  # reorder and save
  filename <- paste(dir.rdata, "ftrslt.RData", sep="")
  save(AWD, file=filename)
  rLog("Saved awards to", filename)
}


################################################################################
# Update the awards archive with a given auction results
#
update_clearingprice_archive <- function(files)
{
  dir.csv <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FTRmodel/"
  dir.rdata <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/"

  # load the existing archive
  filenameCP <- paste(dir.rdata, "ftrcp.RData", sep="")
  if (file.exists(filenameCP)) {
    load(filenameCP)
  } else {
    CP <- NULL   # when you start the archive from scratch ... 
  }
  
  uAuctions <- unique(CP$auction)
  for (fname in files) {
    filename <- paste(dir.csv, fname, sep="")

    res <- .read_FTRCP_file(filename)
    auction <- res$auction[1]
    
    if (auction %in% uAuctions) {    # remove it from the AWD
      ind <- which(CP$auction == auction)
      CP <- CP[-ind,]
    }
    CP <- rbind(CP, res)
  }
  CP$ptid <- as.numeric(CP$ptid)
  
  # reorder and save 
  filename <- paste(dir.rdata, "ftrcp.RData", sep="")
  save(CP, file=filename)
  rLog("Saved clearing prices to", filename)
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
  FTR.load.ISO.env("NEPOOL")

  #mapuser()
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.reports.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.FTR.auction.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")

  
  months <- as.Date("2014-10-01")
  #months <- seq(as.Date("2013-01-01"), by="1 month", length.out=1)
  files <- download_FTR_awards(months)
  update_awards_archive(files)


  files <- download_FTR_clearedprices(months)
  update_clearingprice_archive(files)  


  
  download_FTR_FA( 2014 )


  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.archive_SP.R")
  .update_SP_archive(do.save=TRUE)  # takes 15 min 

  
  startDt <- as.Date("2012-11-01")
  endDt   <- as.Date("2012-11-30")
  FTR_Auction(startDt, endDt)
  
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  parse_auctionName( c("X10", "X11-R2", "F10-1Y", "F13-1Y-R2") )

  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  count_hours_auction(c("F13-R1", "G13-R1", "F13-1Y-R2"), 
    buckets=c("peak", "offpeak"))
  

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.FTR.R")
  auctions <- previousAuctions( parse_auctionName("F14-1Y-R1") )
  
  
  

  
}


  # I should only do the archive incrementally!
  # NEPOOL$.makeAwardsArchive()
  # NEPOOL$.makeClearingPriceArchive()     


  ## auctions <- c("F06", "F06-1Y", "G06", "H06", "J06", "K06", "M06", "N06", "Q06",
  ##               "U06", "V06", "X06", "Z06")

  


    ## indAnnual <- grepl("-", basename(links))
    ## is.annual <- grepl("-", auction)
    ## if (is.annual) {
    ##   links <- links[indAnnual]
    ## } else {
    ##   links <- links[!indAnnual]
    ## }

    ## if (length(links) > 1)
    ##   stop("There should be only one link!  You have", length(links))
    ## res <- .download_csv_from_url(links[1], skip.lines=1)

    ## file.out <- paste(DIRCSV, fileroot, auction, ".csv", sep="")
    ## write.csv(res, file=file.out, row.names=FALSE)
    ## rLog("Wrote ", file.out)


  ##   indAnnual <- grepl("-", basename(links))
  ##   is.annual <- grepl("-", auction)
  ##   if (is.annual) {
  ##     links <- links[indAnnual]
  ##   } else {
  ##     links <- links[!indAnnual]
  ##   }

  ##   if (length(links) > 1)
  ##     stop("There should be only one link!  You have", length(links))
  ##   res <- .download_csv_from_url(links[1], skip.lines=2)

  ##   file.out <- paste(DIRCSV, fileroot, auction, ".csv", sep="")
  ##   write.csv(res, file=file.out, row.names=FALSE)
  ##   rLog("Wrote ", file.out)
  ## }
  
