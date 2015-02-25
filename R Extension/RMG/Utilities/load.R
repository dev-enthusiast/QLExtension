################################################################################
# These two functions let you safely load files and variables within those 
# files without having to worry about corrupt files, missing variables, etc.
# It also supports filehashes, where filehash is the pointer to the environment
#
#


################################################################################
# If the file or variable doesn't exist, fail safe
# If the file is corrupted, fail safe
# "load" should do all this and it doesnt...
# Returns a boolean if loaded.
#
safeLoad <- function( filePath, envir=parent.frame(), quiet=FALSE, filehash=NULL )
{
  if( !file.exists(filePath) )
  {
    if(!quiet) cat("File not found:", filePath, "\n")
    return(FALSE)
  }

  if (is.null(filehash)){
    res = try( load(filePath, envir) )
  } else {
    res <- try( filehash[[basename(filePath)]] )
  }

  if( class(res)=="try-error" )
  {
    if(!quiet) cat("Failed loading file.\n")
    return(FALSE)
  }

  invisible(TRUE)
}


################################################################################
# Only load the specified symbol using the safeLoad function
# Returns the value requested or NULL if unable.
#
varLoad <- function( variable, filePath, quiet=FALSE )
{
  tempEnv = new.env(hash=TRUE)
  if( !safeLoad(filePath, tempEnv, quiet) )
  {
    return(NULL)
  }

  if( !exists(variable, envir=tempEnv) )
  {
    if(!quiet) cat("Variable named:", variable, "not found. Returning NULL.\n")
    return(NULL)
  }

  value = tempEnv[[variable]]
  invisible( value )
}


