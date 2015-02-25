# Run VaR on a given run parameters data.frame 
#
#
# Written by Adrian Dragulescu on 6-Dec-2006

make.IVaR.archive <- function(run, options){
  source(paste(options$source.dir,"aggregate.positions.archive.R",sep=""))
  res <- aggregate.positions.archive(run, options)
  DD <- res[[1]]; VV <- res[[2]]
  cat(paste("Loading market states ... "))
  load(options$save$all.mktData.file)
  cat("Done!\n")
  return(list(DD, VV, mktData))
}

make.IVaR.live <- function(run, options){
  source(paste(options$source.dir,"aggregate.positions.live.R",sep=""))
  res <- aggregate.positions.live(run, options)
  DD <- res[[1]]; VV <- res[[2]]

  filename <- paste(options$save$reports.dir.run, "all.mktData.", options$asOfDate,
                    ".RData", sep="")
  if (file.exists(filename)){load(filename)    # you got lucky!
  } else {                                     # because this step takes time... 
    uCurves <- list(delta = unique(DD[,c("curve.name","contract.dt")]),
                  vega  = unique(VV[,c("curve.name","contract.dt","vol.type")]))
    source(paste(options$source.dir,"Overnight/aggregate.all.market.states.R",sep=""))
    mktData <- aggregate.all.market.states(uCurves, options)
  }
  return(list(DD, VV, mktData))
}


main.run.VaR <- function(run, options){

  #-------------------Try hard to create a decent run name--------------------
  if (is.na(run$run.name) | length(run$run.name)==0){
    rname <- NULL     
    aux <- paste(run$PBT$portfolio[which(!is.na(run$PBT$portfolio))], collapse=".")
    if (length(aux)>0 & aux!=""){rname <- paste("p.", aux, ".", sep="")}
    aux <- paste(run$PBT$book[which(!is.na(run$PBT$book))], collapse=".")
    if (length(aux)>0 & aux!=""){rname <- paste(rname, "b.", aux, ".", sep="")}
    aux <- paste(run$PBT$strategy[which(!is.na(run$PBT$strategy))], collapse=".")
    if (length(aux)>0 & aux!=""){rname <- paste(rname, "s.", aux, ".", sep="")}
    aux <- paste(run$PBT$trade[which(!is.na(run$PBT$trade))], collapse=".")
    if (length(aux)>0 & aux!=""){rname <- paste(rname, "t.", aux, ".", sep="")}
    rname <- gsub("\\.*$", "", rname)  # remove trailing .
    run$run.name <- gsub(" ", "", tolower(rname))
  }  
  options$save$reports.dir.run <- paste(options$save$reports.dir, options$asOfDate,
                                       "/", run$run.name, "/", sep="")
  if (file.access(options$save$reports.dir.run)!=0){
    dir.create(options$save$reports.dir.run, recursive=T)}
  if (options$save$do.IVaR.file){
    options$save$IVaR.filename <- paste(options$save$reports.dir.run, "IVaR.",
      run$run.name, ".xls", sep="")}
  #---------------------------------------------------------------------------
  cat(paste("Loading portfolio hierarchy ... "))
  load(options$save$pHier.file)  
  cat("Done!\n")
  cat(paste("Loading curve info ... "))
  load(options$save$curve.info)  
  cat("Done!\n")
  cat(paste("Loading filled market states ... "))
  load(options$save$filled.mktData.file)
  cat("Done!\n")

  if (options$run$useLivePositions){
    res <- make.IVaR.live(run, options)
  } else { 
    res <- make.IVaR.archive(run, options)
  }
  DD <- res[[1]]; VV <- res[[2]]; mktData <- res[[3]]
  
  #---------------------------------------- construct the IVaR --------------
  hPP  <- subset(mktData$prices, curve.name %in% unique(DD$curve.name))
  IVaR <- merge(DD, hPP, all.x=T)
  if (nrow(VV)>0){  # if you don't have vegas
    hVV  <- subset(mktData$vols, curve.name %in% unique(VV$curve.name))
    IVaR <- rbind(IVaR, merge(VV, hVV, all.x=T))
  }
  colnames(IVaR)[which(colnames(IVaR) == "value")] <- "position"
  IVaR <- IVaR[which(IVaR$position != 0),]  # some =0 after aggregation

  #---------------------------------------- convert prices to USD ----------
  indDates.IVaR  <- grep("-",colnames(IVaR))
  aux <- cbind(IVaR[, c("curve.name","contract.dt","vol.type")], ind=1:nrow(IVaR))
  aux <- merge(aux, den, all.x=T)
  aux <- aux[order(aux$ind),]
  aux$denominated[which(is.na(aux$denominated))] <- "USD"
  IVaR$denominated <- paste("USD/", aux$denominated, sep="")
  FX <- mktData$fx[IVaR$denominated,]
  IVaR[, indDates.IVaR] <- as.matrix(IVaR[, indDates.IVaR])*FX
  
  #---------------------------------------- add the price/vol fillings ------
  IVaR.Prices <- IVaR.Changes <- IVaR
  IVaR$ind.IVaR <- 1:nrow(IVaR)
  fPPVV  <- NULL
  if (length(filled.mktData$prices)>0){
    fPPVV  <- cbind(filled.mktData$prices, vol.type=NA)}   # put the prices
  if (length(filled.mktData$vols)>0){
    fPPVV  <- rbind(fPPVV, filled.mktData$vols)}           # and the vols together
  if (length(fPPVV)>0){                                    # do fillings if you need
    fPPVV  <- cbind(fPPVV, ind.fPPVV=1:nrow(fPPVV))        # and create an index
    aux    <- merge(IVaR[, c("curve.name", "contract.dt", "vol.type", "ind.IVaR")],
                    fPPVV[, c("curve.name", "contract.dt", "vol.type", "ind.fPPVV")], all=F)
    indDates.IVaR  <- grep("-",colnames(IVaR.Prices))
    indDates.fPPVV <- grep("-",colnames(fPPVV))
    if (nrow(aux)>0){                                      # do it only if you need corrections 
      fPPVV[, indDates.fPPVV] <- as.matrix(fPPVV[, indDates.fPPVV])*mktData$fx[fPPVV$denominated,]
      IVaR.Changes[aux$ind.IVaR, indDates.IVaR] <- fPPVV[aux$ind.fPPVV, indDates.fPPVV]
    }
  }
  IVaR.Changes[, indDates.IVaR[-1]] <- IVaR.Changes[,indDates.IVaR[-length(indDates.IVaR)]] -
                  IVaR.Changes[,indDates.IVaR[-1]]   # calculate the changes 
  IVaR.Changes <- IVaR.Changes[,-indDates.IVaR[1]]
  IVaR.Prices$comment   <- ""
  if (length(fPPVV)>0){
    IVaR.Prices$comment[aux$ind.IVaR] <- fPPVV$comment[aux$ind.fPPVV]}
  IVaR <- list(Prices=IVaR.Prices, Changes=IVaR.Changes)

  ind <- which(apply(is.na(IVaR$Changes[,indDates.IVaR]), 1, any))
  if (length(ind)>0){
    msg <- paste("Following curves have no buddies.\nAll positions will be ignored.\n",
      paste(IVaR$Changes$curve.name[ind], sep="", collapse="\n"))
    tkmessageBox(message=msg)
    IVaR$Changes <- IVaR$Changes[-ind,]    # remove the positions.  Should do better!
  }
  #------------------------------------------------------------------------
  source(paste(options$source.dir,"make.class.Factors.R",sep=""))
  class.Factors <- make.class.Factors(IVaR, run, options)
  
  if (length(options$run$is.bidweek.day)>0 && options$run$is.bidweek.day){
    IVaR <- adjust.bidweek(IVaR, options)}

  source(paste(options$source.dir,"make.VaR.report.R",sep=""))
  cat("Start VaR tables ... ")
  make.VaR.report(IVaR, class.Factors, curve.info, options); cat("Done!\n")

  setwd(options$save$reports.dir.run)
  source(paste(options$source.dir,"make.pdf.R",sep=""))
  cat("Start pdf ... "); make.pdf(run, options); cat("Done!\n")

}


##     aux <- paste(na.omit(run$PBT$portfolio), sep="", collapse=".")
##     if (nchar(aux)>0){rname <- paste("p.", aux, sep="")}
##     aux <- paste(na.omit(run$PBT$book), sep="", collapse=".")
##     if (nchar(aux)>0){rname <- paste("b.", aux, sep="")}
##     aux <- paste(na.omit(run$PBT$trade), sep="", collapse=".")
##     if (nchar(aux)>0){rname <- paste("t.", aux, sep="")}



