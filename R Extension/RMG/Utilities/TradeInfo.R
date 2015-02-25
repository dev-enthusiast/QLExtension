################################################################################
# Utility Class to read ATM trade information
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/sendemail.R")
source("H:/user/R/RMG/Utilities/mkdir.R")
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( c("RODBC", "SecDb") )


################################################################################
# File Namespace
#
TradeInfo = new.env(hash=TRUE)


################################################################################
# Read an External Trade Identifier and return all the info in CPSProd for that
# trade. If extended = true it also returns trade leg information.
#
TradeInfo$fromETI <- function( ETI, extended=FALSE )
{
  simpleQuery = paste(
    "select * from tradeauth.atm_trades where ",
    "EXTERNAL_TRADE_ID='", as.character(ETI), "'",
    sep=""
  )

  extendedQuery = paste(
    "select * from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
    "a.EXTERNAL_TRADE_ID='", as.character(ETI), "' and a.AT_ID=b.AT_ID",
    sep=""
  )
  
  query = ifelse( extended, extendedQuery, simpleQuery )

  connectionString <- paste(
    "FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",
    ";UID=tradeauthread;PWD=tradeauthread;", sep="")
  channel = odbcDriverConnect(connectionString)
  data = sqlQuery(channel, query)
  odbcClose(channel)
  
  return(data)
}


################################################################################
# Read a secdb trade id and return all the info in CPSProd for that
# trade. If extended = true it also returns trade leg information.
#
TradeInfo$fromID <- function( ID, extended=FALSE )
{
  simpleQuery = paste(
    "select * from tradeauth.atm_trades where ",
    "TRADE_ID='", as.character(ID), "'",
    sep=""
  )

  extendedQuery = paste(
    "select * from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
    "a.TRADE_ID='", as.character(ID), "' and a.AT_ID=b.AT_ID",
    sep=""
  )
  
  query = ifelse( extended, extendedQuery, simpleQuery )

  connectionString <- paste(
    "FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",
    ";UID=tradeauthread;PWD=tradeauthread;", sep="")
  channel = odbcDriverConnect(connectionString)
  data = sqlQuery(channel, query)
  odbcClose(channel)
  
  return(data)
}


################################################################################
# Read a trader id and return all the info in CPSProd for that
# trade. If extended = true it also returns trade leg information.
#
TradeInfo$fromTrader <- function( traderID, asOfDate, extended=TRUE )
{
  simpleQuery = paste(
    "select * from tradeauth.atm_trades where ",
    "RECORD_TIME>TO_DATE('", asOfDate, " 00:00:00', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "RECORD_TIME<TO_DATE('", asOfDate, " 23:59:59', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "lower(TRADER_ID) = '",tolower(traderID),"'",
    sep=""
    )

  extendedQuery = paste(
    "select * from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
    "a.AT_ID = b.AT_ID ",
    "and ",
    "a.RECORD_TIME>TO_DATE('", asOfDate, " 00:00:00', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "a.RECORD_TIME<TO_DATE('", asOfDate, " 23:59:59', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "lower(TRADER_ID) = '",tolower(traderID),"'",
    sep=""
    )

  query = ifelse( extended, extendedQuery, simpleQuery )

  chan = odbcConnect("CPSPROD", "tradeauthread", "tradeauthread")
  data = sqlQuery(chan, query)
  odbcClose(chan)

  return(data)

}


################################################################################
# Read a book name  and asOfDate and return all the info in CPSProd for that
# book. If extended = true it also returns trade leg information.
#
TradeInfo$fromBook <- function( bookName, asOfDate, extended=TRUE )
{
  simpleQuery = paste(
    "select * from tradeauth.atm_trades where ",
    "RECORD_TIME>TO_DATE('", asOfDate, " 00:00:00', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "RECORD_TIME<TO_DATE('", asOfDate, " 23:59:59', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "lower(portfolio1) = '",tolower(bookName),"'",
    sep=""
    )

  extendedQuery = paste(
    "select * from tradeauth.atm_trades a, tradeauth.atm_trade_leg b where ",
    "a.AT_ID = b.AT_ID ",
    "and ",
    "a.RECORD_TIME>TO_DATE('", asOfDate, " 00:00:00', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "a.RECORD_TIME<TO_DATE('", asOfDate, " 23:59:59', 'YYYY-MM-DD HH24:MI:SS') ",
    "and ",
    "lower(portfolio1) = '",tolower(bookName),"'",
    sep=""
    )

  query = ifelse( extended, extendedQuery, simpleQuery )

  chan = odbcConnect("CPSPROD", "tradeauthread", "tradeauthread")
  data = sqlQuery(chan, query)
  odbcClose(chan)

  return(data)

}


################################################################################
# Append a term label for a given set of trades
#
TradeInfo$appendTerm <- function( tradeData )
{
  tradeData$TERM = NA
  for( rowIndex in seq_len(nrow(tradeData)) )
  {
    rowData = tradeData[rowIndex,]
    tradeData$TERM[rowIndex] = TradeInfo$getTerm( rowData$START_DATE, 
      rowData$END_DATE )
  }

  return(tradeData)
}


################################################################################
# Get the term label for a given start and end date
TradeInfo$getTerm <- function( startDate, endDate )
{

  # if you can't convert it to a date, you get a nice big fat error here
  startDate = as.Date(startDate)
  endDate = as.Date(endDate)
  
  # swap the dates if they're in the wrong order
  outOfOrder = which( startDate > endDate )
  temp = endDate[outOfOrder]
  endDate[outOfOrder] = startDate[outOfOrder]
  startDate[outOfOrder] = temp

  prefix = .getTermPrefix( startDate, endDate )
  
  suffix = .getTermSuffix( startDate, endDate )

  termList = paste(prefix, suffix)

  return(termList)
}

################################################################################
.getTermPrefix <- function( startDate, endDate )
{
  # set up the comparison variables
  startMonth = as.integer( format(startDate, "%m") )
  endMonth = as.integer( format(endDate, "%m") )

  # set up the list of returned terms
  termList = character( length(startDate) )
  
  oneMonthTerm = which( startMonth == endMonth )
  termList[oneMonthTerm] = format( startDate[oneMonthTerm], "%b" )

  calStrip = which( startMonth == 1 & endMonth == 12 )
  termList[calStrip] = format( startDate[calStrip], "Cal" )

  winterStrip = which( startMonth == 1 & endMonth == 2 )
  termList[winterStrip] = format( startDate[winterStrip], "Win" )
  
  winterStrip = which( startMonth == 11 & endMonth == 3 )
  termList[winterStrip] = format( startDate[winterStrip], "Win" )

  springStrip = which( startMonth == 3 & endMonth == 4 )
  termList[springStrip] = format( startDate[springStrip], "Spr" )

  summerStrip = which( startMonth == 7 & endMonth == 8 )
  termList[summerStrip] = format( startDate[summerStrip], "Sum" )

  ngSummerStrip = which( startMonth == 4 & endMonth == 10 )
  termList[ngSummerStrip] = format( startDate[ngSummerStrip], "Sum" )

  q1Strip = which( startMonth == 1 & endMonth == 3 )
  termList[q1Strip] = format( startDate[q1Strip], "Q1" )

  q2Strip = which( startMonth == 4 & endMonth == 6 )
  termList[q2Strip] = format( startDate[q2Strip], "Q2" )

  q3Strip = which( startMonth == 7 & endMonth == 9 )
  termList[q3Strip] = format( startDate[q3Strip], "Q3" )

  q4Strip = which( startMonth == 10 & endMonth == 12 )
  termList[q4Strip] = format( startDate[q4Strip], "Q4" )
  
  return( toupper(termList) )
}


################################################################################
.getTermSuffix <- function( startDate, endDate )
{
  termList = character(length(startDate))
  
  termList = lapply( seq_along(startDate), function(x){
    yearRange = seq(from=startDate[x], to=endDate[x], by="year")
    truncated = format(yearRange, "%y")
    return( paste(truncated, collapse="/") )
  } )

  termList = unlist( termList )

  return(termList)
}


################################################################################
.testGetTerm <- function()
{
  print( getTerm("2008-01-01", "2008-01-01") == "JAN 08" )
  print( getTerm("2010-02-01", "2010-02-28") == "FEB 10" )
  print( getTerm("2008-01-01", "2008-12-31") == "CAL 08" )
  print( getTerm("2008-01-01", "2008-02-29") == "WIN 08" )
  print( getTerm("2008-11-01", "2009-03-31") == "WIN 08/09" )
  print( getTerm("2008-03-01", "2008-04-30") == "SPR 08" )
  print( getTerm("2008-07-01", "2008-08-31") == "SUM 08" )
  print( getTerm("2008-04-01", "2008-10-31") == "SUM 08" )
  print( getTerm("2008-01-01", "2008-03-31") == "Q1 08" )
  print( getTerm("2008-04-01", "2008-06-30") == "Q2 08" )
  print( getTerm("2008-07-01", "2008-09-30") == "Q3 08" )
  print( getTerm("2008-10-01", "2008-12-31") == "Q4 08" )
  print( getTerm("2008-01-01", "2009-01-01") == "JAN 08/09" )

  starts = c("2009-01-01", "2010-04-01")
  ends = c("2009-12-31", "2010-06-30")
  print( getTerm(starts, ends) == c("CAL 09", "Q2 10") )
}

