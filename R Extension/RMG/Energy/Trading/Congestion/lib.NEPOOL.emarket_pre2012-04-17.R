# Download/process Nepool emarket files
#
# download_emkt_genbyparticipant
#
# getURL_emkt
#
# read_genbyparticipant_xml
# read_units_xml

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
#
download_emkt_file <- function(day=as.Date("2011-03-18"),
  token="genbyparticipant",
  OUTDIR="//ceg/cershare/All/Structured Risk/NEPOOL/eMkt/XML/")
{
  url <- paste("https://smd.iso-ne.com/mkt/private/XmlRequest?request=",
    toupper(token), "&DAY=", format(day, "%Y%m%d"), sep="")
  
  fileout <- paste(OUTDIR, tolower(token), "_", format(day), ".xml", sep="")
  filetmp <- paste(OUTDIR, "_", tolower(token), ".tmp", sep="")
  mode <- "GET"
  
  res  <- getURL_emkt(url, fileout, filetmp)
  if (res==0){
    rLog("Downloaded ", token, " file successfully!")
  } else {
    rLog("FAILED!")    
  }
  res  
}


##########################################################################
#
# see _procm elec nepool emkt dnld
# uses: _lib elec nepool emkt fns:::download report
#
getURL_emkt <- function(url, fileout, filetmp, mode="GET", cert="(50027)")
{
  # Need to create a temp file with the request
  out=paste(
    "CPS_URL=", url, "\n",
    "CPS_OUTPUT=", filetmp, "\n",
    "CPS_MODE=", mode, "\n", 
    "CPS_FILE=", fileout, "\n",
    "CPS_CERT=", cert, "\n",
    sep="")
  
  cfgFile <- tempfile()
  sink(cfgFile)
  cat(out)
  sink()
  
  
  cmd <- paste("//ceg/cershare/Data/CPSHttps/cpshttpsv2 -v ", cfgFile, sep="")  
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


##########################################################################
# Return a data.frame
# If make.xlsx=FALSE write to an xlsx
#
read_xml_genbyparticipant <- function(filein, make.xlsx=FALSE)
{
  doc <- xmlRoot(xmlTreeParse(filein))
  doc <- doc[[1]]  # there's only one node

  # process the xml
  res <- xmlSApply(doc, function(x) xmlSApply(x, xmlValue))
  if (length(res)==0){
    rLog("Empty file!  Exiting")
    return(data.frame())
  }
  res <- t(res)
  rownames(res) <- NULL
  res <- as.data.frame(res)

  aux <- melt(res[,-c(1,5,6,9)], id=1:3)
  
  out <- cast(aux, DAY + HOURENDING ~ UNITSHORTNAME + variable, I, fill=NA)

  if (make.xlsx){
    require(xlsx)
    day <- gsub("genbyparticipant_(.*)\\.xml", "\\1", basename(filein))
    fileout <- paste("//ceg/cershare/All/Structured Risk/NEPOOL/Generation Related/",
      "Boston Gen/Plant Dispatch/DA_Awards/DA Awards ", day, ".xlsx", sep="")
    out <- data.frame(sapply(out, as.numeric))
    write.xlsx(out, fileout, row.names=FALSE) 
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

  source("//ceg/cershare/All/Risk/Software/R/prod/Energy/Trading/Congestion/lib.NEPOOL.emarket.R")
  
  asOfDate <- Sys.Date() + 1        # it's DA market!

  DIR <- "//ceg/cershare/All/Structured Risk/NEPOOL/eMkt/XML/"
  download_emkt_file(day=asOfDate, token="demandbid", OUTDIR=DIR)
  
  filein <- paste(DIR, "demandbid_", format(asOfDate), ".xml", sep="")
  out <- read_xml_demandbid(filein, make.xlsx=TRUE)
  

}



#############################################################################
#############################################################################
#############################################################################

  ## require(RCurl)
  ## remotePath = "https://smd.iso-ne.com/mkt/private/XmlRequest?request=GENBYPARTICIPANT&DAY=20110318" 
  ## w = getURL(remotePath)  # almost works
