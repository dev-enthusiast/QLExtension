###############################################################################
# EODPositionFeed.R
# 
# Author: e14600
#
library( RODBC )
source( "H:/user/R/RMG/Utilities/load.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/Database/R/LiveData.R" )
source( "H:/user/R/RMG/Utilities/Database/SAS/VPort.R" )


################################################################################
# Properly connect to secdb from procmon
DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")


################################################################################
# File Namespace
#
EODPositionFeed = new.env(hash=TRUE)


################################################################################
EODPositionFeed$writeData <- function( portfolio )
{
    if( portfolio == "" )
    {
        rError( "Unable to get portfolio name. Exiting.")
        return( 1 )
    }
    
    asOfDate = as.Date( secdb.dateRuleApply( EODPositionFeed$.getAsOfDate(), "-1b") )
    
    livePositions = LiveData$cpsprodPositions()
    
    portfolioBooks = VPort$booksForPortfolio( portfolio, asOfDate )
    
    # Added 10/07/08 for Nick B. They only want to see those two books.
    if( toupper(portfolio) == toupper("Risk Disclosed Structured VAR") )
    {
        portfolioBooks = c("HAUSPOPT", "HAUSSPRD")
    }
    
    bookPositions = subset( livePositions, livePositions$book %in% portfolioBooks )
    
    updatedPositions = EODPositionFeed$.addBookCharacteristics( bookPositions, asOfDate )
    updatedPositions = EODPositionFeed$.addCurveCharacteristics( updatedPositions, asOfDate )
    updatedPositions = EODPositionFeed$.addPriceCharacteristics( updatedPositions, asOfDate )
    
    outputFolder = paste("S:/Risk/Trading Controls/Daily Reports/", 
            format( asOfDate, "%m%d%y" ), " Daily Reports", sep="" )
    if( !file.exists( outputFolder ) ) { dir.create( outputFolder ) }
    
    outputFile = paste( outputFolder, "/", portfolio, "_LIVE.csv", sep="" ) 
    write.csv( updatedPositions, row.names = FALSE, file = outputFile )
    
    invisible( 0 )
}


################################################################################
# Try the procmon date value.
# If that fails, check the R asOfDate.
# If that fails, use today.
#
EODPositionFeed$.getAsOfDate <- function()
{
    runDate = Sys.getenv("PROCMON_RUNDATE")
    asOfDate = Sys.getenv("asOfDate")
    
    if( runDate != "" )
    {
        asOfDate = as.Date( runDate, format="%Y%m%d" )
        
    } else if( asOfDate != "" )
    {
        asOfDate = as.Date( asOfDate )
        
    } else
    {
        asOfDate = Sys.Date()-1
    }
    
    return( asOfDate )
}


################################################################################
# Characteristics that need to be in the output data set - 
#  Book_Name	Office	commod_curve	ContractYear	ContractQuarter	
#  contractmonth	policy_region	policy_subregion	Commodity	Sttype	
#  RiskFactor	position	price	CURVE_UNIT	type	Economic_owner	
#  Executive_owner	Portfolio_mgr	CER_group	PM_Gen_load	PM_region	
#  Upstream_region	Acct_treatment	Organization
#
EODPositionFeed$.addBookCharacteristics <- function( bookPositions, asOfDate )
{
    # Using this to get more of the curve/book characteristics
    positionFile = paste( "S:/All/Risk/Data/VaR/prod/Positions/sas.positions.",
            asOfDate, ".RData", sep="" )
    yesterdaysPositions = varLoad( "positions", positionFile )
    
    
    # Book Characteristics -
    # Book_Name, Office, Economic_owner, 
    # Executive_owner, portfolio_mgr, 
    # cer_group, pm_gen_load, pm_region, upstream_region, 
    # acct_treatment, organization 
    bookTable = unique( yesterdaysPositions[ , c( 
                            'PROFIT_CENTER_1', 'OFFICE', 'ECONOMIC_OWNER', 
                            'EXECUTIVE_OWNER', 'PORTFOLIO_MGR',
                            'CER_GROUP', 'PM_GEN_LOAD', 'PM_REGION', 'UPSTREAM_REGION',
                            'ACCT_TREATMENT', 'ORGANIZATION' )] )
    
    updatedPositions = merge( bookPositions, bookTable, all.x = TRUE, 
            by.x = "book", by.y = "PROFIT_CENTER_1" )
    
    return( updatedPositions )
}    

    
################################################################################
# Curve characteristics - 
#	commod_curve	policy_region	policy_subregion	Commodity	Sttype	
#	CURVE_UNIT		
#
EODPositionFeed$.addCurveCharacteristics <- function( bookPositions, asOfDate )
{
    # Using this to get more of the curve/book characteristics
    positionFile = paste( "S:/All/Risk/Data/VaR/prod/Positions/sas.positions.",
            asOfDate, ".RData", sep="" )
    yesterdaysPositions = varLoad( "positions", positionFile )
    

    curveTable = unique( yesterdaysPositions[ , c( 
                            'LOCATION_NM', "POLICY_REGION", "POLICY_SUBREGION",
                            "POLICY_COMMODITY", "STTYPE", "PRICE_UOM" )] )
    
    updatedPositions = merge( bookPositions, curveTable, all.x = TRUE, 
            by.x = "curve.name", by.y = "LOCATION_NM" )
    
    return( updatedPositions )
}       
    

################################################################################
# Still Missing... - 
#  ContractYear	 ContractQuarter		RiskFactor	price	type
#
# We're not doing risk factor or type, as they're not really useful for anything.
#
EODPositionFeed$.addPriceCharacteristics <- function( bookPositions, asOfDate )
{
    updatedPositions = bookPositions
    
    updatedPositions$CONTRACTYEAR = format( updatedPositions$contract.dt, "%Y" )
    updatedPositions$CONTRACTQUARTER = paste( 
            format( updatedPositions$contract.dt, "%Y"), "QTR", 
            ceiling( as.numeric( format( updatedPositions$contract.dt, "%m" ) ) / 3 ),
            sep = "")
    
    # now merge in the price
    priceFile = paste( "S:/All/Risk/Data/VaR/prod/Prices/sasPrices.",
            asOfDate, ".RData", sep="" )
    prices = varLoad( "hP", priceFile )
    names( prices )[grep( "-", names(prices), fixed=TRUE )] = "price"
    
    updatedPositions = merge( updatedPositions, prices, all.x = TRUE )
    
    return( updatedPositions )
}


################################################################################
# Current portfolios to run for:
#	East Power Trading Portfolio
#	Risk Disclosed Structured VAR
#	PM Management Portfolio
#
#returnCode = EODPositionFeed$writeData( Sys.getenv( "portfolio" ) )
#q( status = returnCode )


portfolios = c( "East Power Trading Portfolio", "Risk Disclosed Structured VAR", "PM Management Portfolio" )
for( portfolio in portfolios )
{
    Sys.setenv( portfolio = portfolio )
    EODPositionFeed$writeData( Sys.getenv( "portfolio" ) )
}


