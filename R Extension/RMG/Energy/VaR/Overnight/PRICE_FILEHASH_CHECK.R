# Check what prices are not in the filehash and report ....
#
#
# Written by Adrian Dragulescu on 6-Mar-2008


################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/ForwardPriceOptions.R")

################################################################################
# File Namespace
#
PFC <- new.env(hash=TRUE)

################################################################################
# Constants
PFC$dir.hPrices  <- "S:/All/Risk/Data/VaR/prod/Prices/"
PFC$dir.filehash <- "R:/Prices/Historical/"


################################################################################
# Main function
#
PFC$main <- function()
{
  asOfDate <- overnightUtils.getAsOfDate()

  rLog("Creating Forward Price Options")
  FP.options <- ForwardPriceOptions$create(asOfDate)
  rLog("Options Created.")
  
  rLog("Connecting to the simulated price filehash ...")
  hPP.env <- .getFileHashEnv(PFC$dir.filehash)

  fileName <- paste(PFC$dir.hPrices, "hPrices.", asOfDate, ".RData", sep="")
  rLog(paste("Loading", fileName))
  load(fileName)
  rLog("Done.")

  uCurves <- unique(hP$curve.name)
  missing <- NULL
  for (ind.c in 1:length(uCurves)){
    rLog(paste("Checking", uCurves[ind.c]))
    this.hP <- subset(hP, curve.name==uCurves[ind.c])

    fhP <- try(hPP.env[[tolower(uCurves[ind.c])]])
    if (class(fhP)=="try-error")
      rLog(paste("Could not read curve:", uCurves[ind.c], "\n", sep=""))
    
    if (is.null(fhP) || !(as.character(asOfDate) %in% names(fhP))){
      missing <- c(missing, uCurves[ind.c])
    }
  }

  toList  <- getEmailDistro("r_developers")
  from    <- "OVERNIGHT-DO-NOT-REPLY@constellation.com"
  if (length(missing)>0){
    subject <- paste("These ", length(missing),
      " curves failed to make it in the filehash for ", asOfDate, sep="")
  } else {
    subject <- "All price curves made it safely to the filehash."
  }
  sendEmail(from, toList, subject, missing)
  
  rLog("Done.")
}

################################ MAIN EXECUTION ################################
.start = Sys.time()

PFC$main()

Sys.time() - .start
