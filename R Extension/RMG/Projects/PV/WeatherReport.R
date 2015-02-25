###############################################################################
# WeatherReport.R
# Author: John Scillieri
# 
# Run this on the "WeatherReport" test day. 
# It will generate the entire report necessary for the PV process. 
# 
library(odfWeave)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Projects/PV/Weather.R")
source("H:/user/R/RMG/Projects/PV/Term.R")


################################################################################
# File Namespace
WeatherReport = new.env(hash=TRUE)


################################################################################
# CONFIGURATION PARAMETERS

# Constants
PV_DIRECTORY = "S:/Risk/Reports/Vol Test/Mar 2009"
TEMPLATE_FILE = "H:/user/R/RMG/Projects/PV/WeatherReportTemplate.odt"
REPORT_PREFIX = "WeatherPVReport_"
SOFFICE_PATH = "C:/Program Files/OpenOffice.org 3/program"
JOD_CONVERTER_PATH = "H:/user/R/RMG/Projects/PV/jodconverter-2.2.1"

# FOR TODAY
AS_OF_DATE = Sys.Date()
POSITION_DATA = Weather$readPositionsFromSecDb( "WTHR Portfolio", Sys.Date() )
#POSITION_DATA = Weather$readPositionFile( file.path(PV_DIRECTORY, "Position_Weather Trading Portfolio_29May08.csv" ) )
#POSITION_DATA = POSITION_DATA[-which(POSITION_DATA$START_DT==as.Date("2008-01-01") & POSITION_DATA$END_DT==as.Date("2008-01-31")),]
QUOTE_DATA = Weather$quoteData( AS_OF_DATE )


## FOR HISTORICAL
#AS_OF_DATE = as.Date("2007-12-19")
#POSITION_DATA = Weather$readPositionFile( file.path(PV_DIRECTORY, "Position_Weather Trading Portfolio_19Dec07.csv") )
#QUOTE_DATA = Weather$quoteData( rDataFile = file.path(PV_DIRECTORY, "WeatherData.RData") ) 
#


################################################################################
# These variables are generated during the reporting process. They are then 
# passed to the odfWeave function to generate the report. If any of them are
# missing at the end of the reporting process, you have an error in your report.

# Section 1
WeatherReport$swapTestResults = NA
WeatherReport$swapCoverage = NA
WeatherReport$vovVerified = NA
WeatherReport$vovTotal = NA

# Section 2
WeatherReport$optionTestResults = NA
WeatherReport$optionCoverage = NA
WeatherReport$optionDeltaCoverage = NA
WeatherReport$optionVegaCoverage = NA

# Section 3
WeatherReport$asOfDate = NA
WeatherReport$positionData = NA

# Section 4
WeatherReport$quoteData = NA


################################################################################
# Generate each piece of the report and output the pdf
#
WeatherReport$main <- function( )
{
    write.csv( QUOTE_DATA, 
            file = paste(PV_DIRECTORY, "/YJ_QUOTES_", AS_OF_DATE, ".csv", sep=""),
            row.names = FALSE )
    
	# Section 1
    WeatherReport$.generateSwapSection( QUOTE_DATA, POSITION_DATA, AS_OF_DATE )
    
    # Section 2
    WeatherReport$.generateOptionSection( QUOTE_DATA, POSITION_DATA, AS_OF_DATE )
    
    # Section 3
    WeatherReport$asOfDate = AS_OF_DATE
    WeatherReport$positionData = WeatherReport$.formatPositionData(POSITION_DATA)
    
    # Section 4
    WeatherReport$quoteData = WeatherReport$.formatQuoteData(QUOTE_DATA)
    
    # Now generate the report!
    outputFile = WeatherReport$.generateReport()
    rLog("Wrote report to:", outputFile)
    
    rLog("\nDone.")
}


################################################################################
# Perform the PV process for all Weather swap positions
#    1. Test if within bid/ask spread
#    2. Calculate coverage ratio
#    3. Calculate value of position verified
#
WeatherReport$.generateSwapSection <- function( quoteData, positionData, asOfDate )
{
    swapQuotes = subset( quoteData, quoteData$TYPE=="SWAP" )
    swapPositions = subset( positionData, positionData$POSITION_TYPE=="SWAP" )
    
    # Step 1
    swapTestResults = Weather$testQuotes( swapQuotes, swapPositions, 
            as.Date( secdb.dateRuleApply( asOfDate, "-1b" ) ) )
    WeatherReport$swapTestResults = 
            swapTestResults[, c( "Structure", "Term", "Unit", "Last", 
                            "Bid", "Ask", "Spread", "Mark", "Mark OK" ) ]
    
    # Step 2
    WeatherReport$swapCoverage = Weather$coverageRatio( swapQuotes, swapPositions )
    
	# Step 3
    WeatherReport$vovTotal = sum(abs(swapPositions$POSITION_SIZE))
    WeatherReport$vovVerified = WeatherReport$vovTotal * 
            WeatherReport$swapCoverage
    
    invisible(NULL)
}



################################################################################
# Perform the PV process for all Weather swap positions
#    1. Test if within bid/ask spread
#    2. Calculate {position, delta, vega} coverage ratio
#
WeatherReport$.generateOptionSection <- function( quoteData, positionData, asOfDate )
{
    optionQuotes = subset( quoteData, quoteData$TYPE == "CALL" | quoteData$TYPE == "PUT" )
    optionPositions = subset( positionData, positionData$POSITION_TYPE != "SWAP" )
    
	# Step 1
    optionTestResults = Weather$testQuotes( optionQuotes, optionPositions, asOfDate )
    WeatherReport$optionTestResults = 
            optionTestResults[, c( "Structure", "Term", "Unit", "Last", 
                            "Bid", "Ask", "Spread", "Mark", "Mark OK" )]

    # Step 2
    WeatherReport$optionCoverage = Weather$coverageRatio( optionQuotes, 
            optionPositions, "POSITION_SIZE" )
    WeatherReport$optionDeltaCoverage = Weather$coverageRatio( optionQuotes, 
            optionPositions, "DELTA" )
    WeatherReport$optionVegaCoverage = Weather$coverageRatio( optionQuotes, 
            optionPositions, "VEGA" )
    
    invisible(NULL)
}


################################################################################
# Clean up the position table for printing to the report
#
WeatherReport$.formatPositionData <- function( positionData )
{
    names(positionData) = c( "Location", "Start Date", "Duration", "Type", 
            "Position", "Strike", "Price", "Underlying", "Delta", "Vega", 
            "Unit", "End Date" )
    
    startDate = as.Date(positionData$"Start Date")
    endDate = as.Date(positionData$"End Date")
    
    positionData$Term = Term$fromDatePair( startDate, endDate )
    
    positionData$Price = dollar(positionData$Price)
    
    positionData$Delta = round(positionData$Delta,5)
    
    positionData = positionData[, c("Location", "Term", "Type", "Position", 
                    "Strike", "Price", "Underlying", "Delta" )]
    
    return(positionData)
}


################################################################################
# Clean up the quote table for printing to the report
#
WeatherReport$.formatQuoteData <- function( quoteData )
{
    names(quoteData) = c( "Structure", "Location", "Start Date", "End Date", 
            "Type", "Strike", "Position", "Limit", "Unit", "Bid Size", 
            "Bid", "Ask", "Ask Size", "Last", "Spread" )
    
    startDate = as.Date(quoteData$"Start Date")
    endDate = as.Date(quoteData$"End Date")
    
    quoteData$Term = Term$fromDatePair( startDate, endDate )
    
    quoteData = quoteData[, c( "Structure", "Term", "Unit", "Bid Size", 
                    "Bid", "Ask", "Ask Size", "Last")]
    
    return(quoteData)
}


################################################################################
# Run the necessary odfWeave and pdf conversion commands to spit out the report
#
WeatherReport$.generateReport <- function( verbose=TRUE )
{
    # Update our table style definition to make them fit in the page    
    styleDef = getStyleDefs()
    styleDef$ArialCentered$fontSize = "8pt"
    styleDef$ArialCenteredBold$fontSize = "8pt"
    setStyleDefs(styleDef)
    
    dateFormat = format(AS_OF_DATE, "%d%b%y")
    odtFile = paste(PV_DIRECTORY, "/", REPORT_PREFIX, dateFormat, ".odt", sep="")
    pdfFile = paste(PV_DIRECTORY, "/", REPORT_PREFIX, dateFormat, ".pdf", sep="")
    
    odfControl = odfWeaveControl()
    odfControl$verbose = verbose
    odfWeave(TEMPLATE_FILE, odtFile, control=odfControl)
    
    setwd(SOFFICE_PATH)
    system('soffice -headless -accept="socket,host=127.0.0.1,port=8100;urp;" -nofirststartwizard', 
            show.output.on.console = verbose)
    
    setwd(JOD_CONVERTER_PATH)
    command = paste("java -jar lib/jodconverter-cli-2.2.1.jar ", 
            "\"", odtFile, "\" ", "\"", pdfFile, "\"", sep="")
    system(command,  show.output.on.console = verbose)
    
    setwd(SOFFICE_PATH)
    system('soffice -unaccept=all',  show.output.on.console = verbose)
    
    return( pdfFile )
} 


################################################################################
WeatherReport$main()





