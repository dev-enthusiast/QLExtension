################################################################################
# This will programmatically let you create Condor DAG files and write them
# out to the disk.
#


################################################################################
# File Namespace
#
DAGCreator = new.env(hash=TRUE)


################################################################################
# Write the DAG as three sections, the job descriptions, the dependency section,
# and the DOT update section.
#
DAGCreator$writeDAG <- function( outputFile )
{
  buffer = character()
  
  # Write out the job section
  jobList = ls(DAGCreator$.jobList)
  for( job in jobList )
  {
    buffer = append( buffer, paste("JOB", job, get(job, DAGCreator$.jobList)) )
  }
  buffer = append( buffer, " " )
  
  # Write the dependency section
  parentList = ls(DAGCreator$.dependencyList)
  for( parent in parentList )
  {
    buffer = append( buffer, paste("PARENT", parent, "CHILD",
      paste( get(parent, DAGCreator$.dependencyList), collapse=" " ) ) )
  }
  buffer = append( buffer, " " )
  
  # Write the DOT update section
  buffer = append( buffer, paste("DOT ", outputFile, ".dot UPDATE", sep="") )
  buffer = append( buffer, " " )
    
  # Save it to the specified file path
  buffer = as.character(buffer)
  buffer = gsub("/", "\\\\", buffer)
  
  writeLines(buffer, con = outputFile)
  rLog("Wrote File:", outputFile)
  return(outputFile)
}


################################################################################
# Add a condor job located at the given file path
#
DAGCreator$addJob <- function( jobName, filePath )
{
  assign(jobName, filePath, DAGCreator$.jobList)
}


################################################################################
# Parses out the parent and child dependencies and reduces them down to a
# unique list of parents.
#
DAGCreator$addDependency <- function( parentList, childList )
{
  parentList = as.character(parentList)
  childList = as.character(childList)
  
  for( parent in parentList )
  {

    if( exists(parent, DAGCreator$.dependencyList) )
    {
      oldValue = get(parent, DAGCreator$.dependencyList)
      assign( parent, append(oldValue, childList),
        DAGCreator$.dependencyList )

    }else
    {
      assign( parent, childList, DAGCreator$.dependencyList )
    }
    
  }
  
}


################################################################################
# Internal variables
DAGCreator$.jobList = new.env(hash=TRUE)
DAGCreator$.dependencyList = new.env(hash=TRUE)

#DAGCreator$addJob("p1", "file1")
#DAGCreator$addJob("p2", "c:/file2")
#DAGCreator$addDependency("p1", "c1")
#DAGCreator$addDependency("p1", "c2")
#DAGCreator$addDependency("p2", "c1")
#DAGCreator$addDependency("p2", c("c3", "c4"))
#DAGCreator$addDependency(c("p1", "p2"), "c5")
#DAGCreator$writeDAG("R:/PotentialExposure/Condor/testDAG.dag")

