###############################################################################
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate. 
# If that fails, use yesterday.
#
main <- function()
{
    powerFactor = as.numeric( Sys.getenv("powerFactor") ) 
    gasFactor = as.numeric( Sys.getenv("gasFactor") )
    coalFactor = as.numeric( Sys.getenv("coalFactor") ) 
    filter = Sys.getenv("filter")
    outputFile = Sys.getenv("outputFile") 
    
    rLog("Running Case:")
    rLog("\tPower Factor:", powerFactor)
    rLog("\tGas Factor:", gasFactor)
    rLog("\tCoal Factor:", coalFactor)
    rLog("\tFilter:", filter)
    rLog("\tOutput File:", outputFile)
    
    if( file.exists( outputFile ) )
    {
        cat("File exists. Skipping...\n")
        return( NULL )
    }
    
    run( .getAsOfDate(), powerFactor, gasFactor, 
            coalFactor, filter, outputFile )
    
    cat("\nDone.\n")
}


################################################################################
main()

