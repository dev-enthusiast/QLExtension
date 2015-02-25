###############################################################################
# OvernightFilesCheck.R
# 
# Safety check to make sure that all the files present for the overnight run
#	are valid RData files. 
#
# NOTE: This will NOT detect if a file is MISSING from the directory 
#
# Author: John Scillieri
# 
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")


################################################################################
# File Namespace
#
OvernightFilesCheck = new.env(hash=TRUE)


################################################################################
# Exported Variables
#
OvernightFilesCheck$DATA_DIR = "S:/All/Risk/Data/VaR/prod"


################################################################################
OvernightFilesCheck$main <- function()
{
    asOfDate <- overnightUtils.getAsOfDate()
    
    OvernightFilesCheck$forDate( asOfDate )   
}


################################################################################
OvernightFilesCheck$allInPath <- function( path = OvernightFilesCheck$DATA_DIR )
{
    RDataFiles = list.files( path = path, pattern = glob2rx("*.RData"), 
            full.names = TRUE, recursive = TRUE )
    
    .scanFileList()
}


################################################################################
# Files Needed For Overnight Run (all are .RData files):
#    Prices
#    Vols
#    Positions
#    Setup
# 
# See individual functions below for details.
#
OvernightFilesCheck$forDate <- function( asOfDate )
{
    BASE_PATH = OvernightFilesCheck$DATA_DIR
    
    date = as.Date(asOfDate)
    
    priceFiles = .priceFiles( date )
    volFiles = .volFiles( date )
    positionFiles = .positionFiles( date )
    setupFiles = .setupFiles( date )
    
    fileList = c( priceFiles, volFiles, positionFiles, setupFiles )
    rTrace("\nComplete File List:\n\n", paste( fileList, collapse="\n" ), sep="" )
    
    .scanFileList( fileList )
    
}


################################################################################
.scanFileList <- function( fileList )
{
    rLog("\nScanning", length(fileList), "files...\n")
    
    for( fileIndex in seq_along(fileList) )
    {
        file = fileList[fileIndex]
        
        rDebug("Loading File", fileIndex, "of", length(fileList), "-", file)
        
        if( safeLoad(file) == FALSE )
        {
            rWarn("The file:", file, "could not be opened!\n")
        }
    }
    
    rLog("\nDone.\n")    
}


################################################################################
# Price Files Needed For Overnight Run (all are .RData files):
#    Prices
#        hPrices.<DATE>
#        corrected.hPrices.<DATE>
#        Market/*
#
.priceFiles <- function( asOfDate )
{
    BASE_PATH = OvernightFilesCheck$DATA_DIR
    
    date = as.Date(asOfDate)
    
    fileList = NULL
    
    priceFile = paste("hPrices.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Prices", priceFile ) )
    
    correctedPriceFile = paste("corrected.hPrices.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Prices", correctedPriceFile ) )
    
    priceMarketDir = file.path( BASE_PATH, "Prices", "Market", "*" )
    fileList = c( fileList, Sys.glob( priceMarketDir ) )
    
    return( fileList )
}


################################################################################
# Volitility Files Needed For Overnight Run (all are .RData files):
#    Vols
#        hVols.<DATE>
#        corrected.hVols.<DATE>
#        Market/*
#
.volFiles <- function( asOfDate )
{
    BASE_PATH = OvernightFilesCheck$DATA_DIR
    
    date = as.Date(asOfDate)
    
    fileList = NULL
    
    volFile = paste("hVols.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Vols", volFile ) )
    
    correctedvolFile = paste("corrected.hVols.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Vols", correctedvolFile ) )
    
    volMarketDir = file.path( BASE_PATH, "Vols", "Market", "*" )
    fileList = c( fileList, Sys.glob( volMarketDir ) )
    
    return( fileList )
}


################################################################################
# Position File Needed For Overnight Run (all are .RData files):
#    Positions
#        vega.positions.<DATE>
#        vcv.positions.<DATE>
#
.positionFiles <- function( asOfDate )
{
    BASE_PATH = OvernightFilesCheck$DATA_DIR
    
    date = as.Date(asOfDate)
    
    fileList = NULL
    
    positionFile = paste("vcv.positions.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Positions", positionFile ) )
    
    vegaFile = paste("vega.positions.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, "Positions", vegaFile ) )
    
    return( fileList )
}


################################################################################
# Position File Needed For Overnight Run (all are .RData files):
#    Setup
#        all.FX
#        curve.info.<DATE>
#        fx.<DATE>
#        options.overnight.<DATE>
#
.setupFiles <- function( asOfDate )
{
    BASE_PATH = OvernightFilesCheck$DATA_DIR
    
    date = as.Date(asOfDate)
    
    fileList = NULL
    
    fileList = c( fileList, file.path( BASE_PATH, "all.FX.RData" ) )
    
    curveInfoFile = paste("curve.info.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, curveInfoFile ) )
    
    fxFile = paste("fx.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, fxFile ) )
    
    optionsFile = paste("options.overnight.", date, ".RData", sep="")
    fileList = c( fileList, file.path( BASE_PATH, optionsFile ) )
    
    return( fileList )
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
OvernightFilesCheck$main()
Sys.time() - .start


