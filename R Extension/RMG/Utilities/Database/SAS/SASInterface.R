###############################################################################
# SASInterface.R
# 
# Author: e14600
#
library(reshape)
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
# File Namespace
#
SASInterface = new.env(hash=TRUE)


################################################################################
SASInterface$getDeltas <- function( asOfDate )
{
    fileName <- paste("S:/All/Risk/Data/VaR/prod/Positions/sas.positions.",
            asOfDate, ".RData", sep="")                          
    
    deltas = varLoad("positions", fileName)
    
    if( is.null( deltas ) )
    {
        return( NULL )
    }
    
    deltas = deltas[, c("PROFIT_CENTER_1", "rf_MktPrice", "LOCATION_NM", "VALUATION_MONTH", "DELTA")]
    names(deltas) <- c("book", "riskFactor", "curve.name", "contract.dt", "value")
    deltas$book = toupper(deltas$book)
    deltas$riskFactor = toupper(deltas$riskFactor)
    deltas$curve.name = toupper(deltas$curve.name)
    deltas$contract.dt = as.Date(deltas$contract.dt)
    
    deltas = cast(deltas, book + riskFactor + curve.name + contract.dt ~ ., sum) 
    deltas$vol.type = NA
    names(deltas) = c("book", "riskFactor", "curve.name", "contract.dt", 
            "value", "vol.type" )
    
    deltas = subset(deltas, deltas$value!=0)
    
    rownames( deltas ) = NULL
    deltas = deltas[, c("book", "riskFactor", "curve.name", "contract.dt", 
                    "vol.type", "value" )]
    
    return( deltas )
}


################################################################################
SASInterface$getVegas <- function( asOfDate )
{
    fileName <- paste("S:/All/Risk/Data/VaR/prod/Positions/sas.positions.",
            asOfDate, ".RData", sep="")         
    
    vegas = varLoad("positions", fileName)
    
    if( is.null( vegas ) )
    {
        return( NULL )
    }
    
    vegas = vegas[, c("PROFIT_CENTER_1", "rf_Vol", "LOCATION_NM", "VALUATION_MONTH", 
                    "VOL_FREQUENCY", "VEGA")]
    names(vegas) <- c("book", "riskFactor", "curve.name", "contract.dt", "vol.type", "value")
    vegas$book = toupper(vegas$book)
    vegas$riskFactor = toupper(vegas$riskFactor)
    vegas$curve.name = toupper(vegas$curve.name)
    vegas$contract.dt = as.Date(vegas$contract.dt)
    vegas$vol.type = toupper(vegas$vol.type)
    
    vegas = cast(vegas, book + riskFactor + curve.name + contract.dt + vol.type ~ ., sum) 
    names(vegas) = c("book", "riskFactor", "curve.name", "contract.dt", "vol.type", "value" )
    
    vegas = subset(vegas, vegas$value != 0)
    vegas$value = vegas$value * 100
    
    rownames( vegas ) = NULL
    
    return( vegas )
}




