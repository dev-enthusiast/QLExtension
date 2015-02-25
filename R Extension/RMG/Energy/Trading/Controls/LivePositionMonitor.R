################################################################################
library(RODBC)
library(reshape)
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/rLogger.R") 
source("H:/user/R/RMG/Utilities/sendEmail.R")
source("H:/user/R/RMG/Utilities/Database/R/LiveData.R")


################################################################################
# Used to cache portfolio mappings
LPCData = new.env(hash=TRUE)

EMAIL_LIST = c( 
        "john.scillieri", "adrian.dragulescu", 
        "joseph.deluzio", "margot.everett", 
        "vladimir.gorny", "aditya.chamria", "yaxing.zhang" )
GROUP_LIST = c("#CCG RMG Portfolio MGT")

STARTING_SET = "StartingPositions"
MOST_RECENT_SET = "MostRecentPositions"



################################################################################
comparePositionSetTo <- function( positionSet, setName, priceData ) 
{
    fileName = getPositionFilePath(setName)
    
    if( !file.exists(fileName) )
    { 
        savePositionSetByName(positionSet, setName)
        return(NULL)
    } 
    
    load(fileName)
    
    positionsFromSetName = eval(parse(text=setName))
    
    comparisonData = comparePositionSets( positionSet, positionsFromSetName, priceData )
    
    invisible(comparisonData)
}


################################################################################
# compare two sets of positions, determine their difference, percent change,
# price and approximate value of the change
#
comparePositionSets <- function( currentPositions, previousPositions, priceData )
{
    currentPositions$BOOK = as.character(currentPositions$BOOK)
    currentPositions$COMMOD_CURVE = as.character(currentPositions$COMMOD_CURVE)
    currentPositions$CONTRACT_DATE = as.Date(currentPositions$CONTRACT_DATE)
    previousPositions$BOOK = as.character(previousPositions$BOOK)
    previousPositions$COMMOD_CURVE = as.character(previousPositions$COMMOD_CURVE)
    previousPositions$CONTRACT_DATE = as.Date(previousPositions$CONTRACT_DATE)
    
    # merge the starting position data with the new information
    names(previousPositions)[which(names(previousPositions)=="DELTA")] = 
            "OLD_DELTA"
    mergedData = merge( currentPositions, previousPositions, all=TRUE )
    
    # determined what actually changed and make NA values = 0 
    changedData = subset( mergedData, mergedData$OLD_DELTA != mergedData$DELTA | 
                    is.na(mergedData$OLD_DELTA) |
                    is.na(mergedData$DELTA) )
    makeZero = which(is.na(changedData$OLD_DELTA))
    changedData$OLD_DELTA[makeZero] = 0
    makeZero = which(is.na(changedData$DELTA))
    changedData$DELTA[makeZero] = 0
    
    
    # format & create the columns we need  
    changedData$DIFFERENCE = (changedData$DELTA - changedData$OLD_DELTA)
    changedData$PERCENT = (changedData$DIFFERENCE / changedData$OLD_DELTA) * 100
    changedData$BOOK = as.character(changedData$BOOK)
    changedData$CONTRACT_DATE = as.Date(changedData$CONTRACT_DATE)   
    
    # get the previous day's price for the curve and calculate the relative 
    # value of the trade
    data = merge( changedData, priceData, all.x=TRUE )
    data$DOLLAR_CHANGE = (data$PRICE * data$DIFFERENCE)
    
    return(data)
}

################################################################################
# Get the asOfDate for the whole system
# 
getAsOfDate <- function()
{
    dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    conString <- paste( dsnPath, ";UID=stratdatread;PWD=stratdatread;", sep="")
    chan = odbcDriverConnect(conString)
    asOfDate = sqlQuery(chan, 
            "select max(PRICING_DATE) from foitsox.sdb_holiday_calendar where calendar='CPS-BAL'")  
    odbcClose(chan)
    
    asOfDate = as.Date(asOfDate[1,1])
    
    if( length(asOfDate) == 0 )
    {
        rWarn("DB Connection For AsOfDate isn't working. Reverting as of date to yesterday.")
        asOfDate = Sys.Date()-1
    }
    
    return(asOfDate)
}


################################################################################
# extract the price history for a given set of positions
#
getPriceData <- function(asOfDate)
{
    require("RODBC")
    
    query <- paste("SELECT commod_curve, start_date, end_date, price ",
            "from foitsox.sdb_prices ",
            "WHERE PRICING_DATE=TO_DATE('", as.character(asOfDate), "','YYYY-MM-DD') ",
            sep="")
    
    connectionString <- paste(
            "FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn",  
            ";UID=stratdatread;PWD=stratdatread;", sep="")  
    channel = odbcDriverConnect(connectionString)
    start = Sys.time()
    hP <- sqlQuery(channel, query)  # all the prices 
    Sys.time() - start
    odbcClose(channel)
    
    hP$START_DATE <- as.Date(hP$START_DATE); hP$END_DATE <- as.Date(hP$END_DATE)
    hP$CONTRACT_DATE <- as.Date(format(hP$START_DATE, "%Y-%m-01")) 
    eom <- as.Date(format(hP$CONTRACT_DATE + 31, "%Y-%m-01"))-1  # end of the month 
    hP  <- hP[(hP$END_DATE == eom),]  # keep only the balance of the month prices
    hP$START_DATE <- hP$END_DATE <- NULL
    hP$COMMOD_CURVE <- as.character(hP$COMMOD_CURVE)
    hP <- hP[!duplicated(hP),]      
    
    return(hP)           
}


################################################################################
getParentForBook <- function( book, asOfDate )
{
    cpsParent = .getCpsprodParentForBook( book, asOfDate )
    
    if( length(cpsParent)==0 )
    {
        return( .getRmsysParentForBook( book ) )
    }
    
    return( cpsParent )
}


################################################################################
.getCpsprodParentForBook <- function( book, asOfDate )
{
    if( is.null(LPCData$CPSPROD) )
    {        
        connectionString = "FileDSN=H:/user/R/RMG/Utilities/DSN/CPSPROD.dsn;UID=stratdatread;PWD=stratdatread;"  
        channel = odbcDriverConnect(connectionString)
        
        query <- paste("SELECT * from foitsox.sdb_portfolio_hierarchy ",
                "WHERE PRICING_DATE=TO_DATE('", 
                as.character(asOfDate), "','YYYY-MM-DD')", sep="")
        LPCData$CPSPROD    <- sqlQuery(channel, query)
        odbcClose(channel)
    }
    
    parents = subset( LPCData$CPSPROD, CHILD==book & PARENT != "CPS Trading Group")
    return( as.character(parents$PARENT[nrow(parents)]) )    
}


################################################################################
.getRmsysParentForBook <- function( book )
{
    if( is.null(LPCData$RMSYS) )
    {        
        dsnPath = "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn"
        conString = paste( dsnPath, ";UID=rmsys_read;PWD=rmsys_read;", sep="")
        con = odbcDriverConnect(conString)
        
        query ="SELECT * from secdb_portfolio_books "
        LPCData$RMSYS = sqlQuery(con, query)
        odbcClose(con)
    }
    
    bookInfo = subset(LPCData$RMSYS, SECDB_NAME==book)
    parentInfo = LPCData$RMSYS[which(LPCData$RMSYS$PB_ID==bookInfo$PARENT_ID),]
    
    return( as.character(parentInfo$SECDB_NAME) )
}


################################################################################
getTopNTotalChanges <- function( data, n=30 )
{
    data = splitDataByBook(data)
    
    # total up the change data to get a general idea of the big movers
    totals = lapply(data, function(x){sum(x$DOLLAR_CHANGE)})
    totals = do.call(rbind, totals)
    totals = data.frame(BookName=rownames(totals), TotalChange=totals)
    if( nrow(totals) == 0 )
    {
        return(NULL)
    }
    
    row.names(totals) = NULL
    
    totals = totals[order(abs(totals$TotalChange), decreasing=TRUE),]
    
    return(totals[1:n,])
}


################################################################################
splitDataByBook <- function( data )
{
    data = split(data, data$BOOK)
    data = lapply(data, function(x){x$BOOK=NULL; x})
    return(data)
}


################################################################################
getPositionFilePath <- function( setName )
{
    fileName = paste( "H:/user/R/RMG/Energy/Trading/Controls/", setName, 
            ".", Sys.Date(), ".RData", sep="" )
    
    return(fileName)
}


################################################################################
savePositionSetByName <- function( positionSet, setName )
{
    assign( eval(setName), positionSet )
    
    save( list=eval(setName), file=getPositionFilePath(setName) )
    
    rLog("Position Set:", setName, "Has Been Saved.")
}


################################################################################
emailChanges <- function( startingData, recentData, asOfDate )
{
    rLog("Generating 10 Largest Book Level Changes...")
    startingBookChanges = formatTradesByBook( startingData, asOfDate )
    startingBookOutput = .getTopTenOuput(
            "10 Largest Book Level Changes Since 7am:", 
            startingBookChanges )  
    
    
    rLog("Generating 10 Largest Commod Level Changes...")
    startingCommodChanges = formatTradesByCommod( startingData )
    startingCommodOutput = .getTopTenOuput(
            "10 Largest Commod Level Changes Since 7am:", 
            startingCommodChanges )  
    
    
    rLog("Formatting Starting Position Changes >= $2M...")
    startingBigTrades = formatTrades( startingData, asOfDate, FALSE )
    startingBigTradesOutput = paste(
            "Position Changes >= $2M Since 7am:\n", 
            startingBigTrades, "\n", sep="\n" )
    
    
    rLog("Sending Email...")
    body = paste("Time:", Sys.time())
    body = paste( body, 
            startingBookOutput,
            startingCommodOutput,
            startingBigTradesOutput, 
            "CCG RMG Analytics\nJohn.Scillieri@constellation.com", sep="\n\n")
    
    subject = "Large Position Change Report"
    toList = paste( paste( EMAIL_LIST, "@constellation.com", sep="" ), collapse=", ")
    groups = paste(GROUP_LIST, collapse=", ", sep="")
    toList = paste( toList, groups, sep=", ")
    
    #toList = "john.scillieri@constellation.com"
    sendEmail(from="DO-NOT-REPLY@constellation.com",
            to=toList, subject, body)
    
    rLog(subject)
}


################################################################################
.getTopTenOuput <- function( label, changeSet )
{
    output = paste( 
            "\n\n",
            label,
            "\n", 
            paste( capture.output( format(changeSet, justify="left") ), 
                    collapse="\n" ),  
            "\n",
            sep="" )  
    
    return( output ) 
}

################################################################################
formatTradesByCommod <- function( tradeData )
{
    if( nrow(tradeData) == 0 )
    {
        return("")
    }
    
    names(tradeData)[which(names(tradeData)=="DOLLAR_CHANGE")] = "value"
    
    #aggregateData = cast(tradeData, COMMOD_CURVE+CONTRACT_DATE~., sum)
    #names(aggregateData) = c("Curve", "Contract", "TotalChange")
    
    aggregateData = cast(tradeData, COMMOD_CURVE~., sum)
    names(aggregateData) = c("Curve", "TotalChange")
    
    aggregateData = aggregateData[ 
            order(abs(aggregateData$TotalChange), decreasing=TRUE), ]
    
    aggregateData$TotalChange = lapply( aggregateData$TotalChange, 
            dollar, digits=0 )
    
    rownames(aggregateData) = NULL
    
    return( na.omit(aggregateData[1:10,]) )
}


################################################################################
formatTradesByBook <- function( tradeData, asOfDate )
{
    bookChanges = na.omit( getTopNTotalChanges(tradeData, n=10) )
    if( is.null(bookChanges) )
    {
        return(bookChanges)
    }  
    
    bookChanges$TotalChange = lapply( bookChanges$TotalChange, dollar, digits=0 )
    
    if( nrow(bookChanges) != 0 )
    {
        bookChanges$Portfolio = lapply( 
                as.character(bookChanges$BookName), 
                getParentForBook, asOfDate )
        
        bookChanges$Portfolio = sub(" Portfolio", "", bookChanges$Portfolio)
        bookChanges$Portfolio = paste(bookChanges$Portfolio, " (", 
                bookChanges$BookName, ")", sep="")
        bookChanges = bookChanges[ , c("Portfolio", "TotalChange") ]
        
        row.names(bookChanges) = NULL
    }
    
    return(bookChanges)
}


################################################################################
formatTrades <- function( tradeData, asOfDate, wideOutput=TRUE )
{
    MIN_TRADE_SIZE = 2000000
    
    bigData = subset( tradeData, 
            abs(tradeData$DOLLAR_CHANGE) >= MIN_TRADE_SIZE )
    
    sortedData = bigData[ order( abs(bigData$DOLLAR_CHANGE), decreasing=TRUE ) , ]
    
    sortedData = na.omit(sortedData)
    
    if( nrow(sortedData) == 0 )
    {
        return("")
    }
    
    dataToPrint = sortedData
    
    dataToPrint$DIFFERENCE = round(dataToPrint$DIFFERENCE, digits=0)
    dataToPrint$OLD_DELTA = round(dataToPrint$OLD_DELTA, digits=0)
    dataToPrint$DELTA = round(dataToPrint$DELTA, digits=0)
    dataToPrint$DOLLAR_CHANGE = lapply( dataToPrint$DOLLAR_CHANGE, 
            dollar, digits=0 )
    
    dataToPrint = dataToPrint[ , c("BOOK", "COMMOD_CURVE", "CONTRACT_DATE", 
                    "OLD_DELTA", "DELTA", "DIFFERENCE", "DOLLAR_CHANGE") ]
    
    row.names(dataToPrint) = NULL
    colnames(dataToPrint) = c("Book", "Curve Name", "Contract Date", "Old Pos", 
            "New Pos", "Diff", "Value")
    
    options(width=300)
    if( wideOutput )
    {
        output = capture.output(dataToPrint)
        output = paste(output, collapse="\n")
        
    } else
    {
        output = .getNarrowOutput( dataToPrint, asOfDate )
    }
    
    return(output)
}     


################################################################################
.getNarrowOutput <- function( dataToPrint, asOfDate )
{
    uniqueBooks = unique(dataToPrint$Book)
    portfolios = lapply(uniqueBooks, getParentForBook, asOfDate)
    
    mapping = data.frame(Book=uniqueBooks, Portfolio=unlist(portfolios))
    dataToPrint = merge(dataToPrint, mapping)
    
    output = NULL
    byPortfolio = split(dataToPrint, dataToPrint$Portfolio)
    for( portfolio in byPortfolio )
    {
        byBook = split( portfolio, portfolio$Book )
        for( book in byBook )
        {
            bookName = as.character(book$Book[1])

            output = paste(output, book$Portfolio[1], "(", bookName, "):\n", 
                    sep="")
            
            book = book[order(book$"Curve Name", book$"Contract Date"),]
            book$"Contract Date" = toupper( format(book$"Contract Date", "%b-%y") )
            
            for( rowIndex in seq_len(nrow(book)) )
            {
                position = book[rowIndex,]
                output = paste(output, 
                        " * ", position$"Curve Name", "(", position$"Contract Date", ")\n",
                        "    From: ", position$"Old Pos", " To: ", position$"New Pos", "\n",
                        "    Change: ", position$Diff, " Value: ", position$Value, "\n",  
                        sep="")
            } 
            output = paste(output, "\n") 
            
        }
    }
}


################################################################################
main <- function()
{
    asOfDate = getAsOfDate()
    
    # Pull the current position set
    rLog("Pulling current positions...")
    positionSet = LiveData$allPositions()
    
    rLog("Pulling current prices...")
    priceData = getPriceData(asOfDate)
    
    # Do the run for the start of day position set
    rLog("Comparing current positions to the start-of-day positions...")
    startingData = comparePositionSetTo(positionSet, STARTING_SET, priceData)
    
    # Now do it for the most recent positions
    rLog("Comparing current positions to the most-recently-saved positions...")
    recentData = comparePositionSetTo(positionSet, MOST_RECENT_SET, priceData) 
    
    rLog("Generating output and emailing changes...")
    if( !is.null(startingData) && !is.null(recentData) )
    {
        emailChanges( startingData, recentData, asOfDate )
    }
    
    # Now overwrite the most recent positions so the run works as expected
    # the next time.
    rLog("Overwriting most-recently-saved position data for next run...")
    savePositionSetByName(positionSet, MOST_RECENT_SET)
    
    rLog("Done.")
}             


################################ MAIN EXECUTION ################################
.start = Sys.time()
main()
print(Sys.time() - .start)

