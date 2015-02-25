###############################################################################
# Cleaned up version of the delta gamma report code, using A-Prime.
#
# Depends on the procmon job: sas/prod/pmAprimedata, usually out by 8:30am
# 
# Author:	J. Scillieri
# Added:	test.R Initiated Mar 17, 2009
# 
library( RODBC )
library( reshape )
source( "H:/user/R/RMG/Projects/CollateralAdHoc/cashFlowChange.R" )
source( "H:/user/R/RMG/Projects/CollateralAdHoc/APrimeHotness/WriteExcelSummary.R" )
source( "H:/user/R/RMG/Utilities/RLogger.R" )
source( "H:/user/R/RMG/Utilities/load.R" )


################################################################################
# Data cache - stores the Raft data so you can rerun without
# reloading the whole file. See the getExposureData function for details.
#
cache = new.env(hash=TRUE)


################################################################################
## Try the procmon date value.
## If that fails, check the R asOfDate. 
## If that fails, use yesterday.
##
getAsOfDate <- function()
{
    runDate = Sys.getenv("PROCMON_RUNDATE")
    asOfDate = Sys.getenv("asOfDate")
    
    if( runDate != "" )
    {
        asOfDate = as.Date( runDate, format="%Y%m%d" )
        
    } else if( asOfDate != "" )
    {
        asOfDate = as.Date( asOfDate )
        
    } else
    { 
        asOfDate = Sys.Date()-1
    }
    
    return( asOfDate )
}


################################################################################
## Query the A-Prime tables to get the base marginable data set
## 
getMarginableData <- function( asOfDate )
{
    if( !is.null( cache$marginableData ) )
    {
        rLog( "Returning marginable data from cache..." )
        return( cache$marginableData )
    }
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            as.character( asOfDate ), "/Aprime/marginableData.RData", sep="" )
    if( file.exists( inputFile ) )
    {
        rLog( "Loading marginable data from file..." )
        marginableData = varLoad("marginableData", inputFile )
        cache$marginableData = marginableData
        return( cache$marginableData )
    }
    
    dsnFile = "RMGDBP.dsn"
    dsnPath = "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", dsnPath, dsnFile, 
            ";UID=RMGDBUSER;PWD=RMGDBPWD_PROD;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    marginableDataQuery = paste( 
            "SELECT upper( credit_nettingagreement ) credit_nettingagreement,
                    upper( credit_csa ) credit_csa                          ,
                    upper( marginal ) marginal                              ,
                    upper( commod_curve ) commod_curve                      ,
                    upper( commodity_grp ) commodity_grp                    ,
                    upper( sttype ) BUCKET                                  ,
                    contractmonth                                           ,
                    contractyear                                            ,
                    pos_adj
                    FROM rmgsas.aprime_base
                    WHERE asofdate       = '", format( asOfDate, "%d%b%y"), "'
                    AND upper( marginal ) <> 'UNMARGIN'
                    AND ( risk_reporting_flag = 'Y' or 
                    upper( cer_group_2 ) in ('DISC OPS DOWNSTREAM', 
                    'DISC OPS UK', 'DISC OPS UK MIRROR') ) 
                    AND pos_adj is not null", sep="")
    
    rLog( "Pulling all marginable data..." )
    marginableData = sqlQuery( con, marginableDataQuery )
    odbcClose( con )
    
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( marginableData, file=inputFile )
    cache$marginableData = marginableData
    
    return( cache$marginableData )
}



################################################################################
## Query the A-Prime tables to get the aggregated credit data
## 
getCreditData <- function( asOfDate )
{
    if( !is.null( cache$creditData ) )
    {
        rLog( "Returning aggregated credit data from cache..." )
        return( cache$creditData )
    }
    
    inputFile = paste("S:/All/Risk/Collateral Scenarios/Data/", 
            as.character( asOfDate ), "/Aprime/creditData.RData", sep="" )
    if( file.exists( inputFile ) )
    {
        rLog( "Loading aggregated credit data from file..." )
        creditData = varLoad("creditData", inputFile )
        cache$creditData = creditData
        return( cache$creditData )
    }
    
    rLog("Getting raft excluded books...")
    excludedRaftBooks = .getRaftExcludedBookList()
    
    dsnFile = "RMGDBP.dsn"
    dsnPath = "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", dsnPath, dsnFile, 
            ";UID=RMGDBUSER;PWD=RMGDBPWD_PROD;", sep="")  
    con <- odbcDriverConnect(connection.string)  
    
    creditDataQuery = paste( 
            " SELECT upper( credit_nettingagreement ) credit_nettingagreement,
                    upper( credit_csa ) credit_csa                          ,
                    upper( marginal ) marginal                              ,
                    agm_cpth                                                ,
                    agm_ourth                                               ,
                    SUM( cashheld ) cashheld                                ,
                    SUM( cashpost) cashpost                                 ,
                    SUM( lcheld ) lcheld                                    ,
                    SUM( lcpost ) lcpost                                    ,
                    SUM( exposure ) exposure
                    FROM rmgsas.aprime_base
                    WHERE asofdate       = '", format( asOfDate, "%d%b%y"), "'
					and book_name not in ('", 
                    paste( excludedRaftBooks, collapse="','"), "') 
                    GROUP BY 
                    credit_nettingagreement,
                    credit_csa             ,
                    marginal               ,
                    agm_cpth               ,
                    agm_ourth
                    ORDER BY
                    credit_nettingagreement,
                    credit_csa             ,
                    marginal               ,
                    agm_cpth               ,
                    agm_ourth", sep="")
    
    rLog( "Pulling aggregated credit data..." )
    creditData = sqlQuery( con, creditDataQuery )
    odbcClose( con )
    
    dir.create( dirname( inputFile ), showWarnings=FALSE )
    save( creditData, file=inputFile )
    cache$creditData = creditData
    
    return( cache$creditData )
}


###############################################################################
.getRaftExcludedBookList <- function( con )
{
    file.dsn <- "RaftEnergyCreditManual.dsn"
    file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"
    connection.string <- paste("FileDSN=", file.dsn.path, file.dsn, 
            ";UID=ExecSPOnly;PWD=EnergyCredit_ExecSPOnly;", sep="")  
    
    con <- odbcDriverConnect(connection.string)  
    
    query = "select	* from EnergyCreditStaging.dbo.PNL_AccountingMethod 
            where UPPER(accounting_method) = UPPER('modeling')"
    
    excludedBooks = sqlQuery( con, query )$BOOK
    odbcClose(con)
    
    return( as.character( excludedBooks ) ) 
}


################################################################################
## This function calculates the stress adjustment for the entire set of 
## position data. It takes detailed position data and two vectors of 
## commodities and their appropriate stress amounts. 
## It returns a vector of stress adjustments equal in length to the original
## data set.
##
calculateStressAdjustment <- function( data, commodities, amounts )
{
    data$STRESS_ADJUSTMENT = 0 
    
    if( length( commodities ) != length( amounts ) )
    {
        stop("You don't have a 1:1 mapping of commodity/amounts")
    }
    
    for( index in seq_along( commodities ) )
    {
        commodity = commodities[index]
        amount = amounts[index]
        
        rowsToInclude = which( data$COMMODITY_GRP == commodity ) 
        data$STRESS_ADJUSTMENT[rowsToInclude] = data$POS_ADJ[rowsToInclude] * amount  
    }
    
    return( data$STRESS_ADJUSTMENT )
}


################################################################################
## This function takes a position data set, stresses it and calculates both 
## the adjusted exposure and the cashflow change that results from that 
## adjusted exposure. 
##
## It returns an aggregated data set at the 
## netting agreement/csa level. 
## 
stressCommodity <- function( originalData, aggregateCreditData, commodities, amounts )
{
    originalData$value = calculateStressAdjustment( originalData, 
            commodities, amounts )
    
    # aggregate up to the credit data level
    aggregateStressedData = cast( originalData, 
            CREDIT_NETTINGAGREEMENT + CREDIT_CSA + MARGINAL ~ ., sum )
    aggregateStressedData = colRename( aggregateStressedData, "(all)", "STRESS_ADJUSTMENT" )

    # merge the two together
    finalData = merge( aggregateCreditData, aggregateStressedData )

    if( nrow( finalData ) != nrow( aggregateStressedData ) )
    {
        stop( "You have a bug in your merge, FIX IT!")
    }
    
    
    # then calculate adjusted exposure & cashflow change
    finalData$ADJUSTED_EXPOSURE = finalData$EXPOSURE + finalData$STRESS_ADJUSTMENT
    
    finalData$CASHFLOW_CHANGE = cashFlowChange( 
            startExposure = finalData$EXPOSURE, 
            endExposure = finalData$ADJUSTED_EXPOSURE, 
            cashPosted = finalData$CASHPOST, 
            lc_posted = finalData$LCPOST, 
            cegThreshold = finalData$AGM_OURTH, 
            cashHeld = finalData$CASHHELD, 
            lc_held = finalData$LCHELD, 
            cptyThreshold = finalData$AGM_CPTH )
    
    return( finalData )
}


################################################################################
## Given a set of stress parameters, return the subset of positions from the
## allData set that are margined as a result of that stress.
## 
getMarginedPositionsFromStress <- function( allData, creditData, commodities, amounts )
{
    stressedData = stressCommodity( allData, creditData, commodities, amounts )
    
    marginedRows = which( stressedData$CASHFLOW_CHANGE != 0 )
    
    marginedList = stressedData[ marginedRows, 
            c("CREDIT_NETTINGAGREEMENT", "CREDIT_CSA") ]

    if( nrow(marginedList) != nrow( unique( marginedList ) ) )
    {
        stop( "You have a bug in your stressedData, FIX IT!")
    }
    
    positionData = merge( allData, marginedList )
    
    return( positionData )
}


################################################################################
## Convenience function used to return a commodity's total position given a set 
## of filtering criteria.
## 
getCommodityPositions <- function( dataSet, commodity, 
        tenor = NULL, year = NULL, bucket = "EVEN")
{
    filtered = subset( dataSet, COMMODITY_GRP == commodity & BUCKET == bucket)
    
    if( !is.null( tenor ) )
    {
        tenor = as.Date( tenor )
        filtered = subset( filtered, as.Date(CONTRACTMONTH) == tenor )
    }
    
    if( !is.null( year ) )
    {
        year = as.integer( year )
        filtered = subset( filtered, CONTRACTYEAR == year )
    }
    
    total = sum( filtered$POS_ADJ )
    
    return( total )
}



################################################################################
## Iterate through all the stress scenarios provided and store the results 
## in a list
## 
getScenarioResults <- function( marginableData, creditData, scenarioData )
{
    scenarioNames = unique( scenarioData$NAME )
    
    resultList = list()
    for( scenario in scenarioNames )
    {
        rLog( "Running scenario:", scenario )
        
        scenarioParameters = subset( scenarioData, NAME == scenario )
        commodities = scenarioParameters$COMMODITY_GRP
        amounts = scenarioParameters$STRESS_VALUE
        filter = unique( scenarioParameters$FILTER )[1]
        filterValue = unique( scenarioParameters$FILTER_VALUE )[1]
        
        if( filter != "" )
        {
            dataToUse = subset( marginableData, marginableData[[filter]] == filterValue )
            
        } else
        {
            dataToUse = marginableData
        }
        
        cashflowChangeData = stressCommodity( dataToUse, creditData, 
                commodities, amounts )
        saveScenarioFile( cashflowChangeData, dataToUse, scenario )
        resultList[[scenario]] = sum( cashflowChangeData$CASHFLOW_CHANGE )
    }
    
    return( resultList )
}


################################################################################
## Save off the stress file for reconcilation purposes
## 
saveScenarioFile <- function( cashflowChangeData, positionData, scenario )
{
    positionData$LABEL = paste( positionData$COMMODITY_GRP, "_", 
            positionData$BUCKET, sep="" )
    positionData = colRename( positionData, "POS_ADJ", "value" )
    commodityData = cast( positionData, CREDIT_NETTINGAGREEMENT + 
                    CREDIT_CSA ~ LABEL, sum, na.rm=TRUE, fill=0 )
    
    dataToWrite = merge( cashflowChangeData, commodityData )
    if( nrow(dataToWrite) != nrow( cashflowChangeData ) ) 
    {
        stop( "You have a bug in your merge in the scenario file, FIX IT!")
    }
    
    basePath = paste("S:/All/Risk/Collateral Scenarios/Reports/", 
            as.character(getAsOfDate()), "/aprime/", sep="")
    dir.create( basePath, showWarnings = FALSE )
    
    write.csv( dataToWrite, row.names = FALSE, 
            file = paste( basePath, scenario, ".csv", sep="") )
}


################################################################################
## Data that is used in the template file:
##     marginableData
##     marginedData
##     downEightSigmaData
##     scenarioData
##     resultList
##
main <- function()
{
    start = Sys.time()
    asOfDate = getAsOfDate()
    rLog( "Running with asOfDate:", as.character( asOfDate ) )
    
    marginableData <<- getMarginableData( asOfDate )
    
    creditData = getCreditData( asOfDate )
    
    scenarioData <<- read.csv("H:/user/R/RMG/Projects/CollateralAdHoc/APrimeHotness/ScenarioList.csv")
    
    rLog( "Creating margined and down-8s data sets..." )
    marginedData <<- getMarginedPositionsFromStress( marginableData, creditData, 
            commodities = c('GAS', 'POWER', 'INTERNATIONAL COAL', 'DOMESTIC COAL'), 
            amounts = c(
                    subset( scenarioData, COMMODITY_GRP=="GAS" & NAME=="Epsilon" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="POWER" & NAME=="Epsilon" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="INTERNATIONAL COAL" & NAME=="Epsilon" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="DOMESTIC COAL" & NAME=="Epsilon" )$STRESS_VALUE ) )
    
    downEightSigmaData <<- getMarginedPositionsFromStress( marginableData, creditData,
            commodities = c('GAS', 'POWER', 'INTERNATIONAL COAL', 'DOMESTIC COAL'), 
            amounts = c(
                    subset( scenarioData, COMMODITY_GRP=="GAS" & NAME=="Dn8s" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="POWER" & NAME=="Dn8s" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="INTERNATIONAL COAL" & NAME=="Dn8s" )$STRESS_VALUE,
                    subset( scenarioData, COMMODITY_GRP=="DOMESTIC COAL" & NAME=="Dn8s" )$STRESS_VALUE ) )
    
    resultList <<- getScenarioResults( marginableData, creditData, scenarioData )
    
    rLog( "Generating report..." )
    writeExcelSummary()  
    
    print( Sys.time() - start )
    rLog( "Done." )
}


################################################################################
main()
