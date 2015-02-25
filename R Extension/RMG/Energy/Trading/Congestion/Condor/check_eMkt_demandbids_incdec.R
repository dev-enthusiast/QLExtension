# Check the demand bids from ISO/nMarket/tsdb!  Yay
#
#
# Written by Adrian Dragulescu on 4/1/2011
#

###################################################################
# compare tsdb data with what it's in the daily spreadsheet
#
.compare_demandbids <- function()
{
  startDt <- as.POSIXct(paste(format(Sys.Date()+1), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()+2), "00:00:00"))

  tsdb <- get_ccg_shortterm_zonal_load(startDt, endDt)

}


###################################################################
# compare incdecs we have on the spreadsheet with the incdecs
# in eMkt
#
.compare_incdecs <- function(asOfDate)
{
  startDt <- as.POSIXct(paste(format(Sys.Date()+1), "01:00:00"))
  endDt   <- as.POSIXct(paste(format(Sys.Date()+2), "00:00:00"))

  fname <- .get_last_incdec_filename(asOfDate)


  

}



###################################################################
###################################################################
require(CEGbase)
require(reshape)
require(XML)
require(xlsx)
require(SecDb)
require(zoo)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.emarket.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")


asOfDate <- Sys.Date() + 1        # it's DA market!

DIR <- "S:/All/Structured Risk/NEPOOL/eMkt/XML/"
download_emkt_file(day=asOfDate, token="demandbid", OUTDIR=DIR)
  
filein <- paste(DIR, "demandbid_", format(asOfDate), ".xml", sep="")
out <- read_xml_demandbid(filein, make.xlsx=TRUE)


