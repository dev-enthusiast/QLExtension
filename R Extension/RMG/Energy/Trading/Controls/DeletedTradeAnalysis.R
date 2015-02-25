################################################################################
# File Namespace
#
DeletedTradeAnalysis = new.env(hash=TRUE)


################################################################################
# Read an External Trade Identifier and return all the info in CPSProd for that
# trade. If extended = true it also returns trade leg information.
#
library(RODBC)
source("H:/user/R/RMG/Utilities/TradeInfo.R")
source("H:/user/R/RMG/Utilities/rLogger.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( "SecDb" )


################################################################################
# Extract the deleted ETIs from the given file
#
.getETIList <- function( fileName )
{
  if( !file.exists(fileName) )
  {
    return(NULL)
  }
  channel = odbcConnectExcel(fileName)
  tradeData = sqlFetch(channel, "Sheet1")
  odbcClose(channel)
  
  etiList = as.character(unique(tradeData$ETI))
  
  return(etiList)
}


################################################################################
# Perform standard formatting on a list of ETIs and prices
#
.formatPriceData <- function( etiList, priceList )
{
  priceData = data.frame(ETI=etiList, Value=priceList)
  priceData = priceData[order(abs(priceData$Value), decreasing=TRUE),]
  priceData$Value = dollar(priceData$Value)
  row.names(priceData) = NULL
  
  return( capture.output(priceData) )
}


################################################################################
# Use the secdb pricing function to determine the value of a trade
#
.priceSecDbTrade <- function( eti )
{
  tradesForETI = unlist( secdb.invoke(
      "_LIB Trade Functions", "TRADE::AllTradesByETI", eti, 0, 0 ) )
  firstTrade = as.character(tradesForETI[1])
  price = secdb.getValueType(firstTrade, "Price")
  
  rDebug("Returning", price, "for ETI:", eti)
  
  return(price)
}


################################################################################
# Use the TradeInfo class (and therefore CPSProd) to price trades
#
.priceTradeViaATM <- function( eti )
{
  tradeInfo = TradeInfo$fromETI( eti, extended=TRUE )
  if( nrow(tradeInfo) == 0 )
  {
    return(NA)
  }
  
  value = tradeInfo$PRICE * tradeInfo$QUANTITY
  
  # We may have multiple prices for a trade, if so, throw a warning and
  # take the first one.
  uniqueValues = unique(value)
  if( length(uniqueValues) != 1 )
  {
    rWarn("Multiple valuations for trade:", eti, "{", uniqueValues, "}.",
      "Returning largest value.")
  }
  price = max(uniqueValues)
  
  rDebug("Returning", price, "for ETI:", eti)
  return( price )
}


################################################################################
# Take a given file and apply the specified pricing function to it
# 
.priceDeletedTradesFile <- function( deletedTradesFile, priceFunction )
{
  # Extract out the ETIs
  etiList = .getETIList(deletedTradesFile)
  if( length(etiList) == 0 )
  {
    return("No Deleted Trades Found") 
  }
  
  # Apply the pricing function specified to price each eti
  priceList = unlist( lapply(etiList, priceFunction) )
  
  #format the price data now that you have it
  priceData = .formatPriceData(etiList, priceList)
  
  return(priceData)
}


################################################################################
DeletedTradeAnalysis$run <- function()
{
  #setLogLevel(RLoggerLevels$LOG_DEBUG)
  
  setwd("S:/CPS_Controllers/Deleted Trades")
  
  asOfDate = format(Sys.Date()-1, "%Y%m%d")
  
  rLog("All Deleted Trades For", asOfDate, "\n")
  
  
  rLog("Pricing SecDb Deleted Trades:")
  secdbFile = paste(asOfDate, "_SDB_Deleted_Trades.xls", sep="")
  secdbPriceData = .priceDeletedTradesFile( secdbFile, .priceSecDbTrade )
  rLog(secdbPriceData, sep="\n")
  
  rLog("Pricing DealNet Deleted Trades:")
  dealNetFile = paste(asOfDate, "_DNet_Deleted_Trades.xls", sep="")
  dealNetPriceData = .priceDeletedTradesFile( dealNetFile, .priceTradeViaATM )
  rLog(dealNetPriceData, sep="\n")
  
  rLog("Pricing RightAngle Deleted Trades:")
  raFile = paste(asOfDate, "_RtAngle_Deleted_Trades.xls", sep="")
  rightAnglePriceData = .priceDeletedTradesFile( raFile, .priceTradeViaATM )
  rLog(rightAnglePriceData, sep="\n")
  
  rLog("Done.") 
}


################################################################################
DeletedTradeAnalysis$main <- function()
{
  body = paste(capture.output(DeletedTradeAnalysis$run()), collapse="\n")

  body = paste( body, 
    "\n\nCCG - Risk Management Group",
    "Risk Analytics & Trading Controls",
    "John.Scillieri@constellation.com", sep="\n")

  subject = "Deleted Trades Analysis Report"
  toList = c( "john.scillieri", "adrian.dragulescu", 
              "joseph.deluzio", "margot.everett", "sailesh.buddhavarapu" )
  toList = "john.scillieri"
  toList = paste( paste(toList, "@constellation.com", sep=""), collapse=", ")

  sendEmail(from="CondoR-AUTOMATED@constellation.com",
    to=toList, subject, body)
}


################################ MAIN EXECUTION ################################
.start = Sys.time()
DeletedTradeAnalysis$main()
Sys.time() - .start
