################################################################################
# Generates the condor file necessary to split the pe simulation job.
# It generates condor files for each job as well as the appropriate .RData file
# that stores the curve list for that job to process.
#

################################################################################
# External Libraries
#
library(RODBC)
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/CurveClassifier.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Environment/Condor/GenerateCondorFile.R")


################################################################################
# File Namespace
#
PECondorUtils = new.env(hash=TRUE)


################################################################################
# Remove the old condor files
#
PECondorUtils$cleanCondorDirectory <- function()
{
  rLog("Cleaning", PE$CONDOR_DIR)
  fileList = list.files(PE$CONDOR_DIR, recursive=TRUE, full.names=TRUE)
  if( length(fileList) != 0 )
  {
    condorFiles = fileList[grep(".condor$", fileList)]
    rDataFiles = fileList[grep(".RData$", fileList)]
    dagFiles = fileList[grep(".DAG$", fileList)]
    subFiles = fileList[grep(".sub$", fileList)]
    logFiles = fileList[grep(".log$", fileList)]
    outFiles = fileList[grep(".out$", fileList)]
    
    file.remove(condorFiles)
    file.remove(rDataFiles)
    file.remove(dagFiles)
    file.remove(subFiles) 
    file.remove(logFiles)
    file.remove(outFiles)
    
    rLog("Files Removed.") 
  }
}


################################################################################
# The first step in the chain is to load all the curve data for use by the 
# simulation job
#
PECondorUtils$createDataLoadJobFile <- function()
{
  jobName = "LoadCurveData"

  condorOutputPath = paste(PE$CONDOR_DIR, jobName, ".condor", sep="")

  inputFilePath = paste( PE$FC_SRC_DIR, jobName, ".R", sep="")
  
  individualJobPortion = 'environment = "$(EnvSettings)"'
  individualJobPortion = append(individualJobPortion, 
    'Queue')
  
  GenerateCondorFile$writeCondorFile(jobName, condorOutputPath, inputFilePath,
    individualJobPortion, retryOnError=TRUE)
}


################################################################################
# The second step in the process is to create the simulation job files for each
# curve type.
#
PECondorUtils$createCurveJobFiles <- function()
{
  allCurvesList = CurveClassifier$loadCurveClassificationList()$curve.name
  allCurvesList = unique(na.omit(allCurvesList))
  
  # Determine the type of each curve
  allCurvesList = data.frame(curve.name=allCurvesList, type=NA)
  allCurvesList$type = lapply(allCurvesList$curve.name, 
    CurveClassifier$getCurveType)
  
  # loads 'allPositions'
  load(PE$COUNTERPARTY_POSITION_FILE)
  activeCurves = data.frame( curve.name = 
    as.character(unique(allPositions$curve.name)) )
  
  # before we do the merge, we need to pull out all the reference curves so 
  # they get simulated even if they have no active positions.
  referenceCurves = subset( allCurvesList, type!=CurveClassifier$CT_MARKET )
  
  # We need to merge the known curve list with the active positions list
  curveList = merge(activeCurves, allCurvesList)
  # and then add back in the reference curves
  curveList = unique(rbind(referenceCurves, curveList))
  
  # Do a set difference to warn about missing curves
  missingCurves = setdiff(as.character(activeCurves$curve.name), 
    as.character(curveList$curve.name))
  cat("The following curves have active positions but are missing from",
    "the simulation list:\n\t", paste(missingCurves, collapse="\n\t"),"\n")

  # output the job files for each type of curve
  types = as.character(unique(curveList$type))
  for( curveType in types )
  {
    dataSubset = subset(curveList, type == curveType)
    PECondorUtils$.writeJobFiles(dataSubset, curveType) 
  }
  
}


################################################################################
# Finally, create a condor job for each counterparty
# 
PECondorUtils$createCounterpartyJobFile <- function(counterpartyList = NULL)
{
  if( is.null(counterpartyList) )
  {
    #this imports a variable named allPositions
    load(PE$COUNTERPARTY_POSITION_FILE)
    
    counterpartyList = as.character(na.omit(unique(allPositions$cparty)))
    counterpartyList = toupper(sort(counterpartyList))
  }

  header = "# Begin individual job section"
  environmentLine = paste( 'environment = "',
    'runName=$(runName) $(EnvSettings)"' )
  header = append(header, environmentLine )
  header = append(header, " ")
  
  fileOutput = header  
  for( counterparty in counterpartyList )
  {
    counterpartyLine = paste("runName = '", counterparty, "'", sep="")
    fileOutput = append(fileOutput, counterpartyLine)

    fileOutput = append(fileOutput, "Queue")
  }
  
  jobName = paste("SimulateCounterparties", sep="")

  condorOutputPath = paste(PE$CONDOR_DIR, jobName, ".condor", sep="")

  inputFilePath = paste( PE$SRC_DIR, "Condor/PECalculatorCondor.R", sep="")
     
  rDebug("About to write file:", condorOutputPath)
  GenerateCondorFile$writeCondorFile(jobName, condorOutputPath, inputFilePath,
    individualJobPortion=fileOutput)

}


################################################################################
# The first step in the chain is to load all the curve data for use by the 
# simulation job
#
PECondorUtils$createCounterpartyStatusJob <- function()
{
  jobName = "CounterpartyStatusCheck"

  condorOutputPath = paste(PE$CONDOR_DIR, jobName, ".condor", sep="")

  inputFilePath = paste(PE$SRC_DIR, "Condor/UpdateCPJob.R", sep="")
  
  individualJobPortion = 'environment = "$(EnvSettings)"'
  individualJobPortion = append(individualJobPortion, 
    'Queue')
  
  GenerateCondorFile$writeCondorFile(jobName, condorOutputPath, inputFilePath,
    individualJobPortion)
}


############################### INTERNAL FUNCTIONS #############################

################################################################################
# This function write the condor and RData files used by the PE Curve simulation
# code. 
#
PECondorUtils$.writeJobFiles <- function( data, type )
{
  name = CurveClassifier$getNamePrefix( type )

  # If we're not a market curve, go ahead and write our information out, there's
  # no preprocessing that needs to be done
  if( type != CurveClassifier$CT_MARKET )
  {
  
    filePath = PECondorUtils$.saveCurveListDataSet( namePrefix = name, 
      data = data )
    PECondorUtils$.createCondorFile( namePrefix = name, 
      dataPath = filePath ) 
  
  } else
  {
    # if it is the market curve data we need to iterate through all the markets
    # and create separate condor jobs for each one.
    
    curveNames = as.character(na.omit(data$curve.name))
    curveNames = strsplit(curveNames, " ")
    for( x in 1:length(curveNames) )
    {
      curveNames[[x]] = curveNames[[x]][2]
    }
    markets = data.frame(matrix(unique(curveNames)))
    names(markets) = "market"

    for( market in markets$market )
    {
      rDebug("Creating market file for market:", market)
      marketName = sub("%MKT%", market, name)
      curvesToMatch = paste("COMMOD", sub("Curves", "", marketName), "")
      rDebug("Matching curves of pattern: ", curvesToMatch, ".", sep="")
      dataSubset = subset( data, regexpr( curvesToMatch, curve.name ) != -1 )
      
      filePath = PECondorUtils$.saveCurveListDataSet( namePrefix = marketName, 
        data = dataSubset )

      # Determine how many packets you should have ... 
      ref.table <- data.frame(no.packets = c( 20,  10,   5,  2,  1),
                              no.curves  = c(600, 300, 100, 50, 25))
      FP_NO_PACKETS <- ref.table$no.packets[
          max(which(ref.table$no.curves >= nrow(dataSubset)))]
      
      PECondorUtils$.createCondorFile( namePrefix = marketName,
          dataPath = filePath, FP_NO_PACKETS)
      
      PECondorUtils$.createCondorFile.agg( namePrefix = marketName,
          dataPath = filePath)   # aggregate them too
    }
  }
}


################################################################################
# If necessary, write out the RData information for that curve type
# 
PECondorUtils$.saveCurveListDataSet <- function( namePrefix, data )
{
  outputDir = paste(PE$CONDOR_DIR, "RData/", sep="")
  if( file.access(outputDir, mode=0) != 0 )
  { 
    mkdir( outputDir )
  }

  fileName = paste(outputDir, namePrefix, ".RData", sep="")
  if( !file.exists(fileName) )
  {
    save(data, file=fileName)
  }else
  {
    rError("File Data set exits, not overwriting:", fileName)
  }
  return(fileName)
}


################################################################################
# Write the condor file for the curve simulation jobs
#
PECondorUtils$.createCondorFile <- function( namePrefix, dataPath,
  FP_NO_PACKETS = 1)
{
  condorOutputPath = paste(PE$CONDOR_DIR, namePrefix, ".condor", sep="")
  jobName = paste("Simulate", namePrefix, sep="")
  inputFilePath = paste( PE$FC_SRC_DIR, "SimulateCurves.R", sep="")
  
  individualJobPortion = ""
  individualJobPortion = append(individualJobPortion, 
    "# Begin individual job section")
  individualJobPortion = append(individualJobPortion,
    paste('environment = "FP_NO_PACKETS=', FP_NO_PACKETS,
    ' FP_PACKET_NO=$(packet) curveListPath=$(curveListPath) $(EnvSettings)"',
    sep=""))
  individualJobPortion = append(individualJobPortion, " ")
  inputLine = paste("curveListPath =", dataPath)
  individualJobPortion = append(individualJobPortion, inputLine)

  for (p in 1:FP_NO_PACKETS){
    individualJobPortion = append(individualJobPortion,
      paste("packet = '", p, "'", sep=""))
    individualJobPortion = append(individualJobPortion, "Queue")
    individualJobPortion = append(individualJobPortion, " ")
  }
  GenerateCondorFile$writeCondorFile(jobName, condorOutputPath, 
    inputFilePath, individualJobPortion, retryOnError=TRUE)
}

################################################################################
# Write the condor file for the price aggregation jobs
#
PECondorUtils$.createCondorFile.agg <- function( namePrefix, dataPath)
{
  condorOutputPath = paste(PE$CONDOR_DIR, "agg_", namePrefix, ".condor", sep="")
  jobName = paste("Aggregate", namePrefix, sep="")
  inputFilePath = paste( PE$FC_SRC_DIR, "AggregateCurves.R", sep="")
  
  individualJobPortion = ""
  individualJobPortion = append(individualJobPortion, 
    "# Begin individual job section")
  individualJobPortion = append(individualJobPortion,
    'environment = "curveListPath=$(curveListPath) $(EnvSettings)"')
  individualJobPortion = append(individualJobPortion, " ")
  inputLine = paste("curveListPath =", dataPath)
  individualJobPortion = append(individualJobPortion, inputLine)

  individualJobPortion = append(individualJobPortion, "Queue")
  individualJobPortion = append(individualJobPortion, " ")

  GenerateCondorFile$writeCondorFile(jobName, condorOutputPath, 
    inputFilePath, individualJobPortion, retryOnError=TRUE)
}
