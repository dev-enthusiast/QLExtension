################################################################################
# This file will calculate the PE for portfolios requested by the Credit Group.
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Energy/VaR/PE/PECalculator.R")   
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/mkdir.R")
source("H:/user/R/RMG/Utilities/write.xls.R")
library("reshape")


################################################################################
# File Namespace
#
CalculateCreditPortfolios = new.env(hash=TRUE)


################################################################################
# Set Up The Portfolio Lists
#
PORT_NAME_ATTR = "Portfolio Name"

BusinessDecisionPortfolio = c( "EMERALDINT", "INTEGRITY", "KIVA BUSD",
  "MIRTRADE", "POLSTEAM", "CONVERSE", "ICG", "BD-TRANS" )
attr(BusinessDecisionPortfolio, PORT_NAME_ATTR) = "Business Decision Portfolio"

UKRiskPortfolio = c( "ACQUARIO", "ADARO", "BAYANRES", "BET-COSCO", "BOCIMAR",
  "BUNGEGENEV", "CARGILLINT", "CESAR COAL", "CMCCOAL", "FORMOSA", "GREYCOALSH",
  "HANJINSHIP", "KAWASAKI", "KLEIMAR", "KPCOAL", "MIRTRADE", "MITSUI",
  "MURSHIPPIN", "POLSTEAM", "SHANGBOAST", "SK EUROPE", "SUEKAG", "SWISSMARIN", 
  "TANGSHAN", "TRANS CAPE", "VENERE SA" )
attr(UKRiskPortfolio, PORT_NAME_ATTR) = "UK Risk Portfolio"

FreightICR7 <- c("KLEIMAR", "SHAGANG", "NOBLECHART", "OXCOAL", "BD-TRANS",
  "ILVA", "DAMICODRY", "PHOENIXBLK", "NORDEN", "SNIM", "BUSHIP", "DEIULEMAR",
  "FLAMENE", "ANTIGONOS", "ZODIACMAR")
attr(FreightICR7, PORT_NAME_ATTR) = "FreightICR7"

portfolioList = list( BusinessDecisionPortfolio, UKRiskPortfolio )

################################################################################
CalculateCreditPortfolios$main <- function()
{
  
  PE$loadPEOptions()
  outputDir = paste( PE$options$save$reports.dir, "Credit", sep="" )
  if(!file.exists(outputDir))
  {
    mkdir(outputDir)
  }

  # Create New Excel File For Portfolio
  for( portfolio in portfolioList )
  {
    rLog("Starting portfolio:",attr(portfolio, PORT_NAME_ATTR))
    excelFile = paste(outputDir, "/", attr(portfolio, PORT_NAME_ATTR), 
      ".xls", sep="")
    if(file.exists(excelFile))
    {
      file.remove(excelFile)
    }
      
    # For each counterparty
    for( counterparty in sort(portfolio, decreasing=TRUE) )
    {
      rLog("Writing Counterparty:", counterparty, "to file...")
      CalculateCreditPortfolios$.writeCPDataToFile( counterparty, 
        excelFile, TRUE )
      CalculateCreditPortfolios$.writeCPDataToFile( counterparty, 
        excelFile, FALSE )
    }

    # Credit didn't want to map to the R: drive, so copy the file to a
    # more visible location.
    networkPath = paste("S:/All/Risk/Reports/Credit/MPE/", PE$options$asOfDate,
      sep="")
    if(!file.exists(networkPath))
    {
      mkdir(networkPath)
    }
    destination = paste(networkPath, "/", attr(portfolio, PORT_NAME_ATTR), 
      ".xls", sep="")
    rLog("Copying file to network drive...")
    file.copy(excelFile, destination, overwrite=TRUE)
    rLog("Done with portfolio:", attr(portfolio, PORT_NAME_ATTR))
  }
      
}

################################################################################
# for the mode given, load it -> reshape it -> write it.
# 
CalculateCreditPortfolios$.writeCPDataToFile <- function( counterparty,
  excelFile, directMode=TRUE )
{
  modeToLoad = ifelse( directMode, "DIRECT", "INDIRECT" )
  
  sheetLabel = ifelse( directMode, "STANDARD", "INDIRECT" )
  sheetName = paste(counterparty, " (", sheetLabel, ")", sep="")
  
  rdataDir = PE$options$save$reports.dir
  cpDataFile = paste( rdataDir, counterparty, "/RData/", counterparty,
    "_", modeToLoad, ".RData", sep="")
  rDebug("Loading File:", cpDataFile)
  if( file.exists(cpDataFile) )
  {
    load(cpDataFile)
    reshapedData = CalculateCreditPortfolios$.reshapeData( aux )
    write.xls(reshapedData, excelFile, sheetName)
    
  } else
  {
    rWarn("Couldn't load file for counterparty:", counterparty)
  }
}

################################################################################
CalculateCreditPortfolios$.reshapeData <- function( dataSet )
{
  
  meltedData = melt( dataSet, id.var=c("what", "contract.dt"), 
    measure.var="value" )
    
  reshapedData = cast( meltedData, contract.dt ~ what )

  colsToChange = which(names(reshapedData)!="contract.dt")
  reshapedData[,colsToChange] = format(reshapedData[,colsToChange], 
    digits=2, nsmall=2)
  
  return(reshapedData)
}


################################ MAIN EXECUTION ################################
.start = Sys.time()

#setLogLevel(RLoggerLevels$LOG_DEBUG)
#Sys.setenv(asOfDate="2008-02-01")

CalculateCreditPortfolios$main()

Sys.time() - .start

