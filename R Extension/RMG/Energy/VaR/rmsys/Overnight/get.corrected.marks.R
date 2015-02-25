# Get all of Frank's table of corrections
#
#
# Written by Adrian Dragulescu on 4-Dec-2006


get.corrected.prices <- function(options){
  query <- paste("select pricing_date, commod_curve, start_date, price, ",
    "updated_time from foitsox.sdb_price_updates where pricing_date >= ",
    "to_date('", options$calc$hdays[1], "', 'yyyy-mm-dd') ",
    "order by updated_time desc", sep="")
  cPP <- sqlQuery(options$connections$CPSPROD, query)
  colnames(cPP) <- c("pricing.dt", "curve.name", "contract.dt", "cprice",
                     "value")
  cPP$value <- as.numeric(cPP$value)
  
  # get the latest update 
  MM <- cast(cPP, pricing.dt + curve.name + contract.dt ~ . , max)
  res <- merge(cPP, MM, all.y=T)
  res <- res[,-which(colnames(res) == "value")]  # remove the update time
  res$pricing.dt  <- as.Date(res$pricing.dt)
  res$contract.dt <- as.Date(res$contract.dt)
  res$curve.name  <- as.character(res$curve.name)
  return(res)   # 354,000 rows as of 16-Jan-2007
}


# Update the hPrices.yyyy-mm-dd.RData files to contain corrected prices
correct.hPrices.file <- function(cPP, options){
  uDays <- unique(cPP$pricing.dt)
  for (d in 1:length(uDays)){
    cat(paste("Correcting prices for day:", uDays[d], "..."))
    file <- paste(options$save$datastore.dir,"hPrices.",
                  options$calc$hdays[d],".RData", sep="")
    load(file)
    aux <- subset(cPP, pricing.dt==uDays[d])
    aux$pricing.dt <- NULL
    hP   <- merge(hP, aux, all.x=T)           # add the price corrections
    ind  <- which(!is.na(hP$cprice))
    cind <- which(names(hP)==as.character(uDays[d]))
    if (length(ind)>0){hP[ind, cind] <- hP$cprice[ind]}
    hP$cprice <- NULL
    save(hP, file=file)
    cat(" Done!\n")
  }
}


##   # most entries are unique
##   ind <- duplicated(cPP[, c("pricing.dt", "curve.name", "contract.dt")])
##   res <- cPP[which(ind),]
##   aux <- cPP[-which(ind),]
  
##   aux <- unique(cPP[, c("curve.name", "update.time")])
##   tab <- sort(table(aux$curve.name))   # some curves have several corrections
##   ind <- cPP$curve.name %in% names(which(tab==1))
##   res <- cPP[ind,]            # add the curves with only one update.time  

##   aux <- cPP[-which(ind), ]   # find the latest correction 
##   colnames(aux)[5] <- "value"
##   MM <- cast(aux, pricing.dt + curve.name + contract.dt + price ~ . , max)

##   res <- rbind(res[,1:4], MM[,1:4])
##   return(res)
## }
