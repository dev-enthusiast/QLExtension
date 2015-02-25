################################################################################
# Utilities to use for the overnight condor run
#
# John Scillieri 2007-05-01
options( stringsAsFactors = FALSE )


################################################################################
# Loads all the required overnight options generated from the 
# main.overnight.data.R file.
# It also sources the required files and restablishes any database connections
# prior to returning the options data frame
#
overnightUtils.loadOvernightParameters <- function(asOfDate, connectToDB=TRUE)
{
  computer = Sys.getenv("COMPUTERNAME")

  print( paste( "overnightUtils: Loading Overnight Parameters in", 
    R.Version()$version.string, "on computer:", computer, "..." ) )
  
  #load the required packages
  overnightUtils.sourceRequiredFiles()
  load.packages( c("RODBC", "xtable", "reshape", "outliers") )


  #read in the overnight params created from that date
  datastore.dir <- "//ceg/cershare/All/Risk/Data/VaR/prod/"
  filename <- paste(datastore.dir, "options.overnight.", asOfDate,
                    ".RData", sep="")
  backupLoad(filename)
  
  if(connectToDB)
  {
    options.overnight = 
      overnightUtils.restablishDatabaseConnections(options.overnight)
  }
  
  print("overnightUtils: Overnight Parameters Loaded.")
  return(options.overnight)
}

################################################################################
# We're trying to get the asOfDate two ways before defaulting to yesterday.
# First we try the arguments passed in, then the environment variable: asOfDate
# and if neither of those are set, we default to today.
#
overnightUtils.getAsOfDate <- function()
{

  #If we're a monday -> asOfDate = Friday, otherwise -> go day before.
  if( format(Sys.time(), "%w") == 1 )
  {
    asOfDate = Sys.Date()-3
  } else
  {
    asOfDate = Sys.Date()-1
  }

  args <- commandArgs()
  indexOfArgsFlag = match("--args", args)
  
  #if we were passed the --args flag and there was something after it
  if (is.na(indexOfArgsFlag)==FALSE && length(args[-(1:indexOfArgsFlag)]) != 0)
  {
    #set the date to the first argument after --args
    args <- args[-(1:indexOfArgsFlag)]
    asOfDate = args[1] 
      
  } else
  {
    #No arguments so check the environment variable
    testIfSet = Sys.getenv("asOfDate")
    if (testIfSet != "")
    {
      asOfDate = testIfSet
    }  
    #otherwise just use the Sys.Date() value set above
  }
  
  print(paste("overnightUtils: Returning asOfDate = ",asOfDate,".", sep=""))
  return(asOfDate)
}

################################################################################
# Because database connections can't be written out, we need to reestablish
# them after loading of the options
#
overnightUtils.restablishDatabaseConnections <- function(options)
{

  print("overnightUtils: Reestablishing database connections...")
  con.string <- paste("FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn",
       ";UID=stratdatread;PWD=stratdatread;", sep="")
  options$connections$CPSPROD <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/VCTRLP.dsn",
       ";UID=vcentral_read;PWD=vcentral_read;", sep="")
  options$connections$VCTRLP <- odbcDriverConnect(con.string)
  con.string <- paste("FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn",
       ";UID=rmsys_read;PWD=rmsys_read;", sep="")
  options$connections$RMSYSP <- odbcDriverConnect(con.string)
    
  print("overnightUtils: Database connections established.")
  return(options)
}

################################################################################
# Source the required files to run the overnight job.
#
overnightUtils.sourceRequiredFiles <- function()
{
  print("overnightUtils: Sourcing required files...")
  sourceDir = "H:/user/R/RMG/Energy/VaR/"
  
  # Source the base dir
  path.dir <- paste(sourceDir,"Base/",sep="")
  lapply(dir(path.dir, "\\.[rR]$", full.name=T), source)  # source it all
  source(paste(sourceDir, "Base/add.price.vol.fillings.R", sep=""))
  source(paste(sourceDir, "Base/adjust.bidweek.R", sep=""))
  source(paste(sourceDir, "Base/aggregate.run.positions.R", sep=""))
  source(paste(sourceDir, "Base/aggregate.run.prices.R", sep=""))
  source(paste(sourceDir, "Base/calc.correlation.sensitivity.R", sep=""))
  source(paste(sourceDir, "Base/calc.CVaR.R", sep=""))
  source(paste(sourceDir, "Base/calc.incremental.VaR.R", sep=""))
  source(paste(sourceDir, "Base/calc.PnL.by.factor.R", sep=""))
  source(paste(sourceDir, "Base/convert.prices.to.USD.R", sep=""))
  source(paste(sourceDir, "Base/core.calc.VaR.R", sep=""))
  source(paste(sourceDir, "Base/get.no.packets.R", sep=""))
  source(paste(sourceDir, "Base/get.portfolio.book.R", sep=""))
  source(paste(sourceDir, "Base/main.run.VaR.R", sep=""))
  source(paste(sourceDir, "Base/make.class.Factors.R", sep=""))
  source(paste(sourceDir, "Base/make.pdf.R", sep=""))
  source(paste(sourceDir, "Base/make.VaR.tables.R", sep=""))
  source(paste(sourceDir, "Base/set.parms.run.R", sep=""))
  source(paste(sourceDir, "Base/utilities.VaR.R", sep=""))
  
  # Source the overnight files 
  source(paste(sourceDir,"Overnight/get.spot.fx.rates.R",sep=""))
  source(paste(sourceDir,"Overnight/get.curve.info.R",sep=""))
  source(paste(sourceDir,"Overnight/get.historical.prices.R",sep=""))
  source(paste(sourceDir,"Overnight/get.price.buddy.curve.R",sep=""))
  source(paste(sourceDir,"Overnight/get.vol.buddy.curve.R",sep=""))
  source(paste(sourceDir,"Overnight/fill.mkt.files.R",sep=""))
  source(paste(sourceDir,"Overnight/fill.one.ts.curve.R",sep=""))
  source(paste(sourceDir,"Overnight/get.corrected.marks.R",sep=""))
  source(paste(sourceDir,"Overnight/make.mkt.files.R",sep=""))
  source(paste(sourceDir,"Overnight/backupLoad.R",sep=""))

  source("H:/user/R/RMG/Utilities/Database/VCENTRAL/get.positions.from.vcv.vcentral.r")
  source("H:/user/R/RMG/Utilities/write.xls.R")
  source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
  source("H:/user/R/RMG/Utilities/easyOutliers.R")
  source("H:/user/R/RMG/Utilities/sendemail.R")
  source("H:/user/R/RMG/Utilities/dollar.R")
  source("H:/user/R/RMG/Utilities/mkdir.R")
  
  print("overnightUtils: Required Files Have Been Sourced.")

}

################################################################################
# Utility to benchmark one or more functions over a number of runs
#
benchmark <- function (..., reps = 1)
{
    args <- list(...)
    assert(is.positive.integer(reps), "Number of repetitions (reps) must be a positive integer")
    assert(length(list) > 0, "Supply functions to test in ...")
    assert(all(as.logical(lapply(args, is.function))), "Arguments to ... expected to be functions")
    assert(!is.null(names(args)), "Functions should be named")
    times <- list()
    for (i in 1:length(args)) {
        functionName <- names(args)[i]
        times[[functionName]] <- system.time(for (j in 1:reps) {
            args[[i]]()
        }, gcFirst = TRUE)[1:3]
    }
    print(times)
    times <- as.data.frame(times)
    attr(times, "reps") <- reps
    rownames(times) <- c("system", "user", "total")
    class(times) <- c("benchmark", class(times))
    times
}


################################################################################
# Let people know a report finished 
# 
overnightUtils.sendCompletionEmail <- function(jobName, reportPath=NA, asAttachment=FALSE)
{
  toList = c( "John.Scillieri", "adrian.dragulescu" )
  toList = paste(paste(toList, "@constellation.com", sep=""), collapse=", ")
  
  bodyText = c( paste("The", jobName, "Completed."),
                paste("It finished executing at:", Sys.time()),
                " ")
                
  if ( !is.na(reportPath) )
  {
    includeMessage = ifelse( asAttachment, 
      "Please see the included attachment.",
      "It can be found at the location provided below." )
    
    bodyText = append( bodyText, includeMessage ) 
  }
  
  linkPath = ifelse( asAttachment, NA, reportPath )
  attachmentPath = ifelse( asAttachment, reportPath, NA )
  
  sendEmail( to=toList,
             from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
             subject=paste("SUCCESS:", jobName),
             links=linkPath,
             attachments=attachmentPath,
             body=bodyText )
}
