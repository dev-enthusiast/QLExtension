###############################################################################
# OvernightVPortPull.R
# 
# Save the overnight vport hierarchy for use in R.
#
# Author: e14600
#


################################################################################
# External Libraries
#
library(RODBC)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# CONSTANTS
#
DATA_DIR = "//ceg/cershare/All/Risk/data/VaR/prod/Portfolios/"


################################################################################
# File Namespace
#
OvernightVPortPull = new.env(hash=TRUE)


################################################################################
OvernightVPortPull$saveHierarchy <- function()
{
    asOfDate = as.Date( secdb.dateRuleApply(Sys.Date(), "-1b") )
    
    connection = odbcConnect("SASPRD", believeNRows=FALSE)
    
    #/SAS_JADE/CURRENT/PROD/PRODCNTL    
    query = paste( "SELECT * FROM PRODCNTL.VPORT" )
    
    hierarchy = sqlQuery(connection, query)
    
    odbcClose(connection)
    
    OUTPUT_FILE = paste( DATA_DIR, "vports.", as.character( asOfDate ), 
            ".RData", sep = "" )
    save( hierarchy, file = OUTPUT_FILE )
    
    rLog( "Saved File:", OUTPUT_FILE )
    
    invisible( NULL )
}


################################################################################
start = Sys.time()

OvernightVPortPull$saveHierarchy()

print( Sys.time() - start )






