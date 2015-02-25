################################################################################
# DomesticDetail.R
#
library(SecDb)
library(reshape)
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/write.xls.R")
source("H:/user/R/RMG/Utilities/Database/SAS/VPort.R")
source("H:/user/R/RMG/Utilities/Database/SAS/SASInterface.R")
source("H:/user/R/RMG/Energy/VaR/Tools/SASVaR.R")


################################################################################
# File Namespace
#
DomesticDetail = new.env(hash=TRUE)


################################################################################
DomesticDetail$outputFilePath <- function( asOfDate )
{
    outputDir = paste("S:/All/Risk/Reports/VaR/prod/", asOfDate, 
            "/UK Coal Reports", sep="")
    if( !file.exists(outputDir) )
    {
        dir.create(outputDir)
    }
    
    outputFile = file.path( outputDir, "DomesticPortfolioDetail.xls")
    
    return( outputFile )
}


################################################################################
DomesticDetail$getDomesticCoalPositions <- function( deltas, asOfDate )
{
    DCTC = "DOMESTIC COAL TOTAL PORTFOLIO"
    bookList = VPort$booksForPortfolio( DCTC, asOfDate )
    
    positionsDCTC = DomesticDetail$commodPositionsForBooks( deltas, bookList )

    return( positionsDCTC )
}


################################################################################
DomesticDetail$commodPositionsForBooks <- function( deltas, bookList )
{
    positions = subset( deltas, deltas$book %in% bookList )
    
    curvesToKeep = grep("( FCO | COL )", positions$curve.name, perl=TRUE)
    positions = positions[curvesToKeep, ]
    
    positions = cast( positions, riskFactor + curve.name + contract.dt + vol.type ~. , sum )
    names(positions) = c("riskFactor", "curve.name", "contract.dt", "vol.type", "value")
    
    return(positions)
}


################################################################################
DomesticDetail$curvesByYear <- function( positions )
{
    positions$year = format( positions$contract.dt , "%Y" )
    positions$value = round(positions$value, 0)
    positions$contract.dt = NULL
    positions$vol.type = NULL
    positionsByYear = cast( positions, curve.name~year, 
            sum, margins=TRUE, fill=0 )
    
    return( positionsByYear )
}


################################################################################
DomesticDetail$cvarByCurveYear <- function( positions )
{
    positions$book = NA
    cvarData = SASVaR$calculateCVaR( positions )
    
    cvarTable = cvarData
    cvarTable$cvar = round(cvarTable$cvar)

    splitNames = strsplit(cvarTable$riskFactor, "_")
    tenors = sapply(splitNames, function(x){x[length(x)]})
    tenors = paste(tenors, "01", sep="" )

    cvarTable$riskFactor = gsub( "PR_", "", cvarTable$riskFactor )
    cvarTable$riskFactor = gsub( "_[0-9]{4}$", "", cvarTable$riskFactor )
    cvarTable$riskFactor = gsub( "_", " ", cvarTable$riskFactor )
    cvarTable$year = format( as.Date(tenors, "%y%m%d"), "%Y")
    cvarTable$book = NULL
    cvarTable$contract.dt = NULL
    cvarTable$vol.type = NULL
    names(cvarTable) = c("curve.name", "value", "year")
    
    cvarTable = cast( cvarTable, curve.name ~ year , sum, margins=TRUE, fill=0 )
    
    return( cvarTable )
}


################################################################################
DomesticDetail$main <- function()
{
    # Initialize SecDb for use on Procmon
    DB = "!Bal ProdSource;!BAL DEV!home!E14600;!BAL Home"
    Sys.setenv(SECDB_SOURCE_DATABASE=DB)
    Sys.setenv(SECDB_DATABASE="Prod")
    
    options(stringsAsFactors=FALSE)
    
    asOfDate = overnightUtils.getAsOfDate()
    
    deltas = SASInterface$getDeltas( asOfDate )

    positions = DomesticDetail$getDomesticCoalPositions( deltas, asOfDate )
    rLog( "Got", nrow(positions), "domestic coal positions with", 
            ncol(positions), "columns" )
    print(str(positions))

    outputFile = DomesticDetail$outputFilePath( asOfDate )
    
    curvePositionTable = DomesticDetail$curvesByYear( positions )
    write.xls( curvePositionTable, sheetname="Positions", file=outputFile )
    rLog( "Wrote curve position table." )
    
    cvarTable = DomesticDetail$cvarByCurveYear( positions )
    write.xls( cvarTable, sheetname="CVaR", file=outputFile )
    rLog( "Wrote CVaR by curve year table." )
    
    invisible(NULL)
    rLog( "Done." )
}


################################################################################
DomesticDetail$main()





