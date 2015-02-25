################################################################################
library("filehash")
library("RODBC")
source("H:/user/r/rmg/energy/var/overnight/overnightutils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Packages/load.packages.R")
load.packages( "SecDb" )

options = overnightUtils.loadOvernightParameters(overnightUtils.getAsOfDate())

filehashOption(defaultType = "RDS")      # need this, dbInit will fail
db  <- dbInit("//nas-msw-07/rmgapp/Prices/Historical")
hPP <- db2env(db)
if (!is.environment(hPP))
{
  rError("Could not load the price filehash environment.\n")
  stop("Exiting.\n")
}  



################################################################################
# Get the current positions for a given book or list of books
#
getLivePositions <- function( bookList )
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
# Calculate the var for a given book using the standard overnight options set.
#
getVaRForBook <- function(book, options)
{
  deltaList = getLivePositions(book)

  if( nrow(deltaList) != 0 )
  {
    allBookData = getMarksFromR( deltaList, options )
    if(is.null(allBookData))
    {
      finalVaR = NA
    } else
    {
      priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
        names(allBookData), extended=TRUE, value=TRUE)

      prices = allBookData[,priceChangeColumns]
      pricesTZero = prices[ , 2:length(prices) ]
      pricesTMinus1 = prices[ , 1:(length(prices)-1)]
      changeData = pricesTZero-pricesTMinus1

      pnlChanges = changeData * allBookData$position

      finalVaR = varFromPNLChanges(pnlChanges)
    }
    
  } else
  {
    finalVaR = 0
  }

  return(finalVaR)
}



################################################################################
# Function to use the file hash price data set and extract the price history
# for a given set of positions
#
getMarksFromR <- function(QQ, options)
{
   calendar <- "CPS-BAL"
   skipWeekends <- FALSE
   ldays <- lapply(as.list(as.character(options$calc$hdays)), as.Date)
   aux <- secdb.invoke("AAD: IsHoliday", "IsHoliday", ldays, calendar, skipWeekends)
   options$calc$hdays <- options$calc$hdays[unlist(aux)==0]
   if (length(options$save$prices.mkt.dir)==0){
     options$save$prices.mkt.dir <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"}

   options$calc$hdays <- sort(options$calc$hdays)       # not necessary
   ind <- which(options$calc$hdays > options$asOfDate)  # if you get carried away
   if (length(ind)>0){options$calc$hdays <- options$calc$hdays[-ind]}

   
   if(nrow(QQ)==0)
   {
    return(NULL)
   }

   QQ$curve.name <- toupper(QQ$curve.name)
   if ("vol.type" %in% names(QQ)){                   # user wants vols
     db  <-  dbInit("R:/Vols/Historical")
     if (!is.environment(hVV)){                      # if not cached
       hVV <<- db2env(db)  # put all vols in a global environment (takes 5 seconds)
     }
     hPV <- hVV
   } else {                                          # user wants prices
     db  <- dbInit("R:/Prices/Historical")
     if (!is.environment(hPP)){                      # if not cached
       hPP <<- db2env(db)  # put all prices in a global environment (takes 5 seconds)
     }
     hPV <- hPP
   }
   dimnames(QQ)[[1]] <- 1:nrow(QQ)       # if you removed some rows
   # create an empty results matrix to be filled
   res <- matrix(NA, nrow=nrow(QQ), ncol=length(options$calc$hdays),
     dimnames=list(NULL, as.character(options$calc$hdays)))

   uCurves <- sort(unique(QQ$curve.name))
   for (ind.c in 1:length(uCurves))
   {  
      curveName = uCurves[ind.c]        

      #if(uCurves[ind.c]=="COMMOD PWJ 5X16 WEST PHYSICAL") browser()
      aux <- subset(QQ, QQ$curve.name==curveName)
      curveData = hPV[[tolower(curveName)]]
      if( is.null(curveData) )
      {
        rLog("Missing curve data for curve:", curveName)
        next
      }
      bux <- merge(aux, curveData)  # all historical data
      ind.n <- which(names(bux) %in% as.character(options$calc$hdays))
      bux <- bux[,ind.n] # get only the history you want from the available data
      correctOrder = rownames(aux[order(aux$curve.name, aux$contract.dt),])
      res[as.numeric(correctOrder), colnames(bux)] <- as.matrix(bux)
   }
   res <- cbind(QQ, res)  # no index mangling because I use rownames(aux)
   return(res)
}



################################################################################
# basic var calculator, takes a n X m matrix of pnl changes 
#
varFromPNLChanges <- function( changeMatrix )
{
  vars = apply(changeMatrix, 2, sum)
  if( length(which(is.na(vars))) != 0 )
  {
    finalVaR = NA

  }else
  {
    finalVaR = sd(vars)*4
  }
  return(finalVaR)  
}



################################################################################
# this pulls all the live position data and uses it to calculate var for
# every book in the set.
#
calculateAllVaR <- function()
{
  chan = odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  query = "select * from stratdat.rs_snapshot order by BOOK, COMMOD_CURVE, CONTRACT_DATE"
  allPositions = sqlQuery(chan, query)
  odbcClose(chan)
  
  names(allPositions) = c("book", "curve.name", "contract.dt", "position")
  allPositions$contract.dt = as.Date(allPositions$contract.dt)
  
  posAndPriceData = getMarksFromR(allPositions, options)
  
  priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]",
    names(posAndPriceData), extended=TRUE, value=TRUE)
  
  prices = posAndPriceData[,priceChangeColumns]
  pricesTZero = prices[ , 2:length(prices) ]
  pricesTMinus1 = prices[ , 1:(length(prices)-1)]
  changeData = pricesTZero-pricesTMinus1
  
  pnlChange = changeData * posAndPriceData$position
  browser()
  dataForVaR = cbind(posAndPriceData$book, pnlChange)
  names(dataForVaR)[1] = "book"
  varBookList = split(dataForVaR, dataForVaR$book)
  varBookList = lapply(varBookList, function(dataSet){dataSet$book=NULL; dataSet})
  
  finalList = lapply(varBookList, varFromPNLChanges)
  
  return(finalList)
}

# Set up the comparison storage variables
original = NULL
current = NULL 

initialize <- function()
{
  original <<- formatVaRList(calculateAllVaR())
  
  current <<- original
}

formatVaRList <- function(varList)
{
  varList = do.call(rbind, varList)
  varList = data.frame(rownames(varList), varList)
  colnames(varList) = c("book", "var")
  rownames(varList) = NULL
  
  return(varList)  
}

getUpdatedDataSet <- function()
{
  current <<- formatVaRList(calculateAllVaR())
  
  dataSet = merge(current, original, by="book", all.x=TRUE)
  names(dataSet) = c("book", "current", "original")
  
  dataSet$delta = dataSet$current - dataSet$original
  dataSet$percentage = dataSet$delta / dataSet$original

  dataSet = dataSet[ order(dataSet$percentage, decreasing=TRUE, na.last=TRUE) ,]
  
  current <<- dataSet[,c(1,3,2,4,5)]
  
  return(4.0)
}

.getBooksForPortfolio <- function(portfolio)
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
       totalBookList = append( totalBookList, .getBooksForPortfolio(security) )
    }
  }
  return(totalBookList)
}