################################################################################
# Displays and saves data for trade history by a given date. 
#

################################################################################
# External Libraries
#
library(RODBC)
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( "SecDb" )
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/mkdir.R")
source("H:/user/R/RMG/Energy/Trading/Controls/AfterHoursTraderList.R")



################################################################################
# File Namespace
#
TradeHistByTime = new.env(hash=TRUE)


################################################################################
TradeHistByTime$totitle <- function(x) 
{
  s <- strsplit(tolower(x), " ")
  capFirstLetters = lapply( lapply(s, substring, 1, 1), toupper)
  restOfWords = lapply( s, substring, 2 )
  fullWords = NULL
  for( wordIndex in seq_along(restOfWords) )
  {
    fullWords[[wordIndex]] = paste( capFirstLetters[[wordIndex]], 
      restOfWords[[wordIndex]], sep="", collapse=" " )
  }
  
  return(fullWords)
}


################################################################################
# This determines a good number of traders to show per plot
#
TradeHistByTime$smartSize <- function(numTraders)
{
  MIN_SIZE = 8  
  MAX_SIZE = 20
  
  tryList = MIN_SIZE:MAX_SIZE
  
  bestDifference = 1000000
  size = 0
  for( toTry in tryList )
  {
    remainder = numTraders %% toTry
    difference = toTry - remainder

    if( (difference <= bestDifference) )
    {
      size = toTry
      bestDifference = difference
    }
  }
  
  return(size)
}


################################################################################
# Display scatter of trades entered by time by trader
#
TradeHistByTime$plotTradeTimeScatters <- function(data, asOfDate, saveToFile)
{
  uniqueTraders = sort(as.character(unique(data$name)), decreasing=TRUE)
  
  #how many traders to draw per plot
  DISPLAY_NUM = TradeHistByTime$smartSize(length(uniqueTraders))
  
  dataToPlot = NULL
  plotNumber = 1
  for( traderIndex in 1:length(uniqueTraders) )
  {
    traderName = uniqueTraders[traderIndex]
    traderData = subset(data, data$name==traderName)

    traderData$traderIndex = traderIndex
    traderData$name = paste(traderData$name, " (", traderData$group, ")", sep="")
    
    dataToPlot = rbind(dataToPlot, traderData)
  
    if( traderIndex %% DISPLAY_NUM == 0 ||
        traderIndex == length(uniqueTraders) )
    {
      TradeHistByTime$.drawPlot( dataToPlot, asOfDate, plotNumber, saveToFile )
      
      plotNumber = plotNumber+1
      dataToPlot=NULL
    }
    
  }
}


################################################################################
# Query the trader table in CPS prod to get trader information
#
TradeHistByTime$nameFromENum <- function( employeeNum, appendGroup=FALSE )
{
  rDebug("Connecting to the database...")
  connectionString <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
       ";UID=tradeauthread;PWD=tradeauthread;", sep="")  
  chan = odbcDriverConnect(connectionString)
  
  query = paste( "select * from tradeauth.trader where lower(ceg_id)='", 
    tolower(employeeNum), "'", sep="" )
    
  # only take the first row because there are eqivalent duplicates in the table
  data = sqlQuery(chan, query)[1,]
  odbcClose(chan)
  
  nameString = data$TRADER_NAME
  
  if( appendGroup )
  {
    nameString = paste(nameString, " (", data$GROUP_ID, ")", sep="") 
  } 
  
  return( as.character(nameString) )
}


################################################################################
# Draw the scatter of trader activity, useful for subbing out the drawing 
# routine without changing when it's displayed.
#
TradeHistByTime$.drawPlot <- function( dataToPlot, asOfDate, plotNumber,
  saveToFile )
{
  if( !saveToFile )
  {
    windows()
  } else
  {
    basePath = paste( "S:/All/Risk/Reports/TradingControlReports/",
      "TradeHistByTime/", asOfDate, sep="" )
    mkdir( basePath )
    if(nchar(as.character(plotNumber))==1)
    {
      plotNumber = paste("0", plotNumber, sep="")
    }
    fileName = paste( basePath, "/TradesByTrader-", plotNumber, ".pdf", sep="" )
    rLog("Saving file:", fileName)
    pdf(fileName)
  }
  
  par( mar=c(5, 9, 4, 2) + 0.1 )
  
  plot( y=dataToPlot$traderIndex, x=dataToPlot$RECORD_TIME,
    xaxt="n", yaxt="n", las=1, cex.axis=.7, ylab="",
    main=paste("Distribution of Trades Entered By Trader\n",asOfDate) )

  axis.POSIXct(1, dataToPlot$RECORD_TIME, format="%I:%M%p")
  
  axis(2, at=dataToPlot$traderIndex, labels=dataToPlot$name, las=1,
    cex.axis=.7 )
  
  fromTime = as.POSIXct(format(asOfDate, "%Y-%m-%d 00:00:00"))
  toTime = as.POSIXct(format(asOfDate+1, "%Y-%m-%d 00:00:00"))
  breaks = seq(fromTime, toTime, by="hour")
  abline(v=breaks, col="light grey")
  
  sob = as.POSIXct(format(asOfDate, "%Y-%m-%d 07:00:00"))
  cob = as.POSIXct(format(asOfDate, "%Y-%m-%d 19:00:00"))
  abline(v=c(sob, cob), col="blue")
  
  if( saveToFile ) 
  {
    dev.off()
  }
}


################################################################################
# Simple histogram for # of trades entered by time
#
TradeHistByTime$plotTradeByTimeHistogram <- function(data, asOfDate, saveToFile)
{

  fromTime = as.POSIXct(format(asOfDate, "%Y-%m-%d 00:00:00"))
  toTime = as.POSIXct(format(asOfDate+1, "%Y-%m-%d 00:00:00"))
  breaks = seq(fromTime, toTime, by="hour")
  
  if( !saveToFile )
  {
    windows()
  } else
  {
    basePath = paste( "S:/All/Risk/Reports/TradingControlReports/",
      "TradeHistByTime/", asOfDate, sep="" )
    mkdir( basePath )
    
    fileName = paste( basePath, "/TradesByTime.pdf", sep="" )
    rLog("Saving file:", fileName)
    pdf(fileName)
  }
  
  hist(data$RECORD_TIME, breaks, freq=TRUE, main="Trades Entered By Time",
    labels=TRUE, xlab="Time", ylab="# of Trades" )
    
  if( saveToFile ) 
  {
    dev.off()
  }

}

################################################################################
# If today's count is not less than the rowCountLeeway, send the warning email
#
TradeHistByTime$.sendStatus <- function( asOfDate )
{
  body = paste("As Of Date:", asOfDate)
  location = paste( "The Trade History By Time Reports Can Be Found At:\n\t", 
    "S:\\All\\Risk\\Reports\\TradingControlReports\\TradeHistByTime\\", 
    asOfDate, "\\", sep="" )
  body = paste(body, location, 
    "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")

  subject = paste("SUCCESS: ", asOfDate, " Trade History By Time Reports ",
    sep="")

  toList ="john.scillieri@constellation.com"

  sendEmail(from="OVERNIGHT-DO-NOT-REPLY@constellation.com",
    to=toList, subject, body)
  
  rLog(subject)
}

################################################################################
TradeHistByTime$afterHoursCheck <- function(asOfDate, data, nameList)
{

  afterHours = subset( data, 
    data$RECORD_TIME <= as.POSIXct(format(asOfDate, "%Y-%m-%d 07:00:00")) |
    data$RECORD_TIME >= as.POSIXct(format(asOfDate, "%Y-%m-%d 19:00:00")) )
  
  unauthorizedTrades = subset(afterHours, !is.element(name, nameList))
  unauthorizedTrades = unauthorizedTrades[
    order(unauthorizedTrades$RECORD_TIME), ]
  
  colsToShow = c( "name", "RECORD_TIME", "PORTFOLIO1", "PORTFOLIO2", "ETI",
    "CURVE_NAME", "START_DATE", "END_DATE", "QUANTITY", "PRICE" )
  byName = split(unauthorizedTrades[,colsToShow], unauthorizedTrades$name)
  
  #print(byName)
  for( traderData in byName )
  {
    rLog( "Trader:", traderData$name[1], "placed", nrow(traderData),
      "after hours trades.") 
  }
  
  basePath = paste( "S:/All/Risk/Reports/TradingControlReports/",
      "TradeHistByTime/", asOfDate, sep="" )
  mkdir( basePath )
  fileName = paste( basePath, "/Unauthorized After Hours Traders.txt", sep="" )
  rLog("Saving file:", fileName)
  
 # writeLines( paste(
 #   "The following traders performed trades outside regular hours:\n", 
  #  paste(unauthorizedTraders,collapse="\n"), sep="" ),
 #   con = fileName  )

}


################################################################################
TradeHistByTime$main <- function()
{
  saveToFile = !interactive()

  rLog("Connecting to the database...")
  connectionString <- paste("FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
       ";UID=tradeauthread;PWD=tradeauthread;", sep="")  
  chan = odbcDriverConnect(connectionString)
  
  asOfDate = Sys.Date()-1
  
  query = paste(
    "select * from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
    "a.AT_ID=b.AT_ID and",
    "a.RECORD_TIME>TO_DATE('", asOfDate, "00:00:00', 'YYYY-MM-DD HH24:MI:SS')",
    "and",
    "a.RECORD_TIME<TO_DATE('", asOfDate, "23:59:59', 'YYYY-MM-DD HH24:MI:SS')"
    )
  
  rLog("Retrieving data...")
  data = sqlQuery(chan, query)
  data$TRADER_ID = tolower(data$TRADER_ID)

  # pull trader name information
  traderQuery = "select unique(ceg_id), trader_name, group_id from tradeauth.trader"
  traderTable = sqlQuery(chan, traderQuery)
  traderTable$CEG_ID = tolower(traderTable$CEG_ID)
  traderTable$TRADER_NAME = as.character(traderTable$TRADER_NAME)
  traderTable$GROUP_ID = as.character(traderTable$GROUP_ID)
  names(traderTable) = c("TRADER_ID", "name", "group")
  
  data = merge(data, traderTable) 
  
  odbcClose(chan)
  
  rLog("Checking for after-hours trades...")
  TradeHistByTime$afterHoursCheck(asOfDate, data, AfterHoursTraderList)

  
  rLog("Plotting data...")
  TradeHistByTime$plotTradeTimeScatters(data, asOfDate, saveToFile)
  TradeHistByTime$plotTradeByTimeHistogram(data, asOfDate, saveToFile)
  
  if( saveToFile )
  {
    TradeHistByTime$.sendStatus(asOfDate)
  }
  
  rLog("Done.")
}

################################ MAIN EXECUTION ################################
.start = Sys.time()
TradeHistByTime$main()
Sys.time() - .start
