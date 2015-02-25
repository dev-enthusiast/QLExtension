################################################################################
# Utilities called from the delta and vega var ticker generator files
#

################################################################################
# External Libraries
#
library(RODBC)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")


################################################################################
# File Namespace
#
TickerUtils = new.env(hash=TRUE)


################################################################################
# Constants
DATA_DIR = "S:/All/Risk/Data/VaR/live"
STARTING_PNL_FILE = paste( DATA_DIR, "/startingPNLChanges.RData", sep="")
VEGA_PNL_FILE = paste( DATA_DIR, "/vegaPNLChanges.RData", sep="")


################################################################################
# Get the asOfDate for the whole system
# 
TickerUtils$getAsOfDate <- function()
{
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    chan = odbcDriverConnect(conString)
    asOfDate = sqlQuery(chan, 
            "select max(PRICING_DATE) from foitsox.sdb_holiday_calendar where calendar='CPS-BAL'")  
    odbcClose(chan)
    
    asOfDate = as.Date(asOfDate[1,1])
    
    if( length(asOfDate) == 0 )
    {
        rWarn("foitsox.sdb_holiday_calendar isn't working. Reverting asOfDate to yesterday.")
        asOfDate = Sys.Date()-1
    }
    
    return(asOfDate)
}


################################################################################
# reads the portfolios requested by the users and creates 1 unified list
#
TickerUtils$getRequestedPortfolios <- function( )
{
    pflFiles = list.files( DATA_DIR, "pfl$", full.names=TRUE)
    portfolios = NULL
    for( file in pflFiles )
    {
        listing = readLines( file, warn=FALSE )
        listing = unlist(strsplit(listing, ";", fixed=TRUE))
        portfolios = c(portfolios, listing)
    }
    
    uniqueList = sort( unique(portfolios) )
    
    return(uniqueList)
}


################################################################################
# Extract the prices, calculate a change matrix and multiply it by the positions
# 
TickerUtils$getPNLChanges <- function( prices, positions )
{
    priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
            names(prices), extended=TRUE, value=TRUE)
    
    prices = as.matrix( prices[,priceChangeColumns] )
    
    pricesTZero = prices[ , 2:ncol(prices) ]
    pricesTMinus1 = prices[ , 1:(ncol(prices)-1)]
    
    changeData = pricesTZero-pricesTMinus1
    
    pnlChange = data.frame(changeData * positions)
    names(pnlChange) = priceChangeColumns[-1]   
    
    return(pnlChange)
}


################################################################################
TickerUtils$varForPortfolio <- function( portfolio, dataForVaR, asOfDate )
{
    bookList = TickerUtils$getBooksForPortfolio(portfolio, asOfDate)
    rDebug("Books:", bookList)
    
    portfolioData = subset(dataForVaR, dataForVaR$book %in% bookList)
    
    cleanData = na.omit(portfolioData)
    if(nrow(cleanData) != nrow(portfolioData))
    {
        rWarn("NAs Present In portfolioData. Omitting...")
    }
    
    varValue = TickerUtils$varFromPNLChanges(cleanData[,-1]) 
    
    rLog("Portfolio:", portfolio, "-", dollar(varValue))
    
    return(varValue)
}


################################################################################
# Return a vector of books for a given portfolio
#
TickerUtils$getBooksForPortfolio <- function(portfolio, pricingDate)
{
    if( is.null(TickerUtils$portfolioMap) )
    {
        TickerUtils$portfolioMap = TickerUtils$createPortfolioMap(pricingDate)
    }
    
    bookList = TickerUtils$portfolioMap[[ toupper(portfolio) ]]
    if(length(bookList)==0)
    {
        return(portfolio)
    }
    
    return(bookList)
}


################################################################################
TickerUtils$createPortfolioMap <- function( pricingDate )
{
    
    pricingDate <- format(pricingDate, "%m/%d/%Y")
    
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    con = odbcDriverConnect(conString)
    
    query = paste(
            "SELECT PARENT, PARENT_TYPE, CHILD, CHILD_TYPE from foitsox.sdb_portfolio_hierarchy ",
            "WHERE PRICING_DATE=TO_DATE('", pricingDate, "','MM/DD/YYYY') ", sep="" )
    MM = sqlQuery(con, query)
    odbcClose(con)
    
    MM$PARENT = toupper(MM$PARENT)
    MM$PARENT_TYPE = toupper(MM$PARENT_TYPE)
    MM$CHILD = toupper(MM$CHILD)
    MM$CHILD_TYPE = toupper(MM$CHILD_TYPE)
    
    uniquePortfolios = unique(MM$PARENT)  
    portfolioMap = new.env(hash=TRUE)
    
    for( portfolioIndex in seq_along(uniquePortfolios) )
    {
        portfolio = uniquePortfolios[portfolioIndex]
        bookList = NULL
        
        children = subset(MM, MM$PARENT==portfolio)
        
        while( length(which(children$CHILD_TYPE=="PORTFOLIO")) )
        {
            portfolioIndex = which(children$CHILD_TYPE=="PORTFOLIO")
            
            child = children[portfolioIndex,]
            
            children = rbind(children, subset(MM, MM$PARENT %in% child$CHILD))
            
            children = children[-portfolioIndex,]
        }
        
        bookList = sort(children$CHILD)
        portfolioMap[[portfolio]] = bookList
        
    }
    
    return(portfolioMap)
}


################################################################################
# basic var calculator, takes a n X m matrix of pnl changes
#
TickerUtils$varFromPNLChanges <- function( changeMatrix )
{
    vars = apply(changeMatrix, 2, sum)
    if( length(which(is.na(vars))) != 0 )
    {
        finalVaR = NA
        
    }else
    {
        finalVaR = sd(vars)*4
    }
    
    return(finalVaR)
}

