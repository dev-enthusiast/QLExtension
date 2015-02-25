# Daily pull of ICE transactions
#
# Written by Adrian Dragulescu on 8-Jul-2010


############################################################################
# copy files for cne
#
.copy_files_cne <- function( asOfDate, tags )
{
  rLog("Move the files for CNE")
  outdir <- "//ceg/cershare/SupplyGroup/PUBLIC/East Curves/ICE/"
  
  lapply(tags, function(tag) {
    filename <- .get_filename(asOfDate, tag)
    file.copy( filename, outdir)
  })

  filesGz <- list.files(outdir,  pattern="\\.gz$", full.names=TRUE)
  lapply(filesGz, function(filename) {
    system(paste("I:/gnu/bin/gunzip.exe -f ", shQuote(filename)))
  })
}


############################################################################
# remove data older than keepDays from csv files
#
.get_cal_power <- function(region=c("NEPOOL"), daysBack=7)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"
  fname <- paste(DIR, "ice.power_trades.", tolower(region), ".RData", sep="")
  load(fname); MM <- dataAll; dataAll <- NULL 
  
  CC <- MM[grepl("^Cal [[:digit:]]{2}$", MM$STRIP), ]   # just the cal
  CC <- CC[!grepl("/", CC$HUBS), ]               # rm the spreads
  #CC$TIME <- as.POSIXct(CC$TIME, format="%m/%d/%Y %I:%M:%S %p")
  #CC$TIME <- as.POSIXct(CC$TIME, format="%Y-%m-%d %H:%M:%S")
  
  CC$PRODUCT <- gsub("^Fin Swap-", "", CC$PRODUCT)
  CC$PRODUCT <- gsub("-", "", CC$PRODUCT)
  CC$PRODUCT <- gsub(" Futures$", "", CC$PRODUCT)
  CC$ZONE <- .fix_hub_names( CC$HUBS )
  CC <- CC[,c( "HUBS", "ZONE",
              "STRIP", "PRODUCT", "QUANTITY", "TIME", "PRICE",
    "ORDER_TYPE", "IS_BLOCK_TRADE")]
  CC <- CC[order(CC$TIME),]
  CC <- subset(CC, TIME > Sys.time()-3600*24*daysBack)

  if (all(is.na(CC$ZONE)))
    CC$ZONE <- NULL
  

  return(CC)
}


############################################################################
# ICE contract volume = 1,000 therms/day.  10 therms = 1 MMBTU
# https://www.theice.com/productguide/ProductSpec.shtml?specId=910
#
.get_gas_trades <- function(hub=c("NBP"), daysBack=7)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/ICE/"
  fname <- paste(DIR, "ice.gas_trades.", tolower(hub), ".RData", sep="")
  load(fname); MM <- dataAll; dataAll <- NULL 
  
  CC <- MM[grepl("(^Cal [[:digit:]]{2}$)|(^Q)|(^Win)|(^Sum)", MM$STRIP), ]   # just the cal
  CC <- CC[-which(grepl("/", CC$STRIP)), ]               # rm the spreads
  #CC$TIME <- as.POSIXct(CC$TIME, format="%m/%d/%Y %I:%M:%S %p")
  #CC$TIME <- as.POSIXct(CC$TIME, format="%Y-%m-%d %H:%M:%S")
  
  CC <- CC[,c( "HUBS",  "STRIP", "PRODUCT", 
             "QUANTITY", "TOTAL_VOLUME", "TIME", "PRICE",
    "ORDER_TYPE", "IS_BLOCK_TRADE")]
  CC <- CC[order(CC$TIME),]
  CC <- subset(CC, TIME > Sys.time()-3600*24*daysBack)
  CC <- CC[rev(order(CC$TIME)),]
  
  return(CC)
}



############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(SecDb)
require(RODBC)


source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.download.R")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ice.stats.R")

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download.ice.prices.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
days <-  seq(asOfDate-15, asOfDate, by="1 day")
#days <- seq(as.Date("2010-01-01"), Sys.Date(), by="1 day")

tags <- c("POWER_TRADES",
          "GAS_TRADES",
          "ICECLEARED_POWER",
          "ICECLEARED_POWEROPTIONS",
          "NGXCLEARED_POWER",
          "ICECLEARED_GAS",
          "NGXCLEARED_GAS")

res <- lapply(tags, function(tag) {
  
  rLog("Download tag ", tag)
  lapply(days, function(day) {
    try( ICE$.downloadFromICE(day, tag) )
  })
  
})  

.copy_files_cne( asOfDate, tags )


rLog("zip the POWER_TRADES")
ICE$.zipInputFiles( "POWER_TRADES" )

rLog("Keep recent history for NEPOOL, PJM, ERCOT")
ICE$.archiveInputFiles(days, filterHub="^(Nepool|ISO-NE)", filterTime=TRUE)
ICE$.archiveInputFiles(days, filterHub="PJM", filterTime=TRUE)
ICE$.archiveInputFiles(days, filterHub="ERCOT", filterTime=TRUE)

# cannot use odbcConnectExcel2007 in procmon, will use xlsx package eventually
#ICE$.archiveInputFiles(days, filterHub="NBP", filterTime=TRUE, tag="GAS_TRADES")




rLog("Email cal trades Nepool ...")
CC <- .get_cal_power(region=c("NEPOOL"), daysBack=14)
if (nrow(CC)>0){
  out <- c(capture.output(print(CC, row.names=FALSE)),
    "\n\nContact: Adrian Dragulescu",
    "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/download.ice.prices")
  to <- paste(c("adrian.dragulescu@constellation.com", 
    "steven.webster@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "ICE Cal trades NEPOOL (past 7 days)", out)
} 


rLog("Email cal trades PJM ...")
CC <- .get_cal_power(region=c("PJM"), daysBack=7)
if (nrow(CC)>0){
  out <- c(capture.output(print(CC, row.names=FALSE)),
    "\n\nContact: Adrian Dragulescu",
    "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/download.ice.prices")

  to <- paste(c("adrian.dragulescu@constellation.com", 
    "steven.webster@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "ICE Cal trades PJM (past 7 days)", out)
} 

rLog("Email NBP trades ...")
CC <- .get_gas_trades(hub=c("NBP"), daysBack=7)
if (nrow(CC)>0){
  out <- c(
    "QUANTITY column is in thousand of therms/day",
    "TOTAL_VOLUME column is in therms",
    capture.output(print(CC, row.names=FALSE)),
    "\n\nContact: Adrian Dragulescu",
    "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/download.ice.prices")
  
  to <- paste(c("adrian.dragulescu@constellation.com", 
    "steven.webster@constellation.com"), sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  from <- "OVERNIGHT_PM_REPORTS@constellation.com"
  sendEmail(from, to, "ICE NBP trades (past 7 days)", out)
} 


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}




