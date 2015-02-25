################################################################################
# Calculate the historical volatility for each portfolio.  
#
# Calculation derived from formula at: http://www.sitmo.com/eq/172
#
historicalVolJob.main <- function()
{
  #Setup any needed options
  rm(list=ls())
  source("H:/user/R/RMG/Energy/VaR/Overnight/overnightUtils.R")
  asOfDate   <- overnightUtils.getAsOfDate()
  options = overnightUtils.loadOvernightParameters(asOfDate)

  #setup our big list variable
  historicalVol = NULL

  #load market data from this folder
  folderToLoadFrom = options$save$prices.mkt.dir
  trimmedPath <- gsub("/$", "", folderToLoadFrom)
  
  files <- list.files(folderToLoadFrom)
  for (fileName in files)
  {
    marketVol = calculateHistoricalVolFromMarketFile(fileName, 
      trimmedPath, asOfDate, options$calc$no.hdays) 

    #bind the market data to the big list of data
    if (is.null(historicalVol))
    {
      historicalVol = marketVol
    }else
    {
      #make sure we got data back
      if (!is.null(marketVol))
      {
        historicalVol = rbind(historicalVol, marketVol)
      }
    }
    
  }

  outputFile <- paste(options$save$prices.mkt.dir, 
    "all.historicalVol.RData", sep="")
  if (nrow(historicalVol)>0){save(historicalVol, file=outputFile)}
  cat("Wrote Output File:", outputFile,"\n")
  
}

################################################################################
# Do the actual calculation for the file of data passed in.
#
calculateHistoricalVolFromMarketFile <- function(fileName, filePath, asOfDate, daysToCalc)
{
  #region = strsplit(fileName, ".", fixed=TRUE)[[1]][2]
  cat("Calculating HistVol For File: ",filePath,.Platform$file.sep,fileName,"\n", sep="")

  fullPath  <- paste(filePath, .Platform$file.sep, fileName, sep="")
  finfo <- file.info(fullPath)
  if (finfo$isdir){return(NULL)}
  if (length(grep("all\..+\.Prices\.RData", fileName, extended=TRUE))==0){return(NULL)}

  print(paste("Loading File:", fileName))
  load(fullPath)

  # if we have 100 data points but only want a window of 10,
  # excessData is 100-10-1 = 89.  We'll use this to get the
  # last 11 data points that make up our window of data to use
  # (price(today) through price(10 days ago) ) /
  #  ( price(yesterday) through price(11 days ago) )
  excessData = (ncol(hP.mkt)-2) - daysToCalc - 1
  cat("Days To Calc:",daysToCalc,"Excess Data:",excessData,"\n")

  # Generate a window of data from now to five years from now
  lowerBounds = as.Date(asOfDate)
  upperBounds = seq(lowerBounds, by="5 years", length=2)[2]

  validData = na.omit(hP.mkt)
  validData = subset(validData, as.Date(contract.dt) > lowerBounds )
  validData = subset(validData, as.Date(contract.dt) < upperBounds )


  # if we don't have more data than our window size (+2 name columns)
  # skip this market 
  if (length(validData) < 2+daysToCalc)
  {
    return(NULL)
  }

  # trim the col names [(1:2)]
  cat("Removing Column Names...\n")
  dividend = validData[,-(1:2)]
  divisor = validData[,-(1:2)]

  # if there's no excess to trim, don't bother
  if (excessData > 0)
  {
    cat("Trimming Excess Data...\n")
    dividend = dividend[,-(1:excessData)]
    divisor = divisor[,-(1:excessData)]
  }
  
  # remove the oldest data point from the dividend and the newest
  # from the divisor
  cat("Truncating Dividend and Divisor...\n")
  dividend = dividend[,-(1)]
  divisor = divisor[,-(length(divisor))]

  # calculate the returns for each day
  returns = log(dividend/divisor)
  
  #remove anything that generated an invalid result
  returns = na.omit(returns)
  removedRows = attr(returns, "na.action")
  
  # if there's an outlier in any of the curve return data, we're going to throw 
  # it out and not worry about that one.
  cat("Removing Curves with Outliers...\n")
  curvesToRemove = apply(returns, 1, easyOutliers.hasOutlier) 
  withRemoveFlag = cbind(returns, curvesToRemove)
  withRemoveFlag = withRemoveFlag[withRemoveFlag$curvesToRemove!=TRUE,]
  returns = withRemoveFlag[,-length(withRemoveFlag)]
    
  # determine the historical volatility using standard SD and adjusted
  cat("Calculating Historical Volitility...\n")
  tradingDaysInAYear=252 
  hVol = apply(returns,1,sd,na.rm=TRUE)*sqrt(tradingDaysInAYear)

  #remove the curves w/outliers & put the row names back on to the matrix
  if ( length(removedRows) == 0 )
  {
    namesWithRemoveFlag = cbind(validData[,(1:2)], curvesToRemove)
    
    #append the new last closed price and price change columns
    validLength = length(validData)
    namesWithRemoveFlag = cbind(namesWithRemoveFlag, 
      validData[,((validLength-1):validLength)])
    namesWithRemoveFlag$PriceChange = 
      namesWithRemoveFlag[,(length(namesWithRemoveFlag))] - 
      namesWithRemoveFlag[,(length(namesWithRemoveFlag)-1)]
  }else
  {
    namesWithRemoveFlag = cbind(validData[-(removedRows),(1:2)], curvesToRemove)
    
    #append the new last closed price and price change columns
    validLength = length(validData)
    namesWithRemoveFlag = cbind(namesWithRemoveFlag, 
      validData[-(removedRows),((validLength-1):validLength)])
    namesWithRemoveFlag$PriceChange = 
      namesWithRemoveFlag[,(length(namesWithRemoveFlag))] - 
      namesWithRemoveFlag[,(length(namesWithRemoveFlag)-1)]
  } 
  namesWithRemoveFlag = namesWithRemoveFlag[namesWithRemoveFlag$curvesToRemove!=TRUE,]
  marketVol = namesWithRemoveFlag[,-(3:4)]
  names(marketVol)[3]="ClosingPrice"
  marketVol = cbind(marketVol, hVol)
    
  cat("Done.\n")  
  
  return(marketVol)

}

################################################################################
#Start the program 
# 
res = try(historicalVolJob.main())
if (class(res)=="try-error"){
  cat("Failed Overnight Historical Volatility Job.\n")
}
