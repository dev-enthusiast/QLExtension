###############################################################################
## EODVolReport.R
## 
## Author: e14600
##
##     CCG EOD Unofficial Forwards     
##    Prices for 12Mar08 (vs 11Mar08)   
##     as of Wed 12Mar08 04:46:13 pm    
## 
##    --->  WTI Crude Exchange  <---   
##       12Mar08 11Mar08    Diff  %Chng
## Apr08  109.92  108.75    1.17   1.08 
## May08  108.57  107.52    1.05   0.98 
## Jun08  107.20  106.22    0.98   0.92 
## Jul08  106.07  105.08    0.99   0.94 
## 2008   105.82  104.69    1.12   1.07 
## 2009   101.13   99.43    1.70   1.71 
## 2010    99.41   97.40    2.00   2.06 
## 2011    99.31   97.25    2.07   2.13 
## 
##      --->  Nat Gas Exchange  <---    
##         12Mar08 11Mar08   Diff  %Chng
## Apr08   10.01   10.00    0.01   0.11 
## May08   10.08   10.06    0.01   0.14 
## Jun08   10.15   10.13    0.01   0.14 
## Jul08   10.23   10.22    0.01   0.15 
## 2008    10.32   10.30    0.02   0.21 
## 2009     9.64    9.57    0.07   0.76 
## 2010     9.05    8.97    0.07   0.83 
## 2011     8.93    8.86    0.07   0.79 
## 
## 
## 
library(SecDb)
source("H:/user/R/RMG/Projects/PV/Term.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")


################################################################################
# File Namespace
#
EODVolReport = new.env(hash=TRUE)


################################################################################
# Cached Data
#
EODVolReport$.volData = NULL


################################################################################
EODVolReport$main <- function( )
{
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    asOfDate = Sys.Date()
    yesterday = as.Date( secdb.dateRuleApply( asOfDate, "-1b" ) ) 
    
    todayName = format( asOfDate, "%d%b%y" )
    yesterdayName = format( yesterday, "%d%b%y" )
    
    OUTPUT_FILE = paste( "S:/All/Risk/Reports/TradingControlReports/EODVolReport/", 
            as.character(asOfDate), ".txt", sep="" )
    file.remove(OUTPUT_FILE)
    
    INPUT_FILE = "H:/user/R/RMG/Energy/Trading/Option.Analysis/EODVolCurves.csv"
    curveTable = read.csv( INPUT_FILE )
    curveTable$curve = as.character(curveTable$curve)
    curveTable$monthly = as.logical(curveTable$monthly)
    curveTable$daily = as.logical(curveTable$daily)
    
    maxHeadingWidth = max(nchar(curveTable$curve)) + nchar(" (Monthly)")
    
    # Print the header
    reportHeadings = NULL
    reportHeadings = rbind( reportHeadings, "CCG EOD Unofficial ATM Vols" )
    reportHeadings = rbind( reportHeadings, paste("For", todayName, "vs", yesterdayName) )
    reportHeadings = rbind( reportHeadings, paste("As Of", format(Sys.time(), "%a %d%b%y %X") ) )
    reportHeadings = rbind( reportHeadings, " ")
    reportHeadings = rbind( reportHeadings, " ")
    
    reportHeadings = format(reportHeadings, justify="centre", width=maxHeadingWidth)
    fileCon = file(OUTPUT_FILE, "at")
    lapply( reportHeadings,  function(x){ writeLines( paste("  ", x, sep=""), fileCon ) } )
    close(fileCon)
    
    for( curveIndex in seq_len( nrow( curveTable ) ) )
    {
        # create a secdb array of structures
        curve = curveTable$curve[curveIndex]
        
        if( curveTable$monthly[curveIndex] == TRUE )
        {
            EODVolReport$.writeDiffData( curve, asOfDate, TRUE, 
                    OUTPUT_FILE, maxHeadingWidth )
        }
        
        if( curveTable$daily[curveIndex] == TRUE )
        {
            EODVolReport$.writeDiffData( curve, asOfDate, FALSE, 
                    OUTPUT_FILE, maxHeadingWidth )
        }
    }
    
    fileCon = file(OUTPUT_FILE, "at")
    writeLines( paste( "This email contains proprietary information and should", 
                    "not be forwarded or distributed to anyone beyond its", 
                    "original recipient." ), fileCon )
    close(fileCon)
    
    EODVolReport$.sendEmail( OUTPUT_FILE )
}


################################################################################
EODVolReport$.writeDiffData <- function( curve, asOfDate, monthly, 
        outputFile, maxHeadingWidth )
{
    volData = EODVolReport$.getVolDiffData( curve, asOfDate, monthly )
    volData = volData[1:25,]
    
    if( is.null(volData) )
    {
        return( NULL )    
    }
    
    fileCon = file(outputFile, "at")
    
    monthLabel = ifelse( monthly, "(Monthly)", "(Daily)" )
    label = paste( curve, monthLabel )
    writeLines( format(label, width=maxHeadingWidth, justify="centre"), fileCon ) 
    
    outputTable = format( volData, width=maxHeadingWidth/ncol(volData) )
    capture.output( outputTable, file=fileCon )
    
    writeLines( "\n", fileCon ) 
    
    close(fileCon)
}


################################################################################
EODVolReport$.getVolDiffData <- function( curve, asOfDate, monthly )
{
    rLog("Calculating Diff For Curve:", curve)
    startOfNextMonth = as.Date( secdb.dateRuleApply( asOfDate, "e+1d" ) ) 
    yesterday = as.Date( secdb.dateRuleApply( asOfDate, "-1b" ) ) 
    
    volToday = EODVolReport$.getVolFromSecDb( curve, monthly )
    volYesterday = EODVolReport$.getHistoricVol( yesterday, curve, asOfDate, monthly )
    
    volData = merge(volToday, volYesterday, sort=TRUE)
    if(nrow(volData) == 0) return(NULL)
    
    todayName = format( asOfDate, "%d%b%y" )
    yesterdayName = format( yesterday, "%d%b%y" )
    volData$Diff = round( volData[[todayName]] - volData[[yesterdayName]], 2 )
    
    volChange = abs( volData$Diff / volData[[yesterdayName]] * 100 )
    volData$"%Chng" = dollar( volChange, do.dollar=FALSE )
    
    volData$Term = format( volData$Term, "%b%y" )
    
    return( volData )
}


################################################################################
EODVolReport$.getVolFromSecDb <- function( curve, monthly=TRUE )
{
    volTypeFlag = ifelse( monthly, "Monthly", "Daily" )
    
    rawData = secdb.invoke("RWrapper", "RWrapper::GetVolsForCurve", 
            curve, volTypeFlag )
    
    dateName = format( Sys.Date(), "%d%b%y" )

    finalData = NULL
    for( tenor in rawData )
    {
        term = tenor$C[[1]]
        expiration = as.Date( secdb.convertTime( tenor$d[[1]] ) )
        volData = tenor$v
        volData = matrix(volData, ncol=2, byrow=TRUE)
        atmRow = mean(seq_len(nrow(volData)))
        atmVal = unlist( volData[ atmRow, 2 ] )
        
        rowData = data.frame( Term=term, Expiration = expiration )
        rowData[[dateName]] = atmVal 
        
        finalData = rbind(finalData, rowData)
    }
    
    finalData[[dateName]] = round( as.numeric(finalData[[dateName]]), 2 )
    
    finalData$Term = Term$fromReutersCode( finalData$Term )
    
    finalData = finalData[ which( finalData$Expiration > Sys.Date() ), ]
    finalData$Expiration = NULL
    
    return( finalData )
}


################################################################################
EODVolReport$.getHistoricVol <- function( asOfDate, curve, startDate, monthly=TRUE )
{
    curve = toupper(curve)
    
    if( is.null( EODVolReport$.volData ) )
    {
        volFile = paste( "S:/All/Risk/Data/VaR/prod/Vols/hVols.", 
                as.character( asOfDate ), ".RData", sep="" )
        EODVolReport$.volData = varLoad( "hV", volFile )
        
        if( is.null( EODVolReport$.volData ) )
        {
            stop("Couldn't load historic vol data for:", as.character(asOfDate))
        }
    }
    
    volTypeFlag = ifelse( monthly, "M", "D" )
    curvesToReturn = which(EODVolReport$.volData$curve.name==curve & 
                    EODVolReport$.volData$vol.type==volTypeFlag )
    
    data = EODVolReport$.volData[curvesToReturn,]
    data$curve.name = NULL
    data$vol.type = NULL
    names(data)[which(names(data)=="contract.dt")] = "Term"
    dateName =  format( asOfDate, "%d%b%y" )
    names(data)[which(names(data)!="Term")] = dateName
    data[[dateName]] = round( data[[dateName]], 2 )
    
    zeroData = which( data[[dateName]] == 0 )
    if( length(zeroData) > 0 )
    {
        data = data[ -zeroData,  ]
    }
    
    return( data )
}


################################################################################
EODVolReport$.sendEmail <- function( fileToRead )
{
    valueLine = paste( readLines(fileToRead), collapse="\n" )
    bodyOfEmail = paste( valueLine, 
            "CEG Risk Management Group - Analytics\njohn.scillieri@constellation.com\n\n", 
            sep="\n\n" )
    
    toList = getEmailDistro("EODVolReport.R")
    
    sendEmail( from="DO-NOT-REPLY@constellation.com",
            to=toList, subject="EOD Vol Report", body=bodyOfEmail )
}


################################################################################
EODVolReport$main()







