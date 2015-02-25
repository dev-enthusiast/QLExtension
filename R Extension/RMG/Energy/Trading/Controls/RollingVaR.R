###############################################################################
# RollingVaR.R
# 
# This file takes a position set and calculates the 70-day VaR for
# every day up to a given date.
#
# Author: John Scillieri
# 
library(RODBC)
source("H:/user/R/RMG/Energy/Trading/Controls/Ticker/TickerUtils.R")
source("H:/user/R/RMG/Utilities/load.R")


################################################################################
getDeltaPositions <- function( )
{
    rLog("Querying database for most recent delta positions...")
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    chan = odbcDriverConnect(conString)
    query = "select * from stratdat.rs_snapshot order by BOOK, COMMOD_CURVE, CONTRACT_DATE"
    deltaPositions = sqlQuery(chan, query)
    odbcClose(chan)
    names(deltaPositions) = c("book", "curve.name", "contract.dt", "position")
    deltaPositions$contract.dt = as.Date(deltaPositions$contract.dt)
    
    return(deltaPositions)
}


################################################################################
getVegaPositions <- function( asOfDate )
{
    rLog("Pulling current vega positions...")
    
    vegaFile = paste("S:/All/Risk/Data/VaR/prod/Positions/vega.positions.", 
            asOfDate, ".RData", sep="")
    vegaPositions = varLoad("vegaPositions", vegaFile)
    
    names(vegaPositions)[5] = "position"
    vegaPositions$position = vegaPositions$position * 100
    
    return(vegaPositions)
}


################################################################################
# Return a data frame of numerics used to convert from foreign currency to usd
#
getConversionTable <- function(curveList, asOfDate)
{
    curveList = as.character(curveList)
    
    currency = lapply(curveList, secdb.getValueType, valueType="Denominated" )
    
    currency = paste("USD/", currency, sep="")
    
    currencyTable = data.frame( 
            curve.name = curveList,
            currency = currency
            )
    
    fxFile = "S:/All/Risk/Data/VaR/prod/all.FX.RData"
    exchangeRateTable = varLoad("allFX", fxFile)
    exchangeRateTable$currency = rownames(exchangeRateTable)
    
    conversionTable = merge(currencyTable, exchangeRateTable)
    
    return(conversionTable)
}


################################################################################
buildFXTable <- function()
{
    fileList = list.files("S:/All/Risk/Data/VaR/prod/", "^fx.+RData", 
            full.names=TRUE)
    allFX = varLoad( "hFX", fileList[1])
    for( file in fileList )
    {
        rLog("Loading:", file)
        hFX = varLoad( "hFX", file) 
        allFX = merge(hFX, allFX)
    }
    
    allFX = allFX[, sort(colnames(allFX))]
    for( rowIndex in seq_len(nrow(allFX)) )
    {
        rowData = allFX[rowIndex, ncol(allFX)]
        correctNameLocation = which(hFX[,ncol(hFX)]==rowData)
        correctName = rownames(hFX)[correctNameLocation]
        rownames(allFX)[rowIndex] = correctName
    }
    
    save(allFX, file="S:/All/Risk/Data/VaR/prod/all.FX.RData")
}


################################################################################
getDeltaPNLChanges <- function( deltaPositions )
{
    # build the price matrix for the delta positions
    curveList = as.character(unique(deltaPositions$curve.name))
    marketList = lapply(strsplit(curveList, " "), function(x){return(x[2])})
    marketList = as.character(unique(unlist(marketList)))
    PATH_TO_MARKET_FILES = "S:/All/Risk/Data/VaR/prod/Prices/Market/"
    allDeltaData = NULL
    for(market in marketList)
    {
        print(paste("loading price market", market))
        marketFile = paste(PATH_TO_MARKET_FILES, "all.", market, ".Prices.RData", sep="")
        #switch this to varload
        load(marketFile)
        
        marketPositions = deltaPositions[grep(market, deltaPositions$curve.name),]
        dataSubset = merge(marketPositions, hP.mkt)
        allDeltaData = rbind(allDeltaData, dataSubset)
    }
    
    # Get the conversion rate for these curves and append it to the positions
    conversionTable = getConversionTable( curveList, asOfDate)
    
    # get our currency conversion data and put it in sorted order
    allConversionData = merge(allDeltaData[,c("curve.name", "contract.dt")], conversionTable)
    allConversionData = allConversionData[order(allConversionData$curve.name,
                    allConversionData$contract.dt),]
    
    #Figure out which days are needed for the rolling var
    priceDays = as.Date(names(allDeltaData)[5:length(names(allDeltaData))])
    firstBusinessDay = which(priceDays>=startDate)[1]
    lastDayOfWindow = firstBusinessDay - 70
    daysToKeep = priceDays[lastDayOfWindow:length(priceDays)]
    
    
    allDeltaData = allDeltaData[, 
            c("curve.name", "contract.dt", "position", as.character(daysToKeep))]
    allDeltaData = allDeltaData[order(allDeltaData$curve.name,
                    allDeltaData$contract.dt),]
    
    # sanity check that we can really multiply the two matrices below
    conversionOrder = allConversionData[,c("curve.name", "contract.dt")]
    pricesOrder = allDeltaData[,c("curve.name", "contract.dt")]
    rownames(conversionOrder) = NULL
    rownames(pricesOrder) = NULL
    if( !all.equal(conversionOrder, pricesOrder) ) 
    {
        error("Unable to correctly match currency exchange rates to prices.")    
    }
    
    # multiple the prices by their currency conversion factors
    priceColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
            names(allDeltaData), extended=TRUE, value=TRUE)
    allDeltaData[,priceColumns] = allDeltaData[,priceColumns] * 
            allConversionData[,priceColumns]
    
    
    deltaData = allDeltaData[, c("position", as.character(daysToKeep))]
    deltaPNLChanges = TickerUtils$getPNLChanges( deltaData, deltaData$position )
    
    return(deltaPNLChanges)
}


################################################################################
getVegaPNLChanges <- function( vegaPositions, startDate )
{
    
    # build the price matrix for the vega positions
    curveList = as.character(unique(vegaPositions$curve.name))
    marketList = lapply(strsplit(curveList, " "), function(x){return(x[2])})
    marketList = as.character(unique(unlist(marketList)))
    
    PATH_TO_VOL_FILES = "S:/All/Risk/Data/VaR/prod/Vols/"
    allVegaData = vegaPositions
    firstDayOfVols = as.Date(secdb.dateRuleApply(startDate, "-72b"))
    dateSequence = as.character( seq(firstDayOfVols, Sys.Date()-1, by="day") )
    for(day in dateSequence)
    {
        marketFile = paste(PATH_TO_VOL_FILES, "hVols.", day, ".RData", sep="")
        
        if( safeLoad(marketFile, quiet=TRUE) )
        {
            rLog("Loaded vol file:", marketFile)
            allVegaData = merge(allVegaData, hV, all.x=TRUE)
        }
    }
    
    #Figure out which days are needed for the rolling var
    priceDays = as.Date(grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
                    names(allVegaData), extended=TRUE, value=TRUE))
    
    vegaData = allVegaData[, c("position", as.character(priceDays))]
    vegaPNLChanges = TickerUtils$getPNLChanges( vegaData, vegaData$position )
    
}


################################################################################
getVaRData <- function( pnlChanges )
{
    varData = NULL
    for( columnIndex in 70:ncol(pnlChanges) )
    {
        startColumn = columnIndex - 69
        currentPNLChanges = na.omit(pnlChanges[,startColumn:columnIndex])
        varValue = TickerUtils$varFromPNLChanges( currentPNLChanges )
        dateString = names(pnlChanges)[columnIndex]
        rLog(startColumn, ":", columnIndex, "-", dateString, "=", varValue)
        rowData = data.frame( Date=dateString, VaR = varValue)
        varData = rbind(varData, rowData)
    }
    
    varData$Date = as.Date(varData$Date)
    varData$VaR = varData$VaR/1000000
    
    return(varData)
}


################################################################################
main <- function( portfolio, startDate )
{
    asOfDate = Sys.Date()-1
    
    bookList = TickerUtils$getBooksForPortfolio(portfolio, asOfDate)
    
    buildFXTable()
    
    # Pull the current deltas and yesterdays vegas   
    deltaPositions = getDeltaPositions()
    vegaPositions = getVegaPositions(asOfDate)
    
    # subset to get only the positions that are applicable
    vegaPositions = subset(vegaPositions, vegaPositions$book %in% bookList)
    deltaPositions = subset(deltaPositions, deltaPositions$book %in% bookList)
    
    deltaPNLChanges = vegaPNLChanges = NULL
    vegaVaRData = data.frame(Date=NA, VaR=NA)
    deltaVaRData = data.frame(Date=NA, VaR=NA)
    if( nrow(deltaPositions) != 0 )
    {
        deltaPNLChanges = getDeltaPNLChanges(deltaPositions)
        deltaVaRData = getVaRData(deltaPNLChanges)
    }
    
    if( nrow(vegaPositions) != 0 )
    {
        vegaPNLChanges = getVegaPNLChanges(vegaPositions, startDate)
        vegaVaRData = getVaRData(vegaPNLChanges)
    } 
    
    if( !is.null(vegaPNLChanges) && !is.null(deltaPNLChanges) )
    {
        sameNames =  intersect(names(vegaPNLChanges), names(deltaPNLChanges))
        deltaPNLChanges = deltaPNLChanges[,sameNames]
        vegaPNLChanges = vegaPNLChanges[,sameNames]
    }
    
    pnlChanges = rbind(deltaPNLChanges, vegaPNLChanges)
    totalVaRData = getVaRData(pnlChanges)
    
    minY = min(totalVaRData$VaR, vegaVaRData$VaR, deltaVaRData$VaR, na.rm=TRUE) * .9
    maxY = max(totalVaRData$VaR, vegaVaRData$VaR, deltaVaRData$VaR, na.rm=TRUE) * 1.1
    
    par(xpd=T, mar=par()$mar+c(0,0,0,4.5))
    subtitle = paste( as.character(Sys.Date()-startDate), 
            "-Day Rolling VaR with Current Positions", sep="" )
    title = paste(portfolio, subtitle, sep="\n")
    
    plot(totalVaRData, main=title, type="l", ylim=c(minY, ymax=maxY),
            xlab=paste("Date (", min(totalVaRData$Date), " to ", 
                    max(totalVaRData$Date), ")", sep=""))
    
    lines(x=vegaVaRData$Date, y=vegaVaRData$VaR, col="Dark Red")
    
    lines(x=deltaVaRData$Date, y=deltaVaRData$VaR, col="Blue")
    
    
    legend(x=max(totalVaRData$Date)+10,y=((minY+maxY)/2), 
            c("Total", "Vega", "Delta"), lwd=1,
            col = c("Black", "Dark Red", "Blue"), title="VaR")
    
    return(totalVaRData)
}


################################################################################
start = Sys.time()

portfolio = "East Power Trading Portfolio"
# max is ~330
startDate = Sys.Date()-330
varData = main(portfolio, startDate)

print(Sys.time()-start)




