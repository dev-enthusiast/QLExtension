###############################################################################
# PVUtils.R
#
# Author: John Scillieri
# 

################################################################################
# External Libraries
#
library(reshape)
library("SecDb")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/write.xls.R")


################################################################################
# File Namespace
#
PVUtils = new.env(hash=TRUE)


################################################################################
# CONSTANTS
#
SLANG_SCRIPT = "WYU: Vol Test"
VOL_FUNCTION = "Vol Test::Main"

VOL_PRICE_FUNCTION = "System Price"
TOP_VEGAS_FUNCTION = "Top Vegas"
COVERAGE_FUNCTION = "Coverage"


################################################################################
# Return the last business day of the month.
#
PVUtils$getAsOfDate <- function( monthStartDate )
{
    lastBusinessDay = secdb.dateRuleApply(as.Date(monthStartDate), "e+1b-1b")
    return( as.Date(lastBusinessDay) )
}


################################################################################
# Return the last business day of the month.
#
PVUtils$getTestDir <- function( monthStartDate )
{
    BASE_PATH = "S:/Risk/Reports/Vol Test/"
    startDate = as.Date(monthStartDate)
    monthYearFolder = format( startDate, "%b %Y" )
    
    finalPath = paste( BASE_PATH, monthYearFolder, "/", sep="" )
    return(finalPath)
}


################################################################################
# 
PVUtils$WritePriceVolDiffs <- function( curves, asOfDate, outputPath )
{
    lastMark = Sys.Date()
    yesterday = Sys.Date()-1
    dateTill = Sys.Date()+ (365*4)
    volATM = 1
    useClose = TRUE
    
    sink(file=outputPath, append=TRUE)
    for( curve in curves )
    {
        volData = .readVolData(curve, lastMark, yesterday, dateTill, volATM, useClose )
        
        priceData = .readPriceData( curve, lastMark, yesterday, dateTill, useClose)
        
        output = merge(priceData, volData, sort=FALSE)
        
        cat(curve, "-", as.character(asOfDate), "\n")
        print(output, quote=FALSE)
        cat("\n\n")
    }
    sink()
    
}

.readVolData <- function( curve, lastMark, yesterday, dateTill, volATM, useClose )
{
    # create a secdb array of structures 
    location = list( list(loc=curve) )
    
    volData = secdb.invoke(
            "_Lib Elec Dly Fut Changes", "DlyFutChanges::GetVolatility", 
            location, lastMark, yesterday, dateTill, volATM, useClose )[[1]]
    
    volToday = data.frame(volData$VolToday)
    volToday = melt(volToday, measure.var=names(volToday))
    names(volToday) = c("Term", "Vol Today")
    
    volYesterday = data.frame(volData$VolYesterday)
    volYesterday = melt(volYesterday, measure.var=names(volYesterday))
    names(volYesterday) = c("Term", "Vol Yesterday")
    
    volData = merge(volToday, volYesterday, sort=FALSE)
    volData$"Vol Today" = as.numeric(as.character(volData$"Vol Today"))
    volData$"Vol Yesterday" = as.numeric(as.character(volData$"Vol Yesterday"))
    
    volData$"Vol Difference" = volData$"Vol Today" - 
            volData$"Vol Yesterday"
    
    return(volData)
}

.readPriceData <- function( curve, lastMark, yesterday, dateTill, useClose )
{
    # create a secdb array of structures 
    location = list( list(loc=curve) )
    
    priceData = secdb.invoke(
            "_Lib Elec Dly Fut Changes", "DlyFutChanges::GetFutStrip", 
            location, lastMark, yesterday, dateTill, useClose)[[1]]
    
    priceToday = data.frame(priceData$FutToday)
    priceToday = melt(priceToday, measure.var=names(priceToday))
    names(priceToday) = c("Term", "Price Today")
    
    priceYesterday = data.frame(priceData$FutYesterday)
    priceYesterday = melt(priceYesterday, measure.var=names(priceYesterday))
    names(priceYesterday) = c("Term", "Price Yesterday")
    
    priceData = merge(priceToday, priceYesterday, sort=FALSE)
    priceData$"Price Today" = as.numeric(as.character(priceData$"Price Today"))
    priceData$"Price Yesterday" = as.numeric(as.character(priceData$"Price Yesterday"))
    
    priceData$"Price Difference" = priceData$"Price Today" - 
            priceData$"Price Yesterday"
    
    return(priceData)
}


################################################################################
PVUtils$SecDbSystemPrices <- function( asOfDate, testDir )
{
    # Run the System Price function 1 more time to get the EOD vol marks
    secdb.invoke(
            SLANG_SCRIPT, VOL_FUNCTION,
            testDir, asOfDate, VOL_PRICE_FUNCTION )
    
    invisible(NULL)
}


################################################################################
PVUtils$SecDbTopVegas <- function( asOfDate, testDir )
{
    
    # Get the top 150 vegas
    # Make sure the VBS Report and rmsys_vegas are both in the test dir!
    secdb.invoke(
            SLANG_SCRIPT, VOL_FUNCTION,
            testDir, asOfDate, TOP_VEGAS_FUNCTION )
    
    invisible(NULL)
    
}


################################################################################
PVUtils$SecDbCoverageReport <- function( asOfDate, testDir )
{
    
    # Run the coverage report
    secdb.invoke(
            SLANG_SCRIPT, VOL_FUNCTION,
            testDir, asOfDate, COVERAGE_FUNCTION )
    
    invisible(NULL)
    
}


################################################################################
#    Input to secdb is a 'input' structure that has the following elements:
#            Pricing Date = Date
#            SD = Date
#            ED = Date
#            Forward = double
#            Type = String {'CALL', 'PUT', 'STRADDLE'}
#            Location = String, eg. 'PJM West'
#            Period = String {'Monthly', 'Daily', 'One-Time'}
#            Strike = Double
PVUtils$WriteSystemPrices <- function( asOfDate, testDir )
{
    
    inputFile = paste(testDir, "VolTest_", format(asOfDate, "%d%b%y"), 
            ".xls", sep="")
    rLog("Reading File:", inputFile, "For Quote Data...")
    con = odbcConnectExcel(inputFile)
    quoteData = sqlFetch(con, "Quotes")
    odbcClose(con)
    
    rLog("Querying SecDb For Option Prices...")
    quoteData$"Option Price" = NA
    for( rowIndex in seq_len(nrow(quoteData)) )
    {
        rowData = quoteData[rowIndex,]
        SD = Term$startDate(as.character(rowData$Term))
        ED = Term$endDate(as.character(rowData$Term))
        Forward = as.numeric(rowData$Forward)
        Type = as.character(rowData$Product)
        Location = as.character(rowData$Location)
        Period = as.character(rowData$Frequency)
        Strike = as.numeric(rowData$Strike)
        
        optionPrice = secdb.invoke( "RWrapper", "RWrapper::getPrice", 
                asOfDate, Location, SD, ED, Period, Type, Forward, Strike )
        if( Location == "CL NYMEX" )
        {
            optionPrice$"Option Price" = optionPrice$"Option Price" * 100           
        }
        
        quoteData$"Option Price"[rowIndex] = optionPrice$"Option Price"
        
        if( !is.null(optionPrice$Forward) )
        {
            quoteData$Forward[rowIndex] = optionPrice$Forward
        }
        if( !is.null(optionPrice$Strike) )
        {
            quoteData$Strike[rowIndex] = optionPrice$Strike
        }
    }
    
    rLog("Calculating Exceptions...")
    quoteData$Mid = (quoteData$"Bid Price" + quoteData$"Ask Price")/2
    
    
    highPrices = which(quoteData$"Option Price" > quoteData$"Ask Price")
    lowPrices = which(quoteData$"Option Price" < quoteData$"Bid Price")
    quoteData$PriceOK = "OK"
    quoteData$PriceOK[highPrices] = "High"
    quoteData$PriceOK[lowPrices] = "Low"
    
    outputFile = paste(testDir, "VolTestOptPrc_", format(Sys.time(), "%H%M"), 
            ".csv", sep="" )
    rLog("Writing Output To:", outputFile)
    write.csv(quoteData, file=outputFile, row.names=FALSE)
    
    rLog("Done.")
}


################################################################################
PVUtils$GetExceptions <- function( asOfDate, testDir )
{
    fileList = list.files( testDir, 
            pattern="VolTestOptPrc.+csv", full.names=TRUE) 
    fileInfo = file.info(fileList)
    fileInfo = fileInfo[order(fileInfo$mtime, decreasing=TRUE),]
    mostRecentFile = rownames(fileInfo)[1]
    
    priceData = read.csv(mostRecentFile)
    
    exceptions = priceData[which(priceData$"OK." != "OK"),]
    
    validExceptions = NULL
    for( rowIndex in seq_len(nrow(exceptions)) )
    {
        exception = exceptions[rowIndex,]
        myRowName = rownames(exception)
        matchingData = subset(priceData, 
                priceData$Region == exception$Region &
                        priceData$SD == exception$SD &
                        priceData$ED == exception$ED &
                        priceData$Type == exception$Type &
                        priceData$Period == exception$Period )
        if( matchingData[nrow(matchingData), "OK."] != "OK" )
        {
            validExceptions = c(validExceptions, rownames(exception))
        }
    }
    
    dataToWrite = priceData[validExceptions,]
    rownames(dataToWrite) = NULL
    dataToWrite = Term$appendToData( dataToWrite, format="%d-%b-%y" )
    dataToWrite$SD = dataToWrite$ED = NULL
    names(dataToWrite) = c("Location", "Forward Price", "Strike", "Product",
            "Period", "Bid", "Ask", "Mid", "System Quote", "Broker", "Term")
    dataToWrite$Broker = ""
    dataToWrite = dataToWrite[, c("Location", "Broker",  "Period", "Term", 
                    "Product", "Forward Price", "Strike", 
                    "Bid", "Ask", "Mid", "System Quote")]
    
    dateFormat = format(asOfDate, "%d%b%y")
    outputFile = paste( testDir, "VolTestExceptionData_", 
            dateFormat, ".csv", sep="" )
    write.csv( dataToWrite, file=outputFile, row.names=FALSE )
    
    return( dataToWrite )
}


################################################################################
PVUtils$WriteExceptions <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    inputFile = paste( testDir, "VolTestExceptionData_", 
            dateFormat, ".csv", sep="" )
    
    exceptionData = read.csv(inputFile)
    
    volTestFile = paste(testDir, "VolTest_", dateFormat, ".xls", sep="")  
    write.xls( exceptionData, volTestFile, "Value of Variance", 
            top.cell="B7", col.names=FALSE)
    
    invisible( NULL )
}


################################################################################
PVUtils$WriteCoverageData <- function( asOfDate, testDir )
{
    dateFormat = format(asOfDate, "%d%b%y")
    coverageFile = paste(testDir, "CoverageRatio_", dateFormat, ".csv", sep="")
    
    coverageData = read.csv(coverageFile, header=FALSE )
    
    QUOTE_HEADER = "Coverage ratio by live quotes"
    VAMA_HEADER = "Coverage ratio by alternative method( Top Vegas )"
    
    table1Start = which(coverageData[,1]==QUOTE_HEADER)
    table2Start = which(coverageData[,1]==VAMA_HEADER)
    table1 = coverageData[table1Start:(table2Start-1),]
    table2 = coverageData[table2Start:nrow(coverageData),]
    
    ABS_VEGAS_HEADER = "Coverage for abs vegas"
    TABLE_END = " "
    
    table1Start = which(table1[,1]==ABS_VEGAS_HEADER)+1
    table2Start = which(table2[,1]==ABS_VEGAS_HEADER)+1
    
    table1End = which(table1[,1]==TABLE_END)-1
    table2End = which(table2[,1]==TABLE_END)-1
    
    quoteData = table1[ table1Start:table1End ,]
    vamaData = table2[table2Start:table2End ,]
    
    colnames(quoteData) = table1[table1Start,]
    colnames(vamaData) = table2[table2Start,]
    quoteData = quoteData[-1,]
    vamaData = vamaData[-1,]
    
    volTestFile = paste(testDir, "VolTest_", dateFormat, ".xls", sep="")  
    write.xls( quoteData, volTestFile, "Coverage Summary", 
            top.cell="A4", col.names=FALSE)
    write.xls( vamaData, volTestFile, "Coverage Summary", 
            top.cell="G4", col.names=FALSE)
    
    invisible(NULL)
}


















