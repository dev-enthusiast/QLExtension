################################################################################
# This cleans all the pdf and rdata generated from the counterparty
# calculation in the PE job.
#
# Steps:
#    Remove PE$options$save$reports.dir
#    List files in PE$options$save$reports.dir folder
#    If fileList != 0 ERROR with message
#    Else recreate folder
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
CleanPEReportData = new.env(hash=TRUE)


################################################################################
CleanPEReportData$main <- function()
{
  asOfDate = overnightUtils.getAsOfDate()
  PE$loadPEOptions(asOfDate)

  rLog("Cleaning Report Directory At:", PE$options$save$reports.dir)
  delete(PE$options$save$reports.dir)

  # List files in asOfDate folder
  afterRemove = list.files( PE$options$save$reports.dir )
  # If fileList != 0 ERROR with message
  if( length(afterRemove) != 0 )
  {
    stop("Unable to remove counterparty data. Exiting.")
  } else
  {
    rLog("Counterparty Data Removed For", as.character(asOfDate))

    rLog("Recreating folder structure...")
    
    mkdir(PE$options$save$reports.dir)
    
##     mkdir(paste(PE$options$save$reports.dir, "RData", sep=""))
    
##     picDirName = toupper(sub(".", "", PE$CURVE_PICTURE_EXT, fixed=TRUE))
##     mkdir(paste(PE$options$save$reports.dir, picDirName, sep=""))
    
    rLog("Done.")
  }


}


