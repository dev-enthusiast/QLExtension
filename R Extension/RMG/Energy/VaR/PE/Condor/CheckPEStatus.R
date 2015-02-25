################################################################################
# This file will determine what counterparties did not complete during the
# PECalculatorCondor run.
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
CheckPEStatus = new.env(hash=TRUE)


################################################################################
# Exported Variables

# The list of unfinished counterparties
CheckPEStatus$unfinishedList = NULL


################################################################################
# This pulls the list of files in the picture directory, gets the list of
# completed counterparties there and then compares that to the list of known
# counterparties.  It then logs that list to the screen.
#
CheckPEStatus$main <- function(asOfDate=NULL)
{
  PE$loadPEOptions(asOfDate)

  completed.runs <- NULL
  all.runs <- list.files( PE$options$save$reports.dir)
  for (r in all.runs){
    file <- paste( PE$options$save$reports.dir, r, "/PDF/",
                  r, "_DIRECT.pdf", sep="")
    if (file.exists(file))
      completed.runs <- c(completed.runs, r)
  }

  return(completed.runs)
}  
##   pictureDir = paste( PE$options$save$reports.dir, "PDF", sep="" )

##   directFileList = list.files( pictureDir, ".+_DIRECT.pdf" )
##   indirectFileList = list.files( pictureDir, ".+_INDIRECT.pdf" )

##   if( length(directFileList) != length(indirectFileList) )
##   {
##     rWarn("Direct/Indirect File Counts Are Not Equal.")
##     rWarn("Direct Picture File Count:", length(directFileList))
##     rWarn("Indirect Picture File Count:", length(indirectFileList))
##   }
  
##   splitNames = strsplit(directFileList, "_", fixed=TRUE)
##   justNames = lapply( splitNames, function(element){element[[1]]} )
##   completedCounterparties = as.character(justNames)
  
##   load(PE$COUNTERPARTY_POSITION_FILE)
##   knownCounterparties = as.character(unique(allPositions$cparty))
  
##   unfinished = sort(setdiff(knownCounterparties, completedCounterparties))
##   CheckPEStatus$unfinishedList = unfinished

##   rLog("There are", length(unfinished), "counterparties that did not finish of",
##    length(knownCounterparties), "known.")
##   if( long==TRUE )
##   {
##     rLog("They are:")
##     for( counterparty in unfinished )
##     {
##       rLog("\t", counterparty)
##     }
##   }
##   invisible(unfinished)


