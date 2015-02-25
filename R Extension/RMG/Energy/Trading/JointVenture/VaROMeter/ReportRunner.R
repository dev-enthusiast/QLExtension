###############################################################################
# ReportRunner.R
# 
#
# Author: e14600
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Energy/Trading/JointVenture/VaROMeter/VaROMeter.R")


################################################################################
# File Namespace
#
ReportRunner = new.env(hash=TRUE)


################################################################################
ReportRunner$BASE_DIR = "S:/All/Risk/Reports/VaROMeter"
ReportRunner$CONFIG_FILE = file.path( ReportRunner$BASE_DIR, "portfolio_config.txt" )


################################################################################
ReportRunner$main <- function()
{
    # Initialize SecDb for use on Procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv( SECDB_SOURCE_DATABASE = DB )
    Sys.setenv( SECDB_DATABASE = "Prod" )
    
    options( stringsAsFactors = FALSE )
    
    # Read a portfolio config file
    portfolios = toupper( readLines( ReportRunner$CONFIG_FILE, warn = FALSE ) )
    
    for( portfolio in portfolios )
    {
        rLog( "Running for:", portfolio )
        try( ReportRunner$.createPortfolioOutput( portfolio ) )
    }
    
    rLog( "Done." )
    invisible( NULL )
}


################################################################################
ReportRunner$.createPortfolioOutput <- function( portfolio )
{
    outputDir = file.path( ReportRunner$BASE_DIR, portfolio )
    
    dir.create( outputDir, FALSE )
    
    VaROMeter$main( portfolio, outputDir )
    
    invisible( NULL )
}


################################################################################
start = Sys.time()
ReportRunner$main()
print( Sys.time() - start )

