# Pull info from PJM eMarket
# see MBM: PJM DAM Gneration Rpt, "_procm elec pjm dam gen tsdb"
#
#
# download_pjm_gen
# read_xml_genbyparticipant
# .soap_request 
#


#######################################################################
# @param logininfo
#
download_pjm_gen <- function( logininfo, day=Sys.Date(), 
  OUTDIR="S:/All/Structured Risk/NEPOOL/eMkt/PJM/" )
{
  account <- strsplit(logininfo, ":")[[1]][1]
  fileOut <- paste(OUTDIR, format(day), "_GENBYPARTICIPANT_", account, ".xml",
                   sep="")
  fileTmp <- paste(OUTDIR, "GENBYPARTICIPANT-CPSI.tmp", sep="")

  rLog("Write the soap request to a file")
  .soap_request(day)  
 
  out <- paste(
#    "CPS_URL = https://pmoyles:moyles1@emkt.pjm.com/emkt/xml/query", "\n",
#    "CPS_URL = https://cpsiauto:Welcome_1@emkt.pjm.com/emkt/xml/query", "\n",
#    "CPS_URL = https://exgnptauto:Welcome_1@emkt.pjm.com/emkt/xml/query", "\n",
    paste("CPS_URL = https://", logininfo, "@emkt.pjm.com/emkt/xml/query", sep=""), "\n",
    "CPS_OUTPUT=", fileTmp, "\n",
    "CPS_MODE=POST", "\n", 
    "CPS_FILE=", fileOut, "\n",
    "CPS_ATTACHFILE=", "S:/All/Structured Risk/NEPOOL/eMkt/pjm.tmp", "\n",
    sep="")

  cfgFile <- tempfile() # "C:/Temp/pjm.cfg"  
  sink(cfgFile)
  cat(out)
  sink()

  cmd <- paste("S:/Data/CPSHttps/cpshttpsv2 -v ", cfgFile, sep="")  
  res <- system(cmd)

  res # 0 is SUCCESS  
}


##########################################################################
# Return a data.frame
# If make.xlsx=FALSE write to an xlsx
#
read_xml_genbyparticipant <- function(filein)
{
  asOfDate <- gsub("(.*)_.*\\.xml", "\\1", basename(filein))
  doc <- xmlRoot(xmlTreeParse(filein))

  mrs <- xmlChildren(doc)[["Body"]][["QueryResponse"]][["MarketResultsSet"]]

  res <- vector("list", length=length(mrs))
  for (i in 1:length(mrs)) {
    x <- mrs[[i]]
    location <- xmlGetAttr(x, "location")
    rLog("Working on", location)
    aux <- ldply(xmlChildren(x), function(y) {
      data.frame(hour = xmlGetAttr(y, "hour"),
                 MW = xmlValue(y[["ClearedMW"]]),
                 price = xmlValue(y[["ClearedPrice"]]))
    })
    res[[i]] <- cbind(UNITID = location, aux)
  }
  res <- do.call(rbind, res)

  rLog("Read the unit name mapping")
  MM <- read.csv("S:/Data/PJMData/XML Data/UnitIDNew.csv")
  MM <- MM[,1:2]
  MM$UNITID <- as.character(MM$UNITID)
  
  res <- merge(res, MM, by="UNITID", all.x=TRUE)
  # unique(res[, c("UNITID", "UnitName")])
  # subset(res, UNITID == "90163101")
   
  if (nrow(res)==0){
    rLog("Empty file!  Exiting")
    return(data.frame())
  }

  out <- res[,3:6]
  out$MW <- as.numeric(out$MW)
  out$price <- as.numeric(out$price)
  
  out
}



#######################################################################
# Make the soap request and write it to a file
#
.soap_request <- function( day=Sys.Date() )
{
  out <- paste('<?xml version="1.0"?>
<env:Envelope xmlns:env="http://schemas.xmlsoap.org/soap/envelope/">
<env:Body>
<QueryRequest xmlns="http://emkt.pjm.com/emkt/xml">
<QueryMarketResults day="', format(day), '" type="Generation">
	<All />
</QueryMarketResults>
</QueryRequest>
</env:Body>
</env:Envelope>', sep="")

  cfgFile <- "S:/All/Structured Risk/NEPOOL/eMkt/pjm.tmp"  
  sink(cfgFile)
  cat(out)
  sink()
}



#######################################################################
#######################################################################
#
.test <- function()
{
  require(CEGbase)
  require(plyr)
  require(XML)
  require(reshape2)
  
  OUTDIR <- "S:/All/Structured Risk/NEPOOL/eMkt/PJM/"
  
  day <- Sys.Date()+1  # it's DA

  filein <- paste("S:/All/Structured Risk/NEPOOL/eMkt/PJM/",
    format(day), "_GENBYPARTICIPANT-CPSI.xml", sep="")

  
  
}
