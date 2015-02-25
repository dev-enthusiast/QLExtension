###############################################################################
# WriteExcelSummary.R
#
# Author: e14600
#
library(odfWeave)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Utilities/OOTools.R")


################################################################################
sendStatusEmail <- function( outputFile, asOfDate )
{
    valueLine = "The Collateral Delta-Gamma Report is attached below."
    
    subject = paste( "Collateral Delta-Gamma", format( asOfDate, "%d%b%y" ) )
    
    toList = getEmailDistro("CollateralStress")
    
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, valueLine, attachments=outputFile )
}


################################################################################
writeExcelSummary <- function()
{
    options( stringsAsFactors=FALSE )
    
    asOfDate = getAsOfDate()
    
    BASE_PATH = "S:/All/Risk/Collateral Scenarios"
    reportDir = file.path( BASE_PATH, "Reports", as.character( asOfDate ) )
    if( !file.exists( reportDir ) ) dir.create( reportDir, showWarnings=TRUE )
    setwd( reportDir )

    zipCmds = c( "S:/All/Risk/Software/zip.exe -r $$file$$ .",
                 "S:/All/Risk/Software/unzip -o $$file$$" )
    control = odfWeaveControl( zipCmd = zipCmds )
    
    TEMPLATE_FILE   = "H:/user/R/RMG/Projects/CollateralDeltaGamma/Collateral Delta Summary - Template.ods" 
    originalODSFile = "H:/user/R/RMG/Projects/CollateralDeltaGamma/originalODSFile.ods"
    odfWeave( TEMPLATE_FILE, originalODSFile, control=control )
    
    outputFileName = paste( reportDir, "/Collateral DGC - ", format( asOfDate, "%d%b%y" ), ".ods", sep="" )
    OOTools$fixODSFile( originalODSFile, outputFileName, control=control )
    
    sendStatusEmail( outputFileName, asOfDate )
    
    rLog("Done.")
}
