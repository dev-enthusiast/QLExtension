################################################################################
# 
#

################################################################################
# External Libraries
#
library(RODBC)
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/CurveClassifier.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
PriceSimulationDAG = new.env(hash=TRUE)


################################################################################
# finish dag buffer output and write file
#
PriceSimulationDAG$create <- function()
{
  dagBuffer = PriceSimulationDAG$.createJobSection();
  dagBuffer = append(dagBuffer,"")
  
  dagBuffer = append( dagBuffer, 
    PriceSimulationDAG$.createCurveSimulationSection() )
  dagBuffer = append(dagBuffer,"")
  
  dagBuffer = append(dagBuffer,"DOT PE_DAG.dot UPDATE")
  dagBuffer = append(dagBuffer,"")
  
  DAG_FILE_PATH = paste(PE$CONDOR_DIR, "PriceSimulation.DAG", sep="")
  writeLines(as.character(dagBuffer), con = DAG_FILE_PATH)
  
  rLog("Wrote File:", DAG_FILE_PATH)
  
  return(DAG_FILE_PATH)
}


################################################################################
# Add job section of all known condor files
#
PriceSimulationDAG$.createJobSection <- function()
{
  jobSection = list()

  fileList = list.files(PE$CONDOR_DIR)
  condorFiles = fileList[grep("Curves.condor$", fileList)]
  if( length(condorFiles) == 0 )
  {
    rError("No Condor Files Found In", PE$CONDOR_DIR)
  }else
  {
    jobNames = gsub(".condor", "", condorFiles)
    
    for( jobIndex in 1:length(jobNames) )
    {
    
      filePath = gsub( "/", "\\\\", paste(PE$CONDOR_DIR,condorFiles[jobIndex], 
        sep=""))
      jobSection = append(jobSection, paste("JOB", jobNames[jobIndex],
        filePath) )
    }
  }
  
  return(jobSection)
}


################################################################################
# add curve simulation heirarchy
#
PriceSimulationDAG$.createCurveSimulationSection <- function()
{
  outputBuffer = list()

  dependencyConstructionRuleList = readLines(
    "H:/user/R/RMG/Energy/VaR/PE/Condor/dependencyConstructionRules.txt" )
  
  # this will store a reduced list of job names to show which condor jobs have
  # no dependencies.
  fileList = list.files(PE$CONDOR_DIR)
  condorFiles = fileList[grep(".condor", fileList)]
  jobNames = gsub(".condor", "", condorFiles)
  namesToFilter = jobNames

  for( line in dependencyConstructionRuleList )
  {
    parentRule = strsplit(line, ";")[[1]][1]
    childrenRule = strsplit(line, ";")[[1]][2]
  
    parentSubset = eval(parse(text=parentRule))
    childrenSubset = eval(parse(text=childrenRule))
    outputBuffer = append( outputBuffer, paste( "PARENT",
      paste(parentSubset, collapse=" "), "CHILD",
      paste(childrenSubset, collapse=" ") ) )
  
    namesToFilter = setdiff(namesToFilter, parentSubset)
    namesToFilter = setdiff(namesToFilter, childrenSubset)
  }
  # add the aggregation for the individual MKT jobs
  agg.jobs <- jobNames[grep("^agg_", jobNames)]
  for (j in 1:length(agg.jobs)){
    parent <- gsub("agg_", "", agg.jobs[j])
    outputBuffer <- append(outputBuffer, paste( "PARENT", parent, "CHILD",
                                               agg.jobs[j]))
  }
  
  return(outputBuffer)
}



