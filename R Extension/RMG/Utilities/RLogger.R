################################################################################
# This class implements logging similar to packages such as Log4j.  
# It is very rudimentary in that all you can do is set the log level and 
# output based on that level.  It's not a class, it's not a package, it's really
# really basic.
#
# To Implement:
#   log with date/time
#   log file name
#   log method name
#   log line number

################################################################################
# Exported Variables
RLoggerLevels = new.env(hash=TRUE)
RLoggerLevels$LOG_OFF = 0
RLoggerLevels$LOG_ERROR = 1
RLoggerLevels$LOG_WARNING = 2
RLoggerLevels$LOG_MESSAGE = 3
RLoggerLevels$LOG_DEBUG = 4
RLoggerLevels$LOG_TRACE = 5

# Private variable used to store the current, global logging level
.RLogger.logLevel <<- RLoggerLevels$LOG_MESSAGE

# Private variable used to store the current output connection
.RLogger.logConnection <<- ""

################################################################################
# Call this function from your class to set the overall logging level.
#
setLogLevel <- function( level )
{
  .RLogger.logLevel <<- level
}


################################################################################
# Call this function to dump logs to a connection (file, text, etc)
#
redirectLogOutput <- function( connection = "" )
{
    .RLogger.logConnection <<- connection
}


################################################################################
# Exported logging functions
# Use these to log in your program
# 
rError <- function( ... ) 
{ 
  .Log(prefix="ERROR:", logLevel=RLoggerLevels$LOG_ERROR, ...)
}

rWarn <- function( ... )
{
  .Log(prefix="Warning:", logLevel=RLoggerLevels$LOG_WARNING, ...)
}

rLog <- function( ... )
{
  # This is a sneaky, hairy little trick.  We're putting the message
  # as the prefix variable because we don't want a space in front of the 
  # message.  It enables us to keep our .Log function simple and using only
  # one conditional if statement (instead of having a 'if prefix == "" '
  .Log(prefix=..., logLevel=RLoggerLevels$LOG_MESSAGE )
}

rDebug <- function( ... )
{
  .Log(prefix="DEBUG:", logLevel=RLoggerLevels$LOG_DEBUG, ...)
}

rTrace <- function( ... )
{
  .Log(prefix="TRACE:", logLevel=RLoggerLevels$LOG_TRACE, ...)
}


################################################################################
# This function performs a similar operation to the unix command 'ls -l' 
# It works exactly like ls() in R except that it will also display object class,
# dimension/length, and object size.  
# It can also take a class filter to view only objects of a certain type.
#
# Usage:
#   lls() # list all objects
#   lls(environmentName) # list all the objects in the specified environment
#   lls(pattern="foo") # list all objects whose name matches pattern specified
#   lls(classFilter="data.frame") # find all data frame objects
#
lls <- function(name, pos=-1, envir=as.environment(pos), all.names=FALSE, 
  pattern, classFilter)
{ 
  if( !missing(name) )
  {
    envir = name
  } else
  {
    envir = parent.frame()
  }
  
  lsList = ls(name, pos, envir, all.names, pattern)   

  cat("\nName: Class, Length, Size\n")
  cat("-------------------------\n")
  for( item in lsList )
  {
    realItem = eval(parse(text = item), envir)
    itemClass = class(realItem)
    itemSize = object.size(realItem)
    itemDimension = paste(dim(realItem), collapse="x" )
    if( itemDimension == "" )
    {
      itemDimension = length(realItem)
    }
    
    classFilterMatches = !missing(classFilter) && itemClass == classFilter
    if( classFilterMatches || missing(classFilter) )
    {
      format(cat(item, ": ", itemClass, ", ", itemDimension, ", ", itemSize, "\n", 
        sep=""), justify="centre")
    } 
  } 
  cat("\n") 
  
  invisible(lsList)
}


################################################################################
# Overload the internal object.size function to return the size in B/KB/MB
#
object.size <-function(x)
{
    size = .Internal(object.size(x)) 
    
    if (size<1024)
    {
        size=paste(size, "B")
    }
    else if (size>=1024 & size < 1024*1024)
    {
        size=paste( round(size/1024, 1), "KB")
    }
    else
    {
        size=paste( round( size/1024/1024, 1 ),"MB")
    }
    
    return(size)
}

        
################################################################################
# Rename the column quickly
#
colRename <-function(data, old, new)
{
    toRename = which( names(data)==old )
    names(data)[toRename] = new
    return( data )
}

############################INTERNAL FUNCTIONS##################################


################################################################################
# Internal workhorse function of the logging class
#
.Log <- function( prefix, logLevel, ... )
{
  if( .RLogger.logLevel >= logLevel )
  {
    cat(prefix, ..., "\n", file = .RLogger.logConnection )
  } 
}

################################################################################
# Doesn't work... we don't have a way to get the filename of the parent
# and the callingLine isn't exactly equal to the source line from the file
.getLineNumber <- function()
{
  strtrim = function(x){sub ("^[ \t]*", "", sub ("[ \t]*$", "", x)) }


  callStack = sys.calls()
  callingLine = callStack[[length(callStack)-3]]
  
  fileName = sys.frames()[[1]]$ofile
  
  sourceCode = readLines(fileName)
  sourceCode = strtrim(noquote(sourceCode))
  
  line = which(as.logical(lapply(sourceCode, function(x){x==callingLine})))
  
  return(line)
}

