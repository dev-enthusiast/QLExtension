###############################################################################
# SASPrices.R
#
# This pulls all the SAS prices by day and saves them into the data dir.
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
SASPrices = new.env(hash=TRUE)


################################################################################
SASPrices$pullAllPrices <- function()
{
    rLog("Beginning price pull...")
    connection = odbcConnect("SASPRD", believeNRows=FALSE)
    
    pricingDates = SASPrices$.getPricingDates( connection )
    
    uniquePairs = SASPrices$.getUniqueCurveTerms( connection ) 
    
    rLog("Generating empty price matrix...")
    priceMatrix = matrix(NA, nrow=nrow(uniquePairs), ncol=length(pricingDates), 
            dimnames=list(NULL, pricingDates))
    
    for( pricingDate in pricingDates )
    {
        hP = SASPrices$.pullPricesForDay( connection, pricingDate )
        print( head(hP) )
        
        outputFile = paste( DATA_DIR, "Prices/sasPrices.", 
                pricingDate, ".RData", sep="")
        save( hP, file=outputFile )
        rLog("Saved file:", outputFile)
        
        
        rLog("Merging Prices With Unique Pairs...")
        hP = merge(uniquePairs, hP, all.x=TRUE)
        hP = hP[order(hP$curve.name, hP$contract.dt), ]
        
        rLog("Inserting values into matrix...")
        priceMatrix[,pricingDate] = hP[[pricingDate]]
        
    }
    
    rLog("Merging matrix with unique pairs...")
    allPriceData = cbind(uniquePairs, priceMatrix)
    
    outputFile = paste(LIVE_DIR, "allPriceData.RData", sep="")
    save(allPriceData, file=outputFile)
    rLog("Saved file:", outputFile)

    odbcClose(connection)
    
}


################################################################################
SASPrices$.getPricingDates <- function( connection )
{
    rLog("Querying for unique pricing dates...")
    
    query = paste("select unique(PRICING_DATE) ",
            "from prices.buddy_price ",
            sep="")
    
    pricingDates = sqlQuery(connection, query)
    
    pricingDates = as.character(as.Date(pricingDates$PRICING_DATE)+1)
    
    return(pricingDates)
}


################################################################################
SASPrices$.getUniqueCurveTerms <- function( connection )
{
    rLog("Querying for unique curve/term pairings...")
    
    query = "select unique(COMMOD_CURVE), START_DATE from prices.buddy_price"
    
    uniquePairs = sqlQuery(connection, query)
    
    names(uniquePairs) = c("curve.name", "contract.dt")
    
    uniquePairs = uniquePairs[ do.call(order, uniquePairs), ]
    
    uniquePairs$curve.name = as.character( uniquePairs$curve.name )
    uniquePairs$contract.dt = as.Date( uniquePairs$contract.dt ) + 1
    
    return( uniquePairs )
}


################################################################################
SASPrices$.pullPricesForDay <- function( connection, pricingDate )
{
    rLog("Pulling prices for", pricingDate)
    
    query = paste("select COMMOD_CURVE, START_DATE, PRICE ",
            "from prices.buddy_price ",
            "where pricing_date={ts'", pricingDate, " 00:00:00'}", 
            sep="")
    
    hP = sqlQuery(connection, query)
    print(dim(hP))
    
    names(hP) = c("curve.name", "contract.dt", pricingDate)
    hP$curve.name = as.character(hP$curve.name)
    hP$contract.dt = as.Date(hP$contract.dt)
    hP$contract.dt = hP$contract.dt + 1
    
    return(hP)
}


################################################################################
start = Sys.time()
SASPrices$pullAllPrices()
print(Sys.time()-start)




