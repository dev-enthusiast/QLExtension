###############################################################################
# PNLMonitor.R
#
# Author: e14600
#
library( SecDb )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )


################################################################################
# File Namespace
#
PNLMonitor = new.env(hash=TRUE)


################################################################################
PNLMonitor$postedValue <- function( portfolio, asOfDate = Sys.Date() )
{
    portfolio = toupper( portfolio )
    
    pnlEstimate = .getPNLEstimate( asOfDate )
    
    pnl = pnlEstimate[[portfolio]]$PNL
    
    return( pnl )
}


################################################################################
.getPNLEstimate <- function( asOfDate = Sys.Date() )
{
    dayOfMonth = format( asOfDate, "%d")
    dayOfMonth = sub( "^0", " ", dayOfMonth )
    monthYear = format( asOfDate, "%b%y" )
    secdbDate = paste( dayOfMonth, monthYear, sep="" )
    
    securityName = paste( "Trader PNL Estimate", secdbDate )
    
    securityCode = paste( 'Contents("', securityName, '")', sep="" )
    invalidateSecurityCode = paste( 'Invalidate(', securityCode, ')' )
    
    secdb.evaluateSlang( invalidateSecurityCode )
    valueType = secdb.evaluateSlang( securityCode )
    
    valueType = valueType$Portfolios
    names(valueType) = toupper( names(valueType) )
    valueType = lapply(valueType, function(x){ names(x) = toupper(names(x)); return(x) } )
    
    return( valueType )
}


################################################################################
.parsePNLValueType <- function( valueType )
{
    finalData = NULL
    for( portfolioIndex in seq_along( valueType ) )
    {
        portfolio = valueType[ portfolioIndex ]

        rowData = data.frame( t( unlist( valueType[[portfolioIndex]] ) ) )
        rowData$portfolio = names(portfolio)
        
        rowData$pnl = ifelse( is.null( rowData$PNL ), NA, rowData$PNL )
        
        finalData = rbind( finalData, rowData )
    }
    
    return( finalData )
}













