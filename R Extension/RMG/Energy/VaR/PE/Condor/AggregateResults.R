################################################################################
# Aggregate overnight results and save to the archive folder
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
AggregateResults = new.env(hash=TRUE)


################################################################################
AggregateResults$main <- function()
{
  PE$loadPEOptions()
  reportsDir <- PE$options$save$reports.dir
  rLog("Reading RData from:", reportsDir)
  
  files <- list.files(reportsDir, full.names=TRUE) # all the dirs
  res <- vector("list", length(files))

  rLog("Reading", length(files), "PE runs...")

  for (f in 1:length(files)){
    runName  <- strsplit(files[f], "/")[[1]]
    runName  <- runName[length(runName)]
    fileName <- paste(files[f], "/RData/", runName, "_DIRECT.RData", sep="")
    if (file.exists(fileName)){load(fileName)}
    res[[f]] <- aux
  }
  
  rLog("Binding Results Array...")
  res <- do.call(rbind, res)

  filename <- paste(PE$ROOT_DIR, "archive/",PE$options$asOfDate,
    ".PE.RData", sep="")
  save(res, file=filename)
  rLog("Results saved to:", filename)
  
  rLog("Casting data for csv save...")
  aux <- cast(res, ... ~ contract.dt, I)  # this takes 4 min

  filename <- paste(PE$ROOT_DIR, "archive/", PE$options$asOfDate,
    ".PE.csv", sep="")
  write.csv(aux, file=filename, row.names=FALSE)

  rLog("Archived overnight results in", filename)
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
AggregateResults$main()
Sys.time() - .start
