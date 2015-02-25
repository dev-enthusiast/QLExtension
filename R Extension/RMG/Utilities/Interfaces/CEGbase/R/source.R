# Pimp the source function to take relative paths 
#
#

source <- function(file, verbose=getOption("verbose"), ...)
{

  isURL <- length(grep("^(ftp|http|file)://", file)) > 0L
  
  if (!file.exists(file) && is.character(file) && !isURL && (file!=""))
    file <- paste(getroot(), file, sep="")
  
  base:::source(file, verbose=verbose, ...)
  
}



  ## if (!(R_ENV_CEG %in% c("user", "dev", "prod", "")))
  ##   stop("Wrong R_ENV_CEG variable name.")
