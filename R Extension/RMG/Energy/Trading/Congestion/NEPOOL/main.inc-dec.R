# Procmon runs this file too.  Process 
# 
#

############################################################################
############################################################################
options(width=200)
require(FTR)
require(CEGbase)

source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.inc-dec.R")

# load NEPOOL MAP
load("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/map.RData")
nfind <- function(x) MAP[grep(toupper(x), MAP$name),]

# check if some of the files have been done earlier today
files <- c("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.RT.RData",
            "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.RT5M.RData",
            "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/nepool.DA.RData")
dates <- as.Date(as.POSIXct(unlist(lapply(files,
  function(x){file.info(x)$mtime})), origin="1970-01-01"))
(filesToRemove <- files[which(dates != Sys.Date())])

if (length(filesToRemove)>0){
  rLog("Try to remove:", filesToRemove)
  file.remove(filesToRemove)  # remove only files from previous day
}

rLog("Get all prices ...")
aux <- get.incdec.hprices(buckets=c("1X8", "1X16"), ptids=NULL,
  do.split=TRUE, do.melt=FALSE, add.RT5M=TRUE)
bPP <- aux[[2]]

ind <- names(unlist(sapply(bPP, function(x){ncol(x)==2})))
bPP <- bPP[ind]

options <- NULL
options$asOfDate <- Sys.Date()
options$trades.file <-
  "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/RData/inddec.trades.RData"

#------------------------------------------------------------
# trigger.1 = see if you can trade if there is persistence,
#   if a inc/dec worked today, will it work tomorrow...
# trigger.2 = see if there is panic in DA after a strong RT day
#
rLog("Looking for trading signals ...")
TRD.PnL <- inc.get.trading.signals( bPP,
  c("trigger.Persistence", "trigger.1DayPanicDA",
    "trigger.7DayDomination"))

rLog("Getting the next day trades ...")
NXT <- next.day.trades( TRD.PnL, options)
if (nrow(NXT)>0){
  NXT <- merge(MAP[,c("ptid", "name", "zone", "area")], NXT, all.y=TRUE)
  NXT <- split(NXT, NXT$area)
  NXT <- lapply(NXT, head)     # pick only the top 6 trades by zone
} else {
  NXT <- "No trade ideas found for tomorrow."
}


out <- ""

#------------------------------------------------------------
# Show the Min/Max nodes
#
out <- c(out, try(max_min_locations(files, pick=6)))

#------------------------------------------------------------
# Run the Hub strategy
#
startDt <- Sys.time() - 3600*24*365  
endDt   <- Sys.time()
on  <- strategy_hub(8:23, startDt, endDt)
out <- c(out, strategy_hub_rpt(on, "Recommend for ONPEAK day"))
off <- strategy_hub(0:7, startDt, endDt)
out <- c(out, "", strategy_hub_rpt(off, "Recommend for OFFPEAK day"))


rLog("Sending email...")
subject <- paste("incs/decs", Sys.Date())
body    <- c(out, capture.output(print(NXT)))
sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
          to="adrian.dragulescu@constellation.com", subject, body)

# make it succeed.  I don't need emails from IT!
returnCode <- 0        
if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}

## NXT <- rbind(
##   data.frame(ptid=388,  bucket="1X16", dec=FALSE),
##   data.frame(ptid=537,  bucket="1X16", dec=TRUE),
             
##   data.frame(ptid=4235, bucket="1X16", dec=FALSE),
##   data.frame(ptid=4202, bucket="1X16", dec=TRUE)             
## )             
## out <- format.next.day.trades( NXT, ptids=NULL, quantity=10)

