#A collection of routines to collect, clean, and transform the data.
# version check
PCAdataVersion = 3

source("read.LIM.r")
# The return type is 
# marketData where
# marketData$modelType = {  1: first difference the data
#                           2: transform to returns (ln first diff)
#                           default: do nothing

# marketData$data = transformed data
# marketData$seedRowDate = label of the row associated with getRow
# marketData$seedData = row data of getRow

transformHistData = function (rawPCdata, modelType = 1, getPriceDate=-1) {
  # rawPCdata is a matrix with rowlabels dates.
  # getPriceDate is the date to return for the seed curve.
  # this function takes some raw data and returns a marketData object
  # see above parameters.
  marketData = list()
  marketData$modelType = modelType
  
  transformedData = rawPCdata[,]
  
  finalRow = nrow(transformedData)         
  if (getPriceDate == -1) {
    getRow = finalRow
    marketData$seedRowDate =  rownames(transformedData)[getRow]
    marketData$seedRow  = (transformedData[getRow,])
  }
    
  else {
    marketData$seedRowDate = as.Date(getPriceDate)
    marketData$seedRow =  transformedData[row.names(transformedData) == marketData$seedRowDate,] 
  }
  
  
  if (modelType == 1)  {
    marketData$data = apply((transformedData),2,diff)
    marketData$modelTypeString = "Model is first difference)"
  } else if (modelType == 2) {
    marketData$data = apply(log(transformedData),2,diff)
    marketData$modelTypeString = "Model is returns (Log first diff)"
  } else {
    marketData$data = transformedData
    marketData$modelTypeString = "Model is standard"
  
  }
  # make sure the seed row is a matrix with 1 row and c colums.
  c = ncol(marketData$data)
  dim(marketData$seedRow) = c(1,c)
  colnames(marketData$seedRow) = colnames(marketData$data)
  return(marketData)
}

getSeason = function(marketData, bySeason) {
    #this retreives the rows of marketData according to the season. 
    # if there are no seasons defined for marketData, then it uses the default seasons
    
    
    
    if (is.null(marketData$months.in.season)) {
      # no seasons defined
      months.in.season = list(c(5:9),c(11,12,1:3),c(4,10)) # default
    } else {
      months.in.season = marketData$months.in.season
    }
    
    if ( (bySeason>length(months.in.season)) || (bySeason<1) ) {
      # no seasons!
      return(marketData$data)
    }
    
    curr.season = months(as.Date(paste("2006-",months.in.season[[bySeason]],"-1",sep="")),TRUE)
    obsDates = as.Date(rownames(marketData$data))
    obsMonths = format(obsDates,"%b")
  
    num.months = length(curr.season)
    dataMatrix = marketData$data[obsMonths == curr.season[1],]
    for (k in 2:num.months) { 
      dataMatrix = rbind(dataMatrix,marketData$data[obsMonths == curr.season[k],])
    }
    return(dataMatrix)
    

}

deseasonalizeData = function(marketData,  months.in.season = list(c(5:9),c(11,12,1:3),c(4,10))) {
# this function deseasonalizes the data by removing seasonol means and std dev.
# rows in season 1 are scaled by season1 stdev etc.
  dataMatrix = marketData$data
  dataMatrix=na.omit(dataMatrix)
  # have to get rid of nas
  
  NObs=nrow(dataMatrix)
  p = ncol(dataMatrix)    
  obsDates = as.Date(rownames(dataMatrix))
  obsMonths = format(obsDates,"%b")
  obsMonthsMatrix = getSeasonObsMonths(obsMonths,months.in.season)
  seas.avg=t(t(t(obsMonthsMatrix) %*% dataMatrix)/apply(obsMonthsMatrix,2,sum))
  dataMatrixMeans =  seas.avg
  dataMatrixDemeaned=dataMatrix - obsMonthsMatrix %*% seas.avg
  
  marketData$months.in.season = months.in.season
  marketData$dataDeseasonalized = dataMatrixDemeaned
  marketData$seas.avg = seas.avg
  marketData$obsMonthsMatrix = obsMonthsMatrix
  return(marketData)
}

scaleBySpot = function(marketData,  months.in.season = list(c(5:9),c(11,12,1:3),c(4,10))) {
  

  marketData = deseasonalizeData(marketData,months.in.season)
  dataMatrixDemeaned = marketData$dataDeseasonalized
  NObs=nrow(dataMatrixDemeaned)
  p = ncol(dataMatrixDemeaned)    
  obsMonthsMatrix = marketData$obsMonthsMatrix
  spot.demeaned = dataMatrixDemeaned[,1]
  spot.var= t(t(t( obsMonthsMatrix) %*% (spot.demeaned*spot.demeaned))/(apply(obsMonthsMatrix,2,sum)-1))
  spot.stdev=sqrt(spot.var)
  scale.vol.matrix = matrix( obsMonthsMatrix%*% (spot.stdev),NObs,p)
  dataMatrixDemeaned = dataMatrixDemeaned / scale.vol.matrix
  marketData$spot.stdev = spot.stdev
  
  marketData$dataDeseasonalized = dataMatrixDemeaned
  return(marketData)
  
}
# Index "IGBBL00" is Henry Hub
source("read.LIM.r")

getSeasonObsMonths = function(obsMonths, months.in.season = list(c(5:9),c(11,12,1:3),c(4,10))) {
 # takes a vector of obsMonths and returns a seasonal matrix (columns indicate that the row is in the above season
 # this is used in deseasonalizing the data
  num.seasons = length(months.in.season)
  seas.mo = NULL
  seas.col.labels =NULL
  obsMonthsMatrix = matrix(0,length(obsMonths),num.seasons)
  
  for (j in 1:num.seasons) {
    curr.season = months(as.Date(paste("2006-",months.in.season[[j]],"-1",sep="")),TRUE)
    seas.mo[[j]] = curr.season
    num.months = length(seas.mo[[j]])
    for (k in 1:num.months) {
        obsMonthsMatrix[,j] = obsMonthsMatrix[,j]+1*(obsMonths == curr.season[k])
    }
    seas.col.labels = c(seas.col.labels,paste("Seas",j,sep=""))
  }            
  colnames(obsMonthsMatrix) = seas.col.labels
  return(obsMonthsMatrix)
  
}

pricingDatesCalcdT <- function(asofDate, pricingDates, months.in.season = list(c(5:9),c(11,12,1:3),c(4,10))) {
  # asofdate is date0
  # pricingDate is a vector of dates
  # returns a matrix whose rows are (dT, seasons) for each pricingDate(i).
  dTVector <- as.numeric(pricingDates - asofDate)
  bySeasonVector=array(0,dim=c(length(pricingDates),1))
    
  obsMonths = format(pricingDates,"%b")
  numSeasons = length(months.in.season)
  for (i in 1:numSeasons) {
    curr.season = months(as.Date(paste("2006-",months.in.season[[i]],"-1",sep="")),TRUE)
    num.months = length(curr.season)
    for (k in 1:num.months) { 
      bySeasonVector[obsMonths == curr.season[k]] = i
    }
  }
  pricingDatesMatrix = cbind(dTVector,bySeasonVector)
  row.names(pricingDatesMatrix) = format(pricingDates)
  colnames(pricingDatesMatrix) = c("dT","Seas")
  return(pricingDatesMatrix)
}

rollingNGFC <- function(startDate, endDate, maxP = 60 ,Verbose=FALSE )
  # maxP is the number of futures to return... prompt to P+maxP
{
    query <- "SHOW NGYrMo: ExpYrMth of NG \n S: index of IGBBL00 \n P0: Close of NG \n "
    #query ="SHOW "
    
    for (i in 2:maxP) {
      if (i<10) query = paste(query,"P",i-1,": Close of NG_0",format(i),"\n",sep="")
      else query = paste(query,"P",i-1,": Close of NG_",format(i,format="d",width=2),"\n",sep="")
    }     
    query = paste(query, "WHEN date is from ",startDate," to ",endDate,"\n  and NG is defined")
                                        
    data.lim <- read.LIM(query)                                     
    
# Diagnostics
	if(nrow(data.lim)==0)
	{
		print(data.lim)
		message("There are no prices for the specified period. Please select another day.")
		return()
	}

  if(length(which(!is.na(unlist(data.lim[1,2:length(data.lim[1,])]))))==0 & length(data.lim[,1])==1)
 	{
 		message("There are no prices for the specified period. Please select another day.")
 		stop("Error")
 	}

  prices  <- data.lim[,-1]
  rownames(prices) <- data.lim[,1]
  prices[is.na(prices[,1]),1]=1  # this removes dates where the prompt month rolled over and hence removes the spikes.
  #prices = na.omit(prices)

  return(prices)
}
