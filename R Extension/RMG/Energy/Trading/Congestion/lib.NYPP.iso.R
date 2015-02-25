################################################################################
# For NYISO, you upload one file ONLY.  So put all your bids into one file.
# Location: S:/Trading/NYISO/TCC/Monthly Auctions/Jul09/ etc. 
# login: dragua/Changeme#1
# To download awards, select first the auction you want, then push button "Reports"
#   Choose CSV from the dropdown, select View MP Bundled Awards.
#
#
# To get daily NYISO operational report: go to http://mis.nyiso.com/public/
# scroll down to Reports & Information, click NY Operational Announcements.
#
# To get the flows between NY and NEPOOL, go to:
# Market Data > Market & Operational Data > Reports & Info tab > Reports> Daily Energy
# it's a csv file named yyyymmddDAM_energy_rep.csv published at 10:00 AM every day. 
# column AE "Net Imports DNI NPX" is what you want
#
#
# Short term transmission outages: Market & Operational Data > Postings by Date > 
# http://mis.nyiso.com/public/csv/outSched/20110128outSched.csv
# Long term transmission outages: Market & Operational Data > Power Grid Data > 
# http://mis.nyiso.com/public/csv/os/outage-schedule.csv
#
# External Libraries
#
require(CEGbase); require(FTR)
source("H:/user/R/RMG/Utilities/read.pdf.R")


################################################################################
# File Namespace, constants
#
NYPP <- new.env(hash=TRUE)
NYPP$options$awards.dir   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"

NYPP$mkt    <- "PWY"
NYPP$region <- "NYPP"
NYPP$hist.start.dt <- as.POSIXct("2003-11-01 00:00:00")   # start with this winter
NYPP$analysis.start.dt <- as.Date("2006-01-01")

################################################################################
# Run this every month
#
NYPP$main <- function(){

#  Steps: 1) download the clearing prices by hand
#         2) download the awards by hand    
#         3) run the Condor/daily.price.append.R
#
  NYPP$.makeMAP()

#  NYPP$.downloadTCCAwards(fileroot="tcc_summary_10-28-08")
  
#  NYPP$.makeAwardsArchive()
  NYPP$.makeClearingPriceArchive() 

  fileNames <- list.files(
    "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/Raw", full.names=TRUE)
  NYPP$.processAwardsSummaryFiles()
  NYPP$.makeAwardsSummaryArchive()
  
}


################################################################################
# Make tsdb price summaries with realized congestion
# TODO: There are two warnings I need to investigate.  probably with first
# cbind. 
NYPP$.makePriceSummaries <- function(nodes, start.dt=NULL, save=FALSE)
{
#  tsdb.symbols <- subset(MAP, ptid %in% nodes)$tsdb.symbol
  tsdb.symbols <- paste("NYPP_DAM_GEN_MCC_", nodes, sep="")
  ind <- which(nodes %/% 60000==1)
  if (length(ind)>0)
     tsdb.symbols[ind] <- gsub("_GEN_", "_ZONE_", tsdb.symbols[ind])

  if (is.null(start.dt)){
    start.dt <- NYPP$hist.start.dt
  } else {
    start.dt <- as.POSIXct(paste(start.dt, "00:00:00")) 
  }
  end.dt   <- as.POSIXct(paste(Sys.Date()-1, "23:00:00")) 
  
  rLog("Read prices from tsdb")
  hP <- NULL
  packets  <- trunc(length(nodes)/100) + 1
  for (p in 1:packets){
    rLog(paste("Working on packet", p, "of", packets))
    ind <- ((p-1)*100+1) : min(p*100, length(nodes))
    symbols <- tsdb.symbols[ind]
    hPP <- FTR.load.tsdb.symbols(symbols, start.dt, end.dt)
    colnames(hPP) <- sapply(strsplit(colnames(hPP), "_"),
                            function(x){x[length(x)]})

    ind.NA <- apply(is.na(hPP), 2, sum)   # check for missing values!
    if (any(ind.NA > 0)){
      missing <- c(missing, ind.NA[which(ind.NA>0)])
      print(ind.NA[which(ind.NA>0)])
    }

    aux <- aggregate(hPP, format(index(hPP), "%Y-%m-%d"), mean )
    if (p==1){
      hP <- aux
    } else {hP  <- cbind(hP, aux)}
  }
   
  index(hP) <- as.Date(index(hP))
  colnames(hP) <- paste("FLAT.", colnames(hP), sep="")

  hP <- -hP  # this is NYPP ! weird as it seems
  
  if (save){
    filename <- paste(NYPP$options$awards.dir, "RData/hPrices.RData", sep="")
    save(hP, file=filename)
  } else {
    return(hP)
  }
}


################################################################################
# Read the ISO awards
#
NYPP$.loadAwards <- function()
{
  rLog("Loading historical awards")
  filename <- paste(NYPP$options$awards.dir, "RData/awds.RData", sep="")
  load(filename)
  rLog("Done.")
  return(AWD)
}


################################################################################
# Read the ISO clearing price file
#
NYPP$.loadClearingPrice <- function()
{
  rLog("Loading historical clearing prices")
  filename <- paste(NYPP$options$awards.dir, "RData/ftrcp.RData", sep="")
  load(filename)
  rLog("Done.")
  return(CP)
}


################################################################################
# Read the settled prices file
#
NYPP$.loadSettlePrice <- function()
{
  rLog("Loading historical clearing prices")
  filename <- paste(NYPP$options$awards.dir, "RData/ftrsp.RData", sep="")
  load(filename)
  rLog("Done.")
  return(SP)
}


################################################################################
# Read the ISO clearing price file
#
NYPP$.loadHistoricalPrice <- function()
{
  rLog("Loading historical realized congestion prices")
  filename <- paste(NYPP$options$awards.dir, "RData/hPrices.RData", sep="")
  load(filename)
  rLog("Done.")
  return(hP)
}
################################################################################
# Load the NEPOOL MAP
#
NYPP$.loadMAP <- function()
{
  rLog("Loading the MAP")
  filename <- paste(NYPP$options$awards.dir, "RData/map.RData", sep="")
  load(filename)
  rLog("Done.")
  return(MAP)
}

################################################################################
# Download awards/settle prices directly from the NYPP ISO.
# from http://www.nyiso.com/public/products/tcc/index.jsp
# Go to the TCC information and announcements link 
# There are the largest two csv files on the page
# SUMMER = MAY -> OCT, WINTER = NOV -> APR
#
# This is 6-mth delayed info.  If you want the latest awards info without  
# identifying the participants see: NYPP$.aggregateLatestAwdsInfo
#
NYPP$.downloadTCCAwards <- function(fileroot="tcc_summary_6-27-08", count=2)
{
  # download the first part 
  fullname <- paste("http://www.nyiso.com/public/webdocs/products/",
    "tcc/general_info/", fileroot, "_001.CSV", sep="")
  con <- url(fullname)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error") rLog(paste("Cannot read url", filename))
  
  res <- try(read.csv(con))
  if (class(res)[1]=="error"){rLog(paste("Failed reading", fullname))}
  close(con)
  newAWD <- res

  # download the second part ... 
  fullname <- paste("http://www.nyiso.com/public/webdocs/products/",
    "tcc/general_info/", fileroot, "_002.CSV", sep="")  
  con <- url(fullname)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error") rLog(paste("Cannot read url", filename))
  
  res <- try(read.csv(con))
  if (class(res)[1]=="error"){rLog(paste("Failed reading", fullname))}
  close(con)
  newAWD <- rbind(newAWD, res)
  
  names(newAWD) <- tolower(names(newAWD))
  newAWD <- newAWD[,c(3:5, 8, 11:14)]
  names(newAWD)[c(1:4,7:8)] <- c("start.dt", "end.dt", "source.ptid", "sink.ptid",
                             "customer.name", "award.ftr.price")
  newAWD$award.ftr.price <- as.numeric(newAWD$award.ftr.price)
  newAWD$award.ftr.mw <- (newAWD$mw.summer + newAWD$mw.winter)/2
  newAWD$start.dt <- as.Date(newAWD$start.dt, "%d-%B-%Y")
  newAWD$end.dt   <- as.Date(newAWD$end.dt,   "%d-%B-%Y")

  # several bids that got lifted appear multiple times, so aggregate the mw
  names(newAWD)[which(names(newAWD)=="award.ftr.mw")] <- "value"
  AWD <- cast(newAWD, ... ~ ., sum, fill=NA)
  names(AWD)[which(names(AWD)=="(all)")] <- "award.ftr.mw"
  AWD <- data.frame(AWD)

  file.out <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/awds.RData"
  save(list=c("AWD"), file=file.out)  

}



################################################################################
# Make the Clearing Price archive from scratch each time ... 
#
NYPP$.makeClearingPriceArchive <- function()
{
  dir.csv <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"
  files <- list.files(dir.csv, full.names=T, pattern="^ftrcp_.*\\.csv$")

  CP <- NULL   # cleared prices
  for (ind.f in seq_along(files)){
    res <- NYPP$.read_clearingPrice_file( files[f] )
    CP <- rbind(CP, res)
  }
  CP <- cbind(class.type="FLAT", CP)
  names(CP)[2:3] <- c("ptid", "name")

  CP$Nodal.Price.Zone <- NULL
  
  filename <- paste(dir.csv, "RData/ftrcp.RData", sep="")
  save(CP, file=filename)
  rLog("Saved ", filename)
}


################################################################################
# Aggreagate the summary awards in a big RData for later use
# 
NYPP$.makeAwardsSummaryArchive <- function()
{
  dirIn  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"
  fileNames <- list.files(dirIn, full.names=TRUE, pattern="awards_summary")

  AWDSUM <- NULL
  for (f in fileNames)
    AWDSUM[[f]] <- read.csv(f)

  AWDSSUM <- do.call(rbind, AWDSUM)
  rownames(AWDSSUM) <- NULL
  
  fileOut <- paste(dirIn, "RData/awdssum.RData", sep="")
  rLog("Save summary awards to", fileOut)
  
}

################################################################################
# Get Load and Generator names in NYPP
# 
NYPP$.getLoadGenNames <- function()
{
  # read the loads
  filename <- paste("http://mis.nyiso.com/public/csv/load/load.csv", sep="")
  con <- url(filename)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error") rLog(paste("Cannot open url", filename))

  res <- NULL
  res <- try(read.csv(con))
  if (class(res)[1]=="error"){rLog(paste("Failed reading", filename))}
  close(con)

  TT <- res
  colnames(TT) <- tolower(colnames(TT))
  TT$node.type <- "load"
  colnames(TT)[1] <- "node.name"
  
  # read the generators
  filename <- paste("http://mis.nyiso.com/public/csv/generator/generator.csv", sep="")
  con <- url(filename)
  res <- try(open(con), silent=T)
  if (class(res)[1] == "try-error") rLog(paste("Cannot open url", filename))

  res <- NULL
  res <- try(read.csv(con))
  if (class(res)[1]=="error"){rLog(paste("Failed reading", filename))}
  close(con)

  colnames(res) <- tolower(colnames(res))
  res$node.type <- "generator"
  colnames(res)[1] <- "node.name"
  
  TT <- rbind(TT, res)
  TT <- TT[order(TT$ptid),]
  TT <- TT[!duplicated(TT$ptid), ]

  return(TT)
}
################################################################################
# Make the NYPP map
# Add the subzone info
NYPP$.makeMAP <- function()
{
  rootdir <- NYPP$options$awards.dir 

  MAP <- FTR.ReportManager("NYPP")

  # add the  MCC locations!
  aux <- data.frame(
    mccLocations = toupper(unlist(secdb.getValueType("Bal Close PWR NYPP MCC",
      "locations"))),
    location = toupper(unlist(secdb.getValueType("Bal Close PWR NYPP MCC",
      "relative flag codes"))))
  # these are duplicated, don't ask me why!
  aux <- aux[-which(aux$mccLocations %in% c("DAM MCC SELKIRK1", "DAM MCC SELKIRK2")),]
  MAP <- merge(MAP, aux, all.x=TRUE, by="location")
  MAP <- MAP[order(MAP$ptid), ]
  
  TT <- NYPP$.getLoadGenNames()
  MAP <- merge(MAP, TT[,c("ptid","subzone")], all.x=TRUE)
  ind <- which(MAP$type == "ZONE")
  MAP$subzone[ind] <- MAP$zone[ind]
  
  
  filename <- paste(rootdir, "RData/map.RData", sep="")
  rLog("Saving the NYPP map to", filename)
  save(MAP, file=filename)
  rLog("Done.")  
  
}

################################################################################
# Get the ptids used in the auction.  Each auction has a special file...
# filein = "att_e_poi_pow_init_reconfig_tcc_auctions_AUT2008_07-21-2008_CLEAN.pdf"
NYPP$.read.auction.ptids <- function(filein)
{
  root   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/PDF/"
  filein <- paste(root, filein, sep="")
  res <- read.pdf(filein)

  res <- res[-(1:6)]                     # get rid of header
  res <- res[which(nchar(res)>1)]        # get rid of empty lines
  res <- res[-grep(" TCC Manual ", res)] # get rid of page numbers
  res <- res[-grep("PTID Name ", res)]   # get rid of this

  aux <- strsplit(res, " {2,}")
  ind <- which(sapply(aux, length)!=3)
  if (length(ind)>0)
    stop("Some lines don't have 3 columns.")

  res <- matrix(unlist(aux), ncol=3, byrow=T)
##   colnames(res) <- tolower(res[1,])      # not working on 7/13/2009!
##   ind <- grep("PTID", res[,"ptid"])
##   if (length(ind)>0) res <- res[-ind, ]
  colnames(res) <- c("name", "ptid", "zone")
  res <- data.frame(res)
  res$ptid <- as.numeric(res$ptid)

  res <- res[order(res$ptid),]
  rownames(res) <- NULL

  return(res)
}


################################################################################
# Make a table with all the NYPP units and their location 
# from the GOLD BOOK file 
NYPP$.make.units.table <- function()
{
  fileIn <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/PDF/",
    "2009_LoadCapacityData_PUBLIC.pdf", sep="")

  source("H:/user/R/RMG/Utilities/read.pdf.R")
  aux <- read.pdf(fileIn, first=50, last=56)

  bux <- aux[-which(aux=="")]  # remove empty lines
  ind <- grep("(?i)Table", bux, perl=TRUE)

  # remove table headings
  bux <- bux[-as.numeric(sapply(ind, function(x){x+0:7}))]

  # clear the end of the table; remove totals & end
  bux <- bux[-c(length(bux)-2, length(bux))]
  
  # index of page numbers
  ind <- grep("^       ", bux)
  noPages <- length(ind)
  rowsPerPage <- diff(c(0,ind))
  indPage <- unlist(mapply(rep, 1:noPages, rowsPerPage))

  # put each page in an element of a list
  cux <- data.frame(indPage=indPage, content=bux)
  cux <- split(cux[,-1], indPage)
  cux <- lapply(cux, function(x){
    if (length(grep("^    ", x[length(x)]))) x <- x[-length(x)]
    x
  })
  
  # apply .alignTablePage to each page
  noFields <- 14
  validateList <- list()
  validateList[["3"]]  <- "[[:alpha:]]"
  validateList[["4"]]  <- "[[:digit:]]+"
  #validateList[["10"]] <- "Y|N| "
  
  dux <- lapply(cux, .alignTablePage, noFields, validateList)

  eux <- lapply(dux, function(x){x[,1:14]})
  fux <- do.call(rbind, eux)
  colnames(fux) <- c("Owner", "Station/Unit", "Zone", "ptid",
    "Town/County/State", "InServiceDate", "NamePlateRating(kW)",
    "SummerCapability(kW)", "WinterCapability(kW)", "CogenUnit(Y/N)",
    "UnitType", "FuelType", "FuelType2", "2008NetEnergy")

  Units <- fux
  write.csv(Units, row.names=FALSE, 
    file="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/Units.csv")

  # save to the RData folder too
  save(Units,
    file="S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/RData/Units.RData")
  
}


################################################################################
# See also: NYPP$.downloadTCCAwards for complete info.
# You can download awards_summary_XXX.csv from the secure TCC website page. 
# Dump the files into a folder so you can refer to them by full filenames
#
# This function parses the files, extracts the relevant info, and 
# saves the data in the root NYPP directory with a standard filename. 
# These files are processed with NYPP$.makeAwardsSummaryArchive()
#
NYPP$.processAwardsSummaryFiles <- function(fileNames, save=FALSE)
{
  .processOneFile <- function(fileName){
    cat("Working on", fileName, "...")
    # make some assumptions on the file format.  As this can change at any time,
    # bugs may sneak in.
    #
    options(warn=-1)   # some bad EOF from the ISO
    MM <- readLines(fileName)
    options(warn=0)
    indBuySell <- grep("POI", MM)
    if (diff(indBuySell)==2)        # no data in the file
      return(NULL)
    header  <- MM[grep("^[[:alpha:]]", MM)]  # rows that start with characters

    dataSales <- dataBuys <- NULL
    indBuys <- (indBuySell[1]+1):(indBuySell[2]-2)
    if ((indBuySell[2]+1)<length(MM)){    
      # if there are there sales
      dataSales <- MM[(indBuySell[2]+1):length(MM)]
      dataSales <- do.call("rbind", strsplit(dataSales, ","))
      dataSales <- data.frame(apply(dataSales, 2, as.numeric))
      colnames(dataSales) <- c("sink.ptid", "source.ptid", "award.ftr.mw",
        "award.ftr.price")
    }
    
    # for sure there are buys
    dataBuys <- MM[indBuys]     # rows that start with digits
    dataBuys <- do.call("rbind", strsplit(dataBuys, ","))
    dataBuys <- data.frame(apply(dataBuys, 2, as.numeric))
    colnames(dataBuys) <- c("source.ptid", "sink.ptid", "award.ftr.mw",
        "award.ftr.price")
    data <- rbind(dataBuys, dataSales)

    # infer the file name, auction, etc. 
    seasonYr <- strsplit(toupper(header[2]), " ")[[1]]
    season   <- seasonYr[1]
    year     <- seasonYr[2]
   
    indRound <- grep("^Round", header)
    indType  <- grep("^Month", header)
    if (length(indRound)==0){
      # for monthly recongfiguration auctions
      month <- toupper(gsub("(.*) .*", "\\1", header[indType+1]))
      monthNo <- match(month, toupper(month.name))
      if (is.na(monthNo))
        stop("Cannot figure out the month for the Monthly auction.")
      
      token1   <- paste(year, "-", sprintf("%02.0f", monthNo), "_", sep="")
      token2   <- ""     
      roundNo  <- ""
      noMonths <- 1
    } else {
      # for multi rounds auctions, capability period
      if (length(indType) > 0)
        stop("Cannot have Month at BOL on a multi round auction file!")
      roundNo <- gsub("Round ([[:digit:]]) .*", "\\1", header[indRound])
      roundNo <- as.numeric(roundNo)
      if (is.na(roundNo))
        stop("You should have a round number!  Please check.")
      period <- toupper(gsub(".* - (.*) TCC.*", "\\1", header[indRound]))
      if (period == "ONE YEAR"){
        token2   <- "1Y"
        noMonths <- 12
      } else if (period == "SIX MONTH"){
        token2   <- "6M"
        noMonths <- 6
      } else {
        stop("Unrecongized period:", period)
      }
      if (season == "AUTUMN"){
        token1 <- paste(year, "-11_", sep="")
      } else if (season == "SPRING"){
        token1 <- paste(year, "-05_", sep="")
      } else {
         stop("Wrong season ", season, " for a multi round auction!")
      }
    }
   
    myFileName <- paste(token1, token2, sep="")
    if (roundNo != "")  
      myFileName <- paste(token1, token2, "_R", roundNo, sep="")
   
    # add the last bells and wistles
    startDate <- as.Date(paste(token1, "01", sep=""), "%Y-%m_%d")
    endDate   <- seq(startDate, by=paste(noMonths-1, "months"), length.out=2)[2]
    data$start.dt <- startDate
    data$end.dt   <- endDate
    data$round    <- roundNo 
    cat(" Done.\n")

    dirOut  <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"
    fileOut <- paste(dirOut, "awards_summary_", myFileName, ".csv", sep="")
    write.csv(data, file=fileOut, row.names=FALSE)
    
    return(1)
  }
  
  
  AA <- lapply(fileNames, .processOneFile)
  

}

#########################################################################
#########################################################################
NYPP$MAP      <- NYPP$.loadMAP()
## NYPP$HRS      <- FTR.bucketHours("NYPP")
## NYPP$HRS.AUCT <- FTR.countHoursAuction("NYPP")  
NYPP$AWD      <- NYPP$.loadAwards() 
NYPP$CP       <- NYPP$.loadClearingPrice()
NYPP$hP       <- NYPP$.loadHistoricalPrice()
## NYPP$SP       <- NYPP$.loadSettlePrice()
NYPP$nfind <- function(x) { NYPP$MAP[grep(toupper(x), NYPP$MAP$name),] }

























## ################################################################################
## # read one clearing price file
## #
## NYPP$.read_clearingPrice_file <- function(filename)
## {
##   rLog(paste("Reading", filename))
  
##   aux <- readLines(files[ind.f], n=20)
##   res <- read.csv(files[ind.f], skip=(grep("^PTID", aux)-1))
##   names(res)[4] <- "clearing.price"

##   aux <- strsplit(basename(files[ind.f]), "_")[[1]][-1]
##   aux <- gsub("\\.csv", "", aux)
##   aux[1] <- paste(aux[1], "-01", sep="")
##   res$start.dt <- as.Date(aux[1])
##   if (length(aux)==1){   # one month only
##      res$end.dt <- seq(as.Date(aux[1]), by="1 month", length.out=2)[2]-1
##   } else if (aux[2]=="1Y"){
##      res$end.dt <- seq(as.Date(aux[1]), by="12 months", length.out=2)[2]-1
##   } else if (aux[2]=="6M"){
##      res$end.dt <-  seq(as.Date(aux[1]), by="6 months", length.out=2)[2]-1
##   } else if (aux[2]=="2Y"){
##      res$end.dt <- seq(as.Date(aux[1]), by="24 months", length.out=2)[2]-1
##   } else {
##     stop("Unknown period!")
##   }  
##   auction <- format.dateMYY(res$start.dt[1], direction=-1)
##   if (length(aux)>1)
##     auction <- paste(auction, paste(aux[-1], sep="", collapse="-"), sep="-")
##   res$auction <- auction
##   no.hours <- 24*as.numeric(res$end.dt[1] - res$start.dt[1] + 1) # ignore DST
##   res$clearing.price <- as.numeric(gsub("\\(", "-",
##     gsub("\\$|) *", "", res$clearing.price)))
##   res$clearing.price.per.hour <- res$clearing.price / no.hours
##   res <- res[,c("PTID", "PTID.Name", "Nodal.Price.Zone", "clearing.price",
##                 "start.dt", "end.dt", "auction", "clearing.price.per.hour")]

   
##   res 
## }


## ################################################################################
## # First save the file from the NYISO website to the csv folder
## # I don't know what is this function, and if it's used again 09/16/2009 -- AAD
## #
## NYPP$.read.auction.awds <- function(filein, options)
## {
##   root   <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NYPP/"
##   filein <- paste(root, filein, sep="")
##   res <- read.csv(filein, skip=8)

##   ind  <- grep("TCCs Offered", res[,1])
##   sell <- res[(ind+2):nrow(res),]
##   buy  <- res[1:(ind-1),]
##   TT   <- rbind(cbind(buy,  buy.sell="BUY"),
##                 cbind(sell, buy.sell="SELL"))
##   colnames(TT) <- c("source.ptid", "sink.ptid", "award.ftr.mw",
##                     "award.ftr.price", "buy.sell")
##   TT$auction    <- options$auction
##   TT$class.type <- "FLAT"
##   TT[,1:4] <- sapply(TT[,1:4], as.numeric)  
##   TT$CP <- TT$award.ftr.price/options$hours["FLAT"]
##   TT$path.no <- 1:nrow(TT)
  
##   ISO.env  <- eval(parse(text=paste(options$region)))  # inherit it

##   TT <- merge(TT, FTR.PathNamesToPTID(TT, ISO.env$MAP, to="zone"))
##   TT <- merge(TT, FTR.PathNamesToPTID(TT, ISO.env$MAP, to="name"))
  
##   return(TT)
## }


## ################################################################################
## # Make the Settle Price archive.
## # Returns a data.frame with columns: ptid, month, class.type, settle.price
## #
## NYPP$.makeSettlePriceArchive <- function()
## {
##   aux <- aggregate(NYPP$hP, format(as.Date(index(NYPP$hP)),
##                                             "%Y-%m-01"), mean, na.rm=TRUE)
##   aux <- melt(as.matrix(aux))
##   bux <- strsplit(as.character(aux[,2]), "\\.")
##   aux$class.type <- sapply(bux, function(x){x[1]})
##   aux$ptid <- as.numeric(sapply(bux, function(x){x[2]}))
##   aux <- aux[,-2]
  
##   names(aux)[1:2] <- c("month", "settle.price")
##   SP <- na.omit(aux)
##   SP$month <- as.Date(as.character(SP$month))
  
##   filename <- paste(dir.csv, "RData/ftrsp.RData", sep="")
##   save(SP, file=filename)
## }
