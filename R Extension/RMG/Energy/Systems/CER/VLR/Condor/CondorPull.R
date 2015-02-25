###############################################################################
# CondorPull.R
# 
# This runs as an individual condor job for a given:
#	business unit, edcName, and rateClass
#
# Author: John Scillieri
# 
library(RODBC)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Energy/Systems/CER/VLR/utils.R")


################################################################################
# File Namespace
#
CondorPull = new.env(hash=TRUE)


################################################################################
# Config variables
#
memory.limit(3*1024)
CondorPull$OUTPUT_DIR = "//nas-msw-07/rmgapp/VLR Data/"
CondorPull$BU_ID_ENV = "BU_ID"
CondorPull$EDC_NAME_ENV = "EDC_NAME"
CondorPull$RATE_CLS_ENV = "RATE_CLASS"


################################################################################
CondorPull$main <- function()
{
    buID = toupper( Sys.getenv( CondorPull$BU_ID_ENV ) )
    edcName = toupper( Sys.getenv( CondorPull$EDC_NAME_ENV ) )
    rateClass = toupper( Sys.getenv( CondorPull$RATE_CLS_ENV ) )
    
    if( buID == "" || edcName == "" || rateClass == "" )
    {
        rError("One of the following environment variables is unspecified:")
        rError(CondorPull$BU_ID_ENV, "=", buID)
        rError(CondorPull$EDC_NAME_ENV, "=", edcName)
        rError(CondorPull$RATE_CLS_ENV, "=", rateClass)
        rLog("Exiting.")
        return(NULL)
    }
    
    outputFile = paste( CondorPull$OUTPUT_DIR, "load.", buID, ".", edcName, ".",
            rateClass, ".RData", sep="" )
    if( file.exists(outputFile) )
    {
        rLog("Output file:", outputFile, "exists. Exiting.")
        return(NULL)
    }
    
    rLog("Pulling Load Data For BU ID:", buID, "EDC Name:", edcName, 
            "Rate Class:", rateClass, "...")
    loadData = .pullHHData( buID, edcName, rateClass )
    rLog("Done Pulling Data, Read", nrow(loadData), "rows.")
    
    
    rLog("Saving to file:", outputFile)
    save(loadData, file=outputFile)
    
    rLog("Done.")
}


################################################################################
# pull the load data and clean it up a bit
.pullHHData <- function( businessUnitID, edcName, rateClass )
{
    options <- NULL
    dir <- "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"
    
    # The V drive may be required to dynamically load some DLLs
    command = "net use V: \\\\ceg\\EntApps /PERSISTENT:NO"
    system(command, show.output.on.console=FALSE)
    print( file.copy( "V:/Oracle/ora90/network/admin/tnsnames.ora", 
                      "C:/Oracle/ora9i/network/admin/tnsnames.ora", 
                      overwrite=TRUE) )
    
    con.str=paste("Driver={Oracle in Local9i};", 
            "CONNECTSTRING=(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL = TCP)",
            "(HOST=oradev-msw-07.ceg.corp.net)(PORT = 1524)))",
            "(CONNECT_DATA =(SID = CERINT)(GLOBAL_NAME = CERINT.CEG.CORP.NET)))",
            ";DBQ=CERINT;UID=CER_PMO_RO;PWD=cerpmoro;", sep="")
    
    options$con <- odbcDriverConnect(con.str)  
    
    
    HH <- get.all.load(businessUnitID, edcName, rateClass, options)
    odbcClose(options$con)
    
    if( nrow(HH) == 0 )
    {
        return(NULL)
    }
    
    HH$hour[which(HH$hour==24)] = 0
    HH$time = as.POSIXct(paste(HH$read.dt, " ", HH$hour, ":00", sep=""))
    HH$read.dt = HH$hour = NULL
    
    HH = unique(HH)
    HH = HH[order(HH$ne.acctno, HH$data.source, HH$time),]
    HH = HH[, c("ne.acctno", "data.source", "time", "kw")]
    
    return(HH)
}


################################ MAIN EXECUTION ################################
.start = Sys.time()

#Sys.setenv(BU_ID="6")
#Sys.setenv(EDC_NAME="WMECO")
#Sys.setenv(RATE_CLASS="G0")

CondorPull$main()

Sys.time() - .start


