###############################################################################
# SASPositions.R
#
# This pulls all the SAS positions for a given day
#
# Author: e14600


################################################################################
# External Libraries
#
library(RODBC)
library(reshape)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
# CONSTANTS
#
DATA_DIR = "//ceg/cershare/All/Risk/Data/VaR/prod/"


################################################################################
# File Namespace
#
SASPositions = new.env(hash=TRUE)


################################################################################
SASPositions$main <- function()
{
    asOfDate = secdb.dateRuleApply(Sys.Date(), "-1b")
    
    rLog("Beginning SAS position pull...")
    positions = SASPositions$.pull()
    
    outputFile = paste(DATA_DIR, "Positions/sas.positions.", 
            as.character(asOfDate), ".RData", sep="")
    save( positions, file = outputFile )
    rLog( "Saved file:", outputFile )
    
    invisible(NULL)
}


################################################################################
SASPositions$.pull <- function()
{
    connection = odbcConnect("SASPRD", believeNRows=FALSE)
    
    columnsToPull = c( 
            "InstType", "VALUATION_DATE", 
            "TRADE_HEADER_ID", "EXTERNALDEALID", "PARENT_DEAL_ID", 
            "SOR_INDICATOR", "SOE_INDICATOR", "INTERNAL_COMPANY", 
            "BROKER", "PROFIT_CENTER_1", "PROFIT_CENTER_2", 
            "DEAL_TYPE", "TRADE_START_DATE", "TRADE_END_DATE", 
            "TRADER", "TRADE_ENTRY_DATE", "SOURCE_FEED", 
            "TRADE_DETAIL_ID", "PRODUCT", "LOCATION_NM", 
            "DELIVERY_POINT", "REGION", "SERVICE_TYPE", "BUY_SELL_TYPE", 
            "TRANSACTION_TYPE", "PERIOD_START_DATE", 
            "PERIOD_END_DATE", "FREQUENCY_INDICATOR", "TIMEZONE", 
            "QUANTITY_BUCKET_TYPE", "PRICE_TYPE", "STRIKE", "PRICE", 
            "PRICE_USD", "MTM_VALUE_NATIVE", "MTM_VALUE_USD", "QUANTITY", 
            "PRICE_UOM", "QTY_UOM", "CURRENCY", "VALUATION_MONTH", 
            "TOTAL_ORIGINAL_QUANTITY", "ACCOUNTING_PERIOD", 
            "INTEREST_RATE", "CORRELATION", "VOLATILITY", 
            "UNDISC_MTM_VALUE_NATIVE", "UNDISC_MTM_VALUE_USD", "PAYMENT_DATE", 
            "EXPIRATION_DATE", "PHY_FIN_FLAG", "SKEW_DELTA", "PNL_VALUE_USD", 
            "UNDISC_DELTA", "DISCOUNT_END_DT", 
            "PROVISION_NAME", "FX_Rate_v", "commod_curve", "DENOMINATED", 
            "Bucket", "Service", "FUTURE_DELIVERY_RULE", 
            "POLICY_COMMODITY", "POLICY_REGION", "POLICY_SUBREGION", "STTYPE", 
            "source_sys", "ACCT_TREATMENT", 
            "ORGANIZATION", "ECONOMIC_OWNER", "PORTFOLIO_MGR", "EXECUTIVE_OWNER", 
            "RISK_REPORTING_FLAG", "PM_REGION", "UPSTREAM_REGION", 
            "CER_GROUP", "PM_GEN_LOAD", "DELTA", "VEGA", "VOL_FREQUENCY", 
            "UOM", "OFFICE", "rf_MktPrice", "rf_Vol" )
    
    query = paste( "SELECT", paste( columnsToPull, collapse = ", " ),
            "FROM POSITION.POSITIONSAGG_ALL", 
            "WHERE VEGA <> 0 OR DELTA <> 0" )
    
    positions = sqlQuery( connection, query )

    odbcClose(connection)
    
    rLog("Retrieved", nrow(positions), "from SAS.")
    
    # Clean up the data 
    #	convert the datetimes to dates (fixing the db error)
    #	convert the factors to characters
    classes = sapply( positions, data.class )
    positions[ , which( classes == "POSIXt" ) ] = 
            lapply( positions[ , which( classes == "POSIXt" ) ], 
                    function(x) { as.Date(x) + 1 } )
    positions[ , which( classes == "factor" ) ] = 
            lapply( positions[ , which( classes == "factor" ) ], 
                    function(x) { as.character(x) } )
    
    return( positions )
}


################################################################################
start = Sys.time()
SASPositions$main()
print(Sys.time()-start)


