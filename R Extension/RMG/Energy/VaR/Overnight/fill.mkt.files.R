# Fill holes in prices and vols historical curves by calling the buddy curve
# methodology. Runs once every night, after the price/vol corrections take place.
# Creates a fresh batch of filled.Mkt.Price/Vols.RData
#
# Last modified by Adrian Dragulescu on 2-Apr-2007

# Manage the price and vol curve fill. 
core.fill.mkt <- function(mkt, hPV.mkt, curve.info, options){  # hPV.mkt can be hP.mkt or hV.mkt

  cat("  Filling curves for market", mkt, "\n")
#  if( mkt == "PJM" ) browser()
  
  ind.Dates <- grep("-", names(hPV.mkt))
  hPV.mkt    <- subset(hPV.mkt, as.Date(contract.dt) >= as.Date(format(options$asOfDate, "%Y-%m-%01")))
  ind <- as.Date(names(hPV.mkt)[ind.Dates]) < (options$calc$hdays[1] - 30) # 30 day cushion
  if (any(ind)){hPV.mkt <- hPV.mkt[, -ind.Dates[ind]]} # get only days you need for VaR
  ind.Dates <- grep("-", names(hPV.mkt))
  
  dateColumns = names(hPV.mkt)[ind.Dates]
  if( length( dateColumns ) == 0 )
  {
      cat("No dates found in hPV.mkt. Skipping.\n")
      return()
  }
      
  ind <- options$calc$hdays %in% as.Date( dateColumns )
  if (any(!ind)){ # if you don't have all days in hPV.mkt (see BRT market)
    hPV.mkt <- cbind(hPV.mkt, array(NA_real_, dim=c(nrow(hPV.mkt), length(which(!ind))),
      dimnames=list(NULL, as.character(options$calc$hdays[!ind]))))
    ind.Dates <- grep("-", names(hPV.mkt))
    ind <- order(names(hPV.mkt)[ind.Dates])
    hPV.mkt[, ind.Dates] <- hPV.mkt[, ind.Dates[ind]]   
  }
  ind.Dates <- grep("-", names(hPV.mkt))      # you may have lesser days
  mkt.cinfo <- merge(hPV.mkt[,c("curve.name", "contract.dt")], curve.info, all.x=T)
  
  # curves without buddy or linked location.  Need to email them and fix  <----
  ind <- which(is.na(mkt.cinfo$linked.location) & is.na(mkt.cinfo$buddy.curve))
  nPP <- mkt.cinfo[ind,]                  # no buddys
  if (nrow(nPP)>0){
      aux <- unique(nPP$curve.name)
      cat("Curves without buddy or linked location:\n")
      cat(paste(aux,"\n",collapse=""))
      # Remove the curves that don't have info
      mkt.cinfo = mkt.cinfo[-ind,]
      hPV.mkt = merge(hPV.mkt, mkt.cinfo[,c("curve.name", "contract.dt")])
      rownames(hPV.mkt) = NULL
      rownames(mkt.cinfo) = NULL
  }

  if (nrow(mkt.cinfo)==0){
    cat("  Could not find any", mkt, "curves in curve.info.\n")
    return()
  }
  ind.isna   <- is.na(hPV.mkt)
  ind.na.sum <- aux <- apply(ind.isna[,ind.Dates], 1, sum) # curves to be filled
  if (length(which(ind.na.sum>0))==0){
    cat("  Curves for", mkt, "don't need to be filled.  Next.\n")
    return()
  }
  aux[(ind.na.sum == 0)] <- 1                     # if all hist. days have data
  mkt.cinfo$min.dt <- names(hPV.mkt[,ind.Dates])[aux] # find fist date you have prices 

  aux <- unique(mkt.cinfo[, c("buddy.curve", "contract.dt")])
  names(aux) <- c("curve.name", "contract.dt")
  aux <- merge(mkt.cinfo[,c("curve.name", "contract.dt", "min.dt")], aux, all.x=T)
  names(aux) <- c("buddy.curve", "contract.dt", "min.dt.buddy")
  mkt.cinfo  <- merge(mkt.cinfo, aux, all.x=T)

  aux <- unique(mkt.cinfo[, c("linked.location", "contract.dt")])
  names(aux) <- c("curve.name", "contract.dt")
  aux <- merge(mkt.cinfo[,c("curve.name", "contract.dt", "min.dt")], aux, all.x=T)
  names(aux) <- c("linked.location", "contract.dt", "min.dt.linkloc")
  mkt.cinfo <- merge(mkt.cinfo, aux, all.x=T)

  mkt.cinfo <- mkt.cinfo[order(mkt.cinfo$curve.name, mkt.cinfo$contract.dt), ]
  mkt.cinfo <- mkt.cinfo[,c("curve.name", "contract.dt", "min.dt", "buddy.curve",
    "min.dt.buddy", "linked.location", "min.dt.linkloc")]
  mkt.cinfo$replacement.curve <- ifelse(!is.na(mkt.cinfo$buddy.curve),
    mkt.cinfo$buddy.curve, ifelse(!is.na(mkt.cinfo$linked.location),
    mkt.cinfo$linked.location, NA_character_))

  mkt.cinfo <- subset(mkt.cinfo, ind.na.sum>1)  # only the ones that need fillings
  hPV.mkt <- subset(hPV.mkt, curve.name %in% unique(c(mkt.cinfo$curve.name,
     mkt.cinfo$replacement.curve)))

  res <- array(list(), dim=nrow(mkt.cinfo))
  for (i in seq_len(nrow(mkt.cinfo))){
 #      cat(i, "\n")
 #     if (i < 35){next}
     res[[i]] <- fill.one.ts.curve(i, mkt, mkt.cinfo, hPV.mkt)  #  <----- CALL 
  }
  fPV <- do.call("rbind", res)
  return(fPV)
}

#===============================================================================
fill.mkt.price.files <- function(options){
  cat("Inside fill.mkt.price.files:\n")
  filename <- paste(options$save$datastore.dir, "curve.info.",
                    options$asOfDate, ".RData",sep="")
  backupLoad(filename)  

  mkt.files <- list.files(paste(options$save$prices.mkt.dir, sep=""),
    pattern="^all\\..*\\.Prices.RData", full.names=TRUE)
  for (file in mkt.files){                # loop over the market files
    mkt <- strsplit(strsplit(file, "all\\.")[[1]][2], "\\.")[[1]][1]
#    if (mkt != "FCO"){next}
#    file <- mkt.files[1]
    backupLoad(file)
    fPP <- core.fill.mkt( mkt, hP.mkt, curve.info, options )
    if (length(fPP)==0){next}
    ind <- append(1:ncol(fPP), ncol(fPP)+1, 2) # curve.name, contract.dt, vol.type
    fPP <- cbind(fPP, vol.type=NA_character_)[ind]
    filename <- paste(options$save$prices.mkt.dir, "filled.", mkt,
      ".Prices.RData", sep="")
    save(fPP, file=filename)
  } 
  cat("Done!\n")
}

#===============================================================================
fill.mkt.vol.files <- function(options){
  cat("Inside fill.mkt.vol.files:\n")
  filename <- paste(options$save$datastore.dir, "curve.info.",
                    options$asOfDate, ".RData",sep="")
  backupLoad(filename)  
  curve.info$curve.name <- paste(curve.info$curve.name, "_D", sep="")
  curve.info$linked.location <- ifelse(curve.info$linked.location != "NA",
     paste(curve.info$linked.location, "_D", sep=""))
  curve.info$buddy.curve <- ifelse(curve.info$buddy.curve != "NA",
     paste(curve.info$buddy.curve, "_D", sep=""))
  aux <- curve.info
  aux$curve.name      <- gsub("_D", "_M", aux$curve.name)
  aux$buddy.curve     <- gsub("_D", "_M", aux$buddy.curve)
  aux$linked.location <- gsub("_D", "_M", aux$linked.location)
  curve.info <- rbind(curve.info, aux)
    
  mkt.files <- list.files(paste(options$save$vols.mkt.dir, sep=""),
    pattern="all\\..*\\.Vols.RData", full.names=TRUE)
  for (file in mkt.files){                # loop over the market files
    mkt <- strsplit(strsplit(file, "all\\.")[[1]][2], "\\.")[[1]][1]
#    if (mkt != "PWO"){next}
#    file <- mkt.files[1]
    backupLoad(file)

    hV.mkt$curve.name <- paste(hV.mkt$curve.name, hV.mkt$vol.type, sep="_")
    hV.mkt$vol.type   <- NULL
    hV.mkt <- hV.mkt[order(hV.mkt$curve.name, hV.mkt$contract.dt), ]    
    fVV <- core.fill.mkt(mkt, hV.mkt, curve.info, options)

    if (length(fVV)==0){next}
    aux <- strsplit(fVV$curve.name, "_")
    fVV$vol.type   <- as.character(lapply(aux, function(x){x[2]}))
    fVV$curve.name <- as.character(lapply(aux, function(x){x[1]}))
    fVV <- fVV[c(1:2, ncol(fVV), 3:(ncol(fVV)-1))] # curve.name, contract.dt, vol.type
    filename <- paste(options$save$vols.mkt.dir, "filled.", mkt,
        ".Vols.RData", sep="")
    save(fVV, file=filename)
  } 
  cat("Done!\n")
}




## subset(mkt.cinfo, curve.name=="COMMOD NG DESTIN PHYSICAL" & contract.dt==as.Date("2007-08-01")) 

## subset(mkt.cinfo, curve.name=="COMMOD NG SOUTH TEXAS" & contract.dt==as.Date("2023-05-01")) 
  
##     subset(hP.mkt, curve.name=="COMMOD NG DAWN PHYSICAL" & contract.dt==as.Date("2007-08-01"))
    
##     subset(hP.mkt, curve.name=="COMMOD NG FGTZN3 GAS-DLY MEAN" & contract.dt==as.Date("2007-07-01"))

## subset(hP.mkt, curve.name=="COMMOD NG FGTZN3 GAS-DLY MEAN" & contract.dt==as.Date("2008-03-01")) 

##     subset(hP.mkt, curve.name=="COMMOD NG DESTIN PHYSICAL" & contract.dt==as.Date("2007-08-01")) 

##   #--------------------------------------------------------------------------------
##   cat("Start the buddy curve for volatilities ... ")
##   fVV <- get.vol.buddy.curve(mktData$vols, den)        # filled vols
##   cat("Done!\n")
  
##   filename <- paste(options$save$datastore.dir, "filled.mktData.", options$asOfDate,
##                     ".RData",sep="")
##   filled.mktData <- list(prices=fPP, vols=fVV)
##   save(filled.mktData, file=filename)
