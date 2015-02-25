################################################################################
# Globals (I know, I'm all ears for a better way to implement a 'class'
#
#connection = NULL


################################################################################
#
ivarInterface.loadFile <- function(fileName)
{

  if (!require(RODBC))
  {
    source("H:/user/R/RMG/Utilities/Packages/load.packages.r")
    load.packages("RODBC")
  }
  
  connection <<- odbcConnectExcel(fileName)
  IVaRData <<- sqlFetch(connection, "Changes")
  
  # The table that holds all the added trades from the calculator
  # We want these separate from the gui
  tradePositionData <<- NULL
      
  curveList <<- unique(IVaRData$"curve#name")
  
  return("File Loaded.")
}

################################################################################
#
ivarInterface.getCurveList <- function()
{
  if ( !("curveList" %in% ls(envir=.GlobalEnv)) )
  {
    curveList <<- unique(IVaRData$"curve#name")
  }
  as.character(curveList)
}

################################################################################
#
ivarInterface.getContractDatesForCurve <- function(curveName)
{
  dateList = unique(subset(IVaRData, IVaRData[,1]==curveName)[,2])

  as.character(sort(dateList))
}

################################################################################
#
ivarInterface.calculateIVaR <- function()
{
  if (!is.null(tradePositionData))
  {
    allIVaR = rbind(IVaRData, tradePositionData)
  }else
  {
    allIVaR = IVaRData
  }

  positionData = allIVaR$position
  
  #only find the columns named as dates
  priceChangeColumns = grep("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]", 
              names(allIVaR), extended=TRUE, value=TRUE)
  
  priceChangeData = allIVaR[,priceChangeColumns]
  vars = apply( priceChangeData * positionData, 2, sum )
  
  #reset the position table for the next calculation
  tradePositionData <<- NULL
  
  return(sd(vars)*4) 
}

################################################################################
#
ivarInterface.addPosition <- function(curveName, contractDate, positionValue)
{
  # Find the existing curve data
  correctCurves = subset(IVaRData, IVaRData[,1]==curveName)
  correctRow = subset(correctCurves, 
    as.Date(correctCurves[,2])==as.Date(contractDate))
  
  # Change the position value for the new position
  correctRow$position = positionValue

  tradePositionData <<- rbind(tradePositionData, correctRow)

  return("Position Added.")
}


################################################################################
#
ivarInterface.testLS <- function()
{
  listToPrint = ls(envir=.GlobalEnv)

  as.character(listToPrint)
}
