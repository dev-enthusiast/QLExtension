
################################################################################
# File Namespace, constants
#
PJM <- new.env(hash=TRUE)

PJM$mkt      <- "PWJ"
PJM$region   <- "PJM"
PJM$ROOT_DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/PJM/"

#PJM$MAP <- PJM$.loadMAP()  # load the map


################################################################################
################################################################################
# Run this every month
#
.main <- function()
{
  require(CEGbase)
  require(FTR)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.iso.R")
  
  FTR.load.ISO.env("PJM")

  PJM$.makeMAP()                  # add the latest pnode table, SecDb nodes


  
}

################################################################################
# Load the NEPOOL MAP
#
PJM$.loadMAP <- function()
{
  filename <- paste(PJM$ROOT_DIR, "RData/map.RData", sep="")
  load(filename)
  rLog("Loaded PJM MAP.")
  
  return(MAP)
}


################################################################################
# Make the PJM MAP, read from SecDB the report manager, clean it a bit, 
# 
PJM$.makeMAP <- function()
{
  
  MAP <- FTR:::FTR.ReportManager("PJM")

  rLog("Saving the PJM map to the DB RData folder")
  filename <- paste(PJM$ROOT_DIR, "RData/map.RData", sep="")
  save(MAP, file=filename)
  rLog("Done.")
}


