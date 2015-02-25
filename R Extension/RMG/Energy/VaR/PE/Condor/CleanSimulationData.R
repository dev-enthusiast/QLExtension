################################################################################
# This cleans all the simulation data for the PE job.
#
# Steps:
#    List folders of date format, YYYY-MM-DD, in PE$BASE_SIM_DIR
#    Remove any folders greater than asOfDate-1 day old
#    Check if asOfDate/<RUN MODE> folder exists
#    If so, remove all subfolders & files
#    List files in asOfDate/<RUN MODE> folder
#    If fileList != 0 ERROR with message
#    Else recreate folder structure
#
    

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/mkdir.R") 
source("H:/user/R/RMG/Utilities/delete.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")


################################################################################
# File Namespace
#
CleanSimulationData = new.env(hash=TRUE)


################################################################################
CleanSimulationData$main <- function(isInteractive=FALSE)
{
  asOfDate = as.Date(overnightUtils.getAsOfDate())

  runMode = ifelse(isInteractive, "interactive", "overnight")

  # rough date format regex
  datePattern = "^[2-9][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9]$"

  fileList = list.files( PE$BASE_SIM_DIR, datePattern, full.names=TRUE )
  dates = as.Date(basename(fileList))
  
  # Remove any folders greater than asOfDate-7 day old
  foldersToRemove = which(dates < asOfDate-7)
  
  # Only remove the overnight subfolders of the days to remove
  deleteThese = paste( fileList[foldersToRemove], "/overnight", sep="" )
  delete(deleteThese)
  
  
  baseDirectory = paste( PE$BASE_SIM_DIR, asOfDate, "/", runMode, sep="")
  
  #    Check if asOfDate folder exists
  if( length(which(dates==asOfDate)) != 0 )
  {
  
    # If so, remove all subfolders & files
    currentDataFileList = list.files( baseDirectory, full.names=TRUE )
    if( length(currentDataFileList) != 0 )
    {
      delete(currentDataFileList)
    }

    # List files in asOfDate folder
    afterRemove = list.files( baseDirectory, full.names=TRUE )
    # If fileList != 0 ERROR with message
    if( length(afterRemove) != 0 )
    {
      stop(paste("Unable to remove simulation data from", baseDirectory,
                 ". Exiting.", sep=""))
    } else
    {
      rLog("Simulation Data Removed For", as.character(asOfDate))
      
      rLog("Recreating folder structure...")
      mkdir(paste(baseDirectory, "/swap/", sep=""))
      mkdir(paste(baseDirectory, "/result/", sep=""))
      mkdir(paste(baseDirectory, "/report/", sep=""))
      rLog("Done.")
    }
  }else
  {
    #Create it so you can write there later.
    rLog("Creating folder structure...")
    mkdir(paste(baseDirectory, sep=""))
    mkdir(paste(baseDirectory, "/swap/", sep=""))
    mkdir(paste(baseDirectory, "/result/", sep=""))
    mkdir(paste(baseDirectory, "/report/", sep=""))
    rLog("Done.")
  }
  
}



