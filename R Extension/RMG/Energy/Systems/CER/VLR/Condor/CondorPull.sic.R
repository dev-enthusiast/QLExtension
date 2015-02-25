###############################################################################
# CondorPull.sic.R
# 
# This runs as an individual condor job for a given:
#	business unit=7 (ERCOT), sic.code, and weather.zone 
#
# Author: John Scillieri / AAD
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
CondorPull$WEATHER_ZN_ENV = "WEATHER_ZN"
CondorPull$SIC_CODE_ENV = "SIC_CODE"


################################################################################
CondorPull$main <- function()
{
    buID = toupper( Sys.getenv( CondorPull$BU_ID_ENV ) )
    weather = toupper( Sys.getenv( CondorPull$WEATHER_ZN_ENV ) )
    sic = toupper( Sys.getenv( CondorPull$SIC_CODE_ENV ) )
    
    if( buID == "" || weather == "" || sic == "" )
    {
        rError("One of the following environment variables is unspecified:")
        rError(CondorPull$BU_ID_ENV, "=", buID)
        rError(CondorPull$WEATHER_ZN_ENV, "=", weather)
        rError(CondorPull$SIC_CODE_ENV, "=", sic)
        rLog("Exiting.")
        return(NULL)
    }
    
    outputFile = paste( CondorPull$OUTPUT_DIR, "load.", buID, ".", weather, ".",
            sic, ".RData", sep="" )
    if( file.exists(outputFile) )
    {
        rLog("Output file:", outputFile, "exists. Exiting.")
        return(NULL)
    }
    
    rLog("Pulling Load Data For BU ID:", buID, "WEATHER:", weather, 
            "SIC:", sic, "...")
    loadData = .pullHHData( buID, weather, sic )
    rLog("Done Pulling Data, Read", nrow(loadData), "rows.")
    
    
    rLog("Saving to file:", outputFile)
    save(loadData, file=outputFile)
    
    rLog("Done.")
}


################################################################################
# pull the load data and clean it up a bit
.pullHHData <- function( businessUnitID, weather, sic )
{
    options <- NULL
    dir <- "//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/"
    
    # The V drive may be required to dynamically load some DLLs
##     command = "net use V: \\\\nas-omf-01\\EntApps /PERSISTENT:NO"
##     system(command, show.output.on.console=FALSE)
##     print( file.copy( "V:/Oracle/ora90/network/admin/tnsnames.ora", 
##                       "C:/Oracle/ora9i/network/admin/tnsnames.ora", 
##                       overwrite=TRUE) )
    
##     con.str=paste("Driver={Oracle in Local9i};", 
##             "CONNECTSTRING=(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL = TCP)",
##             "(HOST=oradev-msw-07.ceg.corp.net)(PORT = 1524)))",
##             "(CONNECT_DATA =(SID = CERINT)(GLOBAL_NAME = CERINT.CEG.CORP.NET)))",
##             ";DBQ=CERINT;UID=CER_PMO_RO;PWD=cerpmoro;", sep="")
    
    con.str <- paste("FileDSN=",dir,"CERINT.dsn",
      ";UID=cer_pmo_ro;PWD=cerpmoro;", sep="")
    options$con <- odbcDriverConnect(con.str)  
    
    
    HH <- get.all.load.weather.sic(businessUnitID, weather, sic, options)
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

#Sys.setenv(BU_ID="7")
#Sys.setenv(WEATHER_ZN="COAST")
#Sys.setenv(SIC_CODE="02")

CondorPull$main()

Sys.time() - .start


