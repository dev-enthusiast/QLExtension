# Monitor the generation needed for RMR and for Voltage Support
#
# Written by Adrian Dragulescu on 20-May-2008



#########################################################################
# Geographic/Nepool/Nepol Data/Nepool Transmission Congestion/
# the code could be refactored to work for any symbols, but I won't do it 
# now...
#
.monitor_mw <- function(startDate, endDate, zeroDaysIgnore,
   symbolsRT, pattern="NEPL_TC_(.*)_.*")
{
  RT <- FTR:::FTR.load.tsdb.symbols(symbolsRT, startDate, endDate)
  colnames(RT) <- gsub(pattern, "\\1", colnames(RT))

  symbolsDA <- gsub("_RT_", "_DA_", symbolsRT)
  DA <- FTR:::FTR.load.tsdb.symbols(symbolsDA, startDate, endDate)
  colnames(DA) <- gsub(pattern, "\\1", colnames(DA))
  
  GG <- cbind(RT, DA)  # merge them
  
  ind <- which(apply(GG, 2, function(x){all(x==0|is.na(x))}))
  if (length(ind)>0)
    GG <- GG[,-ind, drop=FALSE]

  if (nrow(GG) == 0 | ncol(GG)==0)
    return(NULL)

  # aggregate daily
  DD <- cbind(datetime=as.character(index(GG)), data.frame(GG))
  DD$date <- as.Date(substr(DD$datetime, 1, 10))
  DD$datetime <- NULL
  DD  <- melt(DD, id=ncol(DD))
  out <- cast(DD, variable + date ~ .,
    function(x){fivenum(x, na.rm=TRUE)}, fill=0)
  #out <- out[,c(1:3, 5, 7)]
  out <- out[,c(1:2, 5)]         # pick only the median for now...
  out <- cast(out, date ~ variable, function(x){round(x)}, fill=NA, value="X3")
  
  if (all(tail(out[,-1], zeroDaysIgnore)==0))  # if you have zeros in the last few days
    return(NULL)
  else
    return(out)
}


############################################################################
#
.prepare_output <- function(GG)
{
  out <- capture.output(print(GG, row.names=FALSE))

  return(out)  
}

#########################################################################
#
.send_email <- function(GG, header, subject)
{
  if (!is.null(GG)){
    out <- c(paste(header, ":\n\n", sep=""), .prepare_output(GG),
       "\n\n\nContact: Adrian Dragulescu",
       paste("Procmon job:",
        "RMG/R/Reports/Energy/Trading/Congestion/nepool_monitor_generation_RMR"))
    to <- paste(c("rujun.hu@constellation.com", 
      "necash@constellation.com"), sep="", collapse=",")
    #to <- "adrian.dragulescu@constellation.com"
    from <- "OVERNIGHT_PM_REPORTS@constellation.com"
    sendEmail(from, to, subject, out)
  } 
}

  
############################################################################
############################################################################
options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(SecDb)
require(zoo)


source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.generation.R")

returnCode <- 0   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/nepool_monitor_generation_RMR.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate  <- Sys.Date()
startDate <- asOfDate - 31
endDate   <- asOfDate

q( status=0 )   # symbols are not updating anymore.  stop this for a while.


subject <- "Latest NEPOOL MW called for RMR"
header  <- "Generation called for RMR in NEPOOL"
symbolsRT <- OOMRT_TSDB_SYMBOLS
zeroDaysIgnore <- 6 
GG <- .monitor_mw(startDate, endDate, zeroDaysIgnore, symbolsRT)
.send_email(GG, header, subject)

subject <- "Latest NEPOOL MW called for voltage support"
header  <- "Generation called for voltage support in NEPOOL"
symbolsRT <- VARUPRT_TSDB_SYMBOLS
zeroDaysIgnore <- 10                  # these reports are slower to update
GG <- .monitor_mw(startDate, endDate, zeroDaysIgnore, symbolsRT)
.send_email(GG, header, subject)


rLog("Done at ", as.character(Sys.time()))

if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}


