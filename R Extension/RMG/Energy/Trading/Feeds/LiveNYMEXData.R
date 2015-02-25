################################################################################
library(httpRequest)
library(RSQLite)
source("H:/user/R/RMG/Utilities/RLogger.R")


################################################################################
setupNewCommodTable <- function(commodName, connection)
{
  fakeRow = list()
  fakeRow$Symbol = "test"
  fakeRow$Quote = 0
  fakeRow$QuoteTime = Sys.time()
  fakeRow = data.frame(fakeRow)

  tableName = paste(commodName, "Quotes", sep="")
  dbWriteTable( connection, tableName,
    fakeRow, row.names=FALSE, append=TRUE )

  deleteCommand = paste("delete from", tableName, "where Symbol=='test'")
  res = dbGetQuery(connection, deleteCommand)

  invisible(NULL)
}


################################################################################
# Available commodities include {NG, CL, HO}
#
getNewestData <- function(commod="NG")
{
  rLog("Pulling new", commod, "Data...")
  pageLocation = paste( "/exchanges/?r=NYMEX_", commod, sep="" )
  pageData = getToHost("quotes.ino.com", pageLocation)

  TABLE_START = '<TD><A HREF="/chart/?s='
  TABLE_END = paste('<TD COLSPAN=8 ALIGN=CENTER CLASS="mini">All quotes are',
    'exchange delayed.<BR>For charts, options, and latest headline news,',
    'click on the market name.</TD>\n</TR>\n</TABLE>')

  whereToStart = regexpr(TABLE_START, pageData, fixed=TRUE)
  whereToStop = regexpr(TABLE_END, pageData, fixed=TRUE)
  quotesTable = substr(pageData, whereToStart, whereToStop)

  #we need to do this one specifically because it puts extra spaces in the HTML
  AD_REGX = "<TR><TD COLSPAN=8 ALIGN=CENTER><BR><iframe.+</iframe>\n</TD></TR>"
  quotesTable = gsub(AD_REGX, "", quotesTable, perl=TRUE)

  #remove all HTML tags
  quotesTable = gsub("(<(/?[^>]+)>)", "", quotesTable, perl=TRUE)

  #split it into separate terms
  quoteStrings = strsplit(quotesTable, "\n")[[1]]

  #parse through each term and create rows for the data frame
  data = NULL
  for( cell in 0:length(quoteStrings) )
  {
    termNumber = (cell %% 11)+1

    if( termNumber == 1 )
      Symbol = quoteStrings[cell+1]
    else if( termNumber == 2 )
      Description = quoteStrings[cell+1]
    else if( termNumber == 3 )
      Open = quoteStrings[cell+1]
    else if( termNumber == 4 )
      High = quoteStrings[cell+1]
    else if( termNumber == 5 )
      Low = quoteStrings[cell+1]
    else if( termNumber == 6 )
      Quote = quoteStrings[cell+1]
    else if( termNumber == 7 )
      Change = quoteStrings[cell+1]
    else if( termNumber == 8 )
      Percent = quoteStrings[cell+1]
    else if( termNumber == 9 )
      QuoteTime = quoteStrings[cell+1]
    else if( termNumber == 10 )
      next
    else if( termNumber == 11 )
    {
      rowData = NULL
      rowData$Symbol = Symbol
      rowData$Description = Description
      rowData$Open = Open
      rowData$High = High
      rowData$Low = Low
      rowData$Quote = Quote
      rowData$Change = Change
      rowData$Percent = Percent
      rowData$QuoteTime = QuoteTime

      data = rbind(data, data.frame(rowData))
    }

  }

  #clean up the term name
  symbolPrefix = paste( commod, ".", sep="" )
  data$Symbol = gsub(symbolPrefix, "", data$Symbol, fixed=TRUE)

  #remove any small lot size quotes
  quotesToRemove = grep(".E$", data$Symbol)
  if( length(quotesToRemove) != 0 )
  {
    data = data[ -(quotesToRemove), ]
  }
  
  #Convert the time to an easily convertible format.
  data$QuoteTime = as.character(strptime(data$QuoteTime, format="%H:%M"))

  return(data)
}


################################################################################
updateTable <- function(commod, newSet, previousSet, connection)
{
  newSet$OldTime = previousSet$QuoteTime

  #insert any quotes without previous times
  newQuotes = which(!is.na(newSet$QuoteTime) & is.na(newSet$OldTime) )
  if(length(newQuotes) > 0)
  {
    x = newSet[newQuotes, c("Symbol", "Quote", "QuoteTime")]
    insert = paste('insert into ', commod, 'Quotes values ("',
      apply(x, 1, paste, collapse='","'),
      '")', sep="")
    lapply( insert, FUN=dbGetQuery, conn=connection )
  }

  #insert any newer quotes
  toInsert = which(newSet$QuoteTime > previousSet$QuoteTime)
  if(length(toInsert) > 0)
  {
    x = newSet[toInsert, c("Symbol", "Quote", "QuoteTime")]
    insert = paste('insert into ', commod, 'Quotes values ("',
      apply(x, 1, paste, collapse='","'),
      '")', sep="")
    lapply( insert, FUN=dbGetQuery, conn=connection )
  }
  
  totalInserts = length(newQuotes) + length(toInsert)
  rLog("Inserted", totalInserts, "rows into the database.")

  invisible(NULL)
}


################################################################################
getCommodData <- function(commodName, whereClause=NULL)
{
  driver = dbDriver("SQLite")
  connection = dbConnect(driver, dbname="H:/quotes.sdb")

  tableName = paste(commodName, "Quotes", sep="")

  if( is.null(whereClause) )
  {
    data = dbReadTable(connection, tableName)
  }
  else
  {
    data = dbGetQuery( connection,
      paste( "select * from NGQuotes where", whereClause ) )
  }

  dbDisconnect(connection)

  return(data)
}


################################################################################
plotSymbol <- function(symbol, data)
{
  data$Quote = as.numeric(data$Quote)
  data$QuoteTime = as.POSIXct(data$QuoteTime)

  data = subset(data, data$Symbol==symbol)
  plot(data$Quote ~ data$QuoteTime, type="b", main=symbol)
}


################################################################################
main <- function()
{
  commodities = c("NG", "CL", "HO")

  newSet = lapply( commodities, getNewestData )

  while(TRUE)
  {
    rLog("Sleeping for 1 minutes at", as.character(Sys.time()), "...\n")
    Sys.sleep(60)
    
    previousSet = newSet
    newSet = lapply( commodities, getNewestData )

    rLog("Connecting to the database...")
    driver = dbDriver("SQLite")
    connection = dbConnect(driver, dbname="H:/quotes.sdb")

    for( commodIndex in seq_along(commodities) )
    {
      updateTable( commodities[commodIndex], newSet[commodIndex][[1]],
        previousSet[commodIndex][[1]], connection=connection )
    }
    rLog("Closing the database connection...")
    dbDisconnect(connection)
  }

}

#main()































