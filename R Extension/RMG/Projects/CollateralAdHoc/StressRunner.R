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
    options(stringsAsFactors=FALSE)
    
    BASE_PATH = "S:/All/Risk/Collateral Scenarios"
    
    asOfDate = .getAsOfDate()
    
    reportDir = file.path( BASE_PATH, "Reports", as.character( asOfDate ) )
    dir.create( reportDir, show = FALSE )
    
    scenarios = read.csv( file.path( BASE_PATH, "ScenarioList.csv") )
    
    group = Sys.getenv("COLLATERAL_RUN_GROUP")
    scenarios = subset( scenarios, Group == group )
    
    for( rowIndex in seq_len( nrow( scenarios ) ) )
    {
        scenario = scenarios[ rowIndex, ]
        if( scenario$Case == "") next
        
        outputFile = file.path( reportDir, paste( scenario$Case, ".csv", sep="") )
        
        if( file.exists( outputFile ) )
        {
            rLog("File", outputFile, "exists. Skipping...")
            next
        }

        run( asOfDate, 
                powerFactor = scenario$Power, 
                gasFactor = scenario$Gas, 
                coalFactor = scenario$Coal, 
                filter = scenario$Filter,
                outputFile = outputFile )
        
    }
    
    cat("\nDone.\n")
}


################################################################################
main()

