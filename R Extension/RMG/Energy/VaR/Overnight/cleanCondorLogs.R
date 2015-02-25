################################################################################
# Removes and organizes the log files created after a condor run.
# The basic logic is to remove files with size 0 and to move everything else
# into a folder for that date.  This will help prevent the log directory from
# holding thousands of files.
#
# John Scillieri 2007-06-04


################################################################################
#
cleanCondorLogs.main <- function()
{
  logDir <- "//ceg/cershare/All/Risk/Reports/VaR/prod/CondorNightlyLogs"

  for ( file in list.files(logDir) )
  {
    fullPath = paste(logDir, "/", file, sep="" )
    fileInfo <- file.info(fullPath)

    #if its a valid file and not a directory
    if ( !is.na(fileInfo$mtime) && fileInfo$isdir == FALSE )
    {
      modificationDate = as.Date(fileInfo$mtime)

      dateFolderPath = paste( logDir, "/", modificationDate, sep="")
      #if it already exists, this will fail.  No big deal.
      dir.create(dateFolderPath, showWarnings = FALSE)

      #if the size is 0 remove it, otherwise move it to the folder for it's date
      if ( fileInfo$size == 0 )
      {
        file.remove(fullPath)
      } else
      {
        newPath = paste( dateFolderPath, "/", file, sep="" )
        file.rename(fullPath, newPath)
      }
      
    }
    
  }
  
}

################################################################################
# Run the program.
# Ready for Condor.
res = try( cleanCondorLogs.main() )
if ( class(res)=="try-error" ){
  cat("Failed In Clean of Condor Log Directory.\n")
}
