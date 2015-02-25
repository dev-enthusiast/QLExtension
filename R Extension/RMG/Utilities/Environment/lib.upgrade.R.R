# A set of functions to upgrade R to a new version (from the network).
#
#
# 

source("H:/user/R/RMG/Utilities/RLogger.R")

#==============================================================================
# Change location of the Rscript.exe in the Condor files.
#
change.CONDOR.files <- function()
{
  options <- NULL
  options$R.path$old <- "C:/Program Files/R/R-2.5.0/bin/Rscript.exe"
  options$R.path$new <- "//ceg/cershare/All/Risk/Software/R/R-2.6.1/bin/Rscript.exe"

  # where to copy the files ... 
  options$R.repos <- "\\\\ceg\\cershare\\All\\Risk\\Software\\R\\uat"
  options$check.extensions <- c("condor")

  # loop over all the condor files ... 
  all.files <- list.files(options$R.repos, pattern="condor$", full.names=TRUE,
                        recursive=TRUE)
  for (file in all.files){
    rLog("Working on", file)
    aux <- readLines(file)
#  ind <- grep(options$R.path$old, aux)
    ind <- grep("Rscript.exe", aux)

    aux[ind] <- gsub("\\\\", "/", aux[ind])
    aux[ind] <- gsub(options$R.path$old, options$R.path$new, aux[ind])
    aux[ind] <- gsub("/", "\\\\", aux[ind])
    writeLines(aux, con=file) 
  }
}
