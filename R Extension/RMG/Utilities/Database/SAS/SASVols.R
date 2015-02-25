###############################################################################
# SASVols.R
#
# This pulls all the SAS vols by day and saves them into the data dir.
# In the process, it creates a matrix of 71 days used to calculate the live var.
#
# Author: e14600


################################################################################
# External Libraries
#
library(RODBC)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Procmon.R")


################################################################################
# CONSTANTS
#
DATA_DIR = "//ceg/cershare/All/Risk/data/VaR/prod/"
LIVE_DIR = "//ceg/cershare/All/Risk/data/VaR/live/"


################################################################################
# File Namespace
#
SASVols = new.env(hash=TRUE)


################################################################################
SASVols$pullAllVols <- function()
{
    connection = odbcConnect("SASPRD", believeNRows=FALSE)
    
    pricingDates = SASVols$.getPricingDates( connection )
    
    uniquePairs = SASVols$.getUniqueCurveTerms( connection ) 
    
    rLog("Generating empty vol matrix...")
    volMatrix = matrix(NA, nrow=nrow(uniquePairs), ncol=length(pricingDates), 
            dimnames=list(NULL, pricingDates))
    
    for( pricingDate in pricingDates )
    {
        hV = SASVols$.pullVolsForDay( connection, pricingDate )
        print( head(hV) )
        
        outputFile = paste( DATA_DIR, "Vols/sasVols.", 
                pricingDate, ".RData", sep="")
        rLog("Saving file:", outputFile)
        
        save( hV, file=outputFile )
        
        
        rLog("Merging Vols With Unique Pairs...")
        hV = merge(uniquePairs, hV, all.x=TRUE)
        hV = hV[order(hV$curve.name, hV$contract.dt, hV$vol.type), ]
        
        rLog("Inserting values into matrix...")
        volMatrix[,pricingDate] = hV[[pricingDate]]
    }
    
    rLog("Merging matrix with unique pairs...")
    allVolData = cbind(uniquePairs, volMatrix)
    
    outputFile = paste(LIVE_DIR, "allVolData.RData", sep="")
    save(allVolData, file=outputFile)
    rLog("Saved file:", outputFile)
    
    odbcClose(connection)
}


################################################################################
SASVols$.getPricingDates <- function( connection )
{
    rLog("Querying for unique pricing dates...")
    
    query = paste("select unique(PRICING_DATE) ",
            "from prices.buddy_volatility ",
            sep="")
    pricingDates = sqlQuery(connection, query)
    
    pricingDates = as.character(as.Date(pricingDates$PRICING_DATE)+1)
    
    return( pricingDates )
}


################################################################################
SASVols$.getUniqueCurveTerms <- function( connection )
{
    rLog("Querying for unique curve/term/type pairings...")
    
    query = "select unique(COMMOD_CURVE), START_DATE, FREQ_TYPE from prices.buddy_volatility"
    
    uniquePairs = sqlQuery(connection, query)
    
    names(uniquePairs) = c("curve.name", "contract.dt", "vol.type")
    
    uniquePairs = uniquePairs[ do.call(order, uniquePairs), ]
    
    uniquePairs$curve.name = as.character( uniquePairs$curve.name )
    uniquePairs$contract.dt = as.Date( uniquePairs$contract.dt ) + 1
    uniquePairs$vol.type = as.character( uniquePairs$vol.type )
    
    return( uniquePairs )
}


################################################################################
SASVols$.pullVolsForDay <- function( connection, pricingDate )
{
    rLog("Pulling vols for", pricingDate)
    
    query = paste("select COMMOD_CURVE, START_DATE, FREQ_TYPE, VOLATILITY ",
            "from prices.buddy_volatility ",
            "where pricing_date={ts'", pricingDate, " 00:00:00'}", 
            sep="")
    
    sasVols = sqlQuery(connection, query)
    print(dim(sasVols))
    
    names(sasVols) = c("curve.name", "contract.dt", "vol.type", pricingDate)
    sasVols$curve.name = as.character(sasVols$curve.name)
    sasVols$vol.type = as.character(sasVols$vol.type)
    sasVols$contract.dt = as.Date(sasVols$contract.dt)
    sasVols$contract.dt = sasVols$contract.dt + 1

    return(sasVols)
}


################################################################################
start = Sys.time()
SASVols$pullAllVols()
print(Sys.time()-start)




