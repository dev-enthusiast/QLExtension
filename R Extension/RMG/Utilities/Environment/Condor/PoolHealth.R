###############################################################################
# PoolHealth.R
# 
# Author: e14600
#
source("H:/user/R/RMG/Utilities/Environment/Condor/CondorUtilities.R")
source("H:/user/R/RMG/Utilities/Environment/Condor/GenerateCondorFile.R")

INPUT_FILE_PATH = "//ceg/cershare/All/Risk/Software/R/prod/Utilities/Environment/Condor/R26Test.R"
CONDOR_FILE_DIR = "//ceg/cershare/All/Risk/Reports/VaR/test/PoolHealthJobs/" 
CONDOR_RESULTS_DIR = "//ceg/cershare/All/Risk/Reports/VaR/test/PoolHealthResults/" 
JOB_NAME = "PoolTest"
REMOVE_AFTER_5MIN = "periodic_remove = ( (CurrentTime - QDate) > 300 )"

# Remove the old results
file.remove( list.files(CONDOR_RESULTS_DIR, full.names=TRUE) )
file.remove( list.files(CONDOR_FILE_DIR, full.names=TRUE) )

setwd( CONDOR_FILE_DIR )

allMachines = CondorUtilities$getComputerNames( availableOnly=FALSE )
machineList = CondorUtilities$getComputerNames( availableOnly=TRUE )

cat("\nThere are currently", length(allMachines), "machines in the pool.\n\n")

claimedMachines = setdiff( allMachines, machineList )
cat("The following", length(claimedMachines), "machines were not available at", 
        as.character(Sys.time()), ":\n")
print( data.frame( Name=claimedMachines ) )
cat("\n\n")

# Submit a condor job for each available machine
for( machine in machineList )
{
    machine = paste( machine, ".Ceg.Corp.Net", sep="")
    
    condorOutputPath = paste( CONDOR_FILE_DIR, machine, ".condor", sep="" )
     
    GenerateCondorFile$writeCondorFile( JOB_NAME, condorOutputPath, 
            INPUT_FILE_PATH, machine=machine, linesToAppend=REMOVE_AFTER_5MIN )

    # Generate the .condor file
    cat("Submitting Condor File:", condorOutputPath, "\n" )
    command = paste( "C:/condor/bin/condor_submit", condorOutputPath )
    system(command)    
}

cat("\n\n")
SLEEP_DURATION = 10
for( x in seq_len( SLEEP_DURATION ) )
{
    # do a check, if there are none of our jobs in the queue, finish early
    queueInfo = CondorUtilities$allQueueInfo()
    allJobNames = sapply( queueInfo, function(job){ gsub("\\\\", "/", job$In) } )
    ourJobs = which(allJobNames==INPUT_FILE_PATH)
    if( length(ourJobs) == 0 )
    {
        break
    }
    
    system("C:/condor/bin/condor_reschedule reschedule PCG6548")    
    
    cat("Sleeping for", SLEEP_DURATION-x+1, "more minutes (or until our jobs are done)...\n")
    Sys.sleep( 60 )
}
    
allResults = list.files(CONDOR_RESULTS_DIR)
finishedMachines = gsub(".txt", "", allResults)

faultyMachines = setdiff( toupper(machineList), toupper(finishedMachines) )
cat("The following machines may not be working or did not accept jobs:\n")
print( data.frame( Name=faultyMachines ) )

#%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;V:\Oracle\ora90\bin;c:\Progra~1\MATLAB704\bin\win32



    
    