# Update the price filehash with the asOfDate prices.
# Condor ready.  Called by Condor/PRICES_FILEHASH.condor file.
#
# Run from command line:
#   >set BLOCK_SIZE=2700
#   >set R_BLOCK=1 
#   >set asOfDate=2007-09-05
#   >Rscript --vanilla overnightPriceJobFilehash.R 
# 
# Last modified by Adrian Dragulescu on 01-Aug-2007
#
# For debugging: 
# Sys.setenv(asOfDate="2007-09-05"); Sys.setenv(R_BLOCK=1); Sys.setenv(BLOCK_SIZE=20)
#============================================================
overnightPriceJobFilehash.main <- function(){
  
  source("H:/user/R/RMG/Utilities/RLogger.R")  
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  source("H:/user/R/RMG/Energy/VaR/Overnight/filehash.marks.utils.R")
  source("H:/user/R/RMG/Utilities/Packages/load.packages.r")  
  
  asOfDate <- Sys.getenv("asOfDate")
  if (asOfDate==""){                     # if the environment variable is not set 
    asOfDate <- overnightUtils.getAsOfDate()
  } else {
    asOfDate <- as.Date(asOfDate)
  }
  rLog(paste("Running overnightPriceJobFilehash.main for", asOfDate, "\n"), sep="")
  BLOCK_SIZE <- as.numeric(Sys.getenv("BLOCK_SIZE")) # in case you run it by hand
  if (is.na(BLOCK_SIZE)){BLOCK_SIZE <- 500}
  
  options <- NULL
  options$asOfDate <- as.Date(asOfDate)
  options$save$datastore.dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/"
  rLog(paste("Sending ", asOfDate, "prices to the price filehashes ...\n"))
  rLog(paste("Loading ", as.character(asOfDate), " day prices.", sep=""))
  file <- paste(options$save$datastore.dir,"Prices/hPrices.", options$asOfDate,
    ".RData", sep="")
  if (!file.exists(file)){
    stop("Cannot find ", file, "!\n")
  }
  load(file)
  uCurves <- sort(unique(hP$curve.name))

  rLog("Connecting to the price filehash.")
  load.packages(c("filehash"))
  filehashOption(defaultType = "RDS")      # need this, dbInit will fail
  db  <- dbInit("//nas-msw-07/rmgapp/Prices/Historical")
  hPP <- db2env(db)
  if (!is.environment(hPP)){
    rError("Could not load the price filehash environment.\n")
    stop("Exiting.\n")
  }

  block   <- as.numeric(Sys.getenv("R_BLOCK"))
  if (is.na(block)){block <- 1}                  # one block if run by hand  
  ind.start <- max(1, (block-1)*BLOCK_SIZE+1)
  ind.end   <- min((block*BLOCK_SIZE), length(uCurves))
  if (ind.start > length(uCurves)){    # one block to many
    rLog("No more curves to insert.  Exiting")
    return()
  }
  ind.c <- ind.start:ind.end
  ind.dropped <- NULL                 # index of curves dropped
  rLog(paste("Starting to insert curves with index ", ind.start, " to ",
    ind.end, " into the hash...", sep=""))
  for (i in ind.c){                # loop over the curves you need to insert
    res <- insert.one.curve(uCurves[i], db, hPP, hP, options)
    if (class(res)=="try-error"){
      rLog(paste("Could not insert curve:", curve, " into filehash\n", sep=""))
      ind.dropped <- c(ind.dropped, i)
    }
  } 
  if (length(ind.dropped)>0){
    source("H:/user/R/RMG/Utilities/sendemail.R")
    from   <- "adrian.dragulescu@constellation.com"
    toList <- c("john.scillieri", "adrian.dragulescu", "michael.zhang" ,"jing.bai","haibin.sun")
    toList <- paste(paste(toList, "@constellation.com",sep=""), collapse=", ")
    subject <- paste("Could not insert into filehash (", asOfDate, "):")
    body <- paste(uCurves[ind.dropped], "was not inserted into hash.")
    sendEmail(from, toList, subject, body)
  } else {
    rLog(paste("All", length(ind.c), "curves were uploaded successfully."))
  }
}

#---------------------------------------------------------------------
overnightPriceJobFilehash.main()  


