###############################################################################
# SimpleAggregate.R
# 
# TODO: Add comment
#
# Author: John Scillieri
# 
library(reshape)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")


################################################################################
# File Namespace
#
SimpleAggregate = new.env(hash=TRUE)


################################################################################
#
SimpleAggregate$main <- function()
{
    inputFilePath = Sys.getenv("INPUT_PATH")   
    
    name = gsub(".RData", "", basename(inputFilePath))
    
    rLog("Loading file:", inputFilePath)
    loadData = varLoad("loadData", inputFilePath)

    if( !is.null(loadData) )
    {
        loadData = na.omit(loadData)
        
        finalData = .simpleAggregate(loadData, name)
        
    }
    
    rLog("Done.")
    
    invisible(finalData)
}



########################### INTERNAL OBJECTS ###################################


################################################################################
#
.simpleAggregate <- function(HH, name, aggregate.method="overlap") 
{
#    Hierarchy:
#    billingsystem
#    meterreadraw 
#    historical 
#    asbilled
#    pricing
#    spi 
#    

    # convert the data source and clean the hierarchy
    HH$data.source = tolower(as.character(HH$data.source))
    HH[ which(HH$data.source=="billing system"), "data.source"] = "billingsystem"
    HH[ which(HH$data.source=="edi_hu"), "data.source"] = "historical"
    HH[ which(HH$data.source=="back billing"), "data.source"] = "spi"
    HH[ which(HH$data.source=="manual"), "data.source"] = "spi"
    HH[ which(HH$data.source==" "), "data.source"] = "spi"
    HH[ which(is.na(HH$data.source)), "data.source"] = "spi"
    
    #    Heirarchy:
    #    billingsystem, meterreadraw, historical, asbilled, pricing, spi  
    HH$data.source = factor(HH$data.source, 
            levels=c("billingsystem", "meterreadraw", "historical", "asbilled", "pricing", "spi"))
    colnames(HH)[3] <- "value"
    
    toKeep <- cast(HH[,1:3], ne.acctno + data.source ~ ., function(x){max(x)-min(x)})
    names(toKeep)[3] <- "no.hdays"
    toKeep <- subset(toKeep, no.hdays > 360)

    ind <- which(sapply(toKeep, is.factor))   # make 'em character
    toKeep[,ind] <- sapply(toKeep[,ind], as.character)
    
    if (nrow(toKeep)==0){
      rLog("No individual accounts have more than 360 days of historical data.  Exiting.")
      return(NULL)
    } else {
      HH <- merge(HH, toKeep[,1:2], all.x=T)  # extract only the accounts with enough history
      names(HH)[3:4] <- c("time", "value")
      dataToKeep <- cast(HH, ne.acctno + time ~ data.source, mean)
    }
    
    rLog("Applying hierarchy...")
    # Apply the heirarchy (DON'T CHANGE THIS ORDER UNLESS YOU WANT TO CHANGE THE RULES)
    #    billingsystem, meterreadraw, historical, asbilled, pricing, spi  
    dataToKeep$finalValue = NA
    if ("spi" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$spi)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$spi)), "spi"]
    
    if ("pricing" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$pricing)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$pricing)), "pricing"] 
    
    if ("asbilled" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$asbilled)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$asbilled)), "asbilled"] 
    
    if ("historical" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$historical)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$historical)), "historical"]
    
    if ("meterreadraw" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$meterreadraw)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$meterreadraw)), "meterreadraw"] 
    
    if ("billingsystem" %in% names(dataToKeep))
      dataToKeep[which(!is.na(dataToKeep$billingsystem)), "finalValue"] = 
            dataToKeep[which(!is.na(dataToKeep$billingsystem)), "billingsystem"] 
    
    
    # remove the extra cols now that we've picked the data we want
    ind <- which(names(dataToKeep) %in% c("pricing", "asbilled", "historical",
                                          "meterreadraw", "billingsystem", "spi"))
    dataToKeep <- dataToKeep[, -ind]
    
##     #final average for dataToKeep
##     names(dataToKeep) = c("time", "value")
##     dataToKeep$time = as.POSIXct(dataToKeep$time)
##     dataToKeep = na.omit(dataToKeep)

    if (aggregate.method == "simple"){
      rLog("Averaging all accounts...")
      finalData = cast(dataToKeep, time ~ ., mean)
    }
    if (aggregate.method == "overlap"){
      finalData <- .aggregation.overlap(dataToKeep)
      if (is.null(finalData)){return(NULL)}
    }
      
    names(finalData) = c("time", "value")
    finalData$time = as.POSIXct(finalData$time)
    finalData = finalData[ order(finalData$time), ]
    
    outputDir = "//ceg/cershare/All/Risk/Projects/CER.Integration/VLR/Results/Simple5/"
    fileName = paste(outputDir, name, ".csv", sep="")
    rLog("Saving file:", fileName)
    write.csv(finalData, file=fileName, row.names=FALSE)
    
    return(finalData)
}

################################################################################
#
.aggregation.overlap <- function(dataToKeep)
{
  rLog("Inside aggregation overlap")
  names(dataToKeep)[2] <- "value"
  centers <- cast(dataToKeep[,1:2], ne.acctno  ~ ., function(x){c(min(x),
    median(x), max(x))})
  names(centers)[2:4] <- c("min", "median", "max")

#  as.numeric(ISOdatetime(1970,1,1,0,0,0, tz="GMT"))

  the.center <- median(centers$median)
  # remove the accounts that end before the.center, or start after the.center
  ind.accts <- c(which(centers$max < the.center),
                 which(centers$min > the.center))
  if (length(ind.accts)>0){
    centers <- centers[-ind.accts, ]
    dataToKeep <- subset(dataToKeep, ne.acctno %in% centers$ne.acctno)
  }

  names(dataToKeep)[2:3] <- c("time", "value")
  ZZ <- cast(dataToKeep, time ~ ne.acctno, I)

  rLog("We have ", ncol(ZZ)-1, "unique accounts in this combination.")
#  zz <- zoo(ZZ[,-1], ZZ[,1]); plot(zz, col="blue")
  if (ncol(ZZ)>2){
    ind.ranges <- data.frame(
      start = apply(ZZ[,-1], 2, function(x){min(which(!is.na(x)))}),
      end   = apply(ZZ[,-1], 2, function(x){max(which(!is.na(x)))}))
  } else {
    ind.ranges <- data.frame(start=min(ZZ$time), end=max(ZZ$time), count=1)
  }

  ind.result <- c(start=max(ind.ranges$start), end=min(ind.ranges$end),
           count=nrow(ind.ranges))
  if (ind.result["end"] - ind.result["start"] <= 0){
    print("There is no overlapping part to start the algorithm. ")
  }
  if (ind.result["end"]-ind.result["start"] >= 8758){
    print("Maximal overlap.")
    finalData <- ZZ[ind.result["start"]:ind.result["end"], ]
    if (ncol(finalData)>2){
      finalData <- data.frame(time=finalData[,1],
                        value=apply(finalData[,-1], 1, mean, na.rm=T))}    
    return(finalData)
  }
  
  if (ncol(ZZ) < 3){  # only one account, nothing to extend ... 
    rLog("One account with less then 1yr data.  Exiting")
    return(NULL)
  }

  rLog("Start the algo for data extension.")
  sStarts <- sort(ind.ranges$start, decreasing=TRUE)
  sEnds   <- sort(ind.ranges$end)
  finalData <- NULL
  for (r in 2:nrow(ind.ranges)){
    # try to increase to the right ...
    aux <- c(ind.result["start"], end=sEnds[r], ind.result["count"]-1)
    if (aux["end"] - aux["start"] >= 8758){  # found it!
      finalData <- ZZ[aux["start"]:aux["end"], ]
      break
    }  
    # try to increase to the left ...
    aux <- c(start=sStarts[r], ind.result["end"], ind.result["count"]-1)
    if (aux["end"] - aux["start"] >= 8758){  # found it!
      finalData <- ZZ[aux["start"]:aux["end"], ]
      break
    }
    # try to increase to the right and left at the same time...
    aux <- c(start=sStarts[r], end=sEnds[r], count=ind.result["count"]-2)
    if (aux["end"] - aux["start"] >= 8758){  # found it!
      finalData <- ZZ[aux["start"]:aux["end"], ]
      break
    }
    ind.result <- aux
  }
  if (is.null(finalData)){return(NULL)}   # if you cannot find a year worth of data
  
  # remove stray curves ...
  count.na <- apply(finalData[,-1], 2, function(x)sum(is.na(x)))
  ind.problems <- which(count.na >= 48) + 1  # give 2 day leeway
  finalData <- finalData[,-ind.problems]

  # take a final average over the selected accounts
   if (ncol(finalData)>2){
     finalData <- data.frame(time=finalData[,1],
                          value=apply(finalData[,-1], 1, mean, na.rm=T))}  
  return(finalData)  
}

################################################################################
#
.inspectResults <- function(){

  outdir <- "//ceg/cershare/All/Risk/Projects/CER.Integration/VLR/Results/Simple4/"
  fileList <- list.files(outdir, full.names=TRUE)

  HH <- NULL
  for( fileIndex in seq_along(fileList) ){
    file <- fileList[fileIndex]
    rLog("Loading file", fileIndex, "of", length(fileList), ":", file)
    aux <- read.csv(file)
    name <- gsub("\\.csv", "", strsplit(file, "load\\.")[[1]][2])
#    windows()
    plot(aux[,-1], type="l", col="blue", main=name)    
    names(aux)[2] <- name
    
    if (fileIndex == 1){
      HH <- aux
    } else {
      HH <- merge(HH, aux, all=T)
    }
  }
##   fileout <- paste(outdir, "matrix7.csv", sep="")
##   write.csv(HH, file=fileout, row.names=FALSE)
  
}


################################################################################
#
.generateCondorQueueList <- function()
{
    fileList = list.files("//nas-msw-07/rmgapp/VLR Data", full.names=TRUE)
    
    cat(paste("INPUT_PATH='", fileList, "'\nQueue\n\n", sep=""))
}



################################ MAIN EXECUTION ################################
.start = Sys.time()

#Sys.setenv(INPUT_PATH="R:/VLR Data/load.6.PSNH.GV.RData")
x = SimpleAggregate$main()

Sys.time() - .start


################################################################################
##             OLD STUFF
################################################################################
##     accounts = as.character(unique(HH$ne.acctno))
##     dataToKeep = NULL
##     for( actNumIndex in seq_along(accounts) )
##     {
##         actNum = accounts[actNumIndex]
##         rLog("Accessing account", actNumIndex, "of", 
##                 length(accounts), ":", actNum)
##         data = subset(HH, ne.acctno==actNum)
##         data$ne.acctno = NULL
        
##         if( (max(data$time)-min(data$time)) < as.difftime(360, units="days") )
##         {
##             rLog("skipping account", actNum)
##             rLog((max(data$time)-min(data$time)))
##         }else
##         {
## #            data$time = as.POSIXct( 
## #                    strptime( format(data$time, "2007-%m-%d %H-%M-%S"), 
## #                            format="%Y-%m-%d %H-%M-%S" ) )
##             rLog("Unique Data sources:", as.character(unique(data$data.source)))
##             names(data) = c("data.source", "time", "value")
##             byDataSource = cast( data, time~data.source, 
##                     mean, add.missing=TRUE )
            
##             dataToKeep = rbind(dataToKeep, byDataSource)
##         }
##     }

## createBigMatrix <- function() 
## {
##     files = "S:/All/Risk/Projects/CER.Integration/VLR/Results/Simple3"
    
##     fileList = list.files(files, full.names=TRUE)
    
##     timeList = NULL
##     for( fileIndex in seq_along(fileList) )
##     {
##         file = fileList[fileIndex]
##         rLog("Loading file", fileIndex, "of", length(fileList), ":", file)
##         firstTime =  readLines(file, n=2)[2]
##         firstTime = strsplit(firstTime, ",")[[1]][1]
##         firstTime = gsub("\"", "", firstTime)
##         timeList = c(timeList, firstTime)
##     }
    
##     timeList = as.POSIXct(timeList)
##     timeList = sort(timeList)
    
##     timeStart = timeList[1]
##     timeEnd = as.POSIXct( paste(Sys.Date()-1, "23:00:00") )
##     timeRange = seq(from=timeStart, to=timeEnd, by="hours")
    
##     baseSeries = data.frame(time = timeRange)
    
##     outputData = baseSeries
##     for( fileIndex in seq_along(fileList) )
##     {
##         file = fileList[fileIndex]
##         rLog("Loading file", fileIndex, "of", length(fileList), ":", file)
##         data = read.csv(file)
##         data$time = as.POSIXct(data$time)
##         data = cast(data, time~., mean)
##         data$time = as.POSIXct(data$time)
##         names(data) = c("time", "value")
##         fullData = merge(baseSeries, data, all.x=TRUE, by="time")
##         fullData = fullData[order(fullData$time),]
        
##         columnName = paste(strsplit(basename(file), ".", fixed=TRUE)[[1]][3:4], collapse=".")
##         names(fullData)[which(names(fullData)=="value")] = columnName    
##         fullData$time=NULL
##         outputData = cbind(outputData, fullData)
        
##         if( ( ncol(outputData) == 51 ) || (fileIndex == length(fileList)) )
##         {
##             fileName = paste(files,"/", "matrix", fileIndex, ".csv", sep="")
##             rLog("\nwriting file:", fileName, "\n")
##             write.csv(outputData, file=fileName, row.names=FALSE, na="0")
##             outputData = baseSeries
##         }
##     }
## }

 
