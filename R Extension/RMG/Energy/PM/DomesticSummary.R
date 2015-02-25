library(SecDb)
library(reshape)
source("H:/user/R/RMG/Energy/VaR/Tools/SASVaR.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/Database/SAS/VPort.R")
source("H:/user/R/RMG/Utilities/Database/SAS/SASInterface.R")


################################################################################
# File Namespace
#
DomesticSummary = new.env(hash=TRUE)


################################################################################
DomesticSummary$getDeltas <- function( asOfDate )
{
    deltas = SASInterface$getDeltas( asOfDate )
    
    return( deltas )
}


################################################################################
DomesticSummary$commodPositionsForBooks <- function( commodRegex, deltas, bookList )
{
    positions = subset( deltas, deltas$book %in% bookList )
    
    if( !is.null(commodRegex) )
    {
        curvesToKeep = grep(commodRegex, positions$curve.name, perl=TRUE)
        positions = positions[curvesToKeep, ]
    }
    
    positions = cast( positions, riskFactor ~. , sum )
    names(positions) = c("riskFactor", "value")
    
    return(positions)
}


################################################################################
DomesticSummary$positionsByYear <- function( positions )
{
    dates = strsplit( positions$riskFactor, "_" )
    dates = sapply( dates, function(x){ return( x[length(x)] ) } )
    dates = as.Date( paste( dates, "01", sep=""), "%y%m%d" )
    positions$year = format( dates , "%Y" )

    positionsByYear = cast( positions,  .~year , sum )

    return(positionsByYear)
}


################################################################################
DomesticSummary$varByYear <- function( positions )
{
    dates = strsplit( positions$riskFactor, "_" )
    dates = sapply( dates, function(x){ return( x[length(x)] ) } )
    dates = as.Date( paste( dates, "01", sep=""), "%y%m%d" )
    positions$year = format( dates , "%Y" )
    
    splitPositions = split( positions, positions$year )

    varByYear = lapply( splitPositions, function(yearPos)
            {
                rLog("Calculating var by year for:", yearPos$year[1])
                yearPos$year = NULL
                yearVaR = SASVaR$calculate(yearPos)
                return( data.frame(var=yearVaR) )
            })
    return(varByYear)
}


################################################################################
DomesticSummary$cvarByYear <- function( positions )
{
    cvar = SASVaR$calculateCVaR( positions )
    
    dates = strsplit( cvar$riskFactor, "_" )
    dates = sapply( dates, function(x){ return( x[length(x)] ) } )
    dates = as.Date( paste( dates, "01", sep=""), "%y%m%d" )
    cvar$year = format( dates , "%Y" )
    cvar$riskFactor = NULL
    names(cvar) = c("value", "year")
    
    cvarByYear = cast( cvar,  .~year , function(x){dollar(sum(x))} )
    return(cvarByYear)
}


################################################################################
DomesticSummary$outputFile <- function( asOfDate )
{
    outputDir = paste("S:/All/Risk/Reports/VaR/prod/", asOfDate, 
            "/UK Coal Reports", sep="")
    
    if( !file.exists(outputDir) )
    {
        dir.create(outputDir)
    }
    
    outputFile = file.path( outputDir, "DomesticSummary.csv")
    
    return( outputFile )
}


################################################################################
DomesticSummary$getDomesticCoalData <- function( deltas, asOfDate )
{
    DC3P = "DOMESTIC COAL 3RD PARTY PORTFOLIO"
    DCPB = "COAL FLEET PORTFOLIO"
    DCPM = "DOMESTIC COAL PM TRD PORTFOLIO"
    
    row0 = data.frame("As Of" = "Domestic Coal Portfolio", check.names=FALSE )
    
    booksDC3P = VPort$booksForPortfolio( DC3P, asOfDate )
    positionsDC3P = DomesticSummary$commodPositionsForBooks( "( FCO | COL )", 
            deltas, booksDC3P )
    dc3pByYear = DomesticSummary$positionsByYear( positionsDC3P )
    dc3pByYear$value=NULL
    row1 = data.frame("As Of" = " PM - 3rd Party", dc3pByYear, check.names=FALSE )
    row1$Total = sum(dc3pByYear)
    
    booksDCPB = VPort$booksForPortfolio( DCPB, asOfDate )
    positionsDCPB = DomesticSummary$commodPositionsForBooks( "( FCO | COL )",
            deltas, booksDCPB )
    dcpbByYear = DomesticSummary$positionsByYear( positionsDCPB )
    dcpbByYear$value=NULL
    row2 = data.frame("As Of" = " PM - Fleet", dcpbByYear, check.names=FALSE )
    row2$Total = sum(dcpbByYear)
    
    booksDCPM = VPort$booksForPortfolio( DCPM, asOfDate )
    positionsDCPM = DomesticSummary$commodPositionsForBooks( "( FCO | COL )",
            deltas, booksDCPM )
    dcpmByYear = DomesticSummary$positionsByYear( positionsDCPM )
    dcpmByYear$value=NULL
    row3 = data.frame("As Of" = " PM - Trading", dcpmByYear, check.names=FALSE )
    row3$Total = sum(dcpmByYear)
    
    positionsDCTC = rbind( positionsDC3P, positionsDCPB, positionsDCPM )
    positionsDCTC = cast( positionsDCTC, riskFactor ~. , sum )
    names(positionsDCTC) = c("riskFactor", "value")
    
    dctcByYear = DomesticSummary$positionsByYear( positionsDCTC )
    dctcByYear$value=NULL
    row4 = data.frame("As Of" = "Total Coal", dctcByYear, check.names=FALSE )
    row4$Total = sum(dctcByYear)
    
    yearlyVaRTotalDomCoal = DomesticSummary$varByYear(positionsDCTC)
    yearlyVaRTotalDomCoal = do.call( rbind, yearlyVaRTotalDomCoal )
    yearlyVaRTotalDomCoal$var = dollar( yearlyVaRTotalDomCoal$var )
    yearlyVaRTotalDomCoal$year = rownames(yearlyVaRTotalDomCoal)
    colnames( yearlyVaRTotalDomCoal ) = c("value", "year")
    yearlyVaRTotalDomCoal = cast( yearlyVaRTotalDomCoal, .~year )
    yearlyVaRTotalDomCoal$value = NULL
    row5 = data.frame("As Of" = "VaR", yearlyVaRTotalDomCoal, check.names=FALSE )
    
    varTotalDomCoal = dollar( SASVaR$calculate( positionsDCTC ) ) 
    row5$Total = varTotalDomCoal
    
    varTotalDomCoal0810 = dollar( SASVaR$calculate( 
                    positionsDCTC[grep("_(08|09|10)", positionsDCTC$riskFactor),] ) )
    row6 = data.frame("As Of" = "VaR (2008-2010)", 
            "Total"=varTotalDomCoal0810, check.names=FALSE )
    
    cvarTotalDomCoal = DomesticSummary$cvarByYear( positionsDCTC )
    cvarTotalDomCoal$value = NULL
    row7 = data.frame("As Of" = "CVaR", cvarTotalDomCoal, check.names=FALSE )
    
    
    finalTable = merge( row0, row1, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row2, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row3, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row4, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row5, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row6, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row7, all=TRUE, sort=FALSE )
   
    
    return( finalTable )
}


################################################################################
DomesticSummary$curvesByYear <- function( positions )
{
    positions$year = format( positions$contract.dt , "%Y" )
    positions$value = round(positions$value, 0)
    positions$contract.dt = NULL
    positions$vol.type = NULL
    positionsByYear = cast( positions, curve.name~year, sum, margins="grand_col" )
    
    return( positionsByYear )
}


################################################################################
DomesticSummary$getUSCOLOTHData <- function( deltas )
{
    # USCOLOTH Positions    
    USCOLOTH = "USCOLOTH"
    positionsUSCOLOTH = DomesticSummary$commodPositionsForBooks( NULL, deltas, USCOLOTH )
    uscolothByYear = DomesticSummary$positionsByYear( positionsUSCOLOTH )
    uscolothByYear$value = NULL
    
    finalTable = data.frame("As Of" = USCOLOTH, uscolothByYear, check.names=FALSE )
    finalTable$Total = sum(data.frame(uscolothByYear))
    
    return( finalTable )
}


################################################################################
DomesticSummary$getJointPortfolioData <- function( deltas, asOfDate )
{
    PMJB = "DOMESTIC INTL SHARED PORTFOLIO"
    
    row0 = data.frame("As Of" = "PM - Joint Business", check.names=FALSE )
    
    booksPMJB = VPort$booksForPortfolio( PMJB, asOfDate )
    positionsPMJBCoal = DomesticSummary$commodPositionsForBooks( 
            "( FCO | COL )", deltas, booksPMJB )
    pmjbCoalByYear = DomesticSummary$positionsByYear( positionsPMJBCoal )
    pmjbCoalByYear$value = NULL
    row1 = data.frame("As Of" = " Total Coal", pmjbCoalByYear, check.names=FALSE )
    row1$Total = sum(pmjbCoalByYear)
    
    positionsPMJBFreight = DomesticSummary$commodPositionsForBooks( "( FRC | FRT )", deltas, booksPMJB )
    pmjbFreightByYear = DomesticSummary$positionsByYear( positionsPMJBFreight )
    pmjbFreightByYear$value = NULL
    row2 = data.frame("As Of" = " Total Freight VC", pmjbFreightByYear, check.names=FALSE )
    row2$Total = sum(pmjbFreightByYear)
    
    totalPMJB = rbind(positionsPMJBCoal, positionsPMJBFreight)
    totalPMJB = cast( totalPMJB, riskFactor ~. , sum )
    names(totalPMJB) = c("riskFactor", "value")
    
    yearlyVaRTotalPMJB = DomesticSummary$varByYear(totalPMJB)
    yearlyVaRTotalPMJB = do.call(rbind, yearlyVaRTotalPMJB)
    yearlyVaRTotalPMJB$var = dollar(yearlyVaRTotalPMJB$var)
    yearlyVaRTotalPMJB$year = rownames(yearlyVaRTotalPMJB)
    colnames( yearlyVaRTotalPMJB ) = c("value", "year")
    yearlyVaRTotalPMJB = cast( yearlyVaRTotalPMJB, .~year )
    yearlyVaRTotalPMJB$value = NULL
    row3 = data.frame("As Of" = "VaR", yearlyVaRTotalPMJB, check.names=FALSE )    
    row3$Total = dollar( SASVaR$calculate(totalPMJB) ) 

    cvarTotalPMJB = DomesticSummary$cvarByYear(totalPMJB)
    cvarTotalPMJB$value = NULL
    row4 = data.frame("As Of" = "CVaR", cvarTotalPMJB, check.names=FALSE )
    
    finalTable = merge( row0, row1, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row2, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row3, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, row4, all=TRUE, sort=FALSE )

    return( finalTable )
}

################################################################################
DomesticSummary$main <- function()
{
    # Initialize SecDb for use on Procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    options(stringsAsFactors=FALSE)
    
    asOfDate = overnightUtils.getAsOfDate()
    
    deltas = DomesticSummary$getDeltas( asOfDate )
    
    dcTable = DomesticSummary$getDomesticCoalData( deltas, asOfDate )
    
    uscolothTable = DomesticSummary$getUSCOLOTHData( deltas )
    
    jpTable = DomesticSummary$getJointPortfolioData( deltas, asOfDate )
    
    emptyRow1 = data.frame( "As Of" = " " , check.names=FALSE )
    emptyRow2 = data.frame( "As Of" = "  " , check.names=FALSE )
    
    finalTable = merge( dcTable, emptyRow1, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, uscolothTable, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, emptyRow2, all=TRUE, sort=FALSE )
    finalTable = merge( finalTable, jpTable, all=TRUE, sort=FALSE )
    
    yearColumns = sort( as.numeric( colnames(finalTable) ) )
    columnOrder = c("As Of", as.character( yearColumns ), "Total" )
    finalTable = finalTable[, columnOrder]
    
    outputFile = DomesticSummary$outputFile( asOfDate )    
    dateLabel = paste( "As Of:", as.character(asOfDate) )
    colnames(finalTable)[1] = dateLabel
    write.csv( finalTable, file = outputFile, row.names = FALSE, na = "" )

    invisible( finalTable )
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
DomesticSummary$main()
print( Sys.time() - .start )
