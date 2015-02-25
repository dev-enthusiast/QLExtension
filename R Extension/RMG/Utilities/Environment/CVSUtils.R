################################################################################
# This file provides wrappers around CVS functions.
#

################################################################################
# File Namespace
#
CVSUtils = new.env(hash=TRUE)


################################################################################
# This is useful to check if a file you are modifying, moving to prod, etc
# is up-to-date or has been modified, needs a patch, etc.
#
CVSUtils$isFileUpToDate <- function( fileName )
{
  oldBaseDir = getwd()
  basePath = dirname(fileName)
  setwd( basePath )
  
  statusResult = system( paste("cvs status -l",basename(fileName)), intern=TRUE)
  filteredOutput = grep("Status", statusResult, value=TRUE)

  setwd( oldBaseDir )

  returnFlag = ifelse( length(grep("Up-to-date", filteredOutput))==0, FALSE, TRUE )

  return(returnFlag)
}


################################################################################
# This is useful to check if a file you are modifying, moving to prod, etc
# is up-to-date or has been modified, needs a patch, etc.
#
CVSUtils$getLastLogEntry <- function( fileName )
{
  oldBaseDir = getwd()
  basePath = dirname(fileName)
  setwd( basePath )
  
  statusResult = system( paste("cvs log -l",basename(fileName)), intern=TRUE)
  REVISION_SEPARATOR = "----------------------------" 
  splitResult = strsplit(statusResult, split="\n")
  divisionIndexes = which(splitResult==REVISION_SEPARATOR)
  if( !is.na(divisionIndexes[1]) && !is.na(divisionIndexes[2]) )
  {
    finalResult = splitResult[divisionIndexes[1]:divisionIndexes[2]]
  } else
  {
    finalResult = "Initial Commit"
  }
  setwd( oldBaseDir )

  return(as.character(finalResult))
}

