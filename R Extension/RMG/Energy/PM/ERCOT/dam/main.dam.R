# Main Functions that will be stored in a procmon file
#
#

############################################################################
############################################################################
rm(list=ls())



#------------------------------------------------------------
# Run the file to consolidate all RT data 
# STARTDT=as.Date('2011-04-30'); ENDDT=as.Date('2011-05-01')
# need to check if have all data and recollect data for missing RT load MWs after the month ends
# collect data in following way: 12/1-12/11/10  .  12/12-12/18, 12/19-12/20, 12/21-1/28
# res=store.interval.data(startDate=as.Date("2011-04-21"), endDate=as.Date("2011-04-30"), channel_zema_prod); res
# load(file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_2010_12.RData"); min(dfRT$OPR_DATE);max(dfRT$OPR_DATE)
# load(file="S:/All/Portfolio Management/ERCOT/Daily Ercot Bidding/Daily_Analysis/rtdata_hrly_2011.RData"); min(rthourly$OPR_DATE);max(rthourly$OPR_DATE)
# 

## Procmon code
require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(SecDb)
require(RODBC)
require(FTR)
source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
## source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production

      ## Connect to Nodal N-Market
      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
STARTDT=Sys.Date()-1
ENDDT=Sys.Date()
res=store.interval.data(startDate=STARTDT, endDate=ENDDT, conn=channel_zema_prod)
## res=store.interval.data(startDate=as.Date("2011-05-01"), endDate=as.Date("2011-05-05"), channel_zema_prod); res
  odbcClose(channel_zema_prod)






#------------------------------------------------------------
# Email similar-day look-back and upload predicted SPP
# 
## Procmon code
require(CEGbase)
options(width=150)      ##  ‘width’: controls the maximum number of columns on a line used in printing vectors, matrices and arrays, and when filling by ‘cat’.
require(SecDb)
require(reshape)
require(RODBC)
require(FTR)
source("H:/user/R/RMG/Energy/PM/ERCOT/dam/lib.dam.R")  ## For testing
## source("S:/All/Risk/Software/R/prod/Energy/PM/ERCOT/dam/lib.dam.R")  ## For production
      DSN_PATH_zema_prod = paste("FileDSN=","S:/All/Risk/Software/R/prod/Utilities/DSN/ZEMA_simpson.dsn;", "UID=E13805;", "PWD=ERCOT_DAMRT;", sep="")
      channel_zema_prod = odbcDriverConnect(DSN_PATH_zema_prod)
STARTDT=Sys.Date()+1
res=rt.price.fcst(startDate=STARTDT, pkng=4.15, pklmp=36.5, RTwest=data.frame(HE=seq(1,24), limit_wn_rt=c(rep(1576,24),rep(2400,0))), conn=channel_zema_prod)
res
  odbcClose(channel_zema_prod)






#
## load NEPOOL MAP
#load("H:/user/R/RMG/Energy/PM/ERCOT/dam/map.RData")
#nfind <- function(x) MAP[grep(toupper(x), MAP$name),]
#
## check if some of the files have been done earlier today
#files <- c("H:/user/R/RMG/Energy/PM/ERCOT/dam/nepool.RT.RData",
#            "H:/user/R/RMG/Energy/PM/ERCOT/dam/nepool.RT5M.RData",
#            "H:/user/R/RMG/Energy/PM/ERCOT/dam/nepool.DA.RData")
#dates <- as.Date(as.POSIXct(unlist(lapply(files,
#  function(x){file.info(x)$mtime})), origin="1970-01-01"))
#(filesToRemove <- files[which(dates != Sys.Date())])
#
#if (length(filesToRemove)>0){
#  rLog("Try to remove:", filesToRemove)
#  file.remove(filesToRemove)  # remove only files from previous day
#}
#
#rLog("Get all prices ...")
#aux <- get.incdec.hprices(buckets=c("1X8", "1X16"), ptids=NULL,
#  do.split=TRUE, do.melt=FALSE, add.RT5M=TRUE)
#bPP <- aux[[2]]
#
#ind <- names(unlist(sapply(bPP, function(x){ncol(x)==2})))
#bPP <- bPP[ind]
#
#options <- NULL
#options$asOfDate <- Sys.Date()
#options$trades.file <-
#  "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/inddec.trades.RData"
#
##------------------------------------------------------------
## trigger.1 = see if you can trade if there is persistence,
##   if a inc/dec worked today, will it work tomorrow...
## trigger.2 = see if there is panic in DA after a strong RT day
##
#rLog("Looking for trading signals ...")
#TRD.PnL <- inc.get.trading.signals( bPP,
#  c("trigger.Persistence", "trigger.1DayPanicDA",
#    "trigger.7DayDomination"))
#
#rLog("Getting the next day trades ...")
#NXT <- next.day.trades( TRD.PnL, options)
#if (nrow(NXT)>0){
#  NXT <- merge(MAP[,c("ptid", "name", "zone", "area")], NXT, all.y=TRUE)
#  NXT <- split(NXT, NXT$area)
#  NXT <- lapply(NXT, head)     # pick only the top 6 trades by zone
#} else {
#  NXT <- "No trade ideas found for tomorrow."
#}
#
#
#out <- ""
#
##------------------------------------------------------------
## Show the Min/Max nodes
##
#out <- c(out, try(max_min_locations(files, pick=6)))
#
##------------------------------------------------------------
## Run the Hub strategy
##
#startDt <- Sys.time() - 3600*24*365  
#endDt   <- Sys.time()
#on  <- strategy_hub(8:23, startDt, endDt)
#out <- c(out, strategy_hub_rpt(on, "Recommend for ONPEAK day"))
#off <- strategy_hub(0:7, startDt, endDt)
#out <- c(out, "", strategy_hub_rpt(off, "Recommend for OFFPEAK day"))
#
#
#rLog("Sending email...")
#subject <- paste("incs/decs", Sys.Date())
#body    <- c(out, capture.output(print(NXT)))
#sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
#          to="adrian.dragulescu@constellation.com", subject, body)
#
## make it succeed.  I don't need emails from IT!
#returnCode <- 0        
#if( interactive() ){
#  print( returnCode )
#} else {
#  q( status = returnCode )
#}
#
### NXT <- rbind(
###   data.frame(ptid=388,  bucket="1X16", dec=FALSE),
###   data.frame(ptid=537,  bucket="1X16", dec=TRUE),
#             
###   data.frame(ptid=4235, bucket="1X16", dec=FALSE),
###   data.frame(ptid=4202, bucket="1X16", dec=TRUE)             
### )             
### out <- format.next.day.trades( NXT, ptids=NULL, quantity=10)
#
#