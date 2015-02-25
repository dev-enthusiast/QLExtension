################################################################################
# This code displays a constantly updating graph representing the 'live' IVaR
# for a given portfolio. It calculates this by reading the previous night's IVaR
# data and merging that with a changed position list populated by the Risk 
# Server. 
#

################################################################################
# External Libraries
#
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( "SecDb" )


################################################################################
# File Namespace
#
LiveIVaR = new.env(hash=TRUE)


################################################################################
# Takes a portfolio name and a value for how often you'd like to run the
# update code.
#
LiveIVaR$main <- function( portfolio, updateIntervalInSeconds=300 )
{
  HOURS_TO_RUN = 8

  startTime = strptime("09:00:00", format="%H:%M:%S")
  byValue = paste(updateIntervalInSeconds, "sec")
  #+1 to catch the 5pm period
  numberOfPeriods = ((HOURS_TO_RUN*60*60)/updateIntervalInSeconds)+1
  timeRange = seq(from=startTime, by=byValue, length.out=numberOfPeriods)
  
  bookList = LiveIVaR$.getBooksForPortfolio(portfolio)
  asOfDate = secdb.dateRuleApply(Sys.Date(), "-1b")
  ivarData = LiveIVaR$.loadIVaRDataForPortfolio(asOfDate, portfolio)
  
  data = data.frame(timeRange, value=NA)
  for (x in timeRange)
  {
    index = which(x == data$timeRange)
  
    value = LiveIVaR$.getCurrentVaR(bookList, ivarData)/1000000
    
    data$value[index] = value
    data$timeRange[index] = Sys.time()
  
    title = paste(portfolio, "-", prettyNum(value, digits=5), "million")
  
    plot(data, type="l", main=title)
    points(data, pch=20)
  
    Sys.sleep(updateIntervalInSeconds)   # go to sleep
  }
}




########################### INTERNAL OBJECTS ###################################


################################################################################
# This returns the list of changes from the database table populated by the 
# risk server.  The table is updated approximately every minute.
#
LiveIVaR$.getDeltaList <- function( bookList )
{
  chan = odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")

  totalDeltas = NULL

  whereClause = paste("book = '", bookList, "' or ", sep="", collapse="")
  whereClause = substr(whereClause, 1, nchar(whereClause)-4)
  query = paste("select COMMOD_CURVE, CONTRACT_DATE, DELTA from ",
    "stratdat.rs_snapshot where ", whereClause, sep="")
  totalDeltas = sqlQuery(chan, query)
  
  if(nrow(totalDeltas) != 0)
  {
    aggregateDeltas = aggregate( totalDeltas$DELTA,
    by=list( COMMOD_CURVE=totalDeltas$COMMOD_CURVE,
             CONTRACT_DATE=totalDeltas$CONTRACT_DATE ), sum )
  } else
  {
    aggregateDeltas = totalDeltas
  }
  names(aggregateDeltas) = c("curve.name", "contract.dt", "position")
  aggregateDeltas$contract.dt = as.Date(aggregateDeltas$contract.dt)
  odbcClose(chan)
  return(aggregateDeltas)
}


################################################################################
# This returns a list of all the books associated for a given portfolio.
# It recurses through each portfolio definition in SecDb and stops when the
# returned list consists of books only (no portfolios)
#
LiveIVaR$.getBooksForPortfolio <- function(portfolio)
{
  totalBookList = NULL
  securityList = secdb.getValueType(portfolio, "Securities")
  if( is.null(securityList) )
  {
    return(portfolio)
  }
  else
  {
    for( security in securityList )
    {
       totalBookList = append(totalBookList, 
        LiveIVaR$.getBooksForPortfolio(security))
    }
  }
  return(totalBookList)
}


################################################################################
# Calculates if any positions are found in the delta list that were not in the
# previous day's IVaR data.
#
LiveIVaR$.getAddedPositions <- function(IVaRData, deltaData)
{
  fromIVaR = IVaRData[,c(1,2)]
  fromDeltas = deltaData[,c(1,2)]

  fdPairList = paste(fromDeltas$COMMOD_CURVE, fromDeltas$CONTRACT_DATE)
  fiPairList = paste(fromIVaR$COMMOD_CURVE, fromIVaR$CONTRACT_DATE)

  addedPositions = setdiff(fdPairList, fiPairList)
  
  return(addedPositions)
}


################################################################################
# Uses the R IVaR data calculated the night before.
#
LiveIVaR$.loadIVaRDataForPortfolio <- function( asOfDate, portfolioName )
{
  fileName = paste("S:/All/Risk/Reports/VaR/prod/", asOfDate, "/",
    gsub(" ", ".", portfolioName), "/IVaR.", gsub(" ", ".", portfolioName),
    ".xls", sep="")

  if (!require(RODBC))
  {
    source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
    load.packages("RODBC")
  }

  connection = odbcConnectExcel(fileName)
  IVaRData = sqlFetch(connection, "Changes")

  IVaRData = IVaRData[ -(which(IVaRData$"vol#type" == "D")) ,]
  IVaRData$"vol#type" = NULL
  
  names(IVaRData)[1:3] = c("curve.name", "contract.dt", "position")
  odbcClose(connection)
  return(IVaRData)
}


################################################################################
# Performs the actual var calculation by merging the delta positions and the 
# previous nights IVaR data. 
#
LiveIVaR$.getCurrentVaR <- function( bookList, ivarData )
{
  deltaList = LiveIVaR$.getDeltaList(bookList)

  updatedData = merge( ivarData, deltaList, by=c("curve.name", "contract.dt"),
    all.x=TRUE)

  notUpdated = which(is.na(updatedData$position.y))

  updatedData$position.y[notUpdated] = updatedData$position.x[notUpdated]

  updatedData$position.x = updatedData$position.y

  names(updatedData)[which(names(updatedData)=="position.x")] = "position"

  #only find the columns named as dates
  priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]", 
    names(updatedData), extended=TRUE, value=TRUE)
    
  priceChangeData = updatedData[,priceChangeColumns]
  vars = apply( priceChangeData * updatedData$position, 2, sum )

  finalVaR = sd(vars)*4

  return(finalVaR)
}



################################ MAIN EXECUTION ################################

LiveIVaR$main( 
  portfolio = "JKirkpatrick Trading Portfolio", 
  updateIntervalInSeconds = 300 
)




