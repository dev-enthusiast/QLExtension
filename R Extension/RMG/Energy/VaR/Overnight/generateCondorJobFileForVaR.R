################################################################################
# Generate And Run The Job Submission File To Pass To 
# Condor For The Overnight Job.
#   It will run a Condor job for each portfolio found in the overnight
#   config file.
#
# Run As: R --vanilla < generateCondorJobFileForVaR.R >output.txt --args DATE
#   where date is of the format YYYY-MM-DD
#
# Written By: John Scillieri
# Last Updated By: John Scillieri - 01/11/08
#
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")  
source("H:/user/R/RMG/Utilities/Environment/Condor/GenerateCondorFile.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
# Reads the arguments and runs the program
#
#
generateCondorJobFileForVaR.main <- function()
{
    
    asOfDate   <- overnightUtils.getAsOfDate()
     
    PROCMON_DEPENDENCY1 = "RMG/R/Data/Energy/VaR/Overnight/OvernightPriceFillJob" 
    PROCMON_DEPENDENCY2 = "RMG/R/Data/Energy/VaR/Overnight/OvernightVegaJob" 
    PROCMON_DEPENDENCY3 = "RMG/R/Data/Energy/VaR/Overnight/OvernightPositionJob" 
    PROCMON_DEPENDENCY4 = "RMG/R/Data/Energy/VaR/Overnight/OvernightVolJob"
    
    while( ( (Procmon$getStatus(PROCMON_DEPENDENCY1, asOfDate, exact=TRUE) == "Succeeded") &&
             (Procmon$getStatus(PROCMON_DEPENDENCY2, asOfDate, exact=TRUE) == "Succeeded") &&
             (Procmon$getStatus(PROCMON_DEPENDENCY3, asOfDate, exact=TRUE) == "Succeeded") &&
             (Procmon$getStatus(PROCMON_DEPENDENCY4, asOfDate, exact=TRUE) == "Succeeded") ) == FALSE )
    {
        rLog(PROCMON_DEPENDENCY1, "Status:", Procmon$getStatus(PROCMON_DEPENDENCY1, asOfDate, exact=TRUE))
        rLog(PROCMON_DEPENDENCY2, "Status:", Procmon$getStatus(PROCMON_DEPENDENCY2, asOfDate, exact=TRUE))
        rLog(PROCMON_DEPENDENCY3, "Status:", Procmon$getStatus(PROCMON_DEPENDENCY3, asOfDate, exact=TRUE))
        rLog(PROCMON_DEPENDENCY4, "Status:", Procmon$getStatus(PROCMON_DEPENDENCY4, asOfDate, exact=TRUE))
        rLog("Sleeping for 15 mins...")
        Sys.sleep(900)   
    }

    overnightScheduleFile = "H:/user/R/RMG/Energy/VaR/Base/schedule.overnight.xls"
    
    #Read the portfolios from the scheduling Excel file
    portfolioList = readPortfolioList(overnightScheduleFile)
    
    # If we have no portfolios, add one just to make sure the jobs continue.
    # It won't recalculate anyway so it's no big deal
    if( length(portfolioList) == 0 ) portfolioList="NYTRADE"
    
    #and then generate the appropriate job orders for each portfolio
    jobSection = generateJobSection(asOfDate, portfolioList)
    
    jobName = "VAR_PORTFOLIOS"
    inputFilePath = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/run.one.VaR.R"
    condorOutputPath = "//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Overnight/Condor/VAR_PORTFOLIOS.condor"
    
    fileName = GenerateCondorFile$writeCondorFile( jobName, condorOutputPath, 
            inputFilePath, individualJobPortion=jobSection )
    
}



################################################################################
# This reads the overnight config file and extracts the portfolio names
#
# Return - an array of strings of portfolio names
# 
readPortfolioList <- function(fileName)
{
    asOfDate <- as.Date(overnightUtils.getAsOfDate())
    options  <- overnightUtils.loadOvernightParameters(asOfDate)
    
    run.all <- get.portfolio.book(options)
    uRuns   <- sort(unique(run.all$RUN_NAME))
    
    status <- data.frame( 
            name = uRuns,
            run.name = gsub(" ", ".", tolower(uRuns)), 
            have.pdf=FALSE )
    filename <- paste(options$save$overnight.dir, "/", 
            status$run.name, "/VaR.report.pdf", sep="")
    status$have.pdf<- file.exists(filename)
    
    missingPortfolios = status$name[which(status$have.pdf==FALSE)]
}


################################################################################
# This produces the individual argument lists for each Condor Job and their
# appropriate queue commands.
#
# it should look like this:
# 
# #Beginning of individual job section
# asofdate = 2007-04-12
# environment = "asofdate=$(asofdate) portfolio=$(portfolio) $(EnvSettings)"
# #
# portfolio = 'portfolio1'
# Queue
# portfolio = 'portfolio2'
# Queue
# ...
#
# Return - an array of strings to be written to the condor submit file
# 
generateJobSection <- function(asOfDate, portfolioList)
{
    fileOutput = "# Begin individual job section"
    
    # If the job runs for more than X seconds, remove it
    fileOutput = append(fileOutput, 'log = C:\\CondorNightlyLogs\\VAR_PORTFOLIOS.log')
    fileOutput = append(fileOutput, 'maxRunTime = 900')
    fileOutput = append(fileOutput, 'periodic_remove = JobStatus == 2 && ( (RemoteWallClockTime - CumulativeSuspensionTime) > $(maxRunTime) )')
    
    fileOutput = append(fileOutput, 'environment = "portfolio=$(portfolio) $(EnvSettings)"')
    fileOutput = append(fileOutput, " ")
    for (portfolio in portfolioList)
    {
        
        portfolioLine = paste("portfolio = '", portfolio, "'", sep="")
        fileOutput = append(fileOutput, portfolioLine)
        
        fileOutput = append(fileOutput, "Queue")
    }
    
    #Returns
    fileOutput
}


################################################################################
# Tests the above function
# If this fails, find out why!
#
testGenerateJobSection <- function()
{
    # The test data to use
    testArray = c("portfolio1", "portfolio2", "portfolio3")
    testDate = "2007-04-01"
    
    # The output it SHOULD generate
    finalResult = c("# Beginning individual job section",
            'environment = "portfolio=$(portfolio) $(EnvSettings)"',
            "# ", 
            "portfolio = 'portfolio1'",
            "Queue",
            "portfolio = 'portfolio2'",
            "Queue",
            "portfolio = 'portfolio3'",
            "Queue")
    
    # run the function to generate the expected output
    outputArray = generateJobSection(testDate, testArray)
    
    # Test that the two string arrays are equal.  There's probably a better
    # way to do this, but I'm an R noob.
    comparison = (outputArray == finalResult)
    comparison = comparison[comparison==FALSE]
    if (length(comparison)==0)
    {
        print("Test Passed!")
    }
    else
    {
        print("Test Failed!!")
        print(outputArray)
        print("")
        print(finalResult)
    }
}


################################################################################
#Start the program 
# 
res = try(generateCondorJobFileForVaR.main())
if (class(res)=="try-error"){
   cat("Failed Overnight VaR Job File Generation.\n")
}
