#-------------------------------------------------------------------------------
# External Libraries
source("H:/user/R/RMG/Energy/VaR/PE/PE.R")
source("H:/user/R/RMG/Energy/VaR/PE/PEUtils.R")
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/Database/R/extend.price.sims.R")

#-------------------------------------------------------------------------------
# File Namespace
PECalculator <- new.env(hash=TRUE)

#-------------------------------------------------------------------------------
# Constants

# The number of curve simulation rows that can be rbound before being aggregated
# together.  This is used to ensure we don't fill up the hosts memory on large
# portfolios
PECalculator$.MAX_SUBSET_SIZE <- 2500

#-------------------------------------------------------------------------------
# This function takes the position data and calculates the potential exposure.
# It then summarizes the data via indirect and direct modes and outputs
# the results.
#
PECalculator$run <- function( asOfDate, 
                              positionDataFrame, 
                              runName=NULL, 
                              showPlot=TRUE, 
                              writeToFile=FALSE, 
                              isInteractive=FALSE ) 
{
    runName <- toupper(runName)
    PE$loadPEOptions(asOfDate, isInteractive)
  
    positionList <- PECalculator$.filterPositionDataFrame(positionDataFrame)
    if( is.null( positionList ) )
    {
        invisible(NULL)
    }
    
    dataToPlot <- PECalculator$.aggregateSimulations( positionList )
    if( is.null(dataToPlot) )
    {   
        invisible(NULL)
    }
  
    outputDir <- paste(PE$options$save$reports.dir, runName, "/", sep="")
    if( is.na(file.info(outputDir)$isdir) )
    {
        dir.create(outputDir)
    }
    asOfDate <- PE$options$asOfDate
    
    mode              <- "direct"
    directSummaryData <- PECalculator$.summarizeData( dataToPlot, mode=mode )
    PECalculator$.outputData( directSummaryData, 
                              asOfDate,
                              runName, 
                              mode, 
                              showPlot, 
                              writeToFile, 
                              outputDir )
  
    mode                <- "indirect"
    indirectSummaryData <- PECalculator$.summarizeData( dataToPlot, mode=mode )
    PECalculator$.outputData( indirectSummaryData, 
                              asOfDate, 
                              runName, 
                              mode, 
                              showPlot, 
                              writeToFile, 
                              outputDir )
  
    if (writeToFile)
    {  # save the final results to xls
        PEUtils$write.xls( directSummaryData, 
                           indirectSummaryData,
                           positionList, 
                           outputDir )
    }
      
    invisible(NULL)
}

#-------------------------------------------------------------------------------
# INTERNAL OBJECTS

#-------------------------------------------------------------------------------
# This function takes a position data frame whose names must contain
# "curve.name", "contract.dt", "position" and "FO", validates the structure and
# returns the filtered data frame.
PECalculator$.filterPositionDataFrame <- function( positionDataFrame )
{
    # check the names are present
    positionNames   <- names(positionDataFrame)
    hasCurveName    <- "curve.name" %in% positionNames
    hasContractDate <- "contract.dt" %in% positionNames
    hasPosition     <- "position" %in% positionNames
    hasCurrentPrice <- "F0" %in% positionNames
    
    # if its not a correctly formatted data frame
    if( is.null(positionDataFrame) ||
        !hasCurveName || !hasContractDate || !hasPosition || !hasCurrentPrice )
    {
        rError( "Improperly Formatted Position List Data Frame!",
                "It must have columns named curve.name, contract.dt, position and F0.",
                "Setting position list to NULL.", sep="\n" )
        return( NULL )
    }
  
    # subset the data set to extract only those relevant columns
    dataSubset   <- positionDataFrame[,c("curve.name", "contract.dt", "position", "F0")]
    
    # generate the unique set and store it in the PECalculator namespace
    positionList <- unique(dataSubset)
    
    if( nrow(positionList) != nrow(dataSubset) || ncol(positionList) != ncol(dataSubset) )
    {
        rWarn("Non-unique positions were removed from the stored list.")
    }
    
    return(positionList)
}

#-------------------------------------------------------------------------------
# This code iterates over all the curves in a position list, retrieves the
# simulation data for that curve list, and performs the PE calculation
#
PECalculator$.aggregateSimulations <- function( positionList )
{
    # This is where we will store all the data, summed from each curve
    aggregatedData <- NULL
    # This is where we will store the intermediate rows before they are aggregated
    boundRows      <- NULL
      
    # Move cash positions to prompt month
    positionList   <- PEUtils$moveCashPositions( positionList )
    # Error event handler
    if( is.null(positionList) )
    {
        rError( "Unable to aggregate simulation data due to empty position list.\n",
                "Call PECalculator$.filterPositionDataFrame(positionDataFrame), ",
                "store the result, pass it here, and try again." )
        return(NULL)
    }
    
    uniqueCurveList <- unique(positionList$curve.name)
    rDebug("There are", length(uniqueCurveList), "unique curves to load.")
    rLog("Connecting to the simulated price filehash ...")
    sPP.env <<- .getFileHashEnv(PE$CURVE_DATA_DIR)  
    rLog("Done.")
    # load the asOfDay prices only once
    load( paste( "//ceg/cershare/All/Risk/Data/VaR/prod/Prices/hPrices.",  
                 PE$options$asOfDate, ".RData", sep="" ) ) 
    #
    #loop over all curves for PE
    curveCounter = 1
    for( curveName in uniqueCurveList )
    {
      rDebug( "Loading Curve", curveCounter, "of", length(uniqueCurveList), ":", curveName )
      #get all the positions for the current curve
      positionSubset <- subset(positionList, curve.name == curveName)
      this.curve.hP  <- subset(hP, curve.name == curveName)
      names(this.curve.hP)[grep("-", names(this.curve.hP))] <- "price"
      
      curveSubset <- PECalculator$.extractDataForCurve( curveName,
                                                        positionSubset, 
                                                        sPP.env, 
                                                        this.curve.hP)
     sim.contract.dt      <- unique(curveSubset[[1]]$contract.dt)
     pricing.dts          <- unique(curveSubset[[1]]$pricing.dt)
     position.contract.dt <- unique(positionSubset$contract.dt)
     extra.contracts      <- position.contract.dt[which(position.contract.dt > max(sim.contract.dt))] 
      # From the older PE version
      #        if( !is.null(curveSubset) || curveCounter == length(uniqueCurveList))
      #        {
      #            rDebug( "dim of curve subset = ", nrow(curveSubset), "by", ncol(curveSubset) )
      #                                                                                
      #            # add to intermediate data set
      #            if( is.null(boundRows) )      
      #            {
      #                boundRows <- curveSubset
      #            }else
      #            {
      #                #join the two sets
      #                boundRows <- rbind(boundRows, curveSubset)
      #            }
      #        
      #            if( nrow(boundRows) >= PECalculator$.MAX_SUBSET_SIZE || 
      #                curveCounter == length(uniqueCurveList) )
      #            {
      #                
      #                boundRows <- PECalculator$.peCalculation( boundRows )
      #                                           
      #                boundRows <- PECalculator$.aggregateByPricingDate( boundRows )
      #                
      #                if( is.null(aggregatedData) )
      #                {
      #                    aggregatedData <- boundRows
      #                }else
      #                {
      #                    aggregatedData <- rbind(aggregatedData, boundRows)
      #                }
      #                boundRows = NULL
      #            }
      #            rDebug( "dim of bound rows = ", nrow(boundRows), "by", ncol(boundRows) )
      #        }
      #        curveCounter <- curveCounter+1
    ## PE calculation of the simulated 48 contract months only. PE will be extented later
    boundRows <- PECalculator$.peCalculation( curveSubset[[1]] ) #curveSubset[[1]] structure: contract.dt, curve.name, positions, F0, pricing.dt, 1000 simulations
    boundRows <- PECalculator$.aggregateByPricingDate( boundRows )  #boundRows structure: contract.dt, curve.name, positions, F0, pricing.dt, 1000 PEs        
    
    ## extned by contract months
    if(!is.null(extra.contracts)) extra.contracts <- extra.contracts[which(extra.contracts>= max(pricing.dts))]
    last.ext.price  <- NULL
    if(!is.null(extra.contracts)& length(extra.contracts)>0)
    {
      rLog("Need to extend the simulations for curve:", curveName)
      if(is.na(curveSubset[[2]]))
      {
        rLog("There are insufficient data to do the extension of this curve, quit this CP!")
        return(NULL)
      }else
      {
        F0 <- NULL
        ## Extend PE by contract months only 
        for( ind in c(1:length(extra.contracts)))
        {
          #first find F0 on AOD and position for this contract month
          F0       <- positionSubset$F0[which(positionSubset$contract.dt==extra.contracts[ind])]
          position <- positionSubset$position[which(positionSubset$contract.dt==extra.contracts[ind])]
          #error/exception handlers:
          if(is.null(F0))
          {
            rLog("No F0 price for this contract on AOD, quit this CP!")
            return(NULL)
          }
          if(dim(curveSubset[[2]])[1]!= length(pricing.dts))
          {
            rLog("Pricing dates missing in the extension matrix!")
            return(NULL)
          }
          #Update PE for extended contract month:
          extended.price <- F0 + curveSubset[[2]]  #curveSubset[[2]] is the price change matrix. It records
                                                   #the change in price for the last simulated contract months (i.e. the 48th contract month
                                                   # by default). The change is according to pricing dates. So it's a (# of pricing.dt x 1000) matrix 
                                                   # The extension by month assumes that the price changes for the extended contract months (i.e. months that are
                                                   # beyond the 48 month simulated.) are the same for those months, sequenced by pricing dates
          ext.price.end  <- data.frame(contract.dt= as.character(extra.contracts[ind]), extended.price[nrow(extended.price),,drop=F])
          rownames(ext.price.end) <-NULL
          #last.ext.price records the "prices" on the last pricing date within simulation dates. We need to use this to extend over pricing dates later
          if(is.null(last.ext.price))
          {
            last.ext.price <- ext.price.end
          }else
          {
            last.ext.price <- rbind(last.ext.price, ext.price.end)
          }
          boundRows[,-1] <- boundRows[,-1]+ position*(extended.price-F0)
        }#end of for loop
        rLog("Extension of contract months finished!")  
        #
        #Now extend for the extra pricing dates:
        extra.pricing.dts <- extra.contracts
        for(ind in c(1:length(extra.pricing.dts)))
        {
          #find unexpired contract dates and positions
          ext.price  <- subset(last.ext.price, contract.dt >= as.Date(extra.pricing.dts[ind]))
          positions  <- as.matrix(subset(positionSubset, contract.dt >= as.Date(extra.pricing.dts[ind]))$position, ncol=1)
          F0s        <- as.matrix(subset(positionSubset, contract.dt >= as.Date(extra.pricing.dts[ind]))$F0, ncol=1)
          #PE for each contract months
          ext.pe     <- (ext.price[,-1] - F0s) * positions    
          #sum them as PE of this particular pricing date
          ext.pe.thisdate <- data.frame(pricing.dt= as.factor(extra.pricing.dts[ind]), t(as.matrix(apply(ext.pe, 2, sum))))
          if(is.null(boundRows))
          { 
            boundRows <- ext.pe.thisdate
          }else{
            boundRows <-rbind(boundRows, ext.pe.thisdate) #added extended pricing dates
          }
          ext.pe          <- NULL
          exp.pe.thisdate <- NULL          
        }#end of extension over pricing dates
      }#end of else   
    }#end of if
    #put all curves together
    if( is.null(aggregatedData) )
    {
        aggregatedData <- boundRows
    }else
    {
        aggregatedData <- rbind(aggregatedData, boundRows)
    }
    boundRows = NULL
   }
   rDebug("Calling final aggregate...")              
   #Final aggregation over all curves
   aggregatedData <- PECalculator$.aggregateByPricingDate( aggregatedData )
   return(aggregatedData)
}

#-------------------------------------------------------------------------------
# This function takes a curve name, loads the simulation data for that curve,
# and extracts the applicable simulations from the simulation data file, 
# based on the position subset passed in.
#
PECalculator$.extractDataForCurve <- function( curveName, 
                                               positionSubset,
                                               sPP.env, 
                                               this.curve.hP)
{
    rDebug("Extracting Data For Curve:", curveName)
  
#    validContractDates <- as.Date(unique(positionSubset$contract.dt))
#    endOfPositions     <- validContractDates[length(validContractDates)]
    positionContractDates <- as.Date(unique(positionSubset$contract.dt))
    endOfPositions        <- positionContractDates[length(positionContractDates)]
    #browser()
    res.extend <- getSimulationsFor( curveName, 
                             endOfPositions, 
                             PE$options$asOfDate, 
                             sPP.env, 
                             this.curve.hP )

    res       <- res.extend[[1]]
    #extension <- res.extend[[2]]
    validContractDates    <- as.Date(dimnames(res)[[2]])
    endOfRes              <- validContractDates[length(validContractDates)]
    # event handlers
    if( is.null(res) )
    {
        rWarn("Unable to load simulation data for:",curveName,".")
        rWarn("Returning NULL in PECalculator$.extractDataForCurve")
        return(NULL)
    }
    # if the data set loaded from the curve simulations is all NaN then
    # it's invalid so return null.
    if( length(which(is.na(res))) == length(res) )
    {
        rWarn( "All curve simulations for curve name:", curveName, "are NaN." )
        rWarn( "Returning NULL in PECalculator$.extractDataForCurve" )
        return(NULL)
    }
    # if a machine fails to load the secdb library, this is the error we'll see.
    if( is.null(colnames(res)) )
    {
        rWarn( "All curve simulations for curve name:", curveName, 
               "have null column names instead of contract dates." )
        rWarn( "Returning NULL in PECalculator$.extractDataForCurve" )
        return(NULL)
    }
    # if for some reason we're generating more simulations than required
    if( dim(res)[3] != PE$options$fp$nsim )
    {
        res <- res[,,1:PE$options$fp$nsim]
        rWarn("Extra simulations found for curve:", curveName)
    }
    #remove the expired contract dates
    currentMonth <- as.Date(format(Sys.Date(), "%Y-%m-01"))
    currentMonth <- as.Date(format(PE$options$asOfDate,"%Y-%m-01"))  #Note: this will not work for PE backtest
    index        <- which(validContractDates <= currentMonth)
    if( length(index) != 0 ) 
    {
        validContractDates <- validContractDates[-index]
        if( length(validContractDates) == 0 ) 
        {
            rLog("No positions remain after removing invalid contract dates for curve:", curveName)
            return(NULL)
        }
    }
    #convert to chars so we can subset the simulation array by name
    validContractDates <- as.character(validContractDates)
  
    # take only the simulations that are valid for the positions
    filteredByContractDate <- res[, validContractDates, ]
    
    # Need to store these before we reshape the 3D data frame
    validPricingDates <- rownames(filteredByContractDate)
  
    #Recreate the 3D data frame as a 2D data frame
    rDebug("dim(filteredByContractDate) <- ",dim(filteredByContractDate))
    if(length(dim(filteredByContractDate)) != 2)
    {
      ## Note: this part by using apply cannot handle data vector too big in size
      ## Change to handle 7 years of contracts at maximum (assume 4 years of pricing, 
      ## and no more than 14 years of contract)
      contract.length <- dim(filteredByContractDate)[[2]]
#      if(contract.length > 84)
#      {
#        gc()
#        rDebug("Note: contracts too long, some codes may be unable to handle the large size data")
#        filteredByContractDate.short <- filteredByContractDate[,c(1:84),]
#        filteredByContractDate.rest  <- filteredByContractDate[,c(85:contract.length),,drop=F]
#        filteredByContractDate.short <- apply(filteredByContractDate.short, 3, rbind)
#        filteredByContractDate.rest  <- apply(filteredByContractDate.rest, 3, rbind)
#        filteredByContractDate       <- rbind(filteredByContractDate.short, filteredByContractDate.rest)
#        rm(filteredByContractDate.short)
#        rm(filteredByContractDate.rest)
#      }else{
#        filteredByContractDate = apply(filteredByContractDate, 3, rbind)
#      }   
    filteredByContractDate = apply(filteredByContractDate, 3, rbind)
    }
    
    #reappend the contract.dt and pricing.dt to the new 2D dataframe
    contract.dt = as.Date( rep( validContractDates,each=length(validPricingDates) ) )
    pricing.dt = as.Date( rep( validPricingDates, length(validContractDates) ) )
    remappedData = data.frame( contract.dt, pricing.dt, filteredByContractDate )
    rm(filteredByContractDate)
    #rDebug("Data extension finished!")
    gc()
    # merge to extract valid positions only
    # note: sometimes merge cannot handle data of very large size, depending on system memory space
    validPositions      <- vector(mode="list", length =2)
    validPositions[[1]] <- merge(positionSubset, remappedData)
    if(!is.na(res.extend[[2]]))
    {
       validPositions[[2]] <- res.extend[[2]]
    }else{
      validPositions[[2]]  <- NA
    }
        
    rDebug("Merging finished!")
    # remove pricing.dts that are >= one month after the contract.dt
    validPositions[[1]] = subset(validPositions[[1]], 
                            pricing.dt < as.Date(format(contract.dt+32, "%Y-%m-01")))
    gc()
    
    return(validPositions)
}

#-------------------------------------------------------------------------------
# loads a pricing date by contract date by simulation count array called 'res'
getSimulationsFor <- function( curveName, 
                               toDate, 
                               asOfDate, 
                               sPP.env,
                               this.curve.hP)
{
    max.contract <- toDate    # max contract to extend to
  
    # This was built to handle multiple curves, we only pass one.
    res <- extend.onecurve.sims( curveName, max.contract, this.curve.hP,
                              PE$CURVE_DATA_DIR, sPP.env )
  
#    if( length(res) == 0 )
#    {
#        res <- NULL
#    }else
#    {
#        #Therefore, return only the first element of the list
#        res <- res[[1]]
#    }
    
  
    return(res)
}
#-------------------------------------------------------------------------------
PECalculator$.peCalculation <- function( data )
{
    simColumns        <- grep("^sim+", names(data), extended=TRUE)
    data[,simColumns] <- data$position * ( data[,simColumns] - data$F0 )
    
    return(data)
}

#-------------------------------------------------------------------------------
# Calculate historical PE for backcasting purposes.
# Where:
#   PP is a data.frame with historical prices,
#     colnames = c("curve.name", "contract.dt", yyyy-mm-dd, ...)
#   QQ is a data.frame with positions,
#     colnames = c("curve.name", "contract.dt", "position", "F0")
# 
PECalculator$.peHistCalculation <- function(QQ, PP)
{
    mPP                <- melt(PP, id=1:2)
    colnames(mPP)[3:4] <- c("pricing.dt", "price")
    mPP                <- na.omit(mPP)                  # remove expired contracts
    mPP                <- merge(mPP, QQ)
    mPP$exposure       <- mPP$position * (mPP$price - mPP$F0)
  
    res                <- aggregate(mPP$exposure, list(mPP$pricing.dt), sum)
    colnames(res)      <- c("pricing.dt", "Hist.Exposure")
    res$pricing.dt     <- as.Date(as.character(res$pricing.dt))
    
    return(res)
}

#-------------------------------------------------------------------------------
PECalculator$.aggregateByPricingDate <- function( data )
{
    if( !is.null(data) )
    {
        simColumns     <- grep("^sim+", names(data), extended=TRUE)
        data           <- aggregate( data[,simColumns], by=list(factor(data$pricing.dt) ), sum )
        names(data)[1] <- "pricing.dt"
    }
    
    return(data)
}

#-------------------------------------------------------------------------------
# This function takes a data set with the first column named
# pricing.dt and the rest of the columns representing the aggregated sum of
# simulation data.
PECalculator$.summarizeData <- function( dataToPlot, 
                                         probabilities = c(0.50, 0.95, 0.99), 
                                         mode="direct" )
{
    #calculate the summary data
    ind <- grep("sim", names(dataToPlot))
    
    if( mode=="direct" )
    {
        dataToPlot[,ind] = apply(dataToPlot[,ind], c(1,2), max, 0)
    }else if( mode=="indirect" )
    {
        dataToPlot[,ind] = apply(dataToPlot[,ind]*-1, c(1,2), max, 0)
    }else
    {
        rWarn( "Incorrect mode setting.  Defaulting to 'direct'." )
        mode             <- "direct"
        dataToPlot[,ind] <- apply(dataToPlot[,ind], c(1,2), max, 0)
    }
    
    summaryData           <- apply(dataToPlot[,ind], 1, quantile, probs=probabilities)
    colnames(summaryData) <- dataToPlot$pricing.dt
    pricingDates          <- as.Date(dataToPlot$pricing.dt)
    summaryData           <- summaryData/1000000  # show the results in millions
  
    return(summaryData)
}

#-------------------------------------------------------------------------------
# This function determines how to output the summary data, based on the 
# showPlot and writeToFile booleans.
PECalculator$.outputData <- function( summaryData, 
                                      asOfDate, 
                                      plotName="Potential Exposure", 
                                      mode="direct", 
                                      showPlot=TRUE, 
                                      writeToFile=FALSE, 
                                      outputDir="./"  )
{  
    if( writeToFile )
    {
        picType  <- toupper(sub(".", "", PE$CURVE_PICTURE_EXT, fixed=TRUE))
        rDataDir <- paste(outputDir, "RData/", sep="")
        picDir   <- paste(outputDir, picType, "/", sep="")
        
        if( file.access(rDataDir, mode=0) != 0 )
        { 
            mkdir( rDataDir )
        }
        if( file.access(picDir, mode=0) != 0 )
        {
            mkdir( picDir )
        }
        
        #Save the picture file
        fileName <- paste( picDir, plotName, "_", toupper(mode), 
                           PE$CURVE_PICTURE_EXT, sep="")
        PECalculator$.openGraphicsDeviceForFile(fileName)
        PECalculator$.plotAggregatedData( summaryData, paste(plotName, mode, sep=", "))
        dev.off()
        rLog("Wrote file:", fileName) 
        
        #Save the data file
        aux <- expand.grid( what=rownames(summaryData), 
                            contract.dt=colnames(summaryData) )
        aux <- data.frame( asOfDate=rep(asOfDate, nrow(aux)),
                           name=plotName, type=rep(mode, nrow(aux)), aux, 
                           value=as.numeric(summaryData) )
        fileName <- paste(rDataDir, plotName, "_", toupper(mode), ".RData", sep="")
        save(aux, file=fileName)
        rLog("Wrote file:", fileName)
    }
  
    if( showPlot )
    {
        windows()   
        PECalculator$.plotAggregatedData( summaryData, paste(plotName, mode, sep=", ")) 
    }
}
          
#-------------------------------------------------------------------------------
PECalculator$.openGraphicsDeviceForFile <- function( fileName )
{
    rDebug("Opening graphics device for file:", fileName)
    fileParts <- strsplit(fileName, ".", fixed=TRUE)[[1]]
    extension <- tolower(fileParts[length(fileParts)])
    rDebug("Extension to write:", extension) 
    if( extension == "png" )
    {
        png(fileName)
    }else if( extension == "jpg" || extension == "jpeg" )
    {
        jpeg(fileName)
    }else if( extension == "pdf" )
    {
        pdf(fileName)
    }else if( extension == "bmp" )
    {
        bmp(fileName)
    }else
    {
        rError("Unable to save file. Unsupported file extension specified.")
    }  
}

#-------------------------------------------------------------------------------
PECalculator$.plotAggregatedData <- function( summaryData, 
                                              title,
                                              ylim = range(summaryData) )
{
    pricingDates <- as.Date(colnames(summaryData))
  
    plot( pricingDates, y=NULL, xlim=range(pricingDates),
          ylim=ylim, type="l", main=title, 
          ylab="Potential Exposure, M$" )
    colorList <- c("black", "blue", "red", "brown", "violet")
    for( index in 1:nrow(summaryData) )
    {
        lines( pricingDates, summaryData[index,], col=colorList[index] )
        points( pricingDates, summaryData[index,], pch=index, col=colorList[index] )
    }
    
    legend( "topright", legend=rownames(summaryData), 
            col=colorList[1:nrow(summaryData)], 
            text.col=colorList[1:nrow(summaryData)], pch=1:nrow(summaryData) )
}                                                       