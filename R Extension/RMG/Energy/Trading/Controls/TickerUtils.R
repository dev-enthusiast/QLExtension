################################################################################
# Utilities called from the delta and vega var ticker generator files
#

################################################################################
# External Libraries
#
library(RODBC)


################################################################################
# File Namespace
#
TickerUtils = new.env(hash=TRUE)


################################################################################
# Constants
DATA_DIR = "S:/All/Risk/Data/VaR/live"
STARTING_PNL_FILE = paste( DATA_DIR, "/startingPNLChanges.RData", sep="")


################################################################################
# Get the window of days of prices
# 
TickerUtils$getDaysForPriceHistory <- function( asOfDate, windowSize=70 )
{
    asOfDate = as.Date(asOfDate)
    
    query <- paste("select pricing_date, calendar_date, holiday from ",
            "foitsox.sdb_holiday_calendar where calendar = 'CPS-BAL' ",
            "and calendar_date >= to_date('", asOfDate-400,"', 'yyyy-mm-dd') ", 
            "and calendar_date <= pricing_date + 33 and pricing_date in ( ",
            "select max(pricing_date) from foitsox.sdb_holiday_calendar)", sep="")
    
    chan = odbcConnect("CPSPROD", "stratdatread", "stratdatread")
    calendar <- sqlQuery(chan, query)  
    odbcClose(chan)
    
    calendar$CALENDAR_DATE <- as.Date(calendar$CALENDAR_DATE)
    
    aux <- subset(calendar, calendar$HOLIDAY=='N' & 
                    calendar$CALENDAR_DATE <= asOfDate)
    aux <- aux[order(aux$CALENDAR_DATE, decreasing=T), "CALENDAR_DATE"]
    
    hdays <- sort(aux[1:(windowSize+1)])
    
    return( hdays )
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
        listing = readLines(file)
        listing = unlist(strsplit(listing, ";", fixed=TRUE))
        portfolios = c(portfolios, listing)
    }
    
    uniqueList = unique(portfolios)
    
    return(uniqueList)
}



################################################################################
# Return a vector of books for a given portfolio
#
TickerUtils$getBooksForPortfolio <- function(portfolio, pricingDate)
{
    if(TickerUtils$isBook(portfolio, pricingDate))
    {
        return(portfolio)
    }
    
    pricingDate <- format(as.Date(pricingDate), "%m/%d/%Y")
    
    con   <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
    
    getBooks <- function( portfolio, pricingDate, con )
    {
        portfolio = tolower(portfolio)
        query = paste(
                "SELECT CHILD, CHILD_TYPE from foitsox.sdb_portfolio_hierarchy ",
                "WHERE PRICING_DATE=TO_DATE('", pricingDate, "','MM/DD/YYYY') ",
                "AND LOWER(PARENT)='", portfolio , "'", sep="" )
        MM = sqlQuery(con, query)
        portfolios = as.character(MM$CHILD[which(MM$CHILD_TYPE=="Portfolio")])
        books = as.character(MM$CHILD[which(MM$CHILD_TYPE=="Book")])
        for( port in portfolios )
        {
            books = c(books, getBooks(port, pricingDate, con))
        }
        return(books)
    }
    
    bookList = getBooks(portfolio, pricingDate, con)
    
    odbcClose(con)
    uniqueChildren = as.character(bookList)
    return(uniqueChildren)
}



################################################################################
TickerUtils$isBook <- function( name, pricingDate )
{
    pricingDate <- format(as.Date(pricingDate), "%m/%d/%Y")
    
    con   <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
    
    name = tolower(name)
    query = paste(
            "SELECT CHILD, CHILD_TYPE from foitsox.sdb_portfolio_hierarchy ",
            "WHERE PRICING_DATE=TO_DATE('", pricingDate, "','MM/DD/YYYY') ",
            "AND LOWER(CHILD)='", name , "'", sep="" )
    MM = sqlQuery(con, query)
    odbcClose(con)  
    
    if( nrow(MM) < 1 || as.character(MM$CHILD_TYPE[1]) != "Book" )
    {
        return(FALSE)
    } else
    {
        return(TRUE)
    }
    
}



################################################################################
# Return a data frame of numerics used to convert from foreign currency to usd
#
TickerUtils$getConversionTable <- function(curveList, asOfDate)
{
    curveList = as.character(curveList)
    
    currency = lapply(curveList, secdb.getValueType, valueType="Denominated" )
    
    currency = paste("USD/", currency, sep="")
    
    currencyTable = data.frame( 
            curve.name = curveList,
            currency = currency
            )
    
    fxFile = paste("S:/All/Risk/Data/VaR/prod/fx.", asOfDate, ".RData", sep="")
    exchangeRateTable = varLoad("hFX", fxFile)
    exchangeRateTable$currency = rownames(exchangeRateTable)
    
    conversionTable = merge(currencyTable, exchangeRateTable)
    
    return(conversionTable)
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
# The dataForVaR matrix must be P&L changes in dollars and have the associated
# book as a column. If the prefix is "Total", the % change value is calculated
#
TickerUtils$saveVaRData <- function( dataForVaR, portfolioList, 
        asOfDate, prefix="Delta" )
{
    varCSVData = NULL
    
    if( prefix=="Total" )
    {
        startingVaRData = varLoad( "startingPNLChanges", STARTING_PNL_FILE ) 
    }
    
    rLog("Calculating", prefix, "VaR for all portfolios...")
    for( portfolio in portfolioList )
    {
        
        recentVaRValue = TickerUtils$varForPortfolio( portfolio, dataForVaR, 
                asOfDate )
        rLog("Current", prefix, "IVaR:", dollar(recentVaRValue), "\n" )   
        
        if( prefix=="Total" )
        {
            startingVaRValue = TickerUtils$varForPortfolio( portfolio, 
                    startingVaRData, asOfDate )
            rLog("Starting", prefix, "IVaR:", dollar(startingVaRValue), "\n" )
            
            if( is.na(startingVaRValue) || startingVaRValue == 0 )
            {
                percentChange = "-"
                
            } else
            {
                percentChange = (recentVaRValue-startingVaRValue)/startingVaRValue
                percentChange = percentChange * 100
                percentChange = paste( prettyNum( percentChange, digits=2, nsmall=2 ),
                        "%", sep="")
            }
            
        }else
        {
            percentChange = "-"
        }
        
        rowData = data.frame( portfolio = portfolio, 
                value = dollar(recentVaRValue), change=percentChange )
        
        varCSVData = rbind(varCSVData, rowData)
    }
    
    rLog("Writing ", prefix, "VaRData.csv file...", sep="")
    outputFile = paste( DATA_DIR, "/", prefix, "VaRData.csv", sep="" )
    write.csv( varCSVData, file=outputFile, quote=TRUE, 
            row.names=FALSE, col.names=NA )
    
    rLog("Update Complete.\n")  
    
    invisible(NULL) 
}


################################################################################
TickerUtils$varForPortfolio <- function( portfolio, dataForVaR, asOfDate )
{
    rLog("Portfolio:", portfolio)
    
    bookList = TickerUtils$getBooksForPortfolio(portfolio, asOfDate)
    rDebug("Books:", bookList)
    
    portfolioData = subset(dataForVaR, book %in% bookList)
    
    cleanData = na.omit(portfolioData)
    if(nrow(cleanData) != nrow(portfolioData))
    {
        rWarn("NAs Present In portfolioData. Omitting...")
    }
    
    varValue = TickerUtils$varFromPNLChanges(cleanData[,-1]) 
    
    return(varValue)
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




################################################################################
# Create a data frame of all prices/vols that fall in the 71 day window
# This isn't called anywhere, it's just stored incase you need to fully recreate
# the price/vol matrix. 
#
# mode can be either 'Prices' or 'Vols'
#
TickerUtils$saveAllHistoricData <- function(asOfDate, mode="Prices")
{
    # Extract the window of days to use
    pricingDays = TickerUtils$getDaysForPriceHistory(asOfDate)
    
    PRICE_DATA_DIR = paste("S:/All/Risk/Data/VaR/prod/", mode, "/", sep="")
    
    fileList = paste("h", mode, ".", pricingDays, ".RData", sep="")
    
    variableName = ifelse( mode=="Prices", "hP", "hV")
    
    allPriceData = varLoad( variableName, 
            paste(PRICE_DATA_DIR, fileList[1], sep="") )
    allPriceData$value = NULL
    fileList = fileList[-1]
    
    for( file in fileList )
    {
        cat("Loading ", mode, " File: ", file, "...", sep="")
        prices = varLoad( variableName, paste(PRICE_DATA_DIR, file, sep="") )
        cat("Done. ")
        
        #clean it first
        prices$value = NULL
        prices = unique(prices)
        
        cat("Merging Data...")
        allPriceData = merge(allPriceData, prices, all=TRUE)
        cat("Done. \n")
        print(dim(allPriceData))
    }
    
    if( mode=="Prices")
    {
        save(allPriceData, file=paste(DATA_DIR, "/allPriceData.RData", sep=""))
        
    } else
    {
        allVolData = allPriceData
        save(allVolData, file=paste(DATA_DIR, "/allVolData.RData", sep=""))
    }
}




########################### INTERNAL OBJECTS ###################################


################################################################################
#
#.internalFunction <- function() {}

