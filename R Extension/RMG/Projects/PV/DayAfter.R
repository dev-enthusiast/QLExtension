###############################################################################
# DayAfter.R
# Author: John Scillieri
# 
# Run this on the "DayAfter" test day. It will generate the option price file,
# top vegas file, coverage report, and valid exception report. 
#
# ---> IMPORTANT: Only modify the configuration parameters below!  <-------
# 

################################################################################
# CONFIGURATION PARAMETERS
#
START_OF_TEST_MONTH = "2009-03-01"


################################################################################
library(RODBC)
library(SecDb)
library(reshape)
library(xlsReadWrite)
source("H:/user/R/RMG/Projects/PV/PVUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/write.xls.R")


################################################################################
# File Namespace
DayAfter = new.env(hash=TRUE)


################################################################################
# 
DayAfter$main <- function( )
{
    # Should be the TEST DATE
    asOfDate = as.Date("2009-03-31")

    
    testDir = PVUtils$getTestDir(START_OF_TEST_MONTH)
    
    
    # Copy in and fix the Vega By Strike Report for use below.
    DayAfter$CopyVBSFile( asOfDate, testDir )
    
    
    # Run the System Price function 1 more time to get the EOD vol marks
    PVUtils$SecDbSystemPrices( asOfDate, testDir )
    
    
    # Extract & write out exceptions
    PVUtils$GetExceptions( asOfDate, testDir )
    PVUtils$WriteExceptions( asOfDate, testDir )
    
    
    # Get the top 150 vegas
    # Make sure the VBS Report and rmsys_vegas are both in the test dir!
    PVUtils$SecDbTopVegas( asOfDate, testDir )
    
    
    # Run the coverage report
    PVUtils$SecDbCoverageReport( asOfDate, testDir )
    PVUtils$WriteCoverageData( asOfDate, testDir )
}


################################################################################
DayAfter$CopyVBSFile <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    
    VBS_FILE = paste(testDir, "Vega By Strike ", dateFormat, ".xls", sep="")
    
    if( !file.exists(VBS_FILE) )
    {
        rLog("Vega By Strike Report not found. Copying from network drive...")
        VBS_NETWORK_FILE = paste("S:/All/Risk/Vega by Strike/Vega By Strike ", 
                dateFormat, ".xls", sep="")
        if( !file.exists(VBS_NETWORK_FILE) )
        {
            stop("Unable to find network VBS file: ", VBS_NETWORK_FILE, 
                    ". Exiting.")
        }
        file.copy(VBS_NETWORK_FILE, VBS_FILE)
        rLog("File copied from:", VBS_NETWORK_FILE, "to", VBS_FILE)
    }
    
}


################################################################################
DayAfter$FixVBSFile <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    
    VBS_FILE = paste(testDir, "Vega By Strike ", dateFormat, ".xls", sep="")
    
    .fixSheet( VBS_FILE, "Monthly Data")
    .fixSheet( VBS_FILE, "Daily Data")
    
}


################################################################################
.fixSheet <- function( file, sheetName )
{
    colClasses = c(
            "character", "character", "isodate",   "character", 
            "character", "character", "character", "character", 
            "character", "character", "character", "character", 
            "character", "character", "character", "character" )
    # Do a raw read first to get the column count
    data = read.xls(file, sheet=sheetName)
    
    if( ncol(data) != 14 )
    {
        rLog("Misaligned data... fixing sheet:", sheetName)
        data = read.xls(file, sheet=sheetName, colClasses=colClasses)
        
        if( all(data$V15 == " ") )
        {
            rLog("Sheet has already been fixed. Exiting.")
            return(NULL)
        }
        
        badRows = which(data$V15 != "")
        dataToShift = data[ badRows, (ncol(data)-4):ncol(data) ]
        data[ badRows, (ncol(data)-6):(ncol(data)-2) ] = dataToShift
        
        # Do this to clear out the bad rows when we write it back
        data$V15 = data$V16 = " "
        
        data$Vega = as.numeric(data$Vega)
        data$Term = format(as.Date(data$Term), "%b %y")
        write.xls(data, file, "Daily Data")
    }    
}


################################################################################
DayAfter$NewTopVegas <- function( asOfDate, testDir )
{
    library(RODBC)
    library(reshape)
    con = odbcConnect("VCTRLP", "vcentral_read", "vcentral_read")
    query = "select MARKET_FAMILY, MARKET_SYMBOL, CONTRACT_MONTH, FLOW_TYPE, 
            PROFIT_CENTER_1, ECONOMIC_OWNER, CCG_ORGANIZATION, 
            MAPPED_REGION_NAME, DELIVERY_POINT, 
            MAPPED_DELIVERY_POINT, VEGA 
            from VCV.APP_POSITIONS_AGGREGATE
            where VALUATION_DATE='03Sep08' and VEGA !=0 AND
			ACCOUNTING_PORTFOLIO = 'MARK TO MARKET'"
    
    vegaData = sqlQuery(con, query)
    names(vegaData) = c("market", "product", "contract.dt", "qbt", "book", 
            "owner", "org", "region", "subregion", "point", "value"  )
    odbcClose(con)
    
    mtmVegas = cast( vegaData, 
            market + product + contract.dt + qbt + book + owner + org + 
                    region + subregion + point ~ . , fun.aggregate=sum )
    names(mtmVegas)[11] = "vega"
    
    mtmVegas = mtmVegas[order(abs(mtmVegas$vega), decreasing=TRUE),]
    mtmVegas$vega = mtmVegas$vega / 1000
    top150 = mtmVegas[1:150,]
    rownames(top150) = NULL
    
    dateFormat = format(asOfDate, "%d%b%y")
    outputFile = paste( testDir, "NewTop150Vegas_", 
            dateFormat, ".csv", sep="" )
    write.csv(top150, file=outputFile, row.names=FALSE)
}


################################################################################
DayAfter$WriteTopVegaFiles <- function( asOfDate, testDir, numVegas=150 )
{
    
    # Get the previous month's top 150
    oneMonthAgo = as.Date(secdb.dateRuleApply(asOfDate, "-1m"))
    lastMonthStart = format( oneMonthAgo, "%Y-%m-01" )
    previousAsOfDate = PVUtils$getAsOfDate(lastMonthStart)
    previousTestDir = PVUtils$getTestDir(lastMonthStart)
    
    previousTopVegas = DayAfter$createTopVegas( previousAsOfDate, 
            previousTestDir )
    
    # Get the top 150 vegas for this month 
    topVegas = DayAfter$createTopVegas( asOfDate, testDir )
    
    # Generate the appropriate tables for this month and last
    sumOfVegas = getSumOfVegaData(topVegas, numVegas)
    prevSumOfVegas = getSumOfVegaData(previousTopVegas, numVegas)
    
    countOfVegas = getCountOfVegaData(topVegas, numVegas)
    prevCountOfVegas = getCountOfVegaData(previousTopVegas, numVegas)
    
    # Generate the final tables to output
    finalSumTable = .createFinalTable(sumOfVegas, prevSumOfVegas)
    finalCountTable = .createFinalTable(countOfVegas, prevCountOfVegas)
    
    # Write out the tables, replacing NAs with 0s
    dateFormat = format(asOfDate, "%d%b%y")
    
    countOutputFile = paste(testDir, "TopVegaCounts_", 
            dateFormat, ".csv", sep="")
    write.csv(finalCountTable, file=countOutputFile, row.names=FALSE, na="0")
    
    sumOutputFile = paste(testDir, "TopVegaSums_", 
            dateFormat, ".csv", sep="")
    write.csv(finalSumTable, file=sumOutputFile, row.names=FALSE, na="0")
    
    # Write out the top N vegas table
    topVegasFile = paste( testDir, "TopVegas_", dateFormat, ".csv", sep="")
    write.csv( topVegas[1:numVegas,], file=topVegasFile )
    
    
    invisible(NULL)
}


.createFinalTable <- function( thisMonthData, lastMonthData )
{
    # Remove these columns, we're only interested in the totals
    lastMonthData$Daily = lastMonthData$Monthly = NULL
    
    # Merge, placing NAs into points where we don't have totals
    finalTable = merge( thisMonthData, lastMonthData, 
            by=c("Market", "DeliveryPoint"), all.x=TRUE )
    names(finalTable) = c("Market", "Delivery Point", "Daily", "Monthly",
            "Total", "Previous")
    
    return(finalTable) 
}


################################################################################
DayAfter$createTopVegas <- function( asOfDate, testDir, count=150 )
{
    
    vbsData = readVBSFile( asOfDate, testDir )
    rmsysData = readRmSysDataFile( asOfDate, testDir )
    
    joinedData = rbind(vbsData, rmsysData)
    
    #Cast the data and sort it to discover the top vegas
    names(joinedData) = c("Type", "Market", "Term", "QBT", "Region", 
            "Delivery.Point", "value")
    topData = cast(joinedData, 
            Type+Market+Term+QBT+Region+Delivery.Point ~ . , sum)
    names(topData) = c("D/M", "Market", "Term", "QBT", "Region", 
            "Delivery Point", "Vega")
    topData = topData[order(abs(topData$Vega), decreasing=TRUE),]
    rownames(topData) = NULL
    
    return(topData)
}


readVBSFile <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    
    VBS_FILE = paste(testDir, "Vega By Strike ", dateFormat, ".xls", sep="")
    
    # Read the vega data from the VBS file first
    con = odbcConnectExcel(VBS_FILE)
    positiveData = sqlFetch(con, "Positive Vegas")
    negativeData = sqlFetch(con, "Negative Vegas")
    odbcClose(con)
    
    positiveData = positiveData[,c("D/M", "Market", "Term", "QBT", "Region", 
                    "Delivery Point", "Vega")]
    negativeData = negativeData[,c("D/M", "Market", "Term", "QBT", "Region", 
                    "Delivery Point", "Vega")]
    vbsData = rbind(positiveData, negativeData)
    
    return(vbsData)
}


readRmSysDataFile <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    
    RMSYS_FILE = paste(testDir, "Vega_rmSys_", dateFormat, ".xls", sep="")
    
    if( !file.exists(RMSYS_FILE) )
    {
        stop("Unable to find RmSys Vega File: ", RMSYS_FILE, ". Exiting.")
    }
    
    #Read in the rmsys data & reformat it to match the vbs file
    rLog("Opening RmSys Vega File:", RMSYS_FILE)
    con = odbcConnectExcel(RMSYS_FILE)
    sheetName = paste("Vega_rmSYS_", dateFormat, sep="")
    rmsysData = sqlFetch(con, sheetName)
    odbcClose(con)
    
    rmsysData$Market = "NG"
    rmsysData$QBT = NA
    rmsysData$Type = gsub("P", "Put", gsub("C", "Call", rmsysData$CallPut))
    rmsysData$CallPut = NULL
    rmsysData$"D/M" = rmsysData$Class
    rmsysData$"D/M"[which(rmsysData$"D/M" == "Blended")] = "Daily"
    rmsysData$Class = NULL
    rmsysData$Term = rmsysData$Month
    rmsysData$Month = NULL    
    rmsysData$Portfolio = NULL
    rmsysData$Book = NULL
    rmsysData$SecDBBook = NULL
    rmsysData$Strike = NULL
    rmsysData$Theta = NULL
    rmsysData$"Delivery Point" = unlist( lapply( 
                    strsplit(as.character(rmsysData$VolCurve), " "), 
                    function(x){x[1]} ) )
    rmsysData$Region = "RmSYS"
    nymexRows = which(rmsysData$"Delivery Point"=="NYMEX")
    rmsysData$Region[nymexRows] = "NYMEX"
    rmsysData$"Delivery Point"[nymexRows] = "EXCHANGE"
    nonExchangeRows = which(rmsysData$"Delivery Point" != "EXCHANGE")
    for( rowIndex in nonExchangeRows )
    {
        rmsysData$Region[rowIndex] = 
                getRmSysRegion(rmsysData$"Delivery Point"[rowIndex])
    }
    rmsysData$VolCurve = NULL
    rmsysData = rmsysData[, c("D/M", "Market", "Term", "QBT", "Region", 
                    "Delivery Point","Vega")]
    #RmSys convention is $/100 bps in Vol
    rmsysData$Vega = rmsysData$Vega / 1000
    
    return( rmsysData )
}

getSumOfVegaData <- function( topVegas, n=150 )
{
    topVegas = topVegas[1:n,]
    
    sumOfVegas = aggregate( topVegas$Vega, 
            list(topVegas$Market, topVegas$"Delivery Point", topVegas$"D/M"), 
            sum )
    sumOfVegas = sumOfVegas[do.call(order, sumOfVegas),]
    names(sumOfVegas) = c("Market", "DeliveryPoint", "DM", "Sum")
    sumTable = cast(
            melt(sumOfVegas, id.var=c("Market", "DeliveryPoint", "DM")), 
            Market + DeliveryPoint ~ DM, fill=0 )
    sumTable$Total = sumTable$Daily + sumTable$Monthly
    
    return(sumTable)
}


getCountOfVegaData <- function( topVegas, n=150 )
{
    topVegas = topVegas[1:n,]
    
    countOfVegas = aggregate( topVegas$Vega, 
            list(topVegas$Market, topVegas$"Delivery Point", topVegas$"D/M"), 
            length )
    countOfVegas = countOfVegas[do.call(order, countOfVegas),]
    names(countOfVegas) = c("Market", "DeliveryPoint", "DM", "Count")
    countTable = cast(
            melt(countOfVegas, id.var=c("Market", "DeliveryPoint", "DM")), 
            Market + DeliveryPoint ~ DM, fill=0 )
    countTable$Total = countTable$Daily + countTable$Monthly
    
    return(countTable)
}


getRmSysRegion <- function(point)
{
    gasDailyMeanSlang = paste('!iserror(GetSecurity("Commod NG " +"', point, 
            '"+ " GAS-DLY Mean"))', sep="")
    gasDailySlang = paste('!iserror(GetSecurity("Commod NG " +"', point, 
            '"+ " GAS-DLY"))', sep="")
    if( secdb.evaluateSlang(gasDailyMeanSlang) )
    {
        regionSlang = paste('Region Name("Commod NG " +"', point, 
                '"+ " GAS-DLY Mean")', sep="")
        region = secdb.evaluateSlang(regionSlang)
    } else if( secdb.evaluateSlang(gasDailySlang) )
    {
        regionSlang = paste('Region Name("Commod NG " +"', point, 
                '"+ " GAS-DLY")', sep="")
        region = secdb.evaluateSlang(regionSlang)
    } else
    {
        region = "RmSYS"
    }
    
    return(region)
}

################################################################################
DayAfter$main()



