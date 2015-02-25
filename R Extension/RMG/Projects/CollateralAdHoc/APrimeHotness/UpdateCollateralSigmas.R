###############################################################################
# Update the sigma values used in the collateral delta gamma report
# 
# Author:	J. Scillieri, logic originally by M. Marsh
# Added:	UpdateCollateralSigmas.R Initiated Apr 15, 2009
# 
library( SecDb )
library( zoo )
source( "H:/user/R/RMG/Utilities/sendEmail.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )


################################################################################
# Initialize SecDb for use on Procmon
DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

options(stringsAsFactors=FALSE)


################################################################################
main <- function()
{
    
    POWER_CURVE = "PWJ_WEST_ANNUAL_STRIP_FLAT_2010"
    GAS_CURVE = "NG_ANNUAL_STRIP_EXCHANGE_2010"
    COAL_CURVE = "FCO_API2_ANN_STRP_WEEKLY_2010"
    
    startDate = as.Date(secdb.dateRuleApply(Sys.Date(), "-70b"))
    endDate = as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
    
# Get the 1-sigma value for the power, gas and coal commodities
    powerSigma = getSigmaValue( POWER_CURVE, startDate, endDate )
    gasSigma = getSigmaValue( GAS_CURVE, startDate, endDate )
    coalSigma = getSigmaValue( COAL_CURVE, startDate, endDate )
    
# Read in the old data
    SCENARIO_FILE = "H:/user/R/RMG/Projects/CollateralAdHoc/APrimeHotness/ScenarioList.csv"
    originalScenarioData = read.csv( SCENARIO_FILE )
    
    
# Extract only the sigma-specific rows so we can manipulate them below
    sigmaRows = grep("^(Up|Dn)[0-9]+s$", originalScenarioData$NAME)
    sigmaEntries = originalScenarioData[ sigmaRows , ]
    newScenarioData = originalScenarioData[ -sigmaRows, ]
    
# Extract the 'sigma multiplier' (usually 1,2,4,8) so we can use it to multiply
# by the 1-sigma value calculated above
    sigmaFactorMatches = regexpr("[0-9]+", sigmaEntries$NAME)
    startOfNumberStrings = sigmaFactorMatches
    endOfNumberStrings = sigmaFactorMatches + attr(sigmaFactorMatches, "match.length") - 1 
    sigmaValues = substr(sigmaEntries$NAME, startOfNumberStrings, endOfNumberStrings )
    sigmaValues = as.numeric( sigmaValues )
    
    
# Multiply the commodity sigmas by their sigma factor
    powerRows = grep("POWER", sigmaEntries$COMMODITY_GRP)
    sigmaEntries$STRESS_VALUE[ powerRows ] = sigmaValues[ powerRows ] * powerSigma
    
    gasRows = grep("GAS", sigmaEntries$COMMODITY_GRP)
    sigmaEntries$STRESS_VALUE[ gasRows ] = sigmaValues[ gasRows ] * gasSigma
    
    coalRows = grep("COAL", sigmaEntries$COMMODITY_GRP)
    sigmaEntries$STRESS_VALUE[ coalRows ] = sigmaValues[ coalRows ] * coalSigma
    
    
# Negate all of the downward stress scenarios
    sigmaEntries$STRESS_VALUE[ grep("^Dn", sigmaEntries$NAME) ] = 
            sigmaEntries$STRESS_VALUE[ grep("^Dn", sigmaEntries$NAME) ] * -1
    
# recreate the sigma file and write it back out
    newScenarioData = rbind( sigmaEntries, newScenarioData )
    print( newScenarioData )
    write.csv( scenarioData, SCENARIO_FILE, row.names=FALSE )
    rLog( "\nWrote", SCENARIO_FILE )
    
    sendSigmaEmail( originalScenarioData, newScenarioData )
    
    rLog("Done.")
}


################################################################################
getSigmaValue <- function( curve, startDate, endDate )
{
    curveData = tsdb.readCurve( curve, startDate, endDate )
    curveData = zoo( curveData$value, curveData$date )
    
    sigma = sd( diff( curveData ) )
    
    sigma = round( sigma, 3 )
    
    return( sigma )
}


################################################################################
## Email output looks like - 
## 
## 1 sigma is now equal to:
## 
##   Commodity          Old  New   Unit   
## 1 DOMESTIC COAL      2.14 2.100 $/ton  
## 2 GAS                0.10 0.092 $/MMBTU
## 3 INTERNATIONAL COAL 2.14 2.100 $/ton  
## 4 POWER              0.57 0.553 $/MWH  
## 
## RAC, Risk Systems
## John.Scillieri@constellation.com
##
sendSigmaEmail <- function( originalScenarioData, newScenarioData )
{
    Units = data.frame( 
            COMMODITY_GRP = c("INTERNATIONAL COAL", "DOMESTIC COAL", "GAS", "POWER"),
            Unit = c("$/ton", "$/ton", "$/MMBTU", "$/MWH") )
    
    old1Sigma = originalScenarioData[grep("Up1s", originalScenarioData$NAME),
            c("COMMODITY_GRP", "STRESS_VALUE")]
    
    new1Sigma = newScenarioData[grep("Up1s", newScenarioData$NAME),
            c("COMMODITY_GRP", "STRESS_VALUE")]
    
    emailTable = merge( old1Sigma, new1Sigma, by="COMMODITY_GRP" )
    emailTable = merge( emailTable, Units )
    colnames( emailTable ) = c("Commodity", "Old", "New", "Unit" )
    
    print( emailTable )
    body = "1 sigma is now equal to:"
    
    outputTable = capture.output( print.data.frame( emailTable, right=FALSE ) )
    body = paste( body, paste( outputTable, collapse="\n"), sep="\n\n")
    
    body = paste( body,
            "RAC, Risk Systems\nJohn.Scillieri@constellation.com", sep="\n\n")
    
    subject = "Updated collateral stress scenario sigmas"
    
    sendEmail( 
            from = "DO-NOT-REPLY@constellation.com", 
            to = getEmailDistro("CollateralSigmaUpdate"), 
            subject, body )
}


################################################################################
main()
