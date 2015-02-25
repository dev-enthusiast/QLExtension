###############################################################################
# StrategyReport.R
# 
# Author: e14600
#
library(reshape)
library(xlsReadWrite)
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/load.R") 
source("H:/user/R/RMG/Utilities/dollar.R") 
source("H:/user/R/RMG/Energy/PM/SASVaR.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/write.xls.R")


################################################################################
# File Namespace
#
StrategyReport = new.env(hash=TRUE)


################################################################################
StrategyReport$readInputData <- function( asOfDate )
{
    INPUT_FILE = paste( "S:/Data/UK/Coal/Reports/Derived Delta Report/", 
            format( asOfDate, "%Y%m%d" ), "_total.xls", sep="" )
    SHEET_NAME = "Reduced" 
    
    while( !file.exists(INPUT_FILE) )
    {
        rWarn(INPUT_FILE, "not found. Trying previous day...")
        asOfDate = asOfDate - 1
        INPUT_FILE = paste( "S:/Data/UK/Coal/Reports/Derived Delta Report/", 
                format( asOfDate, "%Y%m%d" ), "_total.xls", sep="" )
    }
    
    rawData = read.xls(INPUT_FILE, sheet=SHEET_NAME, from=2, 
            dateTimeAs="idodatetime", checkNames=FALSE )
    names(rawData)[1] = "contract.dt"
    names(rawData)[2:ncol(rawData)] = paste("COMMOD", 
            names(rawData)[2:ncol(rawData)], sep="")
    names(rawData)[2:ncol(rawData)] = toupper( names(rawData)[2:ncol(rawData)] )
    
    return( rawData )
}


################################################################################
StrategyReport$calculateDerivedData <- function( rawData, PAN_CAPE_RATIO, CAPE_RT4_SPREAD )
{
    allData = rawData
    
    allData[["FRTTC TOTAL"]] = ( allData[["COMMOD FRC PM TC PHYSICAL"]] + 
                allData[["COMMOD FRC AVGTC BPI"]] ) / PAN_CAPE_RATIO + 
            ( allData[["COMMOD FRC CAPEAVETC PHYSICAL"]] + 
                allData[["COMMOD FRC AVGTC BCI"]] ) * CAPE_RT4_SPREAD +
            allData[["COMMOD FRT ROUTE4 BCI"]] + 
            allData[["COMMOD FRT ROUTE7 BCI"]]
    
    allData[["API2 IMPLIED"]] = -1 * allData[["FRTTC TOTAL"]]
    
    allData[["API4 IMPLIED"]] = allData[["FRTTC TOTAL"]]
    
    allData[["RESIDUAL API2"]] = ( allData[["COMMOD FCO API2 INDEX"]] - 
                allData[["API2 IMPLIED"]] )
    
    allData[["RESIDUAL API4"]] = allData[["COMMOD FCO API4 INDEX"]] - 
            allData[["FRTTC TOTAL"]] + allData[["COMMOD FCO NEWCASTLE GCI"]]
    
    allData[["NEG NEWCASTLE"]] = -1 * allData[,"COMMOD FCO NEWCASTLE GCI"]
    
    return( allData )
}

################################################################################
StrategyReport$outputFilePath <- function( asOfDate )
{
    outputDir = paste("S:/All/Risk/Reports/VaR/prod/", asOfDate, 
            "/UK Coal Reports", sep="")
    if( !file.exists(outputDir) )
    {
        dir.create(outputDir)
    }
    outputFile = file.path( outputDir, "CVaRByStrategy.xls")
    
    return( outputFile )
}


################################################################################
# STRATEGY1_CURVES = c( FRT ROUTE4 BCI, FRT ROUTE7 BCI, FRC AVGTC BPI, 
#	FRC PM TC PHYSICAL, FRC CAPEAVETC PHYSICAL, FRC AVGTC BCI, 
#	API2 IMPLIED, API4 IMPLIED )
#
StrategyReport$saveImpliedFreightStrategy <- function( allData, outputFile )
{
    strategy1 = allData[, c( "contract.dt", "COMMOD FRT ROUTE4 BCI", "COMMOD FRT ROUTE7 BCI", 
                    "COMMOD FRC AVGTC BPI", "COMMOD FRC PM TC PHYSICAL", 
                    "COMMOD FRC CAPEAVETC PHYSICAL", "COMMOD FRC AVGTC BCI", 
                    "API2 IMPLIED", "API4 IMPLIED" )]
    names(strategy1) = c( "contract.dt", "COMMOD FRT ROUTE4 BCI", "COMMOD FRT ROUTE7 BCI", 
            "COMMOD FRC AVGTC BPI", "COMMOD FRC PM TC PHYSICAL", 
            "COMMOD FRC CAPEAVETC PHYSICAL", "COMMOD FRC AVGTC BCI", 
            "COMMOD FCO API2 WEEKLY", "COMMOD FCO API4 WEEKLY" )     
    
    write.xls( strategy1, sheetname="Strategy1", file=outputFile )
    
    return( strategy1 )
}


################################################################################
# STRATEGY2_CURVES = c( FCO NEWCASTLE GCI, 
#	-FCO NEWCASTLE GCI @ FCO API4 INDEX PRICES)
#
StrategyReport$saveAltPacSpreadStrategy <- function( allData, outputFile )
{
    strategy2 = allData[, c( "contract.dt", "COMMOD FCO NEWCASTLE GCI", "NEG NEWCASTLE" ) ]
    names(strategy2) = c( "contract.dt", "COMMOD FCO NEWCASTLE GCI", "COMMOD FCO API4 WEEKLY" )
    
    write.xls( strategy2, sheetname="Strategy2", file=outputFile )
    
    return( strategy2 )
}


################################################################################
# STRATEGY3_CURVES = RESIDUAL API2 @ FCO API2 INDEX PRICES
#
StrategyReport$saveResidualAPI2Strategy <- function( allData, outputFile )
{
    strategy3 = allData[, c( "contract.dt", "RESIDUAL API2")]
    names(strategy3) = c( "contract.dt", "COMMOD FCO API2 WEEKLY" )
    
    write.xls( strategy3, sheetname="Strategy3", file=outputFile )
    
    return( strategy3 )
}


################################################################################
# STRATEGY4_CURVES = RESIDUAL API4 @ FCO API4 INDEX PRICES
#
StrategyReport$saveResidualAPI4Strategy <- function( allData, outputFile )
{
    strategy4 = allData[, c( "contract.dt","RESIDUAL API4")]
    names(strategy4) = c( "contract.dt", "COMMOD FCO API4 WEEKLY" )
    
    write.xls( strategy4, sheetname="Strategy4", file=outputFile )
    
    return( strategy4 )
}


################################################################################
StrategyReport$createFlatPositionSet <- function( strategyList, nameList )
{
    flatResults = NULL
    for( strategyIndex in seq_along( strategyList ) )
    {
        strategy = strategyList[[strategyIndex]]
        flatStrategy = melt( strategy, 
                measure=names(strategy)[-1] )
        
        flatStrategy$strategy = nameList[[strategyIndex]]
        
        flatResults[[strategyIndex]] = flatStrategy
    }
    
    finalData = do.call( rbind, flatResults )
    finalData = na.omit( finalData )
    
    names(finalData)[which(names(finalData)=="variable")] = "curve.name"
    finalData$vol.type = NA
    finalData$value = as.integer(finalData$value)
    finalData$curve.name = as.character(finalData$curve.name)
    finalData$contract.dt = as.Date(finalData$contract.dt)
    finalData = finalData[ , c("strategy", "curve.name", "contract.dt", "vol.type", "value")]
    
    positions = subset(finalData, 
            finalData$contract.dt >= as.Date(format(Sys.Date(), "%Y-%m-01")) )
    
    return( positions )
}


################################################################################
StrategyReport$varByStrategy <- function( positions )
{
    splitPositions = split(positions, positions$strategy)
    varResults = lapply( splitPositions, function(x)
            {
                x$strategy = NULL
                var = SASVaR$calculate(x)
                return(var)
            })
    
    finalResults = data.frame( Strategy=names(varResults), VaR=unlist(varResults) )
    rownames(finalResults) = NULL
    
    totalVaR = SASVaR$calculate(positions[,-1])
    totalRow = data.frame(Strategy="Total", VaR=totalVaR)
    finalResults = rbind( finalResults, totalRow )
    
    return( finalResults )
}


################################################################################
StrategyReport$cvarByStrategy <- function( positions )
{
    uniquePositions = positions
    uniquePositions$strategy = NULL
    uniquePositions = cast( uniquePositions, curve.name+contract.dt+vol.type~., sum)
    names(uniquePositions) = c("curve.name", "contract.dt", "vol.type", "value")
    mvar = SASVaR$calculateMVaR( uniquePositions )
    
    cvar = merge(positions, mvar, sort=FALSE)
    cvar$cvar = cvar$value * cvar$marginalVaR
    cvar = cvar[, c("strategy", "cvar")]
    names(cvar) = c("strategy", "value")
    aggregateCVaR = cast( cvar, strategy~., sum )
    
    names( aggregateCVaR ) = c( "Strategy", "CVaR" )
    
    totalCVaR = sum(aggregateCVaR$CVaR)
    totalRow = data.frame(Strategy="Total", CVaR=totalCVaR)
    finalResults = rbind( aggregateCVaR, totalRow )
    
    return( finalResults )
}


################################################################################
StrategyReport$positionsByStrategy <- function( positions )
{
    positionsForTable = positions
    positionsForTable$curve.name = NULL
    positionsForTable$contract.dt = NULL
    positionsForTable$vol.type = NULL
    aggregatePositions = cast( positionsForTable, strategy~., sum )
    names(aggregatePositions) = c("Strategy", "Position")
    totalPos = data.frame( Strategy="Total", 
            Position=sum(aggregatePositions$Position) )
    aggregatePositions = rbind( aggregatePositions, totalPos )
    
    return( aggregatePositions )
}


################################################################################
StrategyReport$run <- function()
{
    asOfDate = overnightUtils.getAsOfDate()
    
    rawData = StrategyReport$readInputData( asOfDate )
    
    allData = StrategyReport$calculateDerivedData( rawData, 
            PAN_CAPE_RATIO = 2.16, 
            CAPE_RT4_SPREAD = 4002 )
    
    outputFile = StrategyReport$outputFilePath( asOfDate )
    
    strategy4 = StrategyReport$saveResidualAPI4Strategy( allData, outputFile )
    strategy3 = StrategyReport$saveResidualAPI2Strategy( allData, outputFile )
    strategy2 = StrategyReport$saveAltPacSpreadStrategy( allData, outputFile )
    strategy1 = StrategyReport$saveImpliedFreightStrategy( allData, outputFile )

    strategyList = list( strategy1, strategy2, strategy3, strategy4 )
    nameList = list( "1 - Implied Freight", "2 - Alt-Pac Spread", 
            "3 - Residual API2", "4 - Residual API4" )
    positions = StrategyReport$createFlatPositionSet( strategyList, nameList )
    
    varByStrategy = StrategyReport$varByStrategy( positions )
    cvarByStrategy = StrategyReport$cvarByStrategy( positions )
    positionsByStrategy = StrategyReport$positionsByStrategy( positions )
    
    finalResults = merge( cvarByStrategy, varByStrategy )
    finalResults = merge( positionsByStrategy, finalResults)
    
    finalResults$VaR = dollar(finalResults$VaR)
    finalResults$CVaR = dollar(finalResults$CVaR)
    finalResults$Position = dollar(finalResults$Position, do.dollar=FALSE)
    
    write.xls( finalResults, sheetname="Results", file=outputFile )
    
    invisible( finalResults )
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
StrategyReport$run()
print( Sys.time() - .start )



