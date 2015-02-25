################################################################################
# This class lets you use "named" timers in your code. Named timers let you
# benchmark individual sections of code and pull the results in an
# easily accessible manner.
#
# Example:
#    ... code ...
#    SmartTimer$start("for loop")
#    for( x in bigList )
#    {
#      ... more code ...
#      SmartTimer$start("long function")
#      superBigFFT(dataSet)
#      SmartTimer$stop("long function")
#      ... more code ...
#    }
#    SmartTimer$stop("for loop")
#    ... more code ...
#
#    #This will report that two timers have been created and print their
#    #total run times. You can use this to see that the function 'superBigFFT'
#    #took, for example, 55 seconds of the 105 seconds that the 'for loop' ran.
#    SmartTimer$reportAll()
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
SmartTimer = new.env(hash=TRUE)


################################################################################
# Start a profiling timer of the given name.
#
SmartTimer$start <- function( timerName )
{
  if( !exists( paste(eval(timerName),".totalTime", sep=""),
               envir=SmartTimer$.timerMap) )
  {
    SmartTimer$.setTimerTotalTime(timerName, 0)
  }

  SmartTimer$.setTimerStartTime(timerName, proc.time()[3])
}


################################################################################
# stop the specified timer and append the difference to the total time
#
SmartTimer$stop <- function( timerName )
{
  currentTime = proc.time()[3]

  if( !exists( paste(eval(timerName),".startTime", sep=""),
               envir=SmartTimer$.timerMap) )
  {
    rError("Timer of name:", timerName, "does not exist!")
  } else
  {
    startTime = SmartTimer$.getTimerStartTime(timerName)
  
    difference = currentTime - startTime
    
    previousTotalTime = SmartTimer$.getTimerTotalTime(timerName)
    newTotal = difference+previousTotalTime
    SmartTimer$.setTimerTotalTime(timerName, newTotal)
  }
}


################################################################################
# Report on the state of the total time variable.  NOT how long the timer has
# been running! (If you want that, do a start->stop->report)
#
SmartTimer$report <- function( timerName )
{
  if( !exists( paste(eval(timerName),".totalTime", sep=""),
               envir=SmartTimer$.timerMap) )
  {
    rError("Timer of name:", timerName, "does not exist!")
  } else
  {
    totalTime = SmartTimer$.getTimerTotalTime(timerName)
    rLog("Timer Name:", timerName, "-", totalTime, "seconds.")
  }
}


################################################################################
# Report on the state of the total time variable for all timers.
# NOT how long all timers have been running!
# (If you want that, do a start->stop->report for each timer)
#
SmartTimer$reportAll <- function()
{

  if( length(ls(SmartTimer$.timerMap)) != 0 )
  {
  
    timerList = ls(SmartTimer$.timerMap)
    toRemove = 1:as.integer(length(timerList)/2)*2
    timerList = timerList[-toRemove]
    
    for( timerName in timerList)
    {
      timerName = sub(".startTime", "", timerName)

      totalTime = SmartTimer$.getTimerTotalTime(timerName)
      rLog("Timer Name:", timerName, "-", totalTime, "seconds.")
    }
    
  }
  
}


################################################################################
# Reset the total time of a given timer to 0.
#
SmartTimer$reset <- function( timerName )
{
  if( !exists( paste(eval(timerName),".totalTime", sep=""),
               envir=SmartTimer$.timerMap) )
  {
    rError("Timer of name:", timerName, "does not exist!")
  } else
  {
    SmartTimer$.setTimerTotalTime(timerName, 0)
  }
}


################################################################################
# Reset all timers total time back to 0.
#
SmartTimer$resetAll <- function()
{
  if( length(ls(SmartTimer$.timerMap)) != 0 )
  {
    timerList = ls(SmartTimer$.timerMap)
    toRemove = 1:as.integer(length(timerList)/2)*2
    timerList = timerList[-toRemove]
    for( timerName in timerList)
    {
      timerName = sub(".startTime", "", timerName)

      totalTime = SmartTimer$.setTimerTotalTime(timerName, 0)
    }
  }
}


################################################################################
# Remove the timer from the list of known timers.  Used to clean up printing
#
SmartTimer$remove <- function(timerName)
{
  if( !exists( paste(eval(timerName),".totalTime", sep=""),
               envir=SmartTimer$.timerMap) )
  {
    rError("Timer of name:", timerName, "does not exist!")
  } else
  {
    timerTotalTimeName = paste(eval(timerName),".totalTime", sep="")
    timerStartTimeName = paste(eval(timerName),".startTime", sep="")
    rm(list = eval(timerTotalTimeName), envir=SmartTimer$.timerMap )
    rm(list = eval(timerStartTimeName), envir=SmartTimer$.timerMap )
  }
}


################################################################################
# Cleans out the entire environment
#
SmartTimer$removeAll <- function()
{
  rm(list=ls(SmartTimer$.timerMap))
}


########################### INTERNAL OBJECTS ###################################


################################################################################
# These functions hide the nasty garbage of using character strings as
# variables across multiple environments. Its a little obscure but necessary to
# enable users to specify their own timer names.
#
SmartTimer$.getTimerTotalTime <- function(timerName)
{
  totalTime = get(paste(eval(timerName),".totalTime", sep=""),
    SmartTimer$.timerMap )
  return(totalTime)
}

################################################################################
SmartTimer$.setTimerTotalTime <- function(timerName, value)
{
  assign(paste(eval(timerName),".totalTime", sep=""),
      value, SmartTimer$.timerMap )
}

################################################################################
SmartTimer$.getTimerStartTime <- function(timerName)
{
  startTime = get(paste(eval(timerName),".startTime", sep=""),
    SmartTimer$.timerMap )
  return(startTime)
}

################################################################################
SmartTimer$.setTimerStartTime <- function(timerName, value)
{
  assign(paste(eval(timerName),".startTime", sep=""),
      value, SmartTimer$.timerMap )
}


################################################################################
# Private variables

#Stores the individual timer values
SmartTimer$.timerMap = new.env(hash=TRUE)


