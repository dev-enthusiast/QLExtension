# Get all of Frank's table of corrections
#
#
# Written by Adrian Dragulescu on 4-Dec-2006


get.corrected.prices <- function(options){
  cat("Get the corrected prices from the DB...")
  query <- paste("select pricing_date, commod_curve, start_date, price, ",
    "updated_time from foitsox.sdb_price_updates where pricing_date >= ",
    "to_date('", options$calc$hdays[1], "', 'yyyy-mm-dd') ",
    "order by updated_time desc", sep="")
  cPP <- sqlQuery(options$connections$CPSPROD, query)
  colnames(cPP) <- c("pricing.dt", "curve.name", "contract.dt", "cprice",
                     "value")
  cPP$value <- as.numeric(cPP$value)
  cPP <- unique(cPP)                            # some curves are duplicates!
  ind.dups <- which(duplicated(cPP[,c(1:3,5)])) # and have different prices
  if (length(ind.dups) > 0)    # only subset where there are dups
    cPP <- cPP[-ind.dups, ]    # even for the same time stamp!       
  
  # get the latest update 
  MM  <- cast(cPP, pricing.dt + curve.name + contract.dt ~ . , max)
  names(MM)[ncol(MM)] <- "value"
  chP <- merge(cPP, MM, all.y=T)         # corrected historical prices
  chP <- chP[,-which(colnames(chP) == "value")]  # remove the update time
  chP$pricing.dt  <- as.Date(chP$pricing.dt)
  chP$contract.dt <- as.Date(chP$contract.dt)
  chP$curve.name  <- as.character(chP$curve.name)

  ind <- which(chP$pricing.dt >= options$asOfDate)
  if (length(ind)>0){chP <- chP[-ind,]} # if Frank starts correcting the future
  
  filename <- paste(options$save$datastore.dir, "Prices/corrected.hPrices.",
    options$asOfDate, ".RData", sep="")
  save(chP, file=filename)
  cat("Done.\n")
  return(chP)   # 354,000 rows as of 16-Jan-2007
}


# Update the hPrices.yyyy-mm-dd.RData files to contain corrected prices
# Also, update the all.mkt.Prices.RData files - added 10/23/2007
#
correct.hPrices.file <- function(chP, options){
  uDays <- unique(chP$pricing.dt)
  for (d in 1:length(uDays)){
    cat(paste("Correcting prices for day:", uDays[d], "..."))
    file <- paste(options$save$datastore.dir,"Prices/hPrices.",
                  uDays[d],".RData", sep="")
    if( file.exists(file) )
    {
      backupLoad(file)
      aux <- subset(chP, pricing.dt==uDays[d])
      aux$pricing.dt <- NULL
      hP   <- merge(hP, aux, all=T)           # add the price corrections
      ind  <- which(!is.na(hP$cprice))
      cind <- which(names(hP)==as.character(uDays[d]))
      if (length(ind)>0){hP[ind, cind] <- hP$cprice[ind]}
      hP$cprice <- NULL
      save(hP, file=file)
      cat("After corrections, nrow(hP)=", nrow(hP), "\n", sep="")
    }
    cat(" Done!\n")
  }
  #-------------------------------------------------------------------
  # correct the mkt files too!  Why weren't we doing this before?!
  aux  <- strsplit(chP$curve.name, " ")
  mkt  <- sapply(aux, function(x){x[2]})   # get the market
  uMkt <- unique(mkt)
  for (m in 1:length(uMkt)){
    chP.mkt   <- subset(chP, mkt == uMkt[m])
    names(chP.mkt)[which(names(chP.mkt)=="cprice")] <- "value"
    chP.mkt <- cast(chP.mkt, curve.name + contract.dt ~ pricing.dt, I)
    
    filename <- paste(options$save$prices.mkt.dir, "all.",
      uMkt[m], ".Prices.RData", sep="")
    if (file.exists(filename)){
      backupLoad(filename)               # backupLoad hP.mkt dataframe
      chP.mkt <- merge(hP.mkt[, c("curve.name", "contract.dt")],
                             chP.mkt, all.x=T)
      for (r in 3:ncol(chP.mkt)){  # loop over the days with corrections
        hday <- names(chP.mkt)[r]
        ind  <- !is.na(chP.mkt[,r])
        hP.mkt[ind,hday] <- chP.mkt[ind,r]
      }
      save(hP.mkt, file=filename)
    }                        # don't worry if the mkt does not exist!
    cat("Done with corrections for mkt =", uMkt[m], "\n")
  }
}


#########################################
## function to correct vol
#########################################

get.corrected.vols <- function(options){
  cat("Get the corrected vols from the DB...")
  query <- paste("select pricing_date, commod_curve, contract_date, volatility, ",
    "freq_type, updated_time from foitsox.sdb_volatility_updates where pricing_date >= ",
    "to_date('", options$calc$hdays[1], "', 'yyyy-mm-dd') ",
    "order by updated_time desc", sep="")
  cPV <- sqlQuery(options$connections$CPSPROD, query)
  colnames(cPV) <- c("pricing.dt", "curve.name", "contract.dt", "cvol", "freq", "value")
  cPV$value <- as.numeric(cPV$value)
  cPV <- unique(cPV)                            # some curves are duplicates!
  ind.dups <- which(duplicated(cPV[,c(1:3,5)])) # and have different prices
  if (length(ind.dups)>0)
    cPV <- cPV[-ind.dups, ]    # even for the same time stamp!       
  
  # get the latest update 
  MM  <- cast(cPV, pricing.dt + curve.name + contract.dt ~ . , max)
  chV <- merge(cPV, MM, all.y=T)         # corrected historical vols
  chV <- chV[,-which(colnames(chV) == "(all)")]  # remove the update time
  chV$pricing.dt  <- as.Date(chV$pricing.dt)
  chV$contract.dt <- as.Date(chV$contract.dt)
  chV$curve.name  <- as.character(chV$curve.name)
  filename <- paste(options$save$datastore.dir, "Vols/corrected.hVols.",
    options$asOfDate, ".RData", sep="")
  save(chV, file=filename)
  cat("Done.\n")
  return(chV)   # 
}

# Update the hVols.yyyy-mm-dd.RData files to contain corrected vols
correct.hVols.file <- function(chV, options){
  uDays <- unique(chV$pricing.dt)
  for (d in 1:length(uDays)){
    cat(paste("Correcting prices for day:", uDays[d], "..."))
    file <- paste(options$save$datastore.dir,"Vols/hVols.",
                  uDays[d],".RData", sep="")
    if( file.exists(file) )
    {
      backupLoad(file)
      aux <- subset(chV, pricing.dt==uDays[d])
      aux$pricing.dt <- NULL
      hV   <- merge(hV, aux, all=T)           # add the price corrections
      ind  <- which(!is.na(hV$cvol))
      cind <- which(names(hV)==as.character(uDays[d]))
      if (length(ind)>0){
        hV[ind, cind] <- hV$cvol[ind]
      }
      hV$cvol <- NULL
      save(hV, file=file)
      cat("After corrections, nrow(hV)=", nrow(hV), "\n", sep="")
    }
    cat(" Done!\n")
  }
}

