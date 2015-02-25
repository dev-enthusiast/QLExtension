###############################################################################
# TestDay.R
#
# Author: John Scillieri
# 

###############################################################################
# Query to return all traded option positions for today 
#
#        SELECT   first_name, last_name, counterparty, curve, contract_date,
#option_frequency, buy_sell_cd, call_put_cd, is_ibt, deal_type,
#volume, contract_volume, spot_price
#FROM prism.trade_flat_vw
#WHERE valuation_date = TO_DATE (SYSDATE) 
#AND call_put_cd IS NOT NULL
#AND deal_type not in ('TRANSPORT')
#AND volume <> 0
#ORDER BY trade_date DESC
#


################################################################################
# CONFIGURATION PARAMETERS
#
START_OF_TEST_MONTH = "2009-03-01"


################################################################################
library(RODBC)
source("H:/user/R/RMG/Projects/PV/PVUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
TestDay = new.env(hash=TRUE)


################################################################################
# 
TestDay$main <- function( )
{
    # This date should be the TEST DAY DATE, not TDD-1!
    asOfDate = Sys.Date()

    testDir = PVUtils$getTestDir(START_OF_TEST_MONTH)
    dateFormat = format(asOfDate, "%d%b%y")
    
    volTestFile = paste(testDir, "VolTest_", dateFormat, ".xls", sep="")  
    
    # only do this stuff on the first run of test day!
    if( !file.exists(volTestFile) )
    {     
        TEMPLATE_DIR = "S:/Risk/Reports/Vol Test/Template/Excel/"
        VOL_TEST_TMPL = paste(TEMPLATE_DIR, "VolTestTemplate.xls", sep="")
        VEGA_PLOTS_TMPL = paste(TEMPLATE_DIR, "TopVegaPlotsTemplate.xls", sep="")
        
        dir.create(testDir)
        
        
        file.copy( VOL_TEST_TMPL, volTestFile)
        
        
        vegaPlotsFile = paste(testDir, "TopVegaPlotsTemplate.xls", sep="")
        file.copy( VEGA_PLOTS_TMPL, vegaPlotsFile )    
        
        
        TestDay$WritePriceVolDiffs( asOfDate, testDir )
        
    } else
    {
        # Read in the system prices
        rLog("\nReading System Prices...")
        PVUtils$SecDbSystemPrices( asOfDate, testDir )
        
        # Extract exceptions
        rLog("\nExtracting Exceptions...")
        PVUtils$GetExceptions( asOfDate, testDir )
        
        rLog("\nDone.")
    }
    
}


################################################################################
# 
TestDay$WritePriceVolDiffs <- function( asOfDate, testDir )
{
    NGCurves = c("Commod NG EXCHANGE")
    NGFile = paste(testDir, "NG_", asOfDate, ".txt", sep="")
    file.remove(NGFile)
    PVUtils$WritePriceVolDiffs( NGCurves, asOfDate, outputPath=NGFile )
    
    WestCurves = c(
            "Commod PWP 6x16 SP15 PHYSICAL",
            "Commod PWM 6x16 PHYSICAL" 
            )
    WestFile = paste(testDir, "West_", asOfDate, ".txt", sep="")
    file.remove(WestFile)
    PVUtils$WritePriceVolDiffs( WestCurves, asOfDate, outputPath=WestFile )
    
    EastCurves = c(
            "Commod PWJ 5x16 WEST PHYSICAL",
            "Commod PWJ 5x16 ADHUB PHYSICAL",
            "Commod PWJ 5x16 NIHUB PHYSICAL",
            "Commod PWI 5x16 CINHUB Phys" 
            )
    EastFile = paste(testDir, "East_", asOfDate, ".txt", sep="")
    file.remove(EastFile)
    PVUtils$WritePriceVolDiffs( EastCurves, asOfDate, outputPath=EastFile)
}


################################################################################
TestDay$main()



