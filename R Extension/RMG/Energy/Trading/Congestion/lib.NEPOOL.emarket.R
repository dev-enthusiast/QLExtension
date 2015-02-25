# Download/process Nepool emarket files
#
# download_emkt_file
# .get_filename
# getURL_emkt
# read_xml_demandbid
# read_xml_detaileddemandbyparticipant
# read_xml_genbyparticipant
# read_xml_scheduledetail


##########################################################################
# token can be:
#   genbyparticipant
#   regoffer
#   regunithourly
#   scheduledetail
#   schedulebid
#   scheduleselection
#   unit
#   unithourly
#   unitramprate
#https://smd.iso-ne.com/emktmui/webservices/rest/public/systemsummary?day=20120418
#https://smd.iso-ne.com/emktmui/webservices/rest/unit/byparticipant?day=20120418
#
# @param company one of "CCG", "CNE", "ExGen"
#  
download_emkt_file <- function(day=as.Date("2011-03-18"),
  token="genbyparticipant",
  OUTDIR="S:/All/Structured Risk/NEPOOL/eMkt/XML/",
  company="CCG"
  )
{
  certs <- structure(c("50027", "50017", "50428"),
               names=c("CCG",   "CNE",   "EXGN"))
  
  baseurl  <- "https://smd.iso-ne.com/emktmui/webservices/rest/"
  yyyymmdd <- format(day, "%Y%m%d")
  
  url <- switch(token,
    "systemssummary" = paste(baseurl, "public/systemsummary", sep=""),
    "genbyparticipant" = paste(baseurl, "unit/byparticipant", sep="")
  ) 

  url <- paste(url, "?day=", yyyymmdd, sep="")
  
  fileout <- paste(OUTDIR, tolower(token), "_", format(day), ".xml", sep="")
  filetmp <- paste(OUTDIR, "_", tolower(token), ".tmp", sep="")
  mode <- "GET"

  cert <- certs[company]  # <- take the right account number
  
  res  <- getURL_emkt(url, fileout, filetmp, mode="GET", cert=cert)
  if (res==0){
    rLog("Downloaded ", token, " file successfully!")
  } else {
    rLog("FAILED!")    
  }
  res  
}


###########################################################################
#
.get_filename <- function(day=Sys.Date(), company=c("EXGEN", "CNEGY"),
   content=c("detaileddemandbyparticipant"))
{
  DIR <- if (company == "EXGEN") {
    paste("S:/Data/NEPOOL/XML Data/Old/", format(day, "%Y%m"), "/", sep="")
  } else {
    "???????????S:/Data/NEPOOL/XML Data/Old/"
  }

  pattern <- paste(format(day, "%Y%m%d"), content, ".*\\.xml$", sep="")

  res <- list.files(DIR, pattern=pattern, full.names=TRUE)

  sort(res)[length(res)]
}



##########################################################################
#
# see _procm elec nepool emkt dnld
# uses: _lib elec nepool emkt fns:::download report
#
getURL_emkt <- function(url, fileout, filetmp, mode="GET", cert="50027")
{
  # Need to create a temp file with the request
  out=paste(
    "CPS_URL=", url, "\n",
    "CPS_OUTPUT=", filetmp, "\n",
    "CPS_MODE=", mode, "\n", 
    "CPS_FILE=", fileout, "\n",
    "CPS_CERT=(", cert, ")\n",
    sep="")
  
  cfgFile <- tempfile()
  sink(cfgFile)
  cat(out)
  sink()
  
  
  cmd <- paste("S:/Data/CPSHttps/cpshttpsv2 -v ", cfgFile, sep="")  
  res <- system(cmd)
  
  res   # 0 is SUCCESS
}


##########################################################################
# Return a data.frame
# If make.xlsx=FALSE write to an xlsx
#
read_xml_demandbid <- function(filein, make.xlsx=FALSE)
{
  doc <- xmlRoot(xmlTreeParse(filein))
  doc <- doc[[1]]  # there's only one node

  # process the xml
  res <- xmlSApply(doc, function(x) xmlSApply(x, xmlValue))

  fields <- c("NODEID",  "NODENAME", "BIDTYPE", "DAY", "HOURENDING",
    "SEGMENTID", "MW", "PRICE")
  empty <- rep("", length(fields))
  names(empty) <- fields
  
  # some bids are price insensitive, give them a PRICE field,
  # otherwise you can't stack them!
  aux <- lapply(res, function(x){
    y <- empty
    y[names(x)] <- x
    
    y
  }) 
  
  out <- do.call(rbind, aux)
  rownames(out) <- NULL
  out <- data.frame(out)
  out$MW    <- as.numeric(out$MW)
  suppressWarnings(out$PRICE <- as.numeric(out$PRICE))  # you may have some NA warnings

  out 
}



###########################################################################
# this is what you bid in, pricesensitive, increment, decrement, ...
#
#
read_xml_detaileddemandbyparticipant <- function(filein)
{
  doc <- xmlRoot(xmlTreeParse(filein))

  res <- ldply(xmlChildren(doc), function(x){
    #browser()
    ptid <- xmlGetAttr(x, "ID")
    bidType <- xmlGetAttr(x, "bidType")
    hp <- x[["HourlyProfile"]]
    aux <- ldply(xmlChildren(hp), function(y) {
      #browser()
      data.frame(time=xmlGetAttr(y, "time"),
        ldply(xmlChildren(y),
          function(z) {c(mw=xmlGetAttr(z, "MW"), price=xmlGetAttr(z, "price"))}))
    })
    
    aux[,2] <- ptid
    cbind(bidType=bidType, aux)
  })

  colnames(res)[2] <- c("ptid")
  head(res)


  res
}


##########################################################################
# Return a data.frame
# If make.xlsx=FALSE write to an xlsx
#
read_xml_genbyparticipant <- function(filein, make.xlsx=FALSE)
{
  asOfDate <- gsub(".*_(.*)\\.xml", "\\1", basename(filein))
  doc <- xmlRoot(xmlTreeParse(filein))

  res <- ldply(xmlChildren(doc), function(x){
    uName <- xmlValue(x[["UnitShortName"]])
    hp <- x[["HourlyProfile"]]
    aux <- ldply(xmlChildren(hp), function(y) {
      data.frame(xmlGetAttr(y, "time"), xmlGetAttr(y, "MW"), xmlGetAttr(y, "price"))
    })
    aux[,1] <- uName
    colnames(aux) <- c("unit", "hourBeginning", "MW", "price")
    aux
  })

  if (nrow(res)==0){
    rLog("Empty file!  Exiting")
    return(data.frame())
  }

  aux <- melt(res[,-c(1)], id=1:2)

  # sometimes you don't get any award in an hour.  You need to fill in
  # the data with NA's when you write to the xlsx so the spreadheet always
  # has 24 hours, or 23/25 hours on DST day. 
  existingHrs <- strptime(gsub(":00$", "00",
    sort(unique(aux$hourBeginning))), "%Y-%m-%dT%H:%M:%S%z")
  #strptime("2014-03-27T00:00:00-04:00", "%Y-%m-%dT%H:%M:%S%z")  # NA
  #strptime("2014-03-27T00:00:00-0400", "%Y-%m-%dT%H:%M:%S%z")   # "2014-03-27 EDT"

  allHrs <- seq(as.POSIXct(paste(asOfDate, "00:00:00")),
                as.POSIXct(paste(asOfDate, "23:00:00")), by="1 hour")
  bux <- aux
  bux$allHrs <- as.POSIXct(strptime(gsub(":00$", "00",
    aux$hourBeginning), "%Y-%m-%dT%H:%M:%S%z"))
  bux <- merge(bux, data.frame(allHrs=allHrs), all.y=TRUE)
  bux$allHrs <- format(bux$allHrs, "%Y-%m-%d %H:%M:%S %Z")  
  
  cux <- cast(bux, allHrs ~ unit, length, fill=NA)
  ind <- which(cux[,-1, drop=FALSE] > 2, arr.ind=TRUE)
  if (nrow(ind)>0 &&( any(ind[1,] > 1) || !all(cux[1, ] == 4))) {  
    rLog("Multiple prices/qty for one hour in the file!  Exiting")
    return(data.frame())
  }
    
  
  out <- cast(bux, allHrs ~ unit + variable, I, fill=NA)
  if ("NA_NA" %in% colnames(out))                # if you have this extra column
    out <- out[,-which(colnames(out)=="NA_NA")]  # remove it ...
  
  out <- data.frame(out)
  out[,2:ncol(out)] <- sapply(out[,2:ncol(out)], as.numeric)
  out <- out[grepl(asOfDate, format(out$allHrs)), ] # make sure you only pick this date
  colnames(out)[1] <-"hourBeginning" 
  out <- out[order(out$hourBeginning), ]  # sort it just in case

  
  if (make.xlsx){
    require(xlsx)
    day <- gsub("genbyparticipant_(.*)\\.xml", "\\1", basename(filein))
    fileout <- paste("S:/All/Structured Risk/NEPOOL/Generation Related/",
      "Boston Gen/Plant Dispatch/DA_Awards/DA Awards ", day, ".xlsx", sep="")
    #out <- data.frame(sapply(out, as.numeric))
    write.xlsx(out, fileout, row.names=FALSE)
    rLog("Wrote file", fileout)
  }
  
  out  
}



#############################################################################
#
read_xml_scheduledetail <- function(filein)
{
  doc <- xmlRoot(xmlTreeParse(filein))
  doc <- doc[[1]]  # there's only one node

  # process the xml
  res <- xmlSApply(doc, function(x) xmlSApply(x, xmlValue))
  lapply(res, length) # ! entries with different number of elements
  
  aux <- do.call(rbind, res)

  
}


#############################################################################
#############################################################################
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(XML)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.emarket.R")
  
  asOfDate <- Sys.Date() + 1        # it's DA market!

  DIR <- "S:/All/Structured Risk/NEPOOL/eMkt/XML/"
  download_emkt_file(day=asOfDate, token="demandbid", OUTDIR=DIR)
  
  filein <- paste(DIR, "demandbid_", format(asOfDate), ".xml", sep="")
  out <- read_xml_demandbid(filein, make.xlsx=TRUE)

  filein <- paste(DIR, "genbyparticipant_", format(asOfDate), "_old.xml", sep="")
  out <- read_xml_genbyparticipant(filein, make.xlsx=FALSE)



  filein <- .get_filename(day=as.Date("2013-05-06"), company="EXGEN",
   content=c("detaileddemandbyparticipant"))
  res <- read_xml_detaileddemandbyparticipant(filein)
  unique(res$bidType)
  
  

}



#############################################################################
#############################################################################
#############################################################################

  ## require(RCurl)
  ## remotePath = "https://smd.iso-ne.com/mkt/private/XmlRequest?request=GENBYPARTICIPANT&DAY=20110318" 
  ## w = getURL(remotePath)  # almost works
