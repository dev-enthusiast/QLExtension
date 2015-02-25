###############################################################################
# DeltaArchive.R
# 
# Read the archived house positions from CPSPROD database
#
library(RODBC)
library(SecDb)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Energy/VaR/Tools/SASVaR.R")

readDeltas <- function(asOfDate, con=NULL)
{
    if( is.null( con ) )
    {
        file.dsn <- "CPSPROD.dsn"
        file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"  
        connection.string <- paste("FileDSN=",  file.dsn.path, file.dsn, 
                ";UID=stratdatread;PWD=stratdatread;", sep="")  
        con <- odbcDriverConnect(connection.string)  
    }
    queryCore <- paste("SELECT PORTFOLIO, BOOK, REPORTING_PORTFOLIO, COMMOD_CURVE, MARKET_FAMILY, MARKET_SYMBOL, ",
            "MAPPED_REGION_NAME, REGION_NAME, SERVICE_TYPE, MAPPED_DELIVERY_POINT, QUANTITY_BUCKET_TYPE, ",
            "DELIVERY_MONTH, DELIVERY_YEAR, CONTRACT_DATE, DELTA_VALUE, BOOK_OWNER, FLOW_TYPE, BOOK_ACCT_TREATMENT, ",
            "DELTA_ADJUSTED_VALUE, BOOK_ACCT_TYPE, QUANTITY_UNIT, ACCOUNTING_PORTFOLIO, ",
            "ACCOUNTING_REGION, ACCOUNTING_SUBREGION, SEAT_LABEL ",
            "FROM STRATDAT.DELTA_ARCHIVE ",
            "WHERE REPORT_DATE=TO_DATE('", format(asOfDate, "%m/%d/%Y"),"','MM/DD/YYYY') ",
            "AND REPORT_TYPE='ARCHIVE' ",  sep="")
    
    data <- sqlQuery(con, queryCore) 
    
    data[, "CONTRACT_DATE"] <- as.Date(data[, "CONTRACT_DATE"])
    
    return(data)
}


saveDates <- function( dates )
{
    file.dsn <- "CPSPROD.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"  
    connection.string <- paste("FileDSN=",  file.dsn.path, file.dsn, 
            ";UID=stratdatread;PWD=stratdatread;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    OUTPUT_BASE = "S:/All/Risk/Data/VaR/prod/Positions/delta.archive."
    for( dateIndex in seq_along( dates )  )
    {
        currentDate = dates[ dateIndex ]
        cat("Pulling Date", dateIndex, "of", length(dates), ":", 
                as.character(currentDate), "\n")
        
        QQ.all = readDeltas( currentDate, con )
        
        outputFile = paste( OUTPUT_BASE, as.character(currentDate), 
                ".RData", sep="" )
        save(QQ.all, file = outputFile )
    }
    
    odbcClose( con )
}


getValidPricingDates <- function( endDate, windowSize )
{
    rLog("Getting valid pricing dates...")
    
    # if the price file is there, add it to the list
    windowCounter = 0
    pricingDates = NULL
    while( windowCounter < windowSize )
    {
        fileName = paste( "S:/All/Risk/Data/VaR/prod/Prices/hPrices.",
                endDate,".RData",sep="" )
        
        rLog("trying file:", fileName)
        if( file.exists( fileName ) ) 
        {
            # We found a valid file so increment the counter 
            windowCounter = windowCounter+1
            rLog("found file", windowCounter, "of", windowSize)
            pricingDates = c(pricingDates, as.character(endDate) )
        }
        
        endDate = endDate - 1
    }
    
    return(pricingDates)
}

getConversionTable <- function(curveList, asOfDate, exchangeRateTable=NULL)
{
    curveList = as.character(curveList)
    
    currency = lapply(curveList, secdb.getValueType, valueType="Denominated" )
    
    currency = paste("USD/", currency, sep="")
    
    currencyTable = data.frame( 
            curve.name = curveList,
            currency = currency
            )
    
    if(is.null(exchangeRateTable))
    {
        filename <- paste("S:/All/Risk/Data/VaR/dev/", 
                "allfx.2004-01-01.2007-01-01.RData", sep="")
        
        exchangeRateTable = varLoad("hFX", filename)
        exchangeRateTable$currency = rownames(exchangeRateTable)
        exchangeRateTable = exchangeRateTable[,c("currency", asOfDate)]
    }
    
    conversionTable = merge( currencyTable, exchangeRateTable, all.x=TRUE )
    
    return(conversionTable)
}

getPriceMatrix <- function( pricingDates, uniqueCurveTenors )
{
    rLog("Generating empty price matrix...")
    priceMatrix = matrix( NA, nrow=nrow(uniqueCurveTenors), 
            ncol=length(pricingDates), 
            dimnames=list(NULL, pricingDates) )
    
    filename <- paste("S:/All/Risk/Data/VaR/dev/", 
            "allfx.2004-01-01.2007-01-01.RData", sep="")
    
    exchangeRateTable = varLoad("hFX", filename)
    exchangeRateTable$currency = rownames(exchangeRateTable)
    
    for( pdIndex in seq_along( pricingDates ) )
    {
        pricingDate = pricingDates[pdIndex]
        
        # load historic prices for pricing date
        rLog("loading pricing date:", pricingDate, pdIndex, "of", length(pricingDates))
        fileName = paste( "S:/All/Risk/Data/VaR/prod/Prices/hPrices.",
                pricingDate, ".RData",sep="" )
        hP = varLoad( 'hP', fileName )
        dups = which( duplicated(hP[,c("curve.name", "contract.dt")]) )
        if(length(dups) > 0) hP = hP[-dups,]
        
        conversionTable = getConversionTable( unique(hP$curve.name), 
                pricingDate, exchangeRateTable[,c("currency", pricingDate)] )
        names(conversionTable) = c("currency", "curve.name", "fxrate")
        
        conversionTable = merge( uniqueCurveTenors, conversionTable, all.x=TRUE )
        
        hP = merge( conversionTable, hP, all.x=TRUE )
        hP[[pricingDate]] = hP[[pricingDate]] * hP$fxrate
        
        hP = hP[order(hP$curve.name, hP$contract.dt), ]
        
        rLog("Inserting values into matrix...")
        priceMatrix[,pricingDate] = hP[[pricingDate]]
    }
    
    # browser()
    
    rLog("Merging matrix with unique pairs...")
    uniqueCurveTenors = uniqueCurveTenors[do.call(order, uniqueCurveTenors),]
    allPriceData = cbind(uniqueCurveTenors, priceMatrix)
    
    return( allPriceData )
}

varByMarket <- function( positions )
{
    totalVaR = SASVaR$calculate( positions[,c("curve.name", "contract.dt", "vol.type", "value")] )
    varData = data.frame(market="Total", var=totalVaR)
    
    positions$MARKET = as.character( positions$market.family )
    positions$MARKET[ which( positions$market.family=="Power" | 
                            positions$market.family=="Capacity" |
                            positions$market.family=="Ancillary" ) ] = "Power"
    positions$MARKET[ which( positions$market.family=="NatGas" ) ] = "Gas"
    positions$MARKET[ which( positions$market.family=="Coal" ) ] = "Coal"
    positions$MARKET[ which( positions$market.family=="Emissions" | 
                            positions$market.family=="NOX" |
                            positions$market.family=="SO2" ) ] = "Emissions"
    positions$MARKET[ which( positions$market.family=="Oil" | 
                            positions$market.family=="Refined Oil bbl" |
                            positions$market.family=="Refined Oil gal" |
                            positions$market.family=="Crude Oil" ) ] = "Oil"
    positions$MARKET[ which( positions$market.family=="Freight" ) ] = "Freight"
    
    splitPositions = split( positions, positions$MARKET )
    
    for( marketPositions in splitPositions )
    {
        marketName = marketPositions$MARKET[1]
        rLog("\tRunning for market", marketName)
        #browser()
        marketData = marketPositions[,c("curve.name", "contract.dt", "vol.type", "value")]
        marketVaR = SASVaR$calculate( marketData )
        
        varData = rbind( varData, data.frame(market=marketName, var=marketVaR) )
    }
    
    return( varData )
}


#dateRange = seq( as.Date("2005-01-01"), as.Date("2006-12-01"), by="month")
#for( dateIndex in seq_along( dateRange ) )
#{
#    dateRange[dateIndex] = secdb.dateRuleApply( dateRange[dateIndex], "e+1b-1b" )
#}
#
#saveDates( dateRange )

asOfDate = Sys.getenv("asOfDate")

file = paste( "S:/All/Risk/Data/VaR/prod/Positions/delta.archive.", 
        asOfDate, ".RData", sep="" )

load(file)

#nas = (which(is.na(QQ.all$COMMOD_CURVE)))
#QQ.all$COMMOD_CURVE[nas] = QQ.all$COMMOD_CURVE[nas+1]

cat("File Loaded:", file, "\n")

date = strsplit(basename(file), "\\.")[[1]][3]

positions = QQ.all
positions = positions[, c("BOOK_ACCT_TREATMENT", "COMMOD_CURVE", "MARKET_FAMILY", 
                "CONTRACT_DATE", "DELTA_VALUE" )]
names(positions) = c("portfolio", "curve.name", "market.family", 
        "contract.dt", "value")
#positions = positions[ -which(is.na(positions$curve.name)),]
positions$curve.name = toupper(positions$curve.name)
positions$contract.dt = as.Date( positions$contract.dt)
positions$vol.type = NA

plus5years = as.Date( secdb.dateRuleApply(date, "+5y") )
positions = subset( positions, positions$contract.dt < plus5years )


pricingDates = getValidPricingDates( as.Date(date), 71 )

# Change the price data and recalculate
uniqueCurveTenors = unique( positions[, c("curve.name", "contract.dt")] )

allPriceData = getPriceMatrix( pricingDates, uniqueCurveTenors )

SASVaR$.allPriceData = allPriceData

rLog("VaR By Market All")
varByMarketAll = varByMarket( positions )
varByMarketAll$type = "All"
varByMarketAll$value = round(varByMarketAll$value)
print(varByMarketAll)


rLog("VaR By Market MTM")
mtmPositions =  positions[ which(positions$portfolio=="Mark to Market"), ]
varByMarketMTM = NULL
if( nrow(mtmPositions) > 0 )
{
    varByMarketMTM = varByMarket(mtmPositions)
    varByMarketMTM$type = "MTM"
}
varByMarketMTM$value = round(varByMarketMTM$value)
print(varByMarketMTM)


rLog("VaR By Market Acc")
accPositions = positions[ which(positions$portfolio=="Accrual"), ]
varByMarketAcc = NULL
if( nrow(accPositions) > 0 )
{
    varByMarketAcc = varByMarket( accPositions )
    varByMarketAcc$type = "Acc"
}	
varByMarketAcc$value = round(varByMarketAcc$value)
print(varByMarketAcc)

allData = rbind( varByMarketAll, varByMarketMTM, varByMarketAcc )
date = strsplit(basename(file), "\\.")[[1]][3]
allData$date = as.Date( date )
outputFile = paste("S:/All/Risk/Data/VaR/dev/var/", date, ".csv", sep="")
write.csv( allData, file=outputFile, row.names=FALSE )


