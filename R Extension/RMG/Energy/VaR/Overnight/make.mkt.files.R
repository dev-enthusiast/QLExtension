# Update the  mkt price and vols file with the latest asOfDate data.
# Add the most recent prices to the market files. Run time is 4 min. 
#
# Need only:
# options$asOfDate <- as.Date("2007-03-23")
# options$save$datastore.dir <- "S:/All/Risk/Data/VaR/prod/"
# options$save$datastore.mkt.dir <- "S:/All/Risk/Data/Energy/Historical/"
#
# Last modified by Adrian Dragulescu on 26-Mar-2007


make.mkt.price.files <- function(options){
    cat("Appending price files by market for", as.character(options$asOfDate),
            "... \n")
    file <- paste(options$save$datastore.dir,"Prices/hPrices.", options$asOfDate,
            ".RData", sep="")
    if (!backupLoad(file)){
        cat("Cannot load ", file, "!\n")
        return()
    }
    
    hP <- hP[!duplicated(hP),]               # there are some weird duplicates
    
    aux  <- strsplit(hP$curve.name, " ")
    mkt  <- sapply(aux, function(x){x[2]})   # get the market
    uMkt <- unique(mkt)
    for (m in 1:length(uMkt)){
        hP.mkt   <- subset(hP, mkt == uMkt[m])
        filename <- paste(options$save$prices.mkt.dir, "all.",
                uMkt[m], ".Prices.RData", sep="")
        if (file.exists(filename)){
            hP.mkt1 <- hP.mkt
            backupLoad(filename)
            
            #Filter the hP.mkt file to keep 1yr of history
            TO_KEEP = 255
            if( ncol(hP.mkt) > (TO_KEEP+2) )
            {
                validCols = names(hP.mkt)[c(1:2,(ncol(hP.mkt)-TO_KEEP):ncol(hP.mkt))]
                hP.mkt = hP.mkt[ , validCols ]
            }
            
            ind <- which(names(hP.mkt) %in% names(hP.mkt1)
                            & grep("^[:digit:]", names(hP.mkt)))
            if (length(ind)>0){hP.mkt <- hP.mkt[,-ind]}   # rewrite that day
            hP.mkt <- merge(hP.mkt, hP.mkt1, all=TRUE)
            ind <- order(names(hP.mkt)[-(1:2)])+2
            hP.mkt <- hP.mkt[,c(1:2,ind)]
#      hP.mkt <- hP.mkt[!duplicated(hP.mkt[,1:2]),]
        }
        save(hP.mkt, file=filename)
        cat("Done with", uMkt[m], "\n")
    }
    cat("Done with price mkt files!\n")
}

make.mkt.vol.files <- function(options){
    cat("Appending vol files by market for", as.character(options$asOfDate),
            "... \n")
    file <- paste(options$save$datastore.dir,"Vols/hVols.", options$asOfDate,
            ".RData", sep="")
    if ( !backupLoad(file) ){
        stop("Cannot load ", file, "!\n")
    }  
    
    aux  <- strsplit(hV$curve.name, " ")
    mkt  <- sapply(aux, function(x){x[2]})   # get the market
    uMkt <- unique(mkt)
    for (m in 1:length(uMkt)){
        hV.mkt   <- subset(hV, mkt == uMkt[m])
        filename <- paste(options$save$vols.mkt.dir, "all.",
                uMkt[m], ".Vols.RData", sep="")
        if (file.exists(filename)){
            hV.mkt1 <- hV.mkt
            backupLoad(filename)
            ind <- which(names(hV.mkt) %in% names(hV.mkt1)
                            & grep("^[:digit:]", names(hV.mkt)))
            if (length(ind)>0){hV.mkt <- hV.mkt[,-ind]}   # rewrite that day
            hV.mkt <- merge(hV.mkt, hV.mkt1, all=TRUE)
            ind <- order(names(hV.mkt)[-(1:3)])+3  
            hV.mkt <- hV.mkt[,c(1:3,ind)]
        } 
        save(hV.mkt, file=filename)
        cat("Done with", uMkt[m], "\n")
    }
    cat("Done with vol mkt files!\n")
    
    # fill missing vols with the previous day mark ...
    files <- dir(options$save$vols.mkt.dir, full.names=TRUE, pattern="^all\\.*")
    all.mkts <- sapply(files, function(x){strsplit(x, "\\.")[[1]][2]})
    not.filled <- setdiff(all.mkts, uMkt)  # the markets not filled
    if (length(not.filled)==0){return()}   # everything is fine today
    rLog("Some vol markets were entirely skipped by Jenny!")
    for (mkt in not.filled){
        rLog(paste("Copy yesterday's vols for mkt", mkt))
        filename <- paste(options$save$vols.mkt.dir, "all.",
                mkt, ".Vols.RData", sep="")
        backupLoad(filename)
        if (!(options$asOfDate %in% as.Date(names(hV.mkt)[-(1:3)]))){
            hV.mkt <- cbind(hV.mkt, hV.mkt[, ncol(hV.mkt)])
            names(hV.mkt)[ncol(hV.mkt)] <- as.character(options$asOfDate)
            save(hV.mkt, file=filename)
        }
    } 
}

make.range.mkt.files <- function(what, start.dt, end.dt, options){
    days <- seq(start.dt, end.dt, by=1)
    for (day in as.character(days)){
        loptions <- NULL
        loptions$asOfDate <- as.Date(day)
        loptions$save     <- options$save
        loptions$connections <- options$connections
        if (what=="prices"){      
            make.mkt.price.files(loptions)
        } else if (what=="vols") {
            make.mkt.vol.files(loptions)
        } else {
            cat("Unknown what parameter.  Not prices or vols.\n")
        }
    }
}

clean.mkt.files <- function(){
    path  <- "S:/All/Risk/Data/VaR/dev/Prices/"
    files <- dir(path, full.names=TRUE, pattern="^hPrices*")
    for (file in files){
        backupLoad(file)
        ind <- which(names(hP) %in% c("price","value","(all)"))
        if (length(ind)>0){
            hP <- hP[,-ind]
            cat("Problems with", file, "\n")
            save(hP, file=file)
        }
    }
}
clean.mkt.files <- function(){
    path  <- "S:/All/Risk/Data/VaR/dev/Prices/Market/2006/"
    files <- dir(path, full.names=TRUE, pattern="^all*")
    for (file in files){
        backupLoad(file)
        ind <- which(names(hP.mkt) %in% c("price","value","(all)"))
        if (length(ind)>0){
            hP.mkt <- hP.mkt[,-ind]
            cat("Problems with", file, "\n")
            save(hP.mkt, file=file)
        }
    }
}


##############################################################################
# remove curves with lower case names from mkt files!
#
clean.mkt.files.2 <- function(){
    path  <- "S:/All/Risk/Data/VaR/prod/Prices/Market/"
    files <- dir(path, full.names=TRUE, pattern="^all*")
    for (file in files){
        backupLoad(file)
        uCurves <- sort(unique(hP.mkt$curve.name))
        ind <- which(uCurves != toupper(uCurves))
        if (length(ind)>0){     
            cat("Problems with", file, "\n")
            ind.rm <- which(hP.mkt$curve.name %in% uCurves[ind])
            hP.mkt <- hP.mkt[-ind.rm,]
            save(hP.mkt, file=file)
        }
    }
}


##############################################################################
# Fill the market files with historical data from tsdb
#
fill.data.files.from.tsdb <- function( curve.name )
{
    library(SecDb)
    library(reshape)
    source("H:/user/R/RMG/Utilities/RLogger.R")
    
    curveName <- toupper(curve.name)
    market <- strsplit(curveName, " ")[[1]][2]   # get the market
    
    rLog("Loading Price File For Market:", market)
    fileName <- paste("S:/All/Risk/Data/VaR/prod/Prices/Market/all.", market,
            ".prices.RData", sep="")
    backupLoad(fileName)
    startingDim = dim(hP.mkt)
    
    if (!(curveName %in% unique(hP.mkt$curve.name))){
        rError("Curve does not exist in the hP.mkt file! Exiting.")
        return()
    }
    
    curveRows = which(toupper(hP.mkt$curve.name)==curveName)
    contractDates = as.Date(sort( hP.mkt[curveRows,"contract.dt"] ))
    
    columnNames = colnames(hP.mkt)
    pricingDates = columnNames[grep( "(\\d{4}-\\d{2}-\\d{2})", columnNames, perl=TRUE )]
    pricingDates = sort(pricingDates)
    startDate = as.Date(pricingDates[1])
    endDate = as.Date(pricingDates[length(pricingDates)])
    
    rLog("Reading TSDB Data for:", curveName)
    tsdbData = tsdb.futStrip(curveName, startDate, endDate, contractDates)
    
    # Correct for the case where we don't get back all the pricing dates
    tsdbData$pricing.dt = factor(as.character(tsdbData$pricing.dt), levels=pricingDates)
    
    rLog("Reshaping TSDB Data...")
    filledData = cast(tsdbData, curve.name+contract.dt~pricing.dt, add.missing=TRUE)
    filledData$curve.name = as.character(filledData$curve.name)
    filledData$contract.dt = as.Date(filledData$contract.dt)
    
    rLog("Removing duplicate rows...")
    prependedData = rbind(filledData, hP.mkt)
    oldRows = which(duplicated(prependedData[,c("curve.name", "contract.dt")]))
    hP.mkt = prependedData[-oldRows,]
    rownames(hP.mkt) = NULL
    
    endingDim = dim(hP.mkt)
    if( !all(startingDim == endingDim) )
    {
        rError("The dimensions of the starting and ending hP.mkt file are different! Not saving data!")
        return(NULL)
    }
    
    rLog("Saving file:", fileName)
    save(hP.mkt, file=fileName)
    
    rLog("Done.")
    invisible(hP.mkt)
}


recreateVolMarketFile <- function( market, startDate, endDate, dataPath )
{
    source("H:/user/R/RMG/Utilities/RLogger.R")
    source("H:/user/R/RMG/Utilities/load.R")
    
    rLog("Recreating vol files for market", market)
    
    dateRange = as.character( seq(startDate, endDate, by="day") )
    
    final = NULL
    
    for( date in dateRange )
    {
        file <- paste( dataPath,"Vols/hVols.", date, ".RData", sep="" )
        
        hV = varLoad("hV", file, quiet=TRUE)
        hV$"(all)" = NULL
        hV$freq = NULL
        hV$value = NULL
        
        if( is.null(hV) ){ next }
        
        badVolDesignation = which(is.na(hV$vol.type))
        if(length(badVolDesignation) > 0)
        {
            hV = hV[-badVolDesignation,]
        }
        
        rLog("Merging Date:", date)
        curvesPattern = paste("^COMMOD", market)
        marketCurves = hV[grep(curvesPattern, hV$curve.name), ]
        
        if( is.null(final) )
        {
            final = marketCurves
            
        }else
        {
            final = merge( final, marketCurves, all = TRUE, 
                    by=c("curve.name", "contract.dt", "vol.type") )
        }
    }
    
    hV.mkt = final
    outputPath = paste(dataPath, "Vols/Market/all.", market, ".Vols.RData", sep="")
    save(hV.mkt, file=outputPath)
}


recreatePriceMarketFile <- function( market, startDate, endDate, dataPath="S:/All/Risk/Data/VaR/prod/" )
{
    source("H:/user/R/RMG/Utilities/RLogger.R")
    source("H:/user/R/RMG/Utilities/load.R")
    
    rLog("Recreating price files for market", market)
    
    dateRange = as.character( seq(startDate, endDate, by="day") )
    print(dateRange)
    
    final = NULL
    
    for( dateIndex in seq_along(dateRange) )
    {
        date = dateRange[dateIndex]
        file <- paste( dataPath,"Prices/hPrices.", date, ".RData", sep="" )
        cat("loading file", file, "\n")
        
        hP = varLoad("hP", file, quiet=TRUE)
        hP$"(all)" = NULL
        hP$freq = NULL
        hP$value = NULL
        
        if( is.null(hP) ){ next }
        
        
        rLog("Merging Date:", date)
        curvesPattern = paste("^COMMOD", market)
        marketCurves = hP[grep(curvesPattern, hP$curve.name), ]
        
        if( is.null(final) )
        {
            final = marketCurves
            
        }else
        {
            final = merge( final, marketCurves, all = TRUE, 
                    by=c("curve.name", "contract.dt") )
        }
    }
    
    hP.mkt = final
    outputPath = paste(dataPath, "Prices/Market/all.", market, ".Prices.RData", sep="")
    save(hP.mkt, file=outputPath)
}


findDuplicateMarks <- function( startDate, endDate, dataPath="S:/All/Risk/Data/VaR/prod/" )
{
    source("H:/user/R/RMG/Utilities/RLogger.R")
    source("H:/user/R/RMG/Utilities/load.R")
    
    dateRange = as.character( seq(as.Date(startDate), as.Date(endDate), by="day") )
    
    for( dateIndex in seq_along(dateRange) )
    {
        date = dateRange[dateIndex]
        file <- paste( dataPath,"Prices/hPrices.", date, ".RData", sep="" )
        rLog("Loading file:", file)
        hP = varLoad("hP", file, quiet=TRUE)
        hP$"(all)" = NULL
        hP$freq = NULL
        hP$value = NULL
        
        if( is.null(hP) ){ next }
        
        original = nrow(hP[,c("curve.name", "contract.dt")])
        afterUnique = nrow(unique(hP[,c("curve.name", "contract.dt")]))
        
        if( original != afterUnique )
        {
            rLog("Duplicates Found:", date)
        }
    }
}

findDuplicateMarksMarket <- function( dataPath="S:/All/Risk/Data/VaR/prod/Prices/Market" )
{
    source("H:/user/R/RMG/Utilities/RLogger.R")
    source("H:/user/R/RMG/Utilities/load.R")
    
    fileList = list.files(dataPath, pattern="all\\..+\\.Prices.RData$", full.name=TRUE)
    
    for( file in fileList )
    {
        rLog("Loading file:", file)
        hP.mkt = varLoad("hP.mkt", file, quiet=TRUE)
        
        if( is.null(hP.mkt) ){ next }
        
        original = nrow(hP.mkt[,c("curve.name", "contract.dt")])
        afterUnique = nrow(unique(hP.mkt[,c("curve.name", "contract.dt")]))
        
        if( original != afterUnique )
        {
            rLog("Duplicates Found:", file)
        }
    }
}


## setwd("S:/All/Risk/Data/Energy/Historical/")
## files <- dir()
## for (f in 1:length(files)){
##   fileto <- gsub("\\.Data", ".Prices", files[f])
##   file.copy(files[f], fileto)
## }


#fileList = list.files(pattern="all.*", full=TRUE)
#for( fileName in fileList )
#{
#    cat("Loading file:", fileName, "\n")
#    load(fileName)
#    if( length(which(is.na(hV.mkt$vol.type))) > 0 )
#    {
#        cat(fileName, "has NA vols!\n")
#    }
#}

