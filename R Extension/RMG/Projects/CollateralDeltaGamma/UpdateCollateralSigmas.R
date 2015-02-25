###############################################################################
# Update the sigma values used in the collateral delta gamma report
# 
library( SecDb )
library( reshape )
library( zoo )
source( "H:/user/R/RMG/Utilities/sendEmail.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )

################################################################################
# Initialize SecDb for use on Procmon
#
DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")

options(stringsAsFactors=FALSE)

################################################################################
#
main <- function()
{

    AsOfDate   = as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
    Window = 70;
    GAS_CURVE =   "COMMOD NG EXCHANGE";
    COAL_CURVE =  "COMMOD FCO API2 INDEX";
    POWER_CURVE <- list()
    POWER_CURVE$commodity <- "Commod PWR PJM Physical"
    POWER_CURVE$location <- "Western Hub"    
    POWER_CURVE$Bucket <- "7x24"
    POWER_CURVE$SvcType <- "Energy"
    
    yrWeights = c(2/3,1/3)
    
# Get the 1-sigma value for the power, gas and coal commodities
    commodSigmas = getSigmaValues( AsOfDate, Window, GAS_CURVE, COAL_CURVE, POWER_CURVE, yrWeights  )

    
# Read in the old data
    SCENARIO_FILE = "H:/user/R/RMG/Projects/CollateralDeltaGamma/ScenarioList.csv"
    originalScenarioData = read.csv( SCENARIO_FILE )
    
# Extract only the sigma-specific rows so we can manipulate them below
    sigmaRows       = grep("^(Up|Dn)[0-9]+s$", originalScenarioData$NAME)
    sigmaEntries    = originalScenarioData[ sigmaRows , ]
    newScenarioData = originalScenarioData[ -sigmaRows, ]
    
# Extract the 'sigma multiplier' (usually 1,2,4,8) so we can use it to multiply
# by the 1-sigma value calculated above
    sigmaFactorMatches   = regexpr("[0-9]+", sigmaEntries$NAME)
    startOfNumberStrings = sigmaFactorMatches
    endOfNumberStrings   = sigmaFactorMatches + attr(sigmaFactorMatches, "match.length") - 1 
    sigmaValues          = substr(sigmaEntries$NAME, startOfNumberStrings, endOfNumberStrings )
    sigmaValues          = as.numeric( sigmaValues )
    
# Multiply the commodity sigmas by their sigma factor
    powerRows = grep("POWER", sigmaEntries$COMMODITY_GRP)
    gasRows   = grep("GAS", sigmaEntries$COMMODITY_GRP)
    coalRows  = grep("COAL", sigmaEntries$COMMODITY_GRP)

    sigmaEntries$STRESS_VALUE[ powerRows ] = sigmaValues[ powerRows ] * as.numeric(commodSigmas$PWR)
    sigmaEntries$STRESS_VALUE[ gasRows ]   = sigmaValues[ gasRows ] * as.numeric(commodSigmas$NG)
    sigmaEntries$STRESS_VALUE[ coalRows ]  = sigmaValues[ coalRows ] * as.numeric(commodSigmas$COL)
    
# Negate all of the downward stress scenarios
    sigmaEntries$STRESS_VALUE[ grep("^Dn", sigmaEntries$NAME) ] = 
            sigmaEntries$STRESS_VALUE[ grep("^Dn", sigmaEntries$NAME) ] * -1
    
# recreate the sigma file and write it back out
    newScenarioData = rbind( sigmaEntries, newScenarioData )
    print( newScenarioData )
    write.csv( newScenarioData, SCENARIO_FILE, row.names=FALSE )
    rLog( "\nWrote", SCENARIO_FILE )
    
    sendSigmaEmail( originalScenarioData, newScenarioData )
    
    rLog("Done.")
}

################################################################################
#                   
getSigmaValues <- function( AsOfDate, Window, GAS_CURVE, COAL_CURVE, POWER_CURVE, yrWeights  )
{
    script  <- "_LIB MATLAB Functions"
    func    <- "MATLAB::GetFutCurve"
    begMth12 <-   as.Date(secdb.dateRuleApply(AsOfDate, "+1v+J"))
    endMth12 <-   as.Date(secdb.dateRuleApply(AsOfDate, "12m+e"))
    begMth24 <-   as.Date(secdb.dateRuleApply(AsOfDate, "+13v+J"))
    endMth24 <-   as.Date(secdb.dateRuleApply(AsOfDate, "24m+e"))
    prcDate <- AsOfDate    
    PwrCurve12 <- c()
    PwrCurve24 <- c()
    DatesPrc <- c()
 
    UseFutDates = 0    
    while   (length(PwrCurve12) < Window ){
        x <- secdb.invoke(  script, func, prcDate,begMth12, endMth12, POWER_CURVE$commodity , POWER_CURVE$location, POWER_CURVE$Bucket, POWER_CURVE$SvcType, 0 ) 
        temp <- mean(x$value)
        if( is.na(mean( temp))){
              prcDate <- as.Date(secdb.dateRuleApply(prcDate, "-1b"))
              next;
        }
        PwrCurve12 <- c( mean( x$value), PwrCurve12 )        
        x <- secdb.invoke(  script, func, prcDate,begMth24, endMth24, POWER_CURVE$commodity , POWER_CURVE$location, POWER_CURVE$Bucket, POWER_CURVE$SvcType, 0 ) 
        PwrCurve24 <- c(  mean( x$value), PwrCurve24 )        
        
        DatesPrc <- c(prcDate, DatesPrc)
        prcDate <- as.Date(secdb.dateRuleApply(prcDate, "-1b"))
    }
    sigma12 = sd( diff( PwrCurve12 ) )
    sigma24 = sd( diff( PwrCurve24 ) )    
    sigmaPW = sigma12*yrWeights[1] +   sigma24*yrWeights[2]
    
    sigmaNG = 0    
    x <- tsdb.futStrip( GAS_CURVE,as.Date(DatesPrc[ 1 ]), AsOfDate, seq(begMth12, by="month", length.out=12) )
    x <- cast( x, pricing.dt ~ ., mean )    
    x <- x[   which(x$pricing.dt %in%  DatesPrc) , ]
    x <- colRename( x, "(all)", "value" )
    sigmaNG = yrWeights[1]*sd(diff(x$value))
    
    x <- tsdb.futStrip( GAS_CURVE,as.Date(DatesPrc[ 1 ]), AsOfDate, seq(begMth24, by="month", length.out=12) )
    x <- cast( x, pricing.dt ~ ., mean )    
    x <- x[   which(x$pricing.dt %in%  DatesPrc) , ]
    x <- colRename( x, "(all)", "value" )
    sigmaNG = sigmaNG + yrWeights[2]*sd(diff(x$value))
    

    sigmaCol = 0    
    x <- tsdb.futStrip( COAL_CURVE,as.Date(DatesPrc[ 1 ]), AsOfDate, seq(begMth12, by="month", length.out=12) )
    x <- cast( x, pricing.dt ~ ., mean )    
    x <- x[   which(x$pricing.dt %in%  DatesPrc) , ]
    x <- colRename( x, "(all)", "value" )
    sigmaCol = yrWeights[1]*sd(diff(x$value))
    
    x <- tsdb.futStrip( COAL_CURVE,as.Date(DatesPrc[ 1 ]), AsOfDate, seq(begMth24, by="month", length.out=12) )
    x <- cast( x, pricing.dt ~ ., mean )    
    x <- x[   which(x$pricing.dt %in%  DatesPrc) , ]
    x <- colRename( x, "(all)", "value" )
    sigmaCol = sigmaCol + yrWeights[2]*sd(diff(x$value))
    SigmaValues <- list()
    SigmaValues$PWR <-   sigmaPW;    
    SigmaValues$NG <-   sigmaNG;
    SigmaValues$COL <-   sigmaCol;   
    
    return( SigmaValues ) 
}

################################################################################
# Email output looks like - 
# 
# 1 sigma is now equal to:
# 
#   Commodity          Old  New   Unit   
# 1 DOMESTIC COAL      2.14 2.100 $/ton  
# 2 GAS                0.10 0.092 $/MMBTU
# 3 INTERNATIONAL COAL 2.14 2.100 $/ton  
# 4 POWER              0.57 0.553 $/MWH  
# 
#
sendSigmaEmail <- function( originalScenarioData, newScenarioData )
{
    Units = data.frame( COMMODITY_GRP = c("INTERNATIONAL COAL", "DOMESTIC COAL", "GAS", "POWER"),
                        Unit = c("$/ton", "$/ton", "$/MMBTU", "$/MWH") )
    
    old1Sigma = originalScenarioData[grep("Up1s", originalScenarioData$NAME),
                                     c("COMMODITY_GRP", "STRESS_VALUE")]
    
    new1Sigma = newScenarioData[grep("Up1s", newScenarioData$NAME),
                                c("COMMODITY_GRP", "STRESS_VALUE")]
    
    emailTable = merge( old1Sigma, new1Sigma, by="COMMODITY_GRP" )
    emailTable = merge( emailTable, Units )
    colnames( emailTable ) = c("Commodity", "Old", "New", "Unit" )
    
    print( emailTable )
    body        = "1 sigma is now equal to:"
    outputTable = capture.output( print.data.frame( emailTable, right=FALSE ) )
    body        = paste( body, paste( outputTable, collapse="\n"), sep="\n\n")
    body        = paste( body, "", sep="\n\n")
    subject     = "Updated collateral stress scenario sigmas"
    sendEmail( from = "DO-NOT-REPLY@constellation.com", 
               to = getEmailDistro("CollateralSigmaUpdate"), 
               subject, body )
}

################################################################################
main()
