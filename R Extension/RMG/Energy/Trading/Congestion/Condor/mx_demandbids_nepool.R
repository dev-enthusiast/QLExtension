# Upload MX demand bids to tsdb.
#
# Written by Adrian Dragulescu on 8/23/2011


#################################################################
#
.fix_DST <- function(db)
{
  ind <- which(db$Date %in% c("2011-11-06", "2012-11-04", "2013-11-03",
                              "2014-11-02"))
  if (length(ind) > 0) {
    hr2x <- subset(db[ind,], Hour.Ending == 1)
    hr2x$datetime <- hr2x$datetime + 3600
    db <- rbind(db, hr2x)
    db <- db[order(db$ptid, db$datetime),]
  }
  
  db
}

#################################################################
#
.update_tsdb_symbols <- function(db)
{
  uZones <- unique(db$zone)

  aux <- split(db, db$zone)
  
  for (i in 1:length(aux)) {
    symbol <- paste("mx_nepool_ld_fcst_", names(aux)[i], sep="")
    
    rLog("Updating symbol ", symbol)
    if (!file.exists(paste("T:/Data/pm/nepool/", symbol, ".bin", sep=""))) {
      rLog("Create ", symbol, "symbol")
      path <- paste("T:/Data/pm/nepool/", symbol, ".bin", sep="")
      description <- paste("MX short term load forecast, zone:", names(aux)[i], sep="")
      dataSource  <- "MX Energy, daily csv files"
      realTimeFlag <- TRUE
      tsdbCreate(symbol, path, description, dataSource, realTimeFlag=realTimeFlag)
    }
    
    tsdbUpdate(symbol, data.frame(time=aux[[i]]$datetime, value=aux[[i]]$load))
  }
  
}


#################################################################
#################################################################
options(width=500)  # make some room for the output
options(stringsAsFactors=FALSE)

require(CEGbase)
require(SecDb)
require(reshape)
require(Tsdb)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.ccg.loads.R")


rLog(paste("Start proces at", Sys.time()))
returnCode <- 0    # succeed = 0

asOfDate  <- Sys.Date() + 1
startDt   <- asOfDate-7          #as.Date("2011-08-01")  
endDt     <- asOfDate+4          # allow for weekends

db <- get_mx_demandbids(startDt, endDt, format="long")

db <- .fix_DST(db)

.update_tsdb_symbols(db)


rLog(paste("Done at ", Sys.time()))
q(status = returnCode)


