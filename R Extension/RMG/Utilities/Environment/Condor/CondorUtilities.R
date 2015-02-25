################################################################################
# Utilities for operating with the Condor program through R
#
library(XML)

################################################################################
# File Namespace
#
CondorUtilities = new.env(hash=TRUE)


################################################################################
# Returns all windows computers currently operating in the Condor pool
#
CondorUtilities$getComputerNames <- function( availableOnly = FALSE)
{
    COMMAND = "condor_status"
    
    if( availableOnly )
    {
        COMMAND = paste( COMMAND, "-avail" )
    }
    
    res <- system(COMMAND, intern=TRUE, show.output.on.console=FALSE)
    ind <- grep(" WINNT51", res)  # OK until we go on UNIX
    res <- res[ind]
    
    computers <- sapply(strsplit(res, " "), function(x){x[1]})
    computers <- sapply(strsplit(computers, "\\.C"), function(x){x[1]})
    computers <- strsplit(computers, "[[:alnum:]]+@")
    computers <- sapply(computers, function(x){paste(x, sep="", collapse="")})
    
    computers <- sort(unique(computers))
    
    return(computers)
}


################################################################################
# This produces the actual output file needed to run the condor jobs
#
# Return - an array of strings to be written to the condor submit file
#
CondorUtilities$removeEmptyLogs <- function( logDirectory =
                "//ceg/cershare/All/Risk/Reports/VaR/prod/CondorNightlyLogs" )
{
    
    fileList = data.frame( fileName = list.files(logDirectory),
            isFolder=NA, size=NA)
    fileList$fileName = paste(logDirectory, "/", fileList$fileName, sep="")
    fileInfo = file.info(fileList$fileName)
    fileList$size = fileInfo$size
    fileList$isFolder = fileInfo$isdir
    
    indexesToRemove = which(fileList$size == 0 & fileList$isFolder==FALSE)
    
    deleteList = fileList$fileName[indexesToRemove]
    
    wereDeleted = file.remove(deleteList)
    
    return( length(which(wereDeleted)) )
}

################################################################################
#
CondorUtilities$submitDAGFile <- function( filePath )
{
    setwd( dirname(filePath) )
    
    # Generate the .condor.sub file
    cat("Submitting DAG File:", filePath, "\n" )
    command = paste( "C:/condor/bin/condor_submit_dag",
            "-dagman C:/condor/bin/condor_dagman.exe -f", filePath )
    cat("Running Command:", command, "\n" )
    system(command)
    
}


################################################################################
CondorUtilities$allQueueInfo <- function( global = FALSE)
{
    COMMAND = "condor_q -xml"
    
    if( global )
    {
        COMMAND = paste( COMMAND, "-global" )
    }
    
    text = system(COMMAND, intern=TRUE, 
            show.output.on.console=FALSE)[-(1:3)]
    doc = xmlRoot(xmlTreeParse(text))
    
    allJobs = NULL
    index = 1
    for( jobNode in xmlChildren(doc) )
    {
        jobList = list()
        for( descriptorNode in xmlChildren(jobNode) )
        {
            name = xmlAttrs(descriptorNode)
            value = xmlValue(descriptorNode)
            if( is.null(value) ) 
            {
                value = xmlAttrs(descriptorNode[[1]])
                if( !is.null(value) && value=="t" ) value = TRUE
                if( !is.null(value) && value=="f" ) value = FALSE
            }
            jobList[[name]] = value 
        }
        
        allJobs[[index]] = jobList
        names(allJobs)[index] = paste(jobList$ClusterId, ".", jobList$ProcId, sep="")
        index = index + 1
    }
    
    return(allJobs)
}

