# Aggregate the historical prices, vols and FX in a list, and save it to 
#   file all.mktData.yyyy-mm-dd.RData.
# Where uCurves - a list with unique curves for delta and vega.  
# The function is used by both overnight runs and individual runs. 
#
#
# Written by Adrian Dragulescu on 23-Nov-2006

aggregate.all.market.states <- function(uCurves, options){
  
  #---------------------------------------------------------------------
  cat("Reading in historical prices ... \n")
  hPP <- NULL                         # historical prices
  for (d in 1:length(options$calc$hdays)){
    cat(paste("\t Processing day", options$calc$hdays[d], " ... "))
    file <- paste(options$save$datastore.dir,"hPrices.",
                  options$calc$hdays[d],".RData", sep="")
    if (!file.exists(file)){get.one.day.prices(options$calc$hdays[d], options)}
    load(file)
    hP  <- subset(hP, curve.name %in% uCurves$delta$curve.name)
    hP  <- merge(hP, uCurves$delta, all.y=T)  
    if (d>1){hPP <- merge(hPP, hP, all=TRUE)}else{hPP <- hP}
    cat("Done!\n")
  }
  cat("Done!\n")
  
  #---------------------------------------------------------------------
  cat("Reading in historical vols ... \n")
  hVV   <- NULL                           # historical vols
  if (nrow(uCurves$vega)>0){            # maybe you have no vegas
    for (d in 1:length(options$calc$hdays)){
      cat(paste("\t Processing day", options$calc$hdays[d], " ... "))
      file <- paste(options$save$datastore.dir,"hVols.",
                options$calc$hdays[d],".RData", sep="")
      if (!file.exists(file)){get.one.day.vols(options$calc$hdays[d], options)}
      load(file)    
      hV  <- subset(hV, curve.name %in% uCurves$vega$curve.name)
      hV  <- merge(hV, uCurves$vega, all.y=T)
      if (d>1){hVV <- merge(hVV, hV, all=TRUE)}else{hVV <- hV}
      cat("Done!\n")  
    }
    cat("Done!\n")
  }
  #---------------------------------------------------------------------
  cat("Reading in exchange rates ... ")  
  query <- paste("select pricing_date, rate_name, rate ",
    "from foitsox.sdb_irfx_rate where pricing_date between to_date('",
    options$asOfDate-200, "','yyyy-mm-dd') and to_date('", options$asOfDate,
    "','yyyy-mm-dd') and rate_name like '%/%' and ",
    "start_date = pricing_date and start_date = end_date", sep="")
  hFX <- sqlQuery(options$connections$CPSPROD, query)
  colnames(hFX) <- c("pricing.dt", "rate.name", "value")
  hFX <- cast(hFX, rate.name ~ pricing.dt)
  colnames(hFX) <- gsub("\\.", "-", colnames(hFX))
  colnames(hFX) <- gsub("X", "", colnames(hFX))
  rownames(hFX) <- hFX[,"rate-name"]; hFX <- hFX[,-1]
  hFX <- hFX[, which(colnames(hFX) %in% as.character(options$calc$hdays))]
  hFX["USD/USD", ] <- rep(1, ncol(hFX))
  cat("Done!\n")
  
  #---------------------------------------------------------------------
  mktData <- NULL
  mktData$asOfDate <- options$asOfDate
  mktData$prices   <- hPP
  mktData$vols     <- hVV
  mktData$fx       <- hFX
  if (options$run$is.overnight){
    filename <- paste(options$save$datastore.dir, "all.mktData.",
      options$asOfDate, ".RData", sep="")
  } else {
    filename <- paste(options$save$reports.dir.run, "all.mktData.",
      options$asOfDate, ".RData", sep="")  # for interactive runs
  }
  save(mktData, file=filename)
  cat(paste("Saved all market data to ", filename, ".", sep=""))
  return(mktData)
}

##   #---------add denominated to prices and vols for convenience----------
##   aux <- merge(hPP[, c("curve.name","contract.dt")], den, all.x=T)
##   aux$denominated[which(is.na(aux$denominated))] <- "USD"
##   hPP$denominated <- paste("USD/", aux$denominated, sep="")
  
##   aux <- merge(hPP[, c("curve.name","contract.dt","vol.type")], den, all.x=T)
##   aux$denominated[which(is.na(aux$denominated))] <- "USD"
##   hVV$denominated <- paste("USD/", aux$denominated, sep="")




##   hFX <- reshape(hFX, idvar=c("rate.name"), timevar="pricing.dt",
##                  direction="wide")
##   colnames(hFX) <- gsub("rate.", "", colnames(hFX))
##   hFX[,-1] <- 1/hFX[,-1]
##   hFX <- hFX[, c(1, which(colnames(hFX) %in% as.character(options$calc$hdays)))]
##   hFX$name <- as.character(hFX$name)
##   hFX <- rbind(hFX, c("USD/USD", rep(1, (options$calc$no.hdays+1))))
##   rownames(hFX) <- hFX$name; hFX <- hFX[,-1]
##   hFX <- as.array(hFX)
##   cat("Done!\n")
  

##   files <- list.files(path=options$save$datastore.dir, pattern="hPrices*")
##   fsize <- file.info(paste(options$save$datastore.dir, files, sep=""))
##   ind <- which(fsize$size < 2000)  # some days with little data
##   if (length(ind)>0){
##     cat("Days with problematic price data in CPSPROD:\n")
##     cat(paste(paste(files(ind), sep="", collapse="\n"), "\n"))
##     files <- files[-ind]
##   }
##   days <- sapply(strsplit(files, "\\."), function(x){x[2]})
##   for (f in 1:length(files)){



##   dim(hPP)
##   head(hPP)
##   system.time(
##     tPP <- cast(hPP, curve.name + contract.dt ~ pricing.dt, I)
##   )
##   system.time(
##     tPP <- reshape(hPP, idvar=c("curve.name", "contract.dt"),
##          timevar="pricing.dt", direction="wide")
##   )
